from  .ast  import *
import textwrap

class Component:
    @property
    def tag_space(self):
        return self._cmd[0].split("::")[-1]

    def __enter__(self):
        assert self._rt is None
        # libOpenSeesRT must be imported by Python
        # AFTER if has been loaded by Tcl (this was done
        # when a TclRuntime() is created) so that Tcl stubs
        # are initialized. Otherwise there will be a segfault
        # when a python c-binding attempts to call a Tcl
        # C function. Users should never import OpenSeesPyRT
        # themselves
        from .tcl import TclRuntime
        rt = TclRuntime()
        from . import OpenSeesPyRT as libOpenSeesRT

        if self.tag_space == "uniaxialMaterial":
            rt.model(self)
            tag = self.name if self.name is not None else "1"
            self._builder = libOpenSeesRT.get_builder(rt._interp.interpaddr())
            handle = self._builder.getUniaxialMaterial(tag)

        elif self.tag_space == "section":
            rt.model(self)
            self._builder = libOpenSeesRT.get_builder(rt._interp.interpaddr())
            handle = self._builder.getSection(str(self.name))

        elif self._cmd[0] == "backbone":
            rt.model(self)
            self._builder = libOpenSeesRT.get_builder(rt._interp.interpaddr())
            handle = self._builder.getHystereticBackbone(str(self.name))

        else:
            raise TypeError("Unimplemented type")

        self._rt = rt
        return handle

    def __exit__(self, exception_type, exception_value, exception_traceback):
        assert self._rt is not None
        self._rt = None


    def get_ast(self):
        pass

    def get_cmd(self):
        args = [
            a for arg in self._args
                for a in arg.as_tcl_list(value=getattr(self, arg.field))
        ]
        return self._cmd + args

    def get_cmd_str(self):
        for i in self.get_cmd():
            if isinstance(i,list):
                yield "{\n"
                for j in i:
                    yield f"{j}"
                yield "\n}\n"
            else:
                yield str(i)

    def __call__(self, **kwds):
        return self.copy(**kwds)

    def copy(self, **kwds):
        partial = self.__class__()
        for field in self.__slots__:
            if field in kwds:
                setattr(partial, field, kwds.pop(field))
            else:
                setattr(partial, field, getattr(self, field))
        partial.kwds.update(kwds)
        partial._init()
        return partial

    def _init(self):
        self._rt = None
        self._argdict = {}
        for arg in self._args:
            field = arg.field
            if getattr(self, field) is None:
                try: setattr(self, field, self.kwds[arg.name])
                except Exception as e: pass
            if getattr(self, field) is None:
                try: setattr(self, field, arg._get_value(None))
                except Exception as e: pass
            if getattr(self, field) is None:
                try: setattr(self, field, getattr(
                     getattr(self, arg.kwds["alt"]), field))
                except Exception as e: pass

            self._argdict[field] = arg

    def get_refs(self):
        for ref in self._refs:
            if ref in self._argdict:
                arg = self._argdict[ref]
                val = getattr(self, arg.field)
                if isinstance(arg, Ref):
                    yield val, _get_tagspace(arg.type)
                elif isinstance(arg, Grp):
                    for v in val: yield (v, _get_tagspace(arg.type))
            else:
                val = getattr(self, ref)
                if hasattr(val, "get_refs"):
                    yield val, _get_tagspace(val)
                    yield from val.get_refs()
                else:
                    yield val, _get_tagspace(val)



def _get_tagspace(arg):
    if hasattr(arg, "tag_space"):
        return arg.tag_space
    elif isinstance(arg, str):
        return arg
    else:
        return None

class Cmd:
    cmd = None
    def __init__(self, cmd , defs=None):
        self.cmd = cmd


