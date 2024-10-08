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
// Description: This file contains the class definition for 
// NDFiberSection3d.h. NDFiberSection3d provides the abstraction of a 
// 2d beam section discretized by fibers. The section stiffness and
// stress resultants are obtained by summing fiber contributions.
//
// Written: MHS
// Created: 2012
//
#ifndef NDFiberSection3d_h
#define NDFiberSection3d_h

#include <FrameSection.h>
#include <Vector.h>
#include <Matrix.h>

class NDMaterial;
class Response;

class NDFiberSection3d : public FrameSection
{
  public:
    NDFiberSection3d(); 
    NDFiberSection3d(int tag, int numFibers, double a = 1.0, bool compCentroid=true);
    ~NDFiberSection3d();

    const char *getClassType(void) const {return "NDFiberSection3d";};

    int   setTrialSectionDeformation(const Vector &deforms); 
    const Vector &getSectionDeformation(void);

    const Vector &getStressResultant(void);
    const Matrix &getSectionTangent(void);
    const Matrix &getInitialTangent(void);

    int   commitState(void);
    int   revertToLastCommit(void);    
    int   revertToStart(void);
 
    FrameSection *getFrameCopy();
    const ID &getType();
    int getOrder (void) const;
    
    int sendSelf(int cTag, Channel &theChannel);
    int recvSelf(int cTag, Channel &theChannel, 
		 FEM_ObjectBroker &theBroker);
    void Print(OPS_Stream &s, int flag = 0);
	    
    Response *setResponse(const char **argv, int argc, 
			  OPS_Stream &s);
    int getResponse(int responseID, Information &info);

    int addFiber(NDMaterial& theMat, const double Area, const double yLoc, const double zLoc);

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    int setParameter(const char **argv, int argc, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);
    const Vector& getStressResultantSensitivity(int gradIndex,
						bool conditional);
    const Vector& getSectionDeformationSensitivity(int gradIndex);
    const Matrix& getInitialTangentSensitivity(int gradIndex);
    int commitSensitivity(const Vector& sectionDeformationGradient,
			  int gradIndex, int numGrads);
    // AddingSensitivity:END ///////////////////////////////////////////

  protected: 
    //  private:
    int numFibers, sizeFibers;        // number of fibers in the section
    NDMaterial **theMaterials;        // array of pointers to materials
    double   *matData;                // data for the materials [yloc and area]
    double   kData[36];               // data for ks matrix 
    double   sData[6];                // data for s vector 

    double Abar,QyBar, QzBar;
    double yBar;                      // Section centroid
    double zBar;                      // Section centroid
    bool computeCentroid;
    double alpha;      // Shear shape factor


    static ID code;

    Vector e;          // trial section deformations 
    Vector *s;         // section resisting forces  (axial force, bending moment)
    Matrix *ks;        // section stiffness

// AddingSensitivity:BEGIN //////////////////////////////////////////
    int parameterID;
    Vector dedh; // MHS hack
// AddingSensitivity:END ///////////////////////////////////////////
};

#endif
