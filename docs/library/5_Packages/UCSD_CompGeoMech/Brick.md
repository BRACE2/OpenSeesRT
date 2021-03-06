# Brick?N

## The Eight Node Brick Element

 

    element Brick8N  eletag? node1? node2? node3? node4? node5? node6? node7?
                     node8? matTag?  bf1? bf2? bf3? massDens?

The Brick8N element is the standard eight node three dimensional element
implemented based on tensor operation. The arguments to construct the
element are its tag, eletag, eight nodes ordered according to Figure
[1](#8node){reference-type="ref" reference="8node"}, the material tag,
matTag, the body forces, bf1, bf2, bf3, and the mass density, massDens.
By default, $3\times 3 \times 3$ integration points are used. Users will
be able to specify number of integration points very soon.

The valid queries to a Brick8N element when creating an ElementRecorder
are "force", "stiffness", "stress", "pq", "pqall", "gausspoint",
"plastic" or "plasticGPC". For "stress" output, the six stress
components from each Gauss point are output by the order: $\sigma_x$,
$\sigma_y$, $\sigma_z$, $\tau_{xy}$, $\tau_{xz}$, $\tau_{yz}$. The
stresses can also be output in $p$ and $q$ format by using query "pq",
where $p$ is the hydrostatic pressure, while $q$ is the equivalent
deviatoric stress. In this case, the stress state at one gauss point is
printed in the $pq$ format. If the stress states at all gauss points
need to be printed, use the query "pqall". For "gausspoint", the
coordinates of all Gauss points are printed out. For "plastic", the
equivalent deviatoric plastic strain from each Gauss point is output in
the same order as the coordinates are printed. But the coordinates have
to be output separated. If one needs to output the gauss point
coordinates together with the plastic strain, the query "plasticGPC"
needs to be used.

::: center
![Node numbering for 8 node three dimensional
element.](/home/jeremic/tex/works/Thesis/ZhaohuiYang/Appendex/brick8.eps){#8node
width="9.0cm"}
:::

## The Twenty Node Brick Element

 

    element Brick20N eletag? node1? node2? node3? node4? node5? node6? node7?
            node8?  node9?  node10?  node11?  node12? node13? node14? node15? 
            node16? node17? node18?  node19?  node20? matTag? bf1?  bf2? bf3? 
            massDens?

The Brick8N element is the standard eight node three dimensional element
implemented based on tensor operation. The arguments to construct the
element are its tag, eletag, twenty nodes ordered according to Figure
[2](#20node){reference-type="ref" reference="20node"}, the material tag,
matTag, the body forces, bf1, bf2, bf3, and the mass density, massDens.
By default, $3\times 3 \times 3$ integration points are used. Users will
be able to specify number of integration points very soon.

The valid queries to a Brick20N element when creating an ElementRecorder
are "force", "stiffness", "stress", "pq", "gausspoint", or "plastic".
For "stress" output, the six stress components from each Gauss point are
output by the order: $\sigma_x$, $\sigma_y$, $\sigma_z$, $\tau_{xy}$,
$\tau_{xz}$, $\tau_{yz}$. The stresses can also be output in $p$ and $q$
format by using query "pq", where $p$ is the hydrostatic pressure, while
$q$ is the equivalent deviatoric stress. In this case, the stress state
at one gauss point is printed in the $pq$ format. For "gausspoint", the
coordinates of all Gauss points are printed out. For "plastic", the
equivalent deviatoric plastic strain from each Gauss point is output in
the same order as the coordinates are printed.

::: center
![Node numbering for 20 node three dimensional element.](/home/jeremic/tex/works/Thesis/ZhaohuiYang/Appendex/brick20.eps){#20node width="9cm"}
:::
