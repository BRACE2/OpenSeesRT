---
date   : April 23, 2002
author : Zhaohui(Joey) Yang
Purpose: Visualize the results from Finite element analysis.
...

Date   : April 23, 2002

Author : Zhaohui(Joey) Yang

Purpose: Visualize the results from Finite element analysis.
 
Usage  : 
In  the appropriate directory, type [run], which will start tcl
script   TriSimu_Sand.tcl  using  OpenSees  interpreter.  After
OpenSees  finishes  the  triaxial  simulation,  the script will
start  the  3D  visualizer  Joey3Dtcl.  Or  you  can  start  it
explicitly by running the visualizer separately.


The  3D  visualizer  will  display  a  main  window,  which  is
subdivided  into two subwindows. The subwindow on the left hand
side is the main viewing window. The one on the right hand side
is  the  menu  window,  where  the  user will issue most of the
commands by clicking these buttons.

To  view  the FEM results, several files are formed by the main
tcl  script  to  transfer data from OpenSees to the visualizer.
The  user  does not need to worry about these files, since they
can  be  loaded  by simply clicking the following buttons under
"Data" category:

[Load  Geom]  --  Loading  geometry data, i.e. node and element
info After loading geometry data, the mesh will be displayed on
the left window.

[Load   Displ]--   Loading  displacement  data.  After  loading
displacement  data,  an  animation  of  the deformed shape will
start automatically in xz plane.

[Load  Plast]--  Loading  plastic  info  indicating which gauss
point is in plastic stage.

[Load Stres]-- Loading stress info ( Disabled )

[Load SurfP]-- ( Disabled )

After  loading  all  the data, the user is free to click any of
these  radio  buttons  under  "Rendering  Mode"  category.  The
program  is  going to reder whatever the button says, including
undisplaced  mesh, undisplaced solid, displaced mesh, displaced
solid,  displaced  solid  with undisplaced mesh, displaced mesh
with plastified gauss points, displaced mesh with elastic gauss
points.  The  plastic  zone  is represented by plastified gauss
points.  Currently  Element  checking, Sigma_zz, and Moment and
shear are all disabled. 

It  should  be  noted  that by default, all these commands will
render  results  at  step  0,  which  is  the  end  of applying
isotropic  compression and the start of shearing. The displaced
mesh only shows displacements occurred during shearing stage. To
show  the  total  displacement,  one  needs to activate [Sett.]
button  inside  "Lights and other options" category which is at
the top of the right window.

To  animate  deformed shape, or plastic zone progress, one just
need to set [Step Number] under "Element Filtering" category to
be  -1.  To adjust the animation speed, one can use the spinner
[Animation Speed].

To  change  the  view  angle, one simple way is to select among
[XY],  [YZ]  and [XZ] views. One can also use mouse to drag the
plot  in  order  to  view  at any angle. Specifically, dragging
mouse  horizontally at the center region of the left window will
rotate the plot along Y-axis, dragging vertically at the center
region  of  the  left  window  will  rotate  along  X-axis, and
dragging  vertically  at the area close to the window sides but
inside the left window will rotate along Z-axis.

Inside  category  "Scales  and Parameters", there are couple of
parameters  users  can  adjust  to  get  a  better  view. These
parameters  include  [Point Size] for controlling size of gauss
points, [Mesh Scale] for scaling the mesh, [DispX], [DispY] and
[DispZ] for scaling displacements in X, Y, and Z displacements,
respectively.

By  the way, the user can also use z (shift+z) to zoom in (out)
the  plot.  There  are  also  lots  of  functions  that are not
mentioned  here.  It  is up to the user to experiment it, or to
crack it.

