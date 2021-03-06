# Damage2p

This command is used to construct a three-dimensional material object
that has a Drucker-Prager plasticity model coupled with a two-parameter
damage model.

```tcl
nDMaterial Damage2p $matTag $fcc < -fct $fct > < -E
        $E > < -ni $ni > < -Gt $Gt > < -Gc $Gc > < -rho_bar $rho_bar > 
        < -H $H > < -theta $theta > < -tangent $tangent >
```

</tr>
</tbody>
</table>
<table>
<tbody>
<tr class="odd">
<td><code class="parameter-table-variable">matTag</code></td>
<td><p>integer tag identifying material</p></td>
</tr>
<tr class="even">
<td><code class="parameter-table-variable">fcc</code></td>
<td><p>concrete compressive strength</p></td>
</tr>
<tr class="odd">
<td><code class="parameter-table-variable">fct</code></td>
<td><p>optional concrete tensile strength</p></td>
</tr>
<tr class="even">
<td><code class="parameter-table-variable">E</code></td>
<td><p>optional Young modulus</p></td>
</tr>
<tr class="odd">
<td><code class="parameter-table-variable">ni</code></td>
<td><p>optional Poisson coefficient</p></td>
</tr>
<tr class="even">
<td><code class="parameter-table-variable">Gt</code></td>
<td><p>optional tension fracture energy density</p></td>
</tr>
<tr class="odd">
<td><code class="parameter-table-variable">Gc</code></td>
<td><p>optional compression fracture energy density</p></td>
</tr>
<tr class="even">
<td><code class="parameter-table-variable">rho_bar</code></td>
<td><p>ptional parameter of plastic volume change</p></td>
</tr>
<tr class="odd">
<td><code class="parameter-table-variable">H</code></td>
<td><p>optional linear hardening parameter for plasticity</p></td>
</tr>
<tr class="even">
<td><code class="parameter-table-variable">theta</code></td>
<td><p>optional ratio between isotropic and kinematic hardening</p></td>
</tr>
<tr class="odd">
<td><code class="parameter-table-variable">tangent</code></td>
<td><p>optional integer to choose the computational stiffness
matrix</p></td>
</tr>
</tbody>
</table>

<p>The material formulations for the Damage2p object are
`ThreeDimensional` and `PlaneStrain`.

<h2 id="notes">NOTES</h2>

1. Admissible values: The input parameters vary as follows:
  <table>
  <tbody>
  <tr class="odd">
  <td><code class="parameter-table-variable">fcc</code></td>
  <td><p>negative real value (positive input is changed in sign
  automatically)</p></td>
  </tr>
  <tr class="even">
  <td><code class="parameter-table-variable">fct</code></td>
  <td><p>positive real value (for concrete like materials is less than
  `fcc`)</p></td>
  </tr>
  <tr class="odd">
  <td><code class="parameter-table-variable">Gt</code></td>
  <td><p>positive real value (integral of the stress-strain envelope in
  tension)</p></td>
  </tr>
  <tr class="even">
  <td><code class="parameter-table-variable">Gc</code></td>
  <td><p>positive real value (integral of the stress-strain envelope after
  the peak in compression)</p></td>
  </tr>
  <tr class="odd">
  <td><code class="parameter-table-variable">rhoBar</code></td>
  <td><p>positive real value 0=rhoBar&lt;sqrt(2/3)</p></td>
  </tr>
  <tr class="even">
  <td><code class="parameter-table-variable">H</code></td>
  <td><p>positive real value (usually less than `E`)</p></td>
  </tr>
  <tr class="odd">
  <td><code class="parameter-table-variable">theta</code></td>
  <td><p>positive real value 0=`theta`=1 (with: 0 hardening kinematic only
  and 1 hardening isotropic only</p></td>
  </tr>
  <tr class="even">
  <td><code class="parameter-table-variable">tangent</code></td>
  <td><p>0: computational tangent; 1: damaged secant stiffness (hint: in
  case of strong nonlinearities use it with Krylov-Newton
  algorithm)</p></td>
  </tr>
  </tbody>
  </table>
2. Default values: The Damage2p object hve the following defualt
   parameters:
  <table>
  <tbody>
  <tr class="odd">
  <td><code class="parameter-table-variable">fct</code></td>
  <td><p>= 0.1*abs(fcc)</p></td>
  </tr>
  <tr class="even">
  <td><code class="parameter-table-variable">E</code></td>
  <td><p>`= 4750\*sqrt(abs(fcc))`{.c} if `abs(fcc)`&lt;`2000` because `fcc` is assumed
  in MPa (see ACI 318)</p>
  <p>`= 57000\*sqrt(abs(fcc))` if `abs(fcc)` &gt; `2000` because `fcc` is assumed in
  psi (see ACI 318)</p></td>
  </tr>
  <tr class="odd">
  <td><p><code class="tcl-variable">ni</code></p></td>
  <td><p>= 0.15 (from comparison with tests by Kupfer Hilsdorf Rusch
  1969)</p></td>
  </tr>
  <tr class="even">
  <td><p><code>Gt</code></p></td>
  <td><p>= 1840*fct*fct/E (from comparison with tests by Gopalaratnam and
  Shah 1985)</p></td>
  </tr>
  <tr class="odd">
  <td><code class="parameter-table-variable">Gc</code></td>
  <td><p>= 6250*fcc*fcc/E (from comparison with tests by Karsan and Jirsa
  1969)</p></td>
  </tr>
  <tr class="even">
  <td><code class="parameter-table-variable">rhoBar</code></td>
  <td><p>= 0.2 (from comparison with tests by Kupfer Hilsdorf Rusch
  1969)</p></td>
  </tr>
  <tr class="odd">
  <td><code class="parameter-table-variable">H</code></td>
  <td><p>$= 0.25 E$ (from comparison with tests by Karsan and Jirsa 1969 and
  Gopalaratnam and Shah 1985)</p></td>
  </tr>
  <tr class="even">
  <td><p><code>theta</code></p></td>
  <td><p>$= 0.5$ (from comparison with tests by Karsan and Jirsa 1969 and Gopalaratnam and Shah 1985)</p></td>
  </tr>
  <tr class="odd">
  <td><code class="parameter-table-variable">tangent</code></td>
  <td><p>= 0</p></td>
  </tr>
  </tbody>
  </table>

<h2 id="references">References</h2>
<p>Tesser L.,"Efficient 3-D plastic damage model for cyclic inelastic
  analysis of concrete structures", Report of the University of Padua,
  Italy, 2012. (soon available at paduareserach.cab.unipd.it)</p>
<p>Petek K.A., "Development and application of mixed beam-solid models
  for analysis of soil-pile interaction problems", Ph.D. dissertation,
  Univerisity of Washington, USA, 2006</p>


<h2 id="development_team">Development Team</h2>

<p>Code developed by: 
<span style="color:blue">Leopoldo Tesser - Dept. DICEA - Univeristy of
Padua - Italy</span>,</p>
<p>contact: <span style="color:blue">leopoldo.tesser AT
dicea.unipd.it</span></p>

