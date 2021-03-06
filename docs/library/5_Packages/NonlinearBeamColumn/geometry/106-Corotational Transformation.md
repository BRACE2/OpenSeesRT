# Corotational

This command is used to construct a corotational coordinate
transformation (`CorotCrdTransf`) object. The corotational transformation can
be used in large displacement-small strain problems. 

>NOTE: Currently the transformation does not deal with element loads and will ignore any that are applied to the element.

<p>For a two-dimensional problem:</p>

```tcl
geomTransf Corotational $transfTag < -jntOffset $dXi $dYi $dXj $dYj >
```
<p>For a three-dimensional problem:</p>

```tcl
geomTransf Corotational $transfTag $vecxzX $vecxzY $vecxzZ
```
<hr />
<table>
<tbody>
<tr class="odd">
<td><code class="parameter-table-variable">transfTag</code></td>
<td><p>integer tag identifying transformation</p></td>
</tr>
<tr class="even">
<td><p><code class="parameter-table-variable">vecxzX vecxzY vecxzZ</code></p></td>
<td><p>X, Y, and Z components of vecxz, the vector used to define the
local x-z plane of the local-coordinate system. The local y-axis is
defined by taking the cross product of the vecxz vector and the
x-axis.</p>
<p>These components are specified in the global-coordinate system X,Y,Z
and define a vector that is in a plane parallel to the x-z plane of the
local-coordinate system.</p>
<p>These items need to be specified for the three-dimensional
problem.</p></td>
</tr>
<tr class="odd">
<td><p><code class="parameter-table-variable">dXi dYi</code></p></td>
<td><p>joint offset values -- absolute offsets specified with respect to
the global coordinate system for element-end node i (optional)</p></td>
</tr>
<tr class="even">
<td><p><code class="parameter-table-variable">dXj dYj</code></p></td>
<td><p>joint offset values -- absolute offsets specified with respect to
the global coordinate system for element-end node j (optional)</p></td>
</tr>
</tbody>
</table>

## Examples

<figure>
<img src="/OpenSeesRT/contrib/static/ElementCrossSection.png" title="ElementCrossSection.png"
alt="ElementCrossSection.png" />
<figcaption aria-hidden="true">ElementCrossSection.png</figcaption>
</figure>
<figure>
<img src="/OpenSeesRT/contrib/static/ElementOrientation.png" title="ElementOrientation.png"
alt="ElementOrientation.png" />
<figcaption aria-hidden="true">ElementOrientation.png</figcaption>
</figure>
<figure>
<img src="/OpenSeesRT/contrib/static/ElementVectors.png" title="ElementVectors.png"
alt="ElementVectors.png" />
<figcaption aria-hidden="true">ElementVectors.png</figcaption>
</figure>

```tcl
# Element 1 : tag 1 : vecxZ = zaxis

geomTransf Corotational 1 0 0 -1

# Element 2 : tag 2 : vecxZ = y axis

geomTransf Corotational 2 0 1 0
```
Code Developed by: <span style="color:blue"> Remo Magalhaes de
Souza </span></p>
<p>Images Developed by: <span style="color:blue"> Silvia Mazzoni
</span></p>
