# Stress Density Material

<p>This command is used to construct a multi-dimensional stress density
material object for modeling sand behaviour following the work of
Cubrinovski and Ishihara (1998a,b). Note that as of January 2020 this
material is still undergoing verification testing for more complex
loading and initial conditions.</p>

```tcl
nDMaterial stressDensity $matTag $mDen $eNot $A $n $nu
        $a1 $b1 $a2 $b2 $a3 $b3 $fd $muNot $muCyc $sc $M $patm 
        < $ssl1 $ssl2 $ssl3 $ssl4 $ssl5 $ssl6 $ssl7 $hsl $pmin >
```

<table>
<tbody>
<tr class="odd">
<td><code class="parameter-table-variable">matTag</code></td>
<td><p>integer tag identifying material</p></td>
</tr>
<tr class="even">
<td><code class="parameter-table-variable">mDen</code></td>
<td><p>mass density</p></td>
</tr>
<tr class="odd">
<td><code class="parameter-table-variable">eNot</code></td>
<td><p>initial void ratio</p></td>
</tr>
<tr class="even">
<td><code class="parameter-table-variable">A</code></td>
<td><p>constant for elastic shear modulus</p></td>
</tr>
<tr class="odd">
<td><code class="parameter-table-variable">n</code></td>
<td><p>pressure dependency exponent for elastic shear modulus</p></td>
</tr>
<tr class="even">
<td><code class="parameter-table-variable">nu</code></td>
<td><p>Poisson's ratio</p></td>
</tr>
<tr class="odd">
<td><p><code class="parameter-table-variable">a1</code></p></td>
<td><p>peak stress ratio coefficient (etaMax = a1 + b1*Is)</p></td>
</tr>
<tr class="even">
<td><p><code class="parameter-table-variable">b1</code></p></td>
<td><p>peak stress ratio coefficient (etaMax = a1 + b1*Is)</p></td>
</tr>
<tr class="odd">
<td><p><code class="parameter-table-variable">a2</code></p></td>
<td><p>max shear modulus coefficient (Gn_max = a2 + b2*Is)</p></td>
</tr>
<tr class="even">
<td><p><code class="parameter-table-variable">b2</code></p></td>
<td><p>max shear modulus coefficient (Gn_max = a2 + b2*Is)</p></td>
</tr>
<tr class="odd">
<td><p><code class="parameter-table-variable">a3</code></p></td>
<td><p>min shear modulus coefficient (Gn_min = a3 + b3*Is)</p></td>
</tr>
<tr class="even">
<td><p><code class="parameter-table-variable">b3</code></p></td>
<td><p>min shear modulus coefficient (Gn_min = a3 + b3*Is)</p></td>
</tr>
<tr class="odd">
<td><code class="parameter-table-variable">fd</code></td>
<td><p>degradation constant</p></td>
</tr>
<tr class="even">
<td><code class="parameter-table-variable">muNot</code></td>
<td><p>dilatancy coefficient (monotonic loading)</p></td>
</tr>
<tr class="odd">
<td><code class="parameter-table-variable">muCyc</code></td>
<td><p>dilatancy coefficient (cyclic loading)</p></td>
</tr>
<tr class="even">
<td><code class="parameter-table-variable">sc</code></td>
<td><p>dilatancy strain</p></td>
</tr>
<tr class="odd">
<td><code class="parameter-table-variable">M</code></td>
<td><p>critical state stress ratio</p></td>
</tr>
<tr class="even">
<td><code class="parameter-table-variable">patm</code></td>
<td><p>atmospheric pressure (in appropriate units)</p></td>
</tr>
</tbody>
</table>
<p>Optional steady state line parameters (default values shown for each,
be careful with units)</p>
<table>
<tbody>
<tr class="odd">
<td><p><code>ssl1</code></p></td>
<td><p>void ratio of quasi steady state (QSS-line) at pressure $pmin
(default = 0.877)</p></td>
</tr>
<tr class="even">
<td><p><code>ssl2</code></p></td>
<td><p>void ratio of quasi steady state (QSS-line) at 10 kPa (default =
0.877)</p></td>
</tr>
<tr class="odd">
<td><p><code>ssl3</code></p></td>
<td><p>void ratio of quasi steady state (QSS-line) at 30 kPa (default =
0.873)</p></td>
</tr>
<tr class="even">
<td><p><code>ssl4</code></p></td>
<td><p>void ratio of quasi steady state (QSS-line) at 50 kPa (default =
0.870)</p></td>
</tr>
<tr class="odd">
<td><p><code>ssl5</code></p></td>
<td><p>void ratio of quasi steady state (QSS-line) at 100 kPa (default =
0.860)</p></td>
</tr>
<tr class="even">
<td><p><code>ssl6</code></p></td>
<td><p>void ratio of quasi steady state (QSS-line) at 200 kPa (default =
0.850)</p></td>
</tr>
<tr class="odd">
<td><p><code>ssl7</code></p></td>
<td><p>void ratio of quasi steady state (QSS-line) at 400 kPa (default =
0.833)</p></td>
</tr>
<tr class="even">
<td><p><code>hsl</code></p></td>
<td><p>void ratio of upper reference state (UR-line) for all pressures
(default = 0.895)</p></td>
</tr>
<tr class="odd">
<td><p><code>pmin</code></p></td>
<td><p>pressure corresponding to $ssl1 (default = 1.0 kPa)</p></td>
</tr>
</tbody>
</table>
<p>The material formulations for the stressDensity object are
"PlaneStrain"</p>
<hr />
<h2 id="general_information">General Information</h2>
<p>This nDMaterial object provides the stress density model for sands
under monotonic and cyclic loading as set forth by Cubrinovski and
Ishihara (1998a,b). The original formulation for this model was
applicable to plane strain conditions and this is the only currently
available formulation.</p>
<h3 id="notes">Notes</h3>

