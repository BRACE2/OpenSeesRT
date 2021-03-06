# Section Aggregator

This command is used to construct a SectionAggregator object which
aggregates groups previously-defined UniaxialMaterial objects into a
single section force-deformation model. Each UniaxialMaterial object
represents the section force-deformation response for a particular
section degree-of-freedom (dof). There is no interaction between
responses in different dof directions. The aggregation can include one
previously defined section.

```tcl
section Aggregator $secTag $matTag1 $dof1 $matTag2 $dof2
        ... < -section $sectionTag >
```

<hr />
<table>
<tbody>
<tr class="odd">
<td><p><code class="parameter-table-variable">secTag</code></p></td>
<td><p>unique section tag</p></td>
</tr>
<tr class="even">
<td><p><code>matTag1 matTag2 ...</code></p></td>
<td><p>tag of previously-defined UniaxialMaterial objects</p></td>
</tr>
<tr class="odd">
<td><p><code>dof1 dof2 ...</code></p></td>
<td><p>the force-deformation quantity to be modeled by this section
object. One of the following section dof may be used:</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>P Axial force-deformation</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Mz Moment-curvature about section local z-axis</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Vy Shear force-deformation along section local y-axis</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>My Moment-curvature about section local y-axis</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Vz Shear force-deformation along section local z-axis</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>T Torsion Force-Deformation</p></td>
</tr>
<tr class="even">
<td><p><code class="parameter-table-variable">sectionTag</code></p></td>
<td><p>tag of previously-defined Section object to which the
UniaxialMaterial objects are aggregated as additional force-deformation
relationships</p></td>
</tr>
</tbody>
</table>

<figure>
<img src="/OpenSeesRT/contrib/static/SectionAggregator.gif" title="SectionAggregator.gif" />
<figcaption aria-hidden="true">SectionAggregator.gif</figcaption>
</figure>
<hr />

<p>EXAMPLE</p>
<p>section Aggregator 2 2 Vy -section 4; # create new section with IDtag
2, taking the existing material tag 2 to represent the shear and adding
it to the existing section tag 4, which may be a fiber section where the
interaction betweeen axial force and flexure is already considered.</p>

## Theory

Read if you're using moment-curvature in `BeamWithHinges` element to
represent moment-rotation, or vice versa:
[https://doi.org/10.1193%2F1.4000136](https://doi.org/10.1193%2F1.4000136)

<hr />

<p>Code developed by: <span style="color:blue"> Micheal H. Scott, Oregon State </span></p>

