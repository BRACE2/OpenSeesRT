/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
// 
// Written: MHS
// Created: Feb 2000
// Revised: Dec 2000 for efficiency
//
// Description: This file contains the class definition for FourNodeQuad.
//
#ifndef FourNodeQuad_h
#define FourNodeQuad_h

#include <Element.h>
#include <Matrix.h>
#include <Vector.h>
#include <ID.h>
#include <quadrature/GaussLegendre2D.hpp>

class Node;
class NDMaterial;
class Response;

class FourNodeQuad : public Element,
                     protected GaussLegendre<2,4>
{
  public:
    FourNodeQuad(int tag, int nd1, int nd2, int nd3, int nd4,
		 NDMaterial &m, const char *type,
		 double t, double pressure = 0.0, 
		 double rho = 0.0,
		 double b1 = 0.0, double b2 = 0.0);

    FourNodeQuad();
    ~FourNodeQuad();

    const char *getClassType(void) const {return "FourNodeQuad";};
    static constexpr const char* class_name = "FourNodeQuad";

    int getNumExternalNodes(void) const;
    const ID &getExternalNodes(void);
    Node **getNodePtrs(void);

    int getNumDOF(void);
    void setDomain(Domain *theDomain);

    // public methods to set the state of the element    
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    int update(void);

    // public methods to obtain stiffness, mass, damping and residual information    
    const Matrix &getTangentStiff(void);
    const Matrix &getInitialStiff(void);    
    const Matrix &getMass(void);    

    void zeroLoad();
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector &getResistingForce(void);
    const Vector &getResistingForceIncInertia(void);            

    // Public methods for element output

    Response *setResponse(const char **argv, int argc, OPS_Stream &s);
    int getResponse(int responseID, Information &eleInformation);

    int setParameter(const char **argv, int argc, Parameter &param);
    int updateParameter(int parameterID, Information &info);

    // Inherited from TaggedObject
    void Print(OPS_Stream &s, int flag =0);

    // Inherited from MovableObject
    int sendSelf(int commitTag, Channel &theChannel);
    int recvSelf(int commitTag, Channel &theChannel, FEM_ObjectBroker 
		  &theBroker);

    // RWB; PyLiq1 & TzLiq1 need to see the excess pore pressure and initial stresses.
    friend class PyLiq1;
    friend class TzLiq1;
    friend class QzLiq1; // Sumeet

  protected:
    
  private:
    constexpr static int NDM = 2;    // number of spatial dimensions
    constexpr static int NEN = 4;    // number of nodes
    constexpr static int NDF = 2;    // number of DOFs per node

    // private attributes - a copy for each object of the class

    NDMaterial **theMaterial; // pointer to the ND material objects
    
    ID connectedExternalNodes; // Tags of quad nodes

    Node *theNodes[NEN];

    static double matrixData[64];  // array data for matrix
    static Matrix K;		// Element stiffness, damping, and mass Matrix
    static Vector P;		// Element resisting force vector
    Vector Q;		        // Applied nodal loads
    double b[2];		// Body forces

    double appliedB[2]; // Body forces applied with load pattern, C.McGann, U.Washington
    int applyLoad;      // flag for body force in load, C.McGann, U.Washington
	
    Vector pressureLoad;	// Pressure load at nodes

    double thickness;	        // Element thickness
    double pressure;	        // Normal surface traction (pressure) over entire element
					 // Note: positive for outward normal
    double rho;
    static double shp[3][NEN];	// Stores shape functions and derivatives (overwritten)

    // private member functions - only objects of this class can call these
    double shapeFunction(double xi, double eta);
    void setPressureLoadAtNodes(void);

    Matrix *Ki;
};

#endif

