/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
** ****************************************************************** */
//
// Description: This file contains the class definition for DispBeamColumnNL3d.
// The element displacement field gives rise to constant axial strain and
// linear curvature.
//
// Written: MHS
// Created: Feb 2001
//
#ifndef DispBeamColumnNL3d_h
#define DispBeamColumnNL3d_h

#ifndef _bool_h
#include <stdbool.h>
#endif

#include <Element.h>
#include <Matrix.h>
#include <Vector.h>
#include <ID.h>
#include <BeamIntegration.h>

class Node;
class SectionForceDeformation;
class CrdTransf;
class Response;

class DispBeamColumnNL3d : public Element
{
  public:
    DispBeamColumnNL3d(int tag, int nd1, int nd2,
                     int numSections, SectionForceDeformation **s,
                     BeamIntegration &bi, CrdTransf &coordTransf,
                     double rho = 0.0);
    DispBeamColumnNL3d();
    ~DispBeamColumnNL3d();

    const char *getClassType() const {return "DispBeamColumnNL3d";};
    static constexpr const char* class_name = "DispBeamColumnNL3d";

    int getNumExternalNodes() const;
    const ID &getExternalNodes();
    Node **getNodePtrs();

    int getNumDOF();
    void setDomain(Domain *theDomain);

    // public methods to set the state of the element    
    int commitState();
    int revertToLastCommit();
    int revertToStart();

    // public methods to obtain stiffness, mass, damping and residual information    
    int update();
    const Matrix &getTangentStiff();
    const Matrix &getInitialStiff();
    const Matrix &getMass();

    void zeroLoad();
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector &getResistingForce();
    const Vector &getResistingForceIncInertia();            

    // public methods for element output
    int sendSelf(int commitTag, Channel &theChannel);
    int recvSelf(int commitTag, Channel &theChannel, FEM_ObjectBroker 
                  &theBroker);

    void Print(OPS_Stream &s, int flag =0);

    Response *setResponse(const char **argv, int argc, OPS_Stream &s);
    int getResponse(int responseID, Information &eleInfo);
    int getResponseSensitivity(int responseID, int gradNumber,
                               Information &eleInformation);

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    int setParameter(const char **argv, int argc, Parameter &param);
    int            updateParameter(int parameterID, Information &info);
    int            activateParameter(int parameterID);
    const Vector & getResistingForceSensitivity(int gradNumber);
    const Matrix & getInitialStiffSensitivity(int gradNumber);
    const Matrix & getMassSensitivity(int gradNumber);
    int            commitSensitivity(int gradNumber, int numGrads);
    // AddingSensitivity:END ///////////////////////////////////////////

  protected:
    
  private:
    const Matrix &getInitialBasicStiff();
    void getBasicStiff(Matrix &kb, int initial = 0);

    int numSections;
    SectionForceDeformation **theSections; // pointer to the ND material objects
    CrdTransf *crdTransf;        // pointer to coordinate transformation object 

    BeamIntegration *beamInt;

    ID connectedExternalNodes; // Tags of quad nodes

    Node *theNodes[2];

    static Matrix K;                // Element stiffness, damping, and mass Matrix
    static Vector P;                // Element resisting force vector

    Vector Q;                // Applied nodal loads
    Vector q;                // Basic force
    double q0[5];  // Fixed end forces in basic system
    double p0[5];  // Reactions in basic system

    double rho;                        // Mass density per unit length

    enum {maxNumSections = 20};

    static double workArea[];

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    int parameterID;
    // AddingSensitivity:END ///////////////////////////////////////////
};

#endif