## Examples

The following example provides the input parameters for dry
pluviated Toyura sand (with initial void ratio e = 0.73) after
Cubrinovski and Ishihara (1998b). The units of this analysis are Mg, kN,
s, and m.

```tcl
# mass density
set mDen 1.8

# atmospheric pressure
set patm 98.1

# stress density model parameters
set eNot 0.730
set A 250.0
set n  0.60
set a1 0.58
set b1 0.023
set a2 230.0
set b2 65.0
set a3 79.0
set b3 16.0
set fd 4.0
set muNot 0.22
set muCyc 0.0
set sc 0.0055
set M 0.607 

nDMaterial stressDensity 1 $mDen \
  $eNot $A $n $nu $a1 $b1 $a2 $b2 $a3 $b3 $fd $muNot $ muCyc $sc $M $patm
```

### Example Analysis

Element test with pure shear loading starting from isotropic initial
state of stress.

<details><summary>Tcl Script</summary>

```tcl
# intended number of cycles in the test
set nCycles 120

# shear strain increment for the test

set dg 0.0001
set wg [expr 2.0*$dg]

# max number of steps
set maxStep 20000

# initial confinement pressure (kPa)
set pNot -95.0

# max/min shear stress in the test (kPa)
set CSR 0.2 
set maxShear [expr -$CSR*$pNot]
wipe
model BasicBuilder -ndm 2 -ndf 2

# Create nodes

node 1 0.0 0.0 
node 2 1.0 0.0 
node 3 1.0 1.0 
node 4 0.0 1.0

# Create fixities
fix 1 1 1 
fix 2 1 1 
fix 3 1 1 
fix 4 1 1

# atmospheric pressure
set patm 98.1

# mass density
set mDen 1.8

# steady state line void ratio
set ssl1 0.832 
set ssl2 0.832 
set ssl3 0.810 
set ssl4 0.796 
set ssl5 0.776 
set ssl6 0.756
set ssl7 0.735

# hydrostatic state line void ratio
set hsl 0.852

# reference pressures for state lines
set p1 1.0

# stress density model parameters
set A 250.0
set m 0.60
set nu 0.20
set a1 0.592
set b1 0.021
set a2 291.0
set b2 55.0
set a3 98.0
set b3 13.0
set fd 4.0
set muNot 0.15
set sc 0.0055
set M 0.607

# initial void ratio
set emax 0.885
set emin 0.541
set Dr 0.54
set eNot [expr $emax - $Dr*($emax-$emin)]
set muCyc 0.0

# Create material

nDMaterial stressDensity 2 $mDen $eNot $A $m $nu \
    $a1 $b1 $a2 $b2 $a3 $b3 $fd $muNot $muCyc \
    $sc $M $patm $ssl1 $ssl2 $ssl3 $ssl4 $ssl5 $ssl6 $ssl7 $hsl $p1

nDMaterial InitStress 1 2 $pNot 2

# Create element

element SSPquad 1 1 2 3 4 1 PlaneStrain 1.0 0.0 0.0

# Create recorders
recorder Element -file stress.out -time stress 
recorder Element -file strain.out -time strain 
recorder Node -file disp.out -time -dof 1 2 disp
set dt 0.1

# Create analysis
constraints Penalty 1.0e18 1.0e18 
algorithm Linear 
numberer RCM
system ProfileSPD 
integrator LoadControl $dt 
analysis Static
set dMax [expr 0.6/$wg] 
eval "timeSeries Path 400 -time {0 0.1 0.2 300.2} -values {0 0 0 $dMax} -factor 1.0" 
pattern Plain 400 400 { 
  sp 3 1 $wg 
  sp 4 1 $wg 
} 
analyze 1
setParameter -value 1 -ele 1 materialState
analyze 1

# counter for max number of steps

set count 0 
set cCount 0 
set cyc 1 puts "Beginning of Cycle 1"

# loop through the total number of cycles

for {set i 1} {$i <= [expr 2*$nCycles]} {incr i} { 
  if {$cCount == 2} { 
    set cyc [expr $cyc+1] 
    puts "Beginning of Cycle $cyc" 
    set cCount 0
  }

  # loop within each cycle
  for {set j 1} {$j < 5000} {incr j} {

    # abort if count is greater than max number of steps 
    if {$count &gt;= $maxStep} {break}
    
    # analyze single step and get the current stress 
    analyze 1 
    set count [expr $count + 1]
    
    # get stress from element  
    set stress [eleResponse 1 stress]
    
    # shear stress is component 2
    set tau [lindex $stress 2]

    # signal change in loading direction if needed
    if {[expr abs($tau)] >= $maxShear} {

      # get strain from element 
      set strain [eleResponse 1 strain] 
      set gamma [lindex $strain 2] 
      puts "direction change required: tau = $tau; gamma = $gamma"
      
      # get current displacements of shearing nodes
      set f [expr 2.0*[nodeDisp 3 1]] 
      set b [expr 2.0*[nodeDisp 4 1]]
      
      # puts "current displacement of front row is $f"
      # puts "current displacement of back row is $b"
      
      # get number of steps required to reach current disp from zero
      set nStep [expr round(abs($b/$wg))]
      
      # puts "there are $nStep steps needed to get back to neutral loading"

      # get current time
      set cTime [getTime]

      # puts "current time is $cTime"
      # set an end time for the load patterns

      set zTime [expr $cTime + $nStep*$dt] 
      set eTime [expr $zTime + 100.0*$nStep]

      # puts "end time for the new load pattern is $eTime"

      remove loadPattern [expr 400+$i-1]
      eval "timeSeries Path [expr 400+$i] -time {$cTime $eTime 1e10} -values {1 -1000 -1000}" 
      pattern Plain [expr 400+$i] [expr 400+$i] { 
        sp 3 1 $b sp 4 1 $b 
      } 
      set cCount [expr $cCount + 1] break 
    } 
  }
}
wipe 
```

</details>


## References
<p>Cubrinovski, M. and Ishihara K. (1998a) 'Modelling of sand behaviour
based on state concept,' <em>Soils and Foundations,</em> 38(3),
115-127.</p>
<p>Cubrinovski, M. and Ishihara K. (1998b) 'State concept and modified
elastoplasticity for sand modelling,' <em>Soils and Foundations,</em>
38(4), 213-225.</p>

<hr />

<p>Code developed by: <span style="color:blue">Saumyashuchi Das</span>, University of Canterbury.
Maintained by: <a
href="http://www.civil.canterbury.ac.nz/staff/cmcgann.shtml">Chris McGann</a></p>
