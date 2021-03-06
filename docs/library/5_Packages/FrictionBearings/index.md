# FrictionModel Library

The `frictionModel` command is used to construct a friction model
object, which specifies the behavior of the coefficient of friction in
terms of the absolute sliding velocity and the pressure on the contact
area. The command has at least one argument, the friction model type.
Each type is outlined below.

```tcl
frictionModel frnMdlType? arg1? ...
```

<hr />
The type of friction model created and the additional arguments
required depend on the <strong>frnMdlType?</strong> provided in the
command.

<p>The following contain information about frnMdlType? and the args
required for each of the available friction model types:</p>
<ul>
<li><a href="Coulomb_Friction" title="wikilink">Coulomb
Friction</a></li>
<li><a href="Velocity_Dependent_Friction" title="wikilink">Velocity
Dependent Friction</a></li>
<li><a href="Velocity_and_Normal_Force_Dependent_Friction"
title="wikilink">Velocity and Normal Force Dependent Friction</a></li>
<li><a href="Velocity_and_Pressure_Dependent_Friction"
title="wikilink">Velocity and Pressure Dependent Friction</a></li>
<li><a href="Multi-Linear_Velocity_Dependent_Friction"
title="wikilink">Multi-Linear Velocity Dependent Friction</a></li>
</ul>

<hr />

The following friction model response quantities can be recorded
through the ElementRecorder object, as long as the element has a
friction model associated with it:

- `normalForce`,
- `velocity`, 
- `frictionForce`,
- **COF**

## Examples

```tcl
recorder Element -file Elmt.out -time -ele 1 frictionModel normalForce
```
