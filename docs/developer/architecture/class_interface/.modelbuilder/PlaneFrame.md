


\#include  `</modelbuilder/PlaneFrame.h>`



class PlaneFrame: public ModelBuilder;



ModelBuilder






The PlaneFrame class is used to construct 2d plane frame models from an
input file of specified format.

// Constructor






// Destructor






// Public Methods







The *theDomain* object is used by the ModelBuilder classes constructor.




This does nothing. It is the responsibility of the Domain object to
delete all the domain components when the destructor is called on that
object.




The PlaneFrame will construct the Element, Node, Load and Constraint
objects and add them to the Domain object associated with the
PlaneFrame. To do this the PlaneFrame object will prompt the user for
the name of the input file; if the file cannot be opened an error
message is printed and the program terminates. A sample input file is
given below:


3 3 1 2


3 0.0 0.0


3 0.0 10.0


3 20.0 10.0


3 20.0 0.0


1 10.0 200000.0 100.0 1 2


2 20.0 200000.0 100.0 2 3


3 10.0 200000.0 100.0 3 4


0 0.0


1 0.0


2 0.0


4 2 3


1


1 2


1.0 0.0


0.0 2.0


10.0 0.0 0.0


10.0 10.0 0.0


line 1 contains the number of nodes (4), elements (3), single point
constraints (3), multiple point constraints(1) and number of nodal loads
respectively (2). The next 4 lines contains the nodal data; for each
line a Node object is constructed using the tag and, the number of dof,
and the x and y coords specified. The next 3 lines contains the
elemental data; for each line a beam object is created, the type of beam
object depending on the first integer value of each line (2 == beam2d02,
3 == beam2d03), each beam takes a tag, A,E,I and the tags of the nodes
at end1 and end2.

The next 3 lines contains the single point constraint data; for each
line a SP_Constraint object is created (constrainedNode, constrainedDOF,
value).

The next 5 lines contain the data for the multipoint constraint, the
first line identifies the tag of the constrained node(3), the tag of the
retained node(4) and the number of dof in the relationship for each node
(2 for node 3 and 3 for node 4). The next line contains the
degrees-of-freedom in the constrained node $U_c$, (0 1), the next lines
the degrees-of-freedom in the retained node, $U_r$, (0 1 2) and the next
two lines the $C_{cr}$ matrix defining the relationship
$U_c = C_{cr} U_r$.

whose tag is 5 which has two load cases. The next two lines contain the
NodalLoad information; for each line a NodalLoad is created which acts
on the node specified with the forces xForce, yForce and moment. To
returns 0 if successful, -1 otherwise.