def cmd(cls, cmd=None, args=None, refs=(), namespace=None, **ops):
    if isinstance(cls, str):
        # Called as function (ie my_command = cmd('my_cmd'))
        fields = [arg.field for arg in args] 
        alts = {arg.kwds["alt"] for arg in args if "alt" in arg.kwds}
        obj = struct(cls, fields, args, alts, cmd=[cmd], refs=refs)
    else:
        # Called as class decorator
        fields = [arg.field for arg in cls._args]
        alts = cls._alts if hasattr(cls, "_alts") else None
        refs = cls._refs if hasattr(cls, "_refs") else ()
        name = cls.__name__
        cmd = name.lower()[0] + name[1:]
        obj = struct(name, fields, cls._args, alts, cmd=[cmd], refs=refs, parents=[cls])
    if namespace is not None:
        obj._cmd[0] = namespace + "::" + obj._cmd[0]
    return obj


class LibCmd(Cmd):
    cmd = None
    def __init__(self, cmd , class_name=None, subs={}, args=None, rels=None, about="", defs=None, namespace=None):
        if class_name is None:
            class_name = cmd.title()
        self.class_name = class_name

        if not isinstance(cmd, str):
            self.typ = cmd
            cmd = cmd.__name__
        else:
            self.typ = None
        
        if namespace is not None:
            cmd = namespace + "::" + cmd

        self.__name__ = self.cmd = cmd
        self.args = [] if args is None else args
        self.rels = [] if rels is None else rels
        self.about = about

        for sub in subs:
            setattr(self, sub, self(sub.title(), sub, args=subs[sub]))

    def __call__(self, cls, name=None, args=None, refs=None, inherit=None, **opts):
        if inherit is None: inherit = []
        if self.typ is not None: inherit.append(self.typ)
        if refs is None:
            refs = []
        
        if not isinstance(cls, str):
            if hasattr(cls, "_refs"):
                refs += cls._refs
            args = cls._args
            inherit.append(cls)
            if hasattr(cls, "_name"):
                cls = name = cls._name
            else:
                cls = name = cls.__name__

        if name is None:
            name = cls
            
        args = self.args + args
        fields = [arg.field for arg in args]
        if "alts" in opts: fields += [a.field for a in opts["alts"]]
        alts = {arg.kwds["alt"] for arg in args if "alt" in arg.kwds}
        obj = struct(cls, fields, args, alts, refs=refs, cmd=[self.cmd, name], parents=inherit)
        obj.kwds = opts
        obj._class_name = self.class_name
        setattr(self, name, obj)
        return obj


def struct(name, fields, args = None, alts=None, refs=None, cmd=None, parents=None):
    if cmd is None: cmd = []
    if refs is None: refs = []
    if parents is None: parents = []
    if isinstance(args[0], Tag):
        args[0].kwds["tag_space"] = cmd[0]

    template = textwrap.dedent("""\
    class {name}({parents}):
        __slots__ = ["_argdict"] + {fields!r}
        def __init__(self, {fields_none}, **kwds):
            {self_fields} = {args}
            self.kwds = kwds
            self._refs = {refs!r}
            self._init()
            self._cmd = {cmd!r}
            if hasattr(self,"init"): self.init()

        def keys(self): return self.__slots__
        def __getitem__(self, idx):
            return getattr(self, idx)

    """).format(
        name=name,
        refs=refs,
        cmd =cmd,
        fields=fields,
        parents = ",".join([p.__name__ for p in parents]+["Component"]),
        args=','.join(fields),
        fields_none=','.join(f"{f}=None" for f in fields),
        self_fields=','.join('self.' + f for f in fields)
    )
    d = {'fields': fields, "Arg": Arg, "Component": Component}
    d.update({c.__name__: c for c in parents})
    exec(template, d)
    d[name]._args = args
    return d[name]

def walk_refs(parent):
    P, R = "", ""
    for ref, ts in parent.get_refs():
        p, r = walk_refs(ref[0])
        P = p + P
        R = r + R
    return P, R

class _LineElement:
    @property
    def mesh_interval(self):
        if "split" in self.kwds:
            incr = 2.0 / (self.kwds["split"])
            return ( -1 + i * incr
                for i in range(1, self.kwds["split"]))

Mat = LibCmd("nDMaterial")
Uni = LibCmd("uniaxialMaterial")
Ele = LibCmd("element")
Trf = LibCmd("geomTransf")
Sec = LibCmd("section")
Lnk = LibCmd("rigidLink")

