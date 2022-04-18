// Written: cmp
//
// Description: This file contains the class definition for TclSafeBuilder.
// A TclSafeBuilder adds the commands to create the model for the standard
// models that can be generated using the elements released with the g3
// framework.
#include <modeling/commands.h>
#include <g3_api.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <Matrix.h>
#include <Vector.h>
#include <ID.h>
#include <ArrayOfTaggedObjects.h>
#include <MapOfTaggedObjects.h>

#include <Domain.h>
#include <Node.h>
#include <NodeIter.h>

#include <RigidRod.h>
#include <RigidBeam.h>
#include <RigidDiaphragm.h>

#include <CrdTransf.h>

#include <NodalLoad.h>
#include <Beam2dPointLoad.h>
#include <Beam2dUniformLoad.h>
#include <Beam2dPartialUniformLoad.h>
#include <Beam2dTempLoad.h>
#include <Beam2dThermalAction.h>  //L.Jiang [SIF]
#include <Beam3dThermalAction.h>  //L.Jiang [SIF]
#include <ShellThermalAction.h>   //L.Jiang [SIF]
#include <ThermalActionWrapper.h> //L.Jiang [SIF]
#include <NodalThermalAction.h>   //L.Jiang [SIF]

#include <Beam3dPointLoad.h>
#include <Beam3dUniformLoad.h>
#include <Beam3dPartialUniformLoad.h>
#include <BrickSelfWeight.h>
#include <SurfaceLoader.h>
#include <SelfWeight.h>
#include <LoadPattern.h>

#include <SectionForceDeformation.h>
#include <SectionRepres.h>

#include <UniaxialMaterial.h>
#include <LimitCurve.h>
#include <NDMaterial.h>
#include <TclSafeBuilder.h>
#include <ImposedMotionSP.h>
#include <ImposedMotionSP1.h>
#include <MultiSupportPattern.h>

#include <TimeSeries.h>
#include <PathTimeSeriesThermal.h> //L.Jiang [SIF]
#include <vector>                  //L.Jiang [SIF]
using std::vector;                 // L.Jiang [SIF]
/*
#include <SimulationInformation.h>				//L.Jiang [SIF]
extern SimulationInformation simulationInfo;		//L.Jiang [SIF]
*/
extern const char * getInterpPWD(Tcl_Interp *interp);  //L.Jiang [SIF]

/*--------------------------------------------------------------------

// Added by Scott J. Brandenberg (sjbrandenberg@ucdavis.edu)
#include <PySimple1Gen.h>
#include <TzSimple1Gen.h>
// End added by SJB

// Added by Prishati Raychowdhury  (PRC)
#include <ShallowFoundationGen.h>
//end PRC

#include <YieldSurface_BC.h>
#include <YS_Evolution.h>

#include <HystereticBackbone.h>
#include <BeamIntegration.h>
*/

//////// gnp adding damping ////////////////
#include <Element.h>
////////////////////////////////////////////

/*
extern void TCL_OPS_setModelBuilder(TclSafeBuilder *theNewBuilder);
extern int OPS_ResetInput(ClientData clientData,
                          Tcl_Interp *interp,
                          int cArg,
                          int mArg,
                          TCL_Char **argv,
                          Domain *domain,
                          TclBuilder *builder);
#include <packages.h>
*/


//
// CLASS CONSTRUCTOR & DESTRUCTOR
//
// constructor: the constructor will add certain commands to the interpreter
TclSafeBuilder::TclSafeBuilder(Domain &theDomain, Tcl_Interp *interp, int NDM,
                               int NDF)
    : TclBuilder(theDomain, NDM, NDF), theInterp(interp)
{
  /*
  theSections = new ArrayOfTaggedObjects(32);
  theUniaxialMaterials = new MapOfTaggedObjects();
  theSectionRepresents = new ArrayOfTaggedObjects(32);
  */

  // theYieldSurface_BCs = new ArrayOfTaggedObjects(32);
  // theCycModels = new ArrayOfTaggedObjects(32); //!!
  // theYS_EvolutionModels = new ArrayOfTaggedObjects(32);
  // thePlasticMaterials = new ArrayOfTaggedObjects(32);

  static int ncmd = sizeof(tcl_char_cmds)/sizeof(char_cmd);

  for (int i = 0; i < ncmd; i++)
    Tcl_CreateCommand(interp, 
        tcl_char_cmds[i].name, 
        tcl_char_cmds[i].func, 
        (ClientData)NULL, NULL);
 
  theTclBuilder = this;
  theTclDomain = &theDomain;
  tclEnclosingPattern = 0;
  theTclMultiSupportPattern = 0;

  nodeLoadTag = 0;
  eleArgStart = 0;
  m_runtime = G3_getRuntime(interp);

  Tcl_SetAssocData(interp, "OPS::theTclBuilder", NULL, (ClientData)this);
  Tcl_SetAssocData(interp, "OPS::theTclSafeBuilder", NULL, (ClientData)this);
  G3_setDomain(m_runtime, &theDomain);
  Tcl_SetAssocData(interp, "OPS::theTclDomain", NULL, (ClientData)&theDomain);
}

TclSafeBuilder::~TclSafeBuilder()
{
  /*
  OPS_clearAllTimeSeries();
  OPS_clearAllUniaxialMaterial();
  OPS_clearAllNDMaterial();
  OPS_clearAllSectionForceDeformation();
  OPS_clearAllCrdTransf();
  */
  // OPS_clearAllHystereticBackbone();
  // OPS_clearAllFrictionModel();
  // OPS_clearAllLimitCurve();
  // OPS_clearAllDamageModel();
  // theYieldSurface_BCs->clearAll();
  // theYS_EvolutionModels->clearAll();
  // thePlasticMaterials->clearAll();
  // theCycModels->clearAll(); //!!

/*
  theSections->clearAll();
  theSectionRepresents->clearAll();
  // free up memory allocated in the constructor
  delete theSections;
  delete theSectionRepresents;
*/
  // set the pointers to 0
  theTclDomain = 0;
  theTclBuilder = 0;
  tclEnclosingPattern = 0;
  theTclMultiSupportPattern = 0;
  /* TCL_OPS_setModelBuilder(0); */

  // may possibly invoke Tcl_DeleteCommand() later
  Tcl_DeleteCommand(theInterp, "node");
  Tcl_DeleteCommand(theInterp, "element");
  Tcl_DeleteCommand(theInterp, "uniaxialMaterial");
  Tcl_DeleteCommand(theInterp, "nDMaterial");
  Tcl_DeleteCommand(theInterp, "section");
  Tcl_DeleteCommand(theInterp, "pattern");
  Tcl_DeleteCommand(theInterp, "timeSeries");
  Tcl_DeleteCommand(theInterp, "load");
}

//
// CLASS METHODS
/*
int TclSafeBuilder::getNDM(void) const {return ndm;}
int TclSafeBuilder::getNDF(void) const {return ndf;}
int TclSafeBuilder::buildFE_Model(void) {return 0;}
*/
int TclSafeBuilder::incrNodalLoadTag(void){return ++nodeLoadTag;};
int TclSafeBuilder::decrNodalLoadTag(void){return --nodeLoadTag;};
int TclSafeBuilder::getNodalLoadTag(void) const {return   nodeLoadTag;};

LoadPattern *
TclSafeBuilder::getEnclosingPattern(void) const {return tclEnclosingPattern;};

int
TclSafeBuilder::setEnclosingPattern(LoadPattern* pat){
  tclEnclosingPattern = pat;
  return 1;
};

Domain *
TclSafeBuilder::getDomain(void) const {return theTclDomain;}

TclSafeBuilder *
TclSafeBuilder::getBuilder(void) const {return theTclBuilder;}

int
TclSafeBuilder::addUniaxialMaterial(UniaxialMaterial *mat)
{
  return this->addUniaxialMaterial(*mat);
}

/*
UniaxialMaterial *
TclSafeBuilder::getUniaxialMaterial(int tag)
{
  TaggedObject *mc = theUniaxialMaterials->getComponentPtr(tag);
  if (mc == 0)
    return 0;

  // otherwise cast and return
  UniaxialMaterial *result = (UniaxialMaterial *)mc;
  return result;
}

int
TclSafeBuilder::addNDMaterial(NDMaterial &theMaterial)
{
  bool result = theNDMaterials->addComponent(&theMaterial);
  if (result == true)
    return 0;
  else {
    opserr << "TclSafeBuilder::addNDMaterial() - failed to add material:" << theMaterial; return -1;
  }
}

NDMaterial *
TclSafeBuilder::getNDMaterial(int tag)
{
  TaggedObject *mc = theNDMaterials->getComponentPtr(tag);
  if (mc == 0)
    return 0;

  // otherwise cast and return
  NDMaterial *result = (NDMaterial *)mc;
  return result;
}


int
TclSafeBuilder::addSection(SectionForceDeformation &theSection)
{
  //  bool result = theSections->addComponent(&theSection);
  // bool result = OPS_addSectionForceDeformation(&theSection);
  if (result == true)
    return 0;

  else {
    opserr << "TclSafeBuilder::addSection() - failed to add section: "
           << theSection;

  return -1;
  }
}

SectionForceDeformation *
TclSafeBuilder::getSection(int tag)
{

  // return OPS_getSectionForceDeformation(tag);
  TaggedObject *mc = theSections->getComponentPtr(tag);
  if (mc == 0)
    return 0;

  // do a cast and return
  SectionForceDeformation *result = (SectionForceDeformation *)mc;
  return result;
}
*/

/*
int
TclSafeBuilder::addYS_EvolutionModel(YS_Evolution &theModel)
{
  bool result = theYS_EvolutionModels->addComponent(&theModel);
  if (result == true)
    return 0;
  else {
    opserr << "TclSafeBuilder::addYS_EvolutionModel() - failed to add
model " << theModel; return -1;
  }
}


YS_Evolution *
TclSafeBuilder::getYS_EvolutionModel(int tag)
{
  TaggedObject *mc = theYS_EvolutionModels->getComponentPtr(tag);
  if (mc == 0)
    return 0;

  // otherweise we do a cast and return
  YS_Evolution *result = (YS_Evolution *)mc;
  return result;
}

int
TclSafeBuilder::addYieldSurface_BC(YieldSurface_BC &theYS)
{
//	TaggedObject *mc = &theYS;

  bool result = theYieldSurface_BCs->addComponent(&theYS);
  if (result == true)
    return 0;
  else {
    opserr << "TclSafeBuilder::addYieldSurfaceBC() - failed to add YS: "
<< theYS; return -1;
  }
}

int //!!
TclSafeBuilder::addCyclicModel(CyclicModel &theCM)
{
//	TaggedObject *mc = &theYS;

  bool result = theCycModels->addComponent(&theCM);
  if (result == true)
    return 0;
  else {
    opserr << "TclSafeBuilder::addCyclicModel() - failed to add : " <<
theCM; return -1;
  }
}

YieldSurface_BC *
TclSafeBuilder::getYieldSurface_BC(int tag)
{
  TaggedObject *mc = theYieldSurface_BCs->getComponentPtr(tag);
  if (mc == 0)
    return 0;

  // otherweise we do a cast and return
  YieldSurface_BC *result = (YieldSurface_BC *)mc;
  return result;
}

CyclicModel * //!!
TclSafeBuilder::getCyclicModel(int tag)
{
  TaggedObject *mc = theCycModels->getComponentPtr(tag);
  if (mc == 0)
    return 0;

  // otherweise we do a cast and return
  CyclicModel *result = (CyclicModel *)mc;
  return result;
}

int
TclSafeBuilder::addPlasticMaterial(PlasticHardeningMaterial &theMat)
{
//	TaggedObject *mc = &theYS;

  bool result = thePlasticMaterials->addComponent(&theMat);
  if (result == true)
    return 0;
  else {
    opserr << "TclSafeBuilder::addPlasticMaterial() - failed to add
Material: " << theMat; return -1;
  }
}

PlasticHardeningMaterial *
TclSafeBuilder::getPlasticMaterial(int tag)
{
  TaggedObject *mc = thePlasticMaterials->getComponentPtr(tag);
  if (mc == 0)
    return 0;

  // otherweise we do a cast and return
  PlasticHardeningMaterial *result = (PlasticHardeningMaterial *)mc;
  return result;
}

int
TclSafeBuilder::addSectionRepres(SectionRepres &theSectionRepres)
{
  bool result = theSectionRepresents->addComponent(&theSectionRepres);

  if (result == true)
    return 0;
  else {
      opserr << "TclSafeBuilder::addSectionRepres() - failed to add SectionRepres\n";
      return -1;
  }
}

SectionRepres *
TclSafeBuilder::getSectionRepres(int tag)
{
  TaggedObject *mc = theSectionRepresents->getComponentPtr(tag);
  if (mc == 0) return 0;
  SectionRepres *result = (SectionRepres *)mc;
  return result;
}
*/

TimeSeries *
TclSafeBuilder::getTimeSeries(const std::string &name)
{
  TimeSeries *series = m_TimeSeriesMap.at(name);
  if (series)
    return series->getCopy();
  else
    return 0;
}

int
TclSafeBuilder::addTimeSeries(const std::string &name, TimeSeries *series)
{
  m_TimeSeriesMap[name] = series;
  return 1;
}

int
TclSafeBuilder::addTimeSeries(TimeSeries *series)
{
  const std::string &name = std::to_string(series->getTag());
  m_TimeSeriesMap[name] = series;
  return 1;
}

//
// THE FUNCTIONS INVOKED BY THE INTERPRETER
//

static void
printCommand(int argc, TCL_Char **argv)
{
  opserr << "Input command: ";
  for (int i = 0; i < argc; i++)
    opserr << argv[i] << " ";
  opserr << endln;
}

static int
TclCommand_addNode(ClientData clientData, Tcl_Interp *interp, int argc,
                   TCL_Char **argv)
{
  G3_Runtime *rt = G3_getRuntime(interp);
  TclSafeBuilder *theTclBuilder = G3_getSafeBuilder(rt);
  Domain *theTclDomain = G3_getDomain(rt);

  // ensure the destructor has not been called -
  if (theTclBuilder == 0) {
    opserr << "WARNING builder has been destroyed" << endln;
    return TCL_ERROR;
  }


  int ndm = G3_getNDM(rt);
  int ndf = G3_getNDF(rt);

  // make sure corect number of arguments on command line
  if (argc < 2 + ndm) {
    opserr << "WARNING insufficient arguments\n";
    printCommand(argc, argv);
    opserr << "Want: node nodeTag? [ndm coordinates?] <-mass [ndf values?]>\n";
    return TCL_ERROR;
  }

  Node *theNode = 0;

  // get the nodal id
  int nodeId;
  if (Tcl_GetInt(interp, argv[1], &nodeId) != TCL_OK) {
    opserr << "WARNING invalid nodeTag\n";
    opserr << "Want: node nodeTag? [ndm coordinates?] <-mass [ndf values?]>\n";
    return TCL_ERROR;
  }

  // read in the coordinates and create the node
  double xLoc, yLoc, zLoc;
  if (ndm == 1) {
    // create a node in 1d space
    if (Tcl_GetDouble(interp, argv[2], &xLoc) != TCL_OK) {
      opserr << "WARNING invalid XCoordinate\n";
      opserr << "node: " << nodeId << endln;
      return TCL_ERROR;
    }
    //    theNode = new Node(nodeId,ndf,xLoc);
  }

  else if (ndm == 2) {
    // create a node in 2d space
    if (Tcl_GetDouble(interp, argv[2], &xLoc) != TCL_OK) {
      opserr << "WARNING invalid XCoordinate\n";
      opserr << "node: " << nodeId << endln;
      return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[3], &yLoc) != TCL_OK) {
      opserr << "WARNING invalid YCoordinate\n";
      opserr << "node: " << nodeId << endln;
      return TCL_ERROR;
    }
    //    theNode = new Node(nodeId,ndf,xLoc,yLoc);
  }

  else if (ndm == 3) {
    // create a node in 3d space
    if (Tcl_GetDouble(interp, argv[2], &xLoc) != TCL_OK) {
      opserr << "WARNING invalid XCoordinate\n";
      opserr << "node: " << nodeId << endln;
      return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[3], &yLoc) != TCL_OK) {
      opserr << "WARNING invalid YCoordinate\n";
      opserr << "node: " << nodeId << endln;
      return TCL_ERROR;
    }
    if (Tcl_GetDouble(interp, argv[4], &zLoc) != TCL_OK) {
      opserr << "WARNING invalid ZCoordinate\n";
      opserr << "node: " << nodeId << endln;
      return TCL_ERROR;
    }
    //    theNode = new Node(nodeId,ndf,xLoc,yLoc,zLoc);
  } else {
    opserr << "WARNING invalid ndm\n";
    opserr << "node: " << nodeId << endln;
    ;
    return TCL_ERROR;
  }

  // check for -ndf override option
  int currentArg = 2 + ndm;
  if (currentArg < argc && strcmp(argv[currentArg], "-ndf") == 0) {
    if (Tcl_GetInt(interp, argv[currentArg + 1], &ndf) != TCL_OK) {
      opserr << "WARNING invalid nodal ndf given for node " << nodeId << endln;
      return TCL_ERROR;
    }
    currentArg += 2;
  }

  //
  // create the node
  //

  if (ndm == 1)
    theNode = new Node(nodeId, ndf, xLoc);
  else if (ndm == 2)
    theNode = new Node(nodeId, ndf, xLoc, yLoc);
  else
    theNode = new Node(nodeId, ndf, xLoc, yLoc, zLoc);

  //
  // add the node to the domain
  //

  if (theTclDomain->addNode(theNode) == false) {
    opserr << "WARNING failed to add node to the domain\n";
    opserr << "node: " << nodeId << endln;
    delete theNode; // otherwise memory leak
    return TCL_ERROR;
  }

  while (currentArg < argc) {
    if (strcmp(argv[currentArg], "-mass") == 0) {
      currentArg++;
      if (argc < currentArg + ndf) {
        opserr << "WARNING incorrect number of nodal mass terms\n";
        opserr << "node: " << nodeId << endln;
        return TCL_ERROR;
      }
      Matrix mass(ndf, ndf);
      double theMass;
      for (int i = 0; i < ndf; i++) {
        if (Tcl_GetDouble(interp, argv[currentArg++], &theMass) != TCL_OK) {
          opserr << "WARNING invalid nodal mass term\n";
          opserr << "node: " << nodeId << ", dof: " << i + 1 << endln;
          return TCL_ERROR;
        }
        mass(i, i) = theMass;
      }
      theNode->setMass(mass);
    } else if (strcmp(argv[currentArg], "-dispLoc") == 0) {
      currentArg++;
      if (argc < currentArg + ndm) {
        opserr << "WARNING incorrect number of nodal display location terms, "
                  "need ndm\n";
        opserr << "node: " << nodeId << endln;
        return TCL_ERROR;
      }
      Vector displayLoc(ndm);
      double theCrd;
      for (int i = 0; i < ndm; i++) {
        if (Tcl_GetDouble(interp, argv[currentArg++], &theCrd) != TCL_OK) {
          opserr << "WARNING invalid nodal mass term\n";
          opserr << "node: " << nodeId << ", dof: " << i + 1 << endln;
          return TCL_ERROR;
        }
        displayLoc(i) = theCrd;
      }
      theNode->setDisplayCrds(displayLoc);

    } else if (strcmp(argv[currentArg], "-disp") == 0) {
      currentArg++;
      if (argc < currentArg + ndf) {
        opserr << "WARNING incorrect number of nodal disp terms\n";
        opserr << "node: " << nodeId << endln;
        return TCL_ERROR;
      }
      Vector disp(ndf);
      double theDisp;
      for (int i = 0; i < ndf; i++) {
        if (Tcl_GetDouble(interp, argv[currentArg++], &theDisp) != TCL_OK) {
          opserr << "WARNING invalid nodal disp term\n";
          opserr << "node: " << nodeId << ", dof: " << i + 1 << endln;
          return TCL_ERROR;
        }
        disp(i) = theDisp;
      }
      theNode->setTrialDisp(disp);
      theNode->commitState();

    } else if (strcmp(argv[currentArg], "-vel") == 0) {
      currentArg++;
      if (argc < currentArg + ndf) {
        opserr << "WARNING incorrect number of nodal vel terms\n";
        opserr << "node: " << nodeId << endln;
        return TCL_ERROR;
      }
      Vector disp(ndf);
      double theDisp;
      for (int i = 0; i < ndf; i++) {
        if (Tcl_GetDouble(interp, argv[currentArg++], &theDisp) != TCL_OK) {
          opserr << "WARNING invalid nodal vel term\n";
          opserr << "node: " << nodeId << ", dof: " << i + 1 << endln;
          return TCL_ERROR;
        }
        disp(i) = theDisp;
      }
      theNode->setTrialVel(disp);
      theNode->commitState();

    } else
      currentArg++;
  }

  // if get here we have sucessfully created the node and added it to the domain
  return TCL_OK;
}

int
TclCommand_addNodalLoad(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char **argv)
{
  G3_Runtime *rt = G3_getRuntime(interp);
  TclSafeBuilder *theTclBuilder = G3_getSafeBuilder(rt);
  Domain *theTclDomain = G3_getDomain(rt);
  int nodeLoadTag = theTclBuilder->getNodalLoadTag();
  LoadPattern *theTclLoadPattern = theTclBuilder->getEnclosingPattern();
  // ensure the destructor has not been called -
  if (theTclBuilder == 0) {
    opserr << "WARNING builder has been destroyed - load \n";
    return TCL_ERROR;
  }

  //  int ndf = theTclBuilder->getNDF();

  int ndf = argc - 2;
  NodalLoad *theLoad = 0;

  bool isLoadConst = false;
  bool userSpecifiedPattern = false;
  int loadPatternTag = 0;
  // The above definition are moved forward for the use in both cases

  //-------------Adding Proc for NodalThermalAction, By Liming Jiang, [SIF] 2017
  if ((strcmp(argv[2], "-NodalThermal") == 0) ||
      (strcmp(argv[2], "-nodalThermal") == 0)) {

    int nodeId;
    if (Tcl_GetInt(interp, argv[1], &nodeId) != TCL_OK) {
      opserr << "WARNING invalid nodeId: " << argv[1] << endln;
      return TCL_ERROR;
    }

    Vector *thecrds = new Vector();
    Node *theNode = theTclDomain->getNode(nodeId);
    if (theNode == 0) {
      opserr << "WARNING invalid nodeID: " << argv[1] << endln;
      return TCL_ERROR;
    }
    (*thecrds) = theNode->getCrds();

    int count = 3;
    if (strcmp(argv[count], "-source") == 0) {
      count++;
      const char *pwd = getInterpPWD(interp);
      // simulationInfo.addInputFile(argv[count], pwd);
      TimeSeries *theSeries;
      // default num of temperature input for nodal ThermalAction;
      int dataLen = 9; 

      if (argc - count == 5) {
        // which indicates the nodal thermal action is applied to 3D I section
        // Beam;
        dataLen = 15;
        theSeries = new PathTimeSeriesThermal(nodeId, argv[count], dataLen);
        count++;
        double RcvLoc1, RcvLoc2, RcvLoc3, RcvLoc4;
        if (Tcl_GetDouble(interp, argv[count], &RcvLoc1) != TCL_OK) {
          opserr << "WARNING NodalLoad - invalid loc1  " << argv[count]
                 << " for NodalThermalAction\n";
          return TCL_ERROR;
        }
        if (Tcl_GetDouble(interp, argv[count + 1], &RcvLoc2) != TCL_OK) {
          opserr << "WARNING NodalLoad - invalid loc2  " << argv[count + 1]
                 << " for NodalThermalAction\n";
          return TCL_ERROR;
        }
        if (Tcl_GetDouble(interp, argv[count + 2], &RcvLoc3) != TCL_OK) {
          opserr << "WARNING NodalLoad - invalid loc3  " << argv[count + 2]
                 << " for NodalThermalAction\n";
          return TCL_ERROR;
        }
        if (Tcl_GetDouble(interp, argv[count + 3], &RcvLoc4) != TCL_OK) {
          opserr << "WARNING NodalLoad - invalid loc4  " << argv[count + 3]
                 << " for NodalThermalAction\n";
          return TCL_ERROR;
        }
        // end of recieving data;
        theLoad = new NodalThermalAction(nodeLoadTag, nodeId, RcvLoc1, RcvLoc2,
                                         RcvLoc3, RcvLoc4, theSeries, thecrds);
      }
      // end of for 15 data input;
      else if (argc - count == 3 || argc - count == 10) {

        theSeries = new PathTimeSeriesThermal(nodeId, argv[count]);
        count++;
        Vector locy;
        if (argc - count == 2) {
          double RcvLoc1, RcvLoc2;
          if (Tcl_GetDouble(interp, argv[count], &RcvLoc1) != TCL_OK) {
            opserr << "WARNING NodalLoad - invalid loc1  " << argv[count]
                   << " for NodalThermalAction\n";
            return TCL_ERROR;
          }
          if (Tcl_GetDouble(interp, argv[count + 1], &RcvLoc2) != TCL_OK) {
            opserr << "WARNING NodalLoad - invalid loc2  " << argv[count + 1]
                   << " for NodalThermalAction\n";
            return TCL_ERROR;
          }
          locy = Vector(9);
          locy(0) = RcvLoc1;
          locy(1) = (7 * RcvLoc1 + 1 * RcvLoc2) / 8;
          locy(2) = (6 * RcvLoc1 + 2 * RcvLoc2) / 8;
          locy(3) = (5 * RcvLoc1 + 3 * RcvLoc2) / 8;
          locy(4) = (4 * RcvLoc1 + 4 * RcvLoc2) / 8;
          locy(5) = (3 * RcvLoc1 + 5 * RcvLoc2) / 8;
          locy(6) = (2 * RcvLoc1 + 6 * RcvLoc2) / 8;
          locy(7) = (1 * RcvLoc1 + 7 * RcvLoc2) / 8;
          locy(8) = RcvLoc2;

        } // end of if only recieving one loc data;
        else if (argc - count == 9) {
          double indata[9];
          double BufferData;

          for (int i = 0; i < 9; i++) {
            if (Tcl_GetDouble(interp, argv[count], &BufferData) != TCL_OK) {
              opserr << "WARNING eleLoad - invalid data " << argv[count]
                     << " for -beamThermal 3D\n";
              return TCL_ERROR;
            }
            indata[i] = BufferData;
            count++;
          }
          locy = Vector(indata, 9);
          // temp1,loc1,temp2,loc2...temp9,loc9
        } // end of if only recieving 9 loc data;
        theLoad = new NodalThermalAction(nodeLoadTag, nodeId, locy, theSeries,
                                         thecrds);
        delete thecrds;
      }
      // end of recieving 9 temp data in external file;
      else {
        opserr << "WARNING NodalThermalAction - invalid dataLen\n";
      }
      // end of definition for different data input length(9or15)
    }
    // end for detecting source
    else {
      if (argc - count == 4) {
        double t1, t2, locY1, locY2;
        if (Tcl_GetDouble(interp, argv[count], &t1) != TCL_OK) {
          opserr << "WARNING eleLoad - invalid T1 " << argv[count]
                 << " for NodalThermalAction\n";
          return TCL_ERROR;
        }
        if (Tcl_GetDouble(interp, argv[count + 1], &locY1) != TCL_OK) {
          opserr << "WARNING eleLoad - invalid LocY1 " << argv[count + 1]
                 << " for NodalThermalAction\n";
          return TCL_ERROR;
        }
        if (Tcl_GetDouble(interp, argv[count + 2], &t2) != TCL_OK) {
          opserr << "WARNING eleLoad - invalid T1 " << argv[count]
                 << " for NodalThermalAction\n";
          return TCL_ERROR;
        }
        if (Tcl_GetDouble(interp, argv[count + 3], &locY2) != TCL_OK) {
          opserr << "WARNING eleLoad - invalid LocY1 " << argv[count + 1]
                 << " for NodalThermalAction\n";
          return TCL_ERROR;
        }
        theLoad = new NodalThermalAction(nodeLoadTag, nodeId, t1, locY1, t2,
                                         locY2, thecrds);
      }
      // for defining a uniform gradient thermal action
    }
    // end for source or no source
    if (theLoad == 0) {
      opserr << "WARNING NodalLoad - out of memory creating load " << argv[1];
      return TCL_ERROR;
    }
    // get the current pattern tag if no tag given in i/p
    if (userSpecifiedPattern == false) {
      if (theTclLoadPattern == 0) {
        opserr << "WARNING no current load pattern -NodalThermalAction "
               << nodeId;
        return TCL_ERROR;
      } else
        loadPatternTag = theTclLoadPattern->getTag();
    }
  }
  // end of adding NodalThermalAction -------------end--------- Liming,[SIF]2017

      // start of else block, Liming [SIF]
      else
  {

    // make sure at least one other argument to contain type of system
    if (argc < (2 + ndf)) {
      opserr << "WARNING bad command - want: load nodeId " << ndf << "forces\n";
      printCommand(argc, argv);
      return TCL_ERROR;
    }

    // get the id of the node
    int nodeId;
    if (Tcl_GetInt(interp, argv[1], &nodeId) != TCL_OK) {
      opserr << "WARNING invalid nodeId: " << argv[1];
      opserr << " - load nodeId " << ndf << " forces\n";
      return TCL_ERROR;
    }

    // get the load vector
    Vector forces(ndf);
    for (int i = 0; i < ndf; i++) {
      double theForce;
      if (Tcl_GetDouble(interp, argv[2 + i], &theForce) != TCL_OK) {
        opserr << "WARNING invalid force " << i + 1 << " - load" << nodeId;
        opserr << " " << ndf << " forces\n";
        return TCL_ERROR;
      } else
        forces(i) = theForce;
    }

    // allow some additional options at end of command
    int endMarker = 2 + ndf;
    while (endMarker != argc) {
      if (strcmp(argv[endMarker], "-const") == 0) {
        // allow user to specify const load
        isLoadConst = true;
      } else if (strcmp(argv[endMarker], "-pattern") == 0) {
        // allow user to specify load pattern other than current
        endMarker++;
        userSpecifiedPattern = true;
        if (endMarker == argc ||
            Tcl_GetInt(interp, argv[endMarker], &loadPatternTag) != TCL_OK) {

          opserr << "WARNING invalid patternTag - load " << nodeId << " ";
          opserr << ndf << " forces pattern patterntag\n";
          return TCL_ERROR;
        }
      }
      endMarker++;
    }

    // get the current pattern tag if no tag given in i/p
    if (userSpecifiedPattern == false) {
      if (theTclLoadPattern == 0) {
        opserr << "WARNING no current load pattern - load " << nodeId;
        opserr << " " << ndf << " forces\n";
        return TCL_ERROR;
      } else
        loadPatternTag = theTclLoadPattern->getTag();
    }

    // create the load
    theLoad = new NodalLoad(nodeLoadTag, nodeId, forces, isLoadConst);
    if (theLoad == 0) {
      opserr << "WARNING ran out of memory for load  - load " << nodeId;
      opserr << " " << ndf << " forces\n";
      return TCL_ERROR;
    }

  } // end of Liming change for nodal thermal action , putting the above block
    // into else{ }

  // add the load to the domain
  if (theTclDomain->addNodalLoad(theLoad, loadPatternTag) == false) {
    opserr << "WARNING TclSafeBuilder - could not add load to domain\n";
    printCommand(argc, argv);
    delete theLoad;
    return TCL_ERROR;
  }
  theTclBuilder->incrNodalLoadTag();

  // if get here we have sucessfully created the load and added it to the domain
  return TCL_OK;
}

/*


/////////////////////////////   gnp adding element damping
int
TclCommand_addElementRayleigh(ClientData clientData,
                              Tcl_Interp *interp,
                              int argc,
                              TCL_Char **argv)
{

  if (theTclBuilder == 0) {
    opserr << "WARNING builder has been destroyed" << endln;
    return TCL_ERROR;
  }

  // make sure corect number of arguments on command line
  if (argc < 6) {
    opserr << "WARNING insufficient arguments\n";
    printCommand(argc, argv);
    opserr << "Want: setElementRayleighFactors elementTag?  alphaM? $betaK?
$betaKinit? $betaKcomm? \n"; return TCL_ERROR;
  }

  int eleTag =0;

  if (Tcl_GetInt(interp, argv[1], &eleTag) != TCL_OK) {
    opserr << "WARNING: setElementRayleighFactors invalid eleTag: " << argv[1];
    opserr << " \n";
    return TCL_ERROR;
  }

  double alphaM,betaK,betaKinit,betaKcomm;

  if (Tcl_GetDouble(interp, argv[2], &alphaM) != TCL_OK) {
    opserr << "WARNING : setElementRayleighFactors invalid ";
    opserr << "alphaM: " << argv[2] << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[3], &betaK) != TCL_OK) {
    opserr << "WARNING : setElementRayleighFactors invalid ";
    opserr << "betaK: " << argv[3] << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[4], &betaKinit) != TCL_OK) {
    opserr << "WARNING : setElementRayleighFactors invalid ";
    opserr << "betaKinit: " << argv[4] << endln;
    return TCL_ERROR;
  }

  if (Tcl_GetDouble(interp, argv[5], &betaKcomm) != TCL_OK) {
    opserr << "WARNING : setElementRayleighFactors invalid ";
    opserr << "betaKcomm: " << argv[5] << endln;
    return TCL_ERROR;
  }

  Element* elePtr = theTclDomain->getElement(eleTag);

  if (elePtr == 0)
    opserr << "WARNING : setElementRayleighFactors invalid eleTag: " << eleTag
<< " the element does not exist in the domain \n";


  if ( elePtr->setRayleighDampingFactors(alphaM, betaK, betaKinit, betaKcomm) !=
0 ) { opserr << "ERROR : setElementRayleighFactors: FAILED to add damping
factors for element " << eleTag << "\n";

  }

  return TCL_OK;
}
/////////////////////////////   gnp adding element damping


// the function for creating ne material objects and patterns is in a seperate
file.
// this allows new material and patternobjects to be added without touching this
file.
// does so at the expense of an extra procedure call.

extern int
TclSafeBuilderParameterCommand(ClientData clientData,
                                Tcl_Interp *interp, int argc,
                                TCL_Char **argv,
                                Domain *theDomain,
                                TclSafeBuilder *theTclBuilder);

int
TclCommand_addParameter(ClientData clientData, Tcl_Interp *interp, int argc,
TCL_Char **argv)

{
  return TclSafeBuilderParameterCommand(clientData, interp,
                                         argc, argv, theTclDomain,
theTclBuilder);
}

*/
class TclBasicBuilder;
extern int TclBasicBuilderElementCommand(ClientData clientData,
                                         Tcl_Interp *interp, int argc,
                                         TCL_Char **argv, Domain *theDomain,
                                         TclBasicBuilder *theTclBuilder);

int
TclCommand_addElement(ClientData clientData, Tcl_Interp *interp, int argc,
                      TCL_Char **argv)

{
  G3_Runtime *rt = G3_getRuntime(interp);
  TclSafeBuilder *theTclBuilder = G3_getSafeBuilder(rt);
  Domain *theTclDomain = G3_getDomain(rt);
  return TclBasicBuilderElementCommand(clientData, interp, argc, argv,
                                       theTclDomain,
                                       (TclBasicBuilder *)theTclBuilder);
}
/*
// extern int OPS_LineMesh(Domain& domain, int ndm);
// extern int OPS_TriMesh(Domain& domain);
// extern int OPS_TriReMesh(Domain& domain, int ndf);
int
TclCommand_mesh(ClientData clientData, Tcl_Interp *interp,  int argc,
                TCL_Char **argv)
{
    // ensure the destructor has not been called -
    if (theTclBuilder == 0) {
        opserr << "WARNING builder has been destroyed" << endln;
        return TCL_ERROR;
    }

    int ndm = theTclBuilder->getNDM();

    // make sure corect number of arguments on command line
    if (argc < 2) {
        opserr << "WARNING insufficient arguments\n";
        opserr << "Want: mesh type? ...>\n";
        return TCL_ERROR;
    }

      OPS_ResetInput(clientData, interp,2,argc,argv,theTclDomain,theTclBuilder);

    // mesh type
    int res = 0;
    // if (strcmp(argv[1], "line") == 0) {
    // 	res = OPS_LineMesh(*theTclDomain,ndm);
    // } else if (strcmp(argv[1], "tri") == 0) {
    // 	res = OPS_TriMesh(*theTclDomain);
    // } else {
    // 	opserr<<"WARNING: mesh type "<<argv[1]<<" is unknown\n";
    // 	return TCL_ERROR;
    // }

    if (res < 0) {
        return TCL_ERROR;
    }

    return 0;

}

int
TclCommand_remesh(ClientData clientData, Tcl_Interp *interp,  int argc,
                TCL_Char **argv)
{
    // ensure the destructor has not been called -
    if (theTclBuilder == 0) {
        opserr << "WARNING builder has been destroyed" << endln;
        return TCL_ERROR;
    }

    int ndf = theTclBuilder->getNDF();

    // make sure corect number of arguments on command line
    if (argc < 2) {
        opserr << "WARNING insufficient arguments\n";
        opserr << "Want: mesh type? ...>\n";
        return TCL_ERROR;
    }

      OPS_ResetInput(clientData, interp,2,argc,argv,theTclDomain,theTclBuilder);

    // mesh type
    int res = 0;
    // if (strcmp(argv[1], "line") == 0) {
    // 	//res = OPS_LineMesh(*theTclDomain,ndm);
    // } else if (strcmp(argv[1], "tri") == 0) {
    // 	res = OPS_TriReMesh(*theTclDomain,ndf);
    // } else {
    // 	opserr<<"WARNING: remesh type "<<argv[1]<<" is unknown\n";
    // 	return TCL_ERROR;
    // }

    if (res < 0) {
        return TCL_ERROR;
    }

    return 0;

}

#if defined(OPSDEF_Element_PFEM)
extern int OPS_BgMesh();

int
TclCommand_backgroundMesh(ClientData clientData, Tcl_Interp *interp, int argc,
TCL_Char **argv)
{
    // ensure the destructor has not been called -
    if (theTclBuilder == 0) {
        opserr << "WARNING builder has been destroyed" << endln;
        return TCL_ERROR;
    }

    OPS_ResetInput(clientData, interp, 1, argc, argv, theTclDomain,
theTclBuilder);

    if(OPS_BgMesh() >= 0) return TCL_OK;
    else return TCL_ERROR;
    return TCL_OK;
}
#endif // _OPS_Element_PFEM

extern void* OPS_LobattoBeamIntegration(int& integrationTag, ID& secTags);
extern void* OPS_LegendreBeamIntegration(int& integrationTag, ID& secTags);
extern void* OPS_NewtonCotesBeamIntegration(int& integrationTag, ID& secTags);
extern void* OPS_RadauBeamIntegration(int& integrationTag, ID& secTags);
extern void* OPS_TrapezoidalBeamIntegration(int& integrationTag, ID& secTags);
extern void* OPS_CompositeSimpsonBeamIntegration(int& integrationTag, ID& secTags);
extern void* OPS_UserDefinedBeamIntegration(int& integrationTag, ID& secTags);
extern void* OPS_FixedLocationBeamIntegration(int& integrationTag, ID& secTags); 
extern void* OPS_LowOrderBeamIntegration(int& integrationTag, ID& secTags); 

extern void* OPS_MidDistanceBeamIntegration(int& integrationTag, ID& secTags); 
extern void* OPS_UserHingeBeamIntegration(int& integrationTag, ID& secTags); 
extern void* OPS_HingeMidpointBeamIntegration(int& integrationTag, ID& secTags); 
extern void* OPS_HingeRadauBeamIntegration(int& integrationTag, ID& secTags); 
extern void* OPS_HingeRadauTwoBeamIntegration(int& integrationTag, ID& secTags); 
extern void* OPS_HingeEndpointBeamIntegration(int& integrationTag, ID& secTags);

int
TclCommand_addBeamIntegration(ClientData clientData, Tcl_Interp *interp, int
argc, TCL_Char **argv)
{
    if (argc < 2) {
        opserr << "WARNING: want beamIntegration type itag...\n";
        return TCL_ERROR;
    }

      OPS_ResetInput(clientData, interp,2,argc,argv,theTclDomain,theTclBuilder);

    int iTag;
    ID secTags;
    BeamIntegration* bi = 0;
    if (strcmp(argv[1],"Lobatto") == 0) {
        bi = (BeamIntegration*)OPS_LobattoBeamIntegration(iTag,secTags);
    } else if (strcmp(argv[1],"Legendre") == 0) {
        bi = (BeamIntegration*)OPS_LegendreBeamIntegration(iTag,secTags);
    } else if (strcmp(argv[1],"NewtoCotes") == 0) {
        bi = (BeamIntegration*)OPS_NewtonCotesBeamIntegration(iTag,secTags);
    } else if (strcmp(argv[1],"Radau") == 0) {
        bi = (BeamIntegration*)OPS_RadauBeamIntegration(iTag,secTags);
    } else if (strcmp(argv[1],"Trapezoidal") == 0) {
        bi = (BeamIntegration*)OPS_TrapezoidalBeamIntegration(iTag,secTags);
    } else if (strcmp(argv[1],"CompositeSimpson") == 0) {
        bi =
(BeamIntegration*)OPS_CompositeSimpsonBeamIntegration(iTag,secTags); } else if
(strcmp(argv[1],"UserDefined") == 0) { bi =
(BeamIntegration*)OPS_UserDefinedBeamIntegration(iTag,secTags); } else if
(strcmp(argv[1],"FixedLocation") == 0) { bi =
(BeamIntegration*)OPS_FixedLocationBeamIntegration(iTag,secTags); } else if
(strcmp(argv[1],"LowOrder") == 0) { bi =
(BeamIntegration*)OPS_LowOrderBeamIntegration(iTag,secTags); } else if
(strcmp(argv[1],"MidDistance") == 0) { bi =
(BeamIntegration*)OPS_MidDistanceBeamIntegration(iTag,secTags); } else if
(strcmp(argv[1],"UserHinge") == 0) { bi =
(BeamIntegration*)OPS_UserHingeBeamIntegration(iTag,secTags); } else if
(strcmp(argv[1],"HingeMidpoint") == 0) { bi =
(BeamIntegration*)OPS_HingeMidpointBeamIntegration(iTag,secTags); } else if
(strcmp(argv[1],"HingeRadau") == 0) { bi =
(BeamIntegration*)OPS_HingeRadauBeamIntegration(iTag,secTags); } else if
(strcmp(argv[1],"HingeRadauTwo") == 0) { bi =
(BeamIntegration*)OPS_HingeRadauTwoBeamIntegration(iTag,secTags); } else if
(strcmp(argv[1],"HingeEndpoint") == 0) { bi =
(BeamIntegration*)OPS_HingeEndpointBeamIntegration(iTag,secTags); } else {
        opserr<<"WARNING: integration type "<<argv[1]<<" is unknown\n";
        return TCL_ERROR;
    }

    if (bi == 0) {
        opserr<<"WARNING: failed to create beam integration\n";
        return TCL_ERROR;
    }

    BeamIntegrationRule* rule = new BeamIntegrationRule(iTag,bi,secTags);
    if (rule == 0) {
        opserr<<"WARNING: failed to create beam integration\n";
        delete bi;
        return TCL_ERROR;
    }

    // Now add the
    if(OPS_addBeamIntegrationRule(rule) == false) {
        opserr<<"WARNING: could not add BeamIntegrationRule.";
        delete rule; // invoke the destructor, otherwise mem leak
        return TCL_ERROR;;
    }

    return TCL_OK;
}

*/
extern int TclBasicBuilderUniaxialMaterialCommand(ClientData, Tcl_Interp *, int,
                                                  TCL_Char **, Domain *);
extern int TclSafeBuilderUniaxialCommand(ClientData, Tcl_Interp *, int,
                                         TCL_Char **, Domain *);
int
TclCommand_addUniaxialMaterial(ClientData clientData, Tcl_Interp *interp,
                               int argc, TCL_Char **argv)
{
  int stat = TclSafeBuilderUniaxialCommand(clientData, interp, argc, argv,
                                       G3_getDomain(G3_getRuntime(interp)));
  return stat;
}

/*
extern int
Tcl_AddLimitCurveCommand (ClientData clienData, Tcl_Interp *interp, int argc,
TCL_Char **argv, Domain *theDomain);

int
TclCommand_addLimitCurve(ClientData clientData, Tcl_Interp *interp, int argc,
TCL_Char **argv)
{
  return Tcl_AddLimitCurveCommand(clientData, interp, argc, argv, theTclDomain);
}

extern int
TclSafeBuilderNDMaterialCommand (ClientData clienData, Tcl_Interp
*interp, int argc, TCL_Char **argv, TclSafeBuilder *theTclBuilder);

int
TclCommand_addNDMaterial(ClientData clientData, Tcl_Interp *interp,
                            int argc,    TCL_Char **argv)

{
  return TclSafeBuilderNDMaterialCommand(clientData, interp,
                                          argc, argv, theTclBuilder);
}

extern int
TclSafeBuilderYieldSurface_BCCommand (ClientData clienData, Tcl_Interp
*interp, int argc, TCL_Char **argv, TclSafeBuilder *theTclBuilder);

int
TclCommand_addYieldSurface_BC(ClientData clientData, Tcl_Interp *interp,
                                    int argc, TCL_Char **argv)

{
  return TclSafeBuilderYieldSurface_BCCommand(clientData, interp,
                                                argc, argv, theTclBuilder);
}

extern int
TclSafeBuilderYS_EvolutionModelCommand (ClientData clienData, Tcl_Interp
*interp, int argc, TCL_Char **argv, TclSafeBuilder *theTclBuilder);

int
TclCommand_addYS_EvolutionModel(ClientData clientData, Tcl_Interp *interp,
                                    int argc, TCL_Char **argv)

{
  return TclSafeBuilderYS_EvolutionModelCommand(clientData, interp,
                                                argc, argv, theTclBuilder);
}

extern int
TclSafeBuilderPlasticMaterialCommand (ClientData clienData, Tcl_Interp
*interp, int argc, TCL_Char **argv, TclSafeBuilder *theTclBuilder);

int
TclCommand_addYS_PlasticMaterial(ClientData clientData, Tcl_Interp *interp,
                                    int argc, TCL_Char **argv)

{
  return TclSafeBuilderPlasticMaterialCommand(clientData, interp,
                                                argc, argv, theTclBuilder);
}

//!!
extern int TclSafeBuilderCyclicModelCommand(ClientData clienData,
Tcl_Interp *interp, int argc, TCL_Char **argv, TclSafeBuilder
*theTclBuilder); int TclCommand_addCyclicModel(ClientData clientData, Tcl_Interp
*interp, int argc, TCL_Char **argv)

{
  return TclSafeBuilderCyclicModelCommand(clientData, interp,
                                                argc, argv, theTclBuilder);
}

extern int TclSafeBuilderDamageModelCommand(ClientData clienData,
Tcl_Interp *interp, int argc, TCL_Char **argv);


int
TclCommand_addDamageModel(ClientData clientData, Tcl_Interp *interp,
                                    int argc, TCL_Char **argv)
{
  TclSafeBuilder *theTclBuilder =
      (TclSafeBuilder *)Tcl_GetAssocData(interp, "OPS::theTclBuilder", NULL);
  Domain *theTclDomain = theTclBuilder->getDomain();
  return TclSafeBuilderDamageModelCommand(clientData, interp, argc, argv);

}
*/

extern int TclPatternCommand(ClientData clientData, Tcl_Interp *interp,
                             int argc, TCL_Char **argv, Domain *theDomain);

static int
TclCommand_addPattern(ClientData clientData, Tcl_Interp *interp, int argc,
                      TCL_Char **argv)
{
  TclSafeBuilder *theTclBuilder =
      (TclSafeBuilder *)Tcl_GetAssocData(interp, "OPS::theTclBuilder", NULL);
  Domain *theTclDomain = theTclBuilder->getDomain();
  return TclPatternCommand(clientData, interp, argc, argv, theTclDomain);
}

extern TimeSeries *TclTimeSeriesCommand(ClientData clientData,
                                        Tcl_Interp *interp, int argc,
                                        TCL_Char **argv, Domain *theDomain);

static int
TclCommand_addTimeSeries(ClientData clientData, Tcl_Interp *interp, int argc,
                         TCL_Char **argv)
{
  TclSafeBuilder *theTclBuilder = (TclSafeBuilder *)Tcl_GetAssocData(
      interp,"OPS::theTclSafeBuilder", NULL);
  Domain *theTclDomain = theTclBuilder->getDomain();

  TimeSeries *theSeries = TclTimeSeriesCommand(clientData, interp, argc - 1,
                                               &argv[1], theTclDomain);

  if (theSeries != 0) {
    if (theTclBuilder->addTimeSeries(argv[2], theSeries))
      return TCL_OK;
    else
      return TCL_ERROR;
  }
  return TCL_ERROR;
}

/*

extern int
TclGroundMotionCommand(ClientData clientData,
                       Tcl_Interp *interp,
                       int argc,
                       TCL_Char **argv,
                       MultiSupportPattern *thePattern);

int
TclCommand_addGroundMotion(ClientData clientData, Tcl_Interp *interp,
                           int argc, TCL_Char **argv)

{
  return TclGroundMotionCommand(clientData, interp, argc, argv,
                                theTclMultiSupportPattern);
}

*/

/*


int
TclCommand_addElementalLoad(ClientData clientData, Tcl_Interp *interp, int argc,
                         TCL_Char **argv)
{
  // ensure the destructor has not been called -
  if (theTclBuilder == 0) {
    opserr << "WARNING current builder has been destroyed - eleLoad\n";
    return TCL_ERROR;
  }

  if (theTclLoadPattern == 0) {
    opserr << "WARNING no active load pattern - eleLoad\n";
    return TCL_ERROR;
  }

  int ndm = theTclBuilder->getNDM();
  ElementalLoad *theLoad = 0;

  ID theEleTags(0,16);

  // we first create an ID containing the ele tags of all elements
  // for which the load applies.
  int count = 1;
  int doneEle = 0;
  int eleCount = 0;
  while (doneEle == 0 && count < argc) {
    if (strcmp(argv[count],"-ele") == 0) {
      count ++;
      int eleStart = count;
      int eleEnd = 0;
      int eleID;
      while (count < argc && eleEnd == 0) {
        if (Tcl_GetInt(interp, argv[count], &eleID) != TCL_OK)
          eleEnd = count;
        else
          count++;
      }
      if (eleStart != eleEnd) {
        for (int i=eleStart; i<eleEnd; i++) {
          Tcl_GetInt(interp, argv[i], &eleID);
          theEleTags[eleCount++] = eleID;
        }
      }
    }
    else if (strcmp(argv[count],"-range") == 0) {
      count ++;
      int eleStart, eleEnd;
      if (Tcl_GetInt(interp, argv[count], &eleStart) != TCL_OK) {
        opserr << "WARNING eleLoad -range invalid eleStart " << argv[count] <<
"\n"; return TCL_ERROR;
      }
      count++;
      if (Tcl_GetInt(interp, argv[count], &eleEnd) != TCL_OK) {
        opserr << "WARNING eleLoad -range invalid eleEnd " << argv[count] <<
"\n"; return TCL_ERROR;
      }
      count++;
      for (int i=eleStart; i<=eleEnd; i++)
        theEleTags[eleCount++] = i;
    } else
      doneEle = 1;
  }


  // we then create the load
  if (strcmp(argv[count],"-type") != 0) {
    opserr << "WARNING eleLoad - expecting -type option but got "
         << argv[count] << endln;
    return TCL_ERROR;
  }
  count++;
  if (strcmp(argv[count],"-beamUniform") == 0 ||
      strcmp(argv[count],"beamUniform") == 0){
    count++;
    if (ndm == 2) {
      double wta;
      double waa = 0.0;
      if (count >= argc || Tcl_GetDouble(interp, argv[count], &wta) != TCL_OK) {
        opserr << "WARNING eleLoad - invalid wt for beamUniform \n";
        return TCL_ERROR;
      }
      count++;
      if (count < argc && Tcl_GetDouble(interp, argv[count], &waa) != TCL_OK) {
        opserr << "WARNING eleLoad - invalid wa for beamUniform \n";
        return TCL_ERROR;
      }
      double aL = 0.0;
      double bL = 1.0;
      count++;
      if (count < argc && Tcl_GetDouble(interp, argv[count], &aL) != TCL_OK) {
        opserr << "WARNING eleLoad - invalid aOverL for beamUniform \n";
        return TCL_ERROR;
      }
      count++;
      if (count < argc && Tcl_GetDouble(interp, argv[count], &bL) != TCL_OK) {
        opserr << "WARNING eleLoad - invalid bOverL for beamUniform \n";
        return TCL_ERROR;
      }
      double wab = waa;
      double wtb = wta;
      count++;
      if (count < argc && Tcl_GetDouble(interp, argv[count], &wtb) != TCL_OK) {
        opserr << "WARNING eleLoad - invalid wt for beamUniform \n";
        return TCL_ERROR;
      }
      count++;
      if (count < argc && Tcl_GetDouble(interp, argv[count], &wab) != TCL_OK) {
        opserr << "WARNING eleLoad - invalid wa for beamUniform \n";
        return TCL_ERROR;
      }
      for (int i=0; i<theEleTags.Size(); i++) {
        if (aL > 0.0 || bL < 1.0 || wta != wtb || waa != wab)
          theLoad = new Beam2dPartialUniformLoad(eleLoadTag, wta, wtb, waa, wab,
aL, bL, theEleTags(i)); else theLoad = new Beam2dUniformLoad(eleLoadTag, wta,
waa, theEleTags(i));

        if (theLoad == 0) {
          opserr << "WARNING eleLoad - out of memory creating load of type " <<
argv[count] ; return TCL_ERROR;
        }

        // get the current pattern tag if no tag given in i/p
        int loadPatternTag = theTclLoadPattern->getTag();

        // add the load to the domain
        if (theTclDomain->addElementalLoad(theLoad, loadPatternTag) == false) {
          opserr << "WARNING eleLoad - could not add following load to domain:\n
"; opserr << theLoad; delete theLoad; return TCL_ERROR;
        }
        eleLoadTag++;
      }

      return 0;
    }
    else if (ndm == 3) {
      double wy, wz;
      double wx = 0.0;
      if (count >= argc || Tcl_GetDouble(interp, argv[count], &wy) != TCL_OK) {
        opserr << "WARNING eleLoad - invalid wy for beamUniform \n";
        return TCL_ERROR;
      }
      count++;
      if (count >= argc || Tcl_GetDouble(interp, argv[count], &wz) != TCL_OK) {
        opserr << "WARNING eleLoad - invalid wz for beamUniform \n";
        return TCL_ERROR;
      }
      count++;
      if (count < argc && Tcl_GetDouble(interp, argv[count], &wx) != TCL_OK) {
        opserr << "WARNING eleLoad - invalid wx for beamUniform \n";
        return TCL_ERROR;
      }
      double aL = 0.0;
      double bL = 1.0;
      count++;
      if (count < argc && Tcl_GetDouble(interp, argv[count], &aL) != TCL_OK) {
        opserr << "WARNING eleLoad - invalid aOverL for beamUniform \n";
        return TCL_ERROR;
      }
      count++;
      if (count < argc && Tcl_GetDouble(interp, argv[count], &bL) != TCL_OK) {
        opserr << "WARNING eleLoad - invalid bOverL for beamUniform \n";
        return TCL_ERROR;
      }

      for (int i=0; i<theEleTags.Size(); i++) {
        if (aL > 0.0 || bL < 1.0)
          theLoad = new Beam3dPartialUniformLoad(eleLoadTag, wy, wz, wx, aL, bL,
theEleTags(i)); else theLoad = new Beam3dUniformLoad(eleLoadTag, wy, wz, wx,
theEleTags(i));

        if (theLoad == 0) {
          opserr << "WARNING eleLoad - out of memory creating load of type " <<
argv[count] ; return TCL_ERROR;
        }

        // get the current pattern tag if no tag given in i/p
        int loadPatternTag = theTclLoadPattern->getTag();

        // add the load to the domain
        if (theTclDomain->addElementalLoad(theLoad, loadPatternTag) == false) {
          opserr << "WARNING eleLoad - could not add following load to domain:\n
"; opserr << theLoad; delete theLoad; return TCL_ERROR;
        }
        eleLoadTag++;
      }

      return 0;

    }
    else {
      opserr << "WARNING eleLoad beamUniform currently only valid only for ndm=2
or 3\n"; return TCL_ERROR;
    }

  } else if (strcmp(argv[count],"-beamPoint") == 0 ||
             strcmp(argv[count],"beamPoint") == 0 ) {
    count++;
    if (ndm == 2) {
      double P, x;
      double N = 0.0;
      if (count >= argc || Tcl_GetDouble(interp, argv[count], &P) != TCL_OK) {
        opserr << "WARNING eleLoad - invalid P for beamPoint\n";
        return TCL_ERROR;
      }
      if (count+1 >= argc || Tcl_GetDouble(interp, argv[count+1], &x) != TCL_OK)
{ opserr << "WARNING eleLoad - invalid xDivL for beamPoint\n"; return TCL_ERROR;
      }
      if (count+2 < argc && Tcl_GetDouble(interp, argv[count+2], &N) != TCL_OK)
{ opserr << "WARNING eleLoad - invalid N for beamPoint\n"; return TCL_ERROR;
      }

      if (x < 0.0 || x > 1.0) {
        opserr << "WARNING eleLoad - invalid xDivL of " << x;
        opserr << " for beamPoint (valid range [0.0, 1.0]\n";
        return TCL_ERROR;
      }


      for (int i=0; i<theEleTags.Size(); i++) {
        theLoad = new Beam2dPointLoad(eleLoadTag, P, x, theEleTags(i), N);

        if (theLoad == 0) {
          opserr << "WARNING eleLoad - out of memory creating load of type " <<
argv[count] ; return TCL_ERROR;
        }

        // get the current pattern tag if no tag given in i/p
        int loadPatternTag = theTclLoadPattern->getTag();

        // add the load to the domain
        if (theTclDomain->addElementalLoad(theLoad, loadPatternTag) == false) {
          opserr << "WARNING eleLoad - could not add following load to domain:\n
"; opserr << theLoad; delete theLoad; return TCL_ERROR;
        }
        eleLoadTag++;
      }

      return 0;

    }
    else if (ndm == 3) {
      double Py, Pz, x;
      double N = 0.0;
      if (count >= argc || Tcl_GetDouble(interp, argv[count], &Py) != TCL_OK) {
        opserr << "WARNING eleLoad - invalid Py for beamPoint\n";
        return TCL_ERROR;
      }
      if (count+1 >= argc || Tcl_GetDouble(interp, argv[count+1], &Pz) !=
TCL_OK) { opserr << "WARNING eleLoad - invalid Pz  for beamPoint\n"; return
TCL_ERROR;
      }
      if (count+2 >= argc || Tcl_GetDouble(interp, argv[count+2], &x) != TCL_OK)
{ opserr << "WARNING eleLoad - invalid xDivL for beamPoint\n"; return TCL_ERROR;
      }
      if (count+3 < argc && Tcl_GetDouble(interp, argv[count+3], &N) != TCL_OK)
{ opserr << "WARNING eleLoad - invalid N for beamPoint\n"; return TCL_ERROR;
      }

      if (x < 0.0 || x > 1.0) {
        opserr << "WARNING eleLoad - invalid xDivL of " << x;
        opserr << " for beamPoint (valid range [0.0, 1.0]\n";
        return TCL_ERROR;
      }

      for (int i=0; i<theEleTags.Size(); i++) {
        theLoad = new Beam3dPointLoad(eleLoadTag, Py, Pz, x, theEleTags(i), N);

        if (theLoad == 0) {
          opserr << "WARNING eleLoad - out of memory creating load of type " <<
argv[count] ; return TCL_ERROR;
        }

        // get the current pattern tag if no tag given in i/p
        int loadPatternTag = theTclLoadPattern->getTag();

        // add the load to the domain
        if (theTclDomain->addElementalLoad(theLoad, loadPatternTag) == false) {
          opserr << "WARNING eleLoad - could not add following load to domain:\n
"; opserr << theLoad; delete theLoad; return TCL_ERROR;
        }
        eleLoadTag++;
      }
      return 0;
    }
    else {
      opserr << "WARNING eleLoad beamPoint type currently only valid only for
ndm=2 or 3\n"; return TCL_ERROR;
    }
  }
  // Added Joey Yang UC Davis
  else if (strcmp(argv[count],"-BrickW") == 0) {

      for (int i=0; i<theEleTags.Size(); i++) {
        theLoad = new BrickSelfWeight(eleLoadTag, theEleTags(i));

        if (theLoad == 0) {
          opserr << "WARNING eleLoad - out of memory creating load of type " <<
argv[count] ; return TCL_ERROR;
        }

        // get the current pattern tag if no tag given in i/p
        int loadPatternTag = theTclLoadPattern->getTag();

        // add the load to the domain
        if (theTclDomain->addElementalLoad(theLoad, loadPatternTag) == false) {
          opserr << "WARNING eleLoad - could not add following load to domain:\n
"; opserr << theLoad; delete theLoad; return TCL_ERROR;
        }
        eleLoadTag++;
      }
      return 0;
  }
  // Added: C.McGann, U.Washington
  else if ((strcmp(argv[count],"-surfaceLoad") == 0) ||
(strcmp(argv[count],"-SurfaceLoad") == 0)) { count++; for (int i=0;
i<theEleTags.Size(); i++) { theLoad = new SurfaceLoader(eleLoadTag,
theEleTags(i));

              if (theLoad == 0) {
                  opserr << "WARNING eleLoad - out of memory creating load of
type " << argv[count] ; return TCL_ERROR;
              }

              // get the current pattern tag if no tag given in i/p
              int loadPatternTag = theTclLoadPattern->getTag();

              // add the load to the domain
              if (theTclDomain->addElementalLoad(theLoad, loadPatternTag) ==
false) { opserr << "WARNING eleLoad - could not add following load to domain:\n
"; opserr << theLoad; delete theLoad; return TCL_ERROR;
              }
                  eleLoadTag++;
      }
      return 0;
  }
  // Added: C.McGann, U.Washington
  else if ((strcmp(argv[count],"-selfWeight") == 0) ||
(strcmp(argv[count],"-SelfWeight") == 0)) { count++;

    double xf = 0.0, yf = 0.0, zf = 0.0;
    if (Tcl_GetDouble(interp, argv[count], &xf) != TCL_OK) {
          opserr << "WARNING eleLoad - invalid xFactor " << argv[count] << " for
-selfWeight\n"; return TCL_ERROR;
        }
    if (Tcl_GetDouble(interp, argv[count+1], &yf) != TCL_OK) {
          opserr << "WARNING eleLoad - invalid yFactor " << argv[count+1] << "
for -selfWeight\n"; return TCL_ERROR;
        }
    if (count+2 < argc) { // adding to stop seg faults
      if (Tcl_GetDouble(interp, argv[count+2], &zf) != TCL_OK) {
            opserr << "WARNING eleLoad - invalid zFactor " << argv[count+2] << "
for -selfWeight\n"; return TCL_ERROR;
      }
    }

    for (int i=0; i<theEleTags.Size(); i++) {
      theLoad = new SelfWeight(eleLoadTag, xf, yf, zf, theEleTags(i));

      if (theLoad == 0) {
        opserr << "WARNING eleLoad - out of memory creating load of type " <<
argv[count] ; return TCL_ERROR;
      }

      // get the current pattern tag if no tag given in i/p
      int loadPatternTag = theTclLoadPattern->getTag();

      // add the load to the domain
      if (theTclDomain->addElementalLoad(theLoad, loadPatternTag) == false) {
        opserr << "WARNING eleLoad - could not add following load to domain:\n
"; opserr << theLoad; delete theLoad; return TCL_ERROR;
      }
      eleLoadTag++;
    }
    return 0;
  }

 ///---------------------- Adding identifier for ThermalAction : [END]
[SIF]----------------------------------------------//
  //-----------------Adding tcl command for shell thermal action,
2013..[Begin]--------------------- else if (strcmp(argv[count], "-shellThermal")
== 0) { count++;
          //so far three kinds of temperature distribution
          //(1) 9 temperature points, i.e. 8 layers
          //(2) 5 temperature points, i.e. 4 layers
          //(3) 2 temperature points, i.e. 1 layers: linear or uniform

          double t1, locY1, t2, locY2; //t3, locY3, t4, locY4, t5, locY5, t6,
locY6, t7, locY7, t8, locY8, t9, locY9;
          // 9 temperature points are given,i.e. 8 layers are defined; Also the
9 corresponding vertical coordinate is given.
          // the temperature at each fiber is obtained by interpolating of
temperatures at the nearby temperature points.
          //Start to add source file
          if (strcmp(argv[count], "-source") == 0) {
                  if (strcmp(argv[count + 1], "-node") != 0) {
                          count++;

                          const char *pwd = getInterpPWD(interp);
                          // simulationInfo.addInputFile(argv[count], pwd);
                          TimeSeries* theSeries = new
PathTimeSeriesThermal(eleLoadTag, argv[count]);

                          count++;

                          double RcvLoc1, RcvLoc2;
                          if (argc - count == 2) {

                                  if (Tcl_GetDouble(interp, argv[count],
&RcvLoc1) != TCL_OK) { opserr << "WARNING eleLoad - invalid single loc  " <<
argv[count] << " for -beamThermal\n"; return TCL_ERROR;
                                  }
                                  if (Tcl_GetDouble(interp, argv[count + 1],
&RcvLoc2) != TCL_OK) { opserr << "WARNING eleLoad - invalid single loc  " <<
argv[count + 1] << " for -beamThermal\n"; return TCL_ERROR;
                                  }

                          }
                          else {
                                  opserr << "WARNING eleLoad - invalid input for
-shellThermal\n";
                          }

                          for (int i = 0; i<theEleTags.Size(); i++) {
                                  theLoad = new ShellThermalAction(eleLoadTag,
RcvLoc1, RcvLoc2, theSeries, theEleTags(i)); if (theLoad == 0) { opserr <<
"WARNING eleLoad - out of memory creating load of type " << argv[count]; return
TCL_ERROR;
                                  }

                                  // get the current pattern tag if no tag given
in i/p int loadPatternTag = theTclLoadPattern->getTag();

                                  // add the load to the domain
                                  if (theTclDomain->addElementalLoad(theLoad,
loadPatternTag) == false) { opserr << "WARNING eleLoad - could not add following
load to domain:\n "; opserr << theLoad; delete theLoad; return TCL_ERROR;
                                  }
                                  eleLoadTag++;
                          }
                  }
                  //if not using nodal thermal action input
                  else {
                          for (int i = 0; i<theEleTags.Size(); i++) {
                                  theLoad = new ShellThermalAction(eleLoadTag,
theEleTags(i)); if (theLoad == 0) { opserr << "WARNING eleLoad - out of memory
creating load of type " << argv[count]; return TCL_ERROR;
                                  }

                                  // get the current pattern tag if no tag given
in i/p int loadPatternTag = theTclLoadPattern->getTag();

                                  // add the load to the domain
                                  if (theTclDomain->addElementalLoad(theLoad,
loadPatternTag) == false) { opserr << "WARNING eleLoad - could not add following
load to domain:\n "; opserr << theLoad; delete theLoad; return TCL_ERROR;
                                  }
                                  eleLoadTag++;
                          }//end of for loop
                          return 0;
                  }//end of <if(strcmp(argv[count+1],"-node") = 0)>
          }
          //end of the interface for importing temperature data from external
file else
          {
                  if (argc - count == 18) {
                          double indata[18];
                          double BufferData;

                          for (int i = 0; i<18; i++) {
                                  if (Tcl_GetDouble(interp, argv[count],
&BufferData) != TCL_OK) { opserr << "WARNING eleLoad - invalid data " <<
argv[count] << " for -beamThermal 3D\n"; return TCL_ERROR;
                                  }
                                  indata[i] = BufferData;
                                  count++;
                          }

                          //temp1,loc1,temp2,loc2...temp9,loc9

                          for (int i = 0; i<theEleTags.Size(); i++) {
                                  theLoad = new ShellThermalAction(eleLoadTag,
                                          indata[0], indata[1], indata[2],
indata[3], indata[4], indata[5], indata[6], indata[7], indata[8], indata[9],
indata[10], indata[11], indata[12], indata[13], indata[14], indata[15],
                                          indata[16], indata[17],
theEleTags(i));


                                  if (theLoad == 0) {
                                          opserr << "WARNING eleLoad - out of
memory creating load of type " << argv[count]; return TCL_ERROR;
                                  }

                                  // get the current pattern tag if no tag given
in i/p int loadPatternTag = theTclLoadPattern->getTag();

                                  // add the load to the domain
                                  if (theTclDomain->addElementalLoad(theLoad,
loadPatternTag) == false) { opserr << "WARNING eleLoad - could not add following
load to domain:\n "; opserr << theLoad; delete theLoad; return TCL_ERROR;
                                  }
                                  eleLoadTag++;
                          }
                          return 0;
                  }
                  // 5 temperatures are given, i.e. 4 layers are defined.
                  else if (argc - count == 10) {
                          double indata[10];
                          double BufferData;

                          for (int i = 0; i<10; i++) {
                                  if (Tcl_GetDouble(interp, argv[count],
&BufferData) != TCL_OK) { opserr << "WARNING eleLoad - invalid data " <<
argv[count] << " for -beamThermal 3D\n"; return TCL_ERROR;
                                  }
                                  indata[i] = BufferData;
                                  count++;
                          }

                          //temp1,loc1,temp2,loc2...temp5,loc5

                          for (int i = 0; i<theEleTags.Size(); i++) {
                                  theLoad = new ShellThermalAction(eleLoadTag,
                                          indata[0], indata[1], indata[2],
indata[3], indata[4], indata[5], indata[6], indata[7], indata[8], indata[9],
theEleTags(i));


                                  if (theLoad == 0) {
                                          opserr << "WARNING eleLoad - out of
memory creating load of type " << argv[count]; return TCL_ERROR;
                                  }

                                  // get the current pattern tag if no tag given
in i/p int loadPatternTag = theTclLoadPattern->getTag();

                                  // add the load to the domain
                                  if (theTclDomain->addElementalLoad(theLoad,
loadPatternTag) == false) { opserr << "WARNING eleLoad - could not add following
load to domain:\n "; opserr << theLoad; delete theLoad; return TCL_ERROR;
                                  }
                                  eleLoadTag++;
                          }
                          return 0;
                  }
                  // two temperature is given,
                  //if the two temperatures are equal,i.e. uniform Temperature
change in element
                  //if the two temperatures are different,i.e. linear
Temperature change in element else if (argc - count == 4) { if
(Tcl_GetDouble(interp, argv[count], &t1) != TCL_OK) { opserr << "WARNING eleLoad
- invalid T1 " << argv[count] << " for -shellThermal\n"; return TCL_ERROR;
                          }

                          if (Tcl_GetDouble(interp, argv[count + 1], &locY1) !=
TCL_OK) { opserr << "WARNING eleLoad - invalid LocY1 " << argv[count + 1] << "
for -shellThermal\n"; return TCL_ERROR;
                          }
                          if (Tcl_GetDouble(interp, argv[count + 2], &t2) !=
TCL_OK) { opserr << "WARNING eleLoad - invalid T2 " << argv[count] << " for
-shellThermal\n"; return TCL_ERROR;
                          }

                          if (Tcl_GetDouble(interp, argv[count + 3], &locY2) !=
TCL_OK) { opserr << "WARNING eleLoad - invalid LocY2 " << argv[count + 1] << "
for -shellThermal\n"; return TCL_ERROR;
                          }

                          for (int i = 0; i<theEleTags.Size(); i++) {
                                  theLoad = new ShellThermalAction(eleLoadTag,
                                          t1, locY1, t2, locY2, theEleTags(i));


                                  if (theLoad == 0) {
                                          opserr << "WARNING eleLoad - out of
memory creating load of type " << argv[count]; return TCL_ERROR;
                                  }

                                  // get the current pattern tag if no tag given
in i/p int loadPatternTag = theTclLoadPattern->getTag();

                                  // add the load to the domain
                                  if (theTclDomain->addElementalLoad(theLoad,
loadPatternTag) == false) { opserr << "WARNING eleLoad - could not add following
load to domain:\n "; opserr << theLoad; delete theLoad; return TCL_ERROR;
                                  }
                                  eleLoadTag++;
                          }
                          return 0;
                  }
                  //finish the temperature arguments
                  else {
                          opserr << "WARNING eleLoad -shellThermalLoad invalid
number of temperature aguments,/n looking for 0, 1, 2 or 4 arguments.\n";
                  }
          }
          //end of if(recieved argument is not "source" or direct temperature
input)//Liming,2014
  }
  //-----------------Adding tcl command for shell thermal action,
2013..[End]-----------------------

  else if (strcmp(argv[count], "-ThermalWrapper") == 0 || strcmp(argv[count],
"-thermalWrapper") == 0) {

          count++;
          Vector loc = 0;;
          ID NodalThermal = 0;
          int numNodal = 0;

          if (strcmp(argv[count], "-nodeLoc") == 0 || strcmp(argv[count],
"nodeLoc") == 0)
          {
                  count++;
                  numNodal = (argc - count) / 2;
                  loc.resize(numNodal);
                  NodalThermal.resize(numNodal);

                  for (int i = 0; i<numNodal; i++) {

                          double Dblloc;
                          int NodalTtag;

                          if (Tcl_GetDouble(interp, argv[count + i * 2 + 1],
&Dblloc) != TCL_OK) { opserr << "WARNING NodalLoad - invalid loc  " <<
argv[count] << " for NodalThermalAction\n"; return TCL_ERROR;
                          }

                          if (Tcl_GetInt(interp, argv[count + 2 * i],
&NodalTtag) != TCL_OK) { opserr << "WARNING invalid nodeId: " << argv[1]; return
TCL_ERROR;
                          }

                          loc(i) = Dblloc; NodalThermal(i) = NodalTtag;
                  }
                  //end of for loop over numNodal
          }
          //end of nodelLoc
          else if (strcmp(argv[count], "-node") == 0 || strcmp(argv[count],
"node") == 0)
          {
                  count++;
                  numNodal = argc - count;
                  NodalThermal.resize(numNodal);

                  for (int i = 0; i<numNodal; i++) {

                          int NodalTtag;

                          if (Tcl_GetInt(interp, argv[count + i], &NodalTtag) !=
TCL_OK) { opserr << "WARNING invalid nodeId: " << argv[1]; return TCL_ERROR;
                          }

                          NodalThermal(i) = NodalTtag;
                  }
                  //end of for loop over numNodal
          }
          //end of node tag

          //Obtain Pointers to NodalThermalAction;
          Node* theNode = 0;
          NodalThermalAction** theNodalThermals = 0;
          theNodalThermals = new NodalThermalAction*[numNodal];

          for (int i = 0; i<numNodal; i++) {
            theNode = theTclDomain->getNode(NodalThermal(i));
            theNodalThermals[i] = theNode->getNodalThermalActionPtr();
            if (theNodalThermals[i] == 0) {
              opserr << "WARNING:: An empty nodalThermalAction detected for
ThermalActionWrapper" << endln; return TCL_ERROR;
            }
          }

          for (int i = 0; i<theEleTags.Size(); i++) {
            if (numNodal == 2) {
              theLoad = new ThermalActionWrapper(eleLoadTag, theEleTags(i),
theNodalThermals[0], theNodalThermals[1]);
            }
            else if (numNodal == 3) {
              theLoad = new ThermalActionWrapper(eleLoadTag, theEleTags(i),
theNodalThermals[0], theNodalThermals[1], theNodalThermals[2]);
            }
            else if (numNodal == 4) {
              theLoad = new ThermalActionWrapper(eleLoadTag, theEleTags(i),
theNodalThermals[0], theNodalThermals[1], theNodalThermals[2],
theNodalThermals[3]);
            }
            else if (numNodal == 5) {
              theLoad = new ThermalActionWrapper(eleLoadTag, theEleTags(i),
theNodalThermals[0], theNodalThermals[1], theNodalThermals[2],
theNodalThermals[3], theNodalThermals[4]);
            }
            else if (numNodal == 6) {
              theLoad = new ThermalActionWrapper(eleLoadTag, theEleTags(i),
theNodalThermals[0], theNodalThermals[1], theNodalThermals[2],
theNodalThermals[3], theNodalThermals[4], theNodalThermals[5]);
            }


            if (theLoad == 0) {
              opserr << "WARNING eleLoad - out of memory creating load of type "
<< argv[count]; return TCL_ERROR;
                  }

                  if (loc != 0)
                          ((ThermalActionWrapper*)theLoad)->setRatios(loc);

                  // get the current pattern tag if no tag given in i/p
                  int loadPatternTag = theTclLoadPattern->getTag();

                  // add the load to the domain
                  if (theTclDomain->addElementalLoad(theLoad, loadPatternTag) ==
false) { opserr << "WARNING eleLoad - could not add following load to domain:\n
"; opserr << theLoad; delete theLoad; return TCL_ERROR;
                  }
                  eleLoadTag++;
          }//end of for loop
          return 0;
  }
  //------------------------end  of using
ThermalActionWrapper--------------------------
  //-----------------Adding tcl command for beam thermal action(2D&3D),
2013..[Begin]---------------

  else if (strcmp(argv[count], "-beamThermal") == 0) {
          count++;
          //For two dimensional model
          if (ndm == 2) {
                  // The thermal action can be defined with external data file,
which is identified with "-source"
                  // The external file itself can either be elemental data or
nodal data, the latter is identified with "-node" if (strcmp(argv[count],
"-source") == 0) {

                          if (strcmp(argv[count + 1], "-node") == 0) {
                                  for (int i = 0; i<theEleTags.Size(); i++) {
                                          theLoad = new
Beam2dThermalAction(eleLoadTag, theEleTags(i)); if (theLoad == 0) { opserr <<
"WARNING eleLoad - out of memory creating load of type " << argv[count]; return
TCL_ERROR;
                                          }

                                          // get the current pattern tag if no
tag given in i/p int loadPatternTag = theTclLoadPattern->getTag();

                                          // add the load to the domain
                                          if
(theTclDomain->addElementalLoad(theLoad, loadPatternTag) == false) { opserr <<
"WARNING eleLoad - could not add following load to domain:\n "; opserr <<
theLoad; delete theLoad; return TCL_ERROR;
                                          }
                                          eleLoadTag++;
                                  }//end of for loop
                                  return 0;

                          }
                          //end of <if(strcmp(argv[count+1],"-node") != 0)>
                          else {
                                  count++;
                                  const char *pwd = getInterpPWD(interp);
                                  // simulationInfo.addInputFile(argv[count],
pwd); TimeSeries* theSeries = new PathTimeSeriesThermal(eleLoadTag,
argv[count]);

                                  count++;
                                  Vector locs(9);
                                  //---------------for recieving 2 arguments
                                  if (argc - count == 2) {
                                          double RcvLoc1, RcvLoc2;
                                          if (Tcl_GetDouble(interp, argv[count],
&RcvLoc1) != TCL_OK) { opserr << "WARNING eleLoad - invalid single loc  " <<
argv[count] << " for -beamThermal\n"; return TCL_ERROR;
                                          }
                                          if (Tcl_GetDouble(interp, argv[count +
1], &RcvLoc2) != TCL_OK) { opserr << "WARNING eleLoad - invalid single loc  " <<
argv[count + 1] << " for -beamThermal\n"; return TCL_ERROR;
                                          }

                                          locs(0) = RcvLoc1; locs(8) = RcvLoc2;
                                          for (int i = 1; i<8; i++) {
                                                  locs(i) = locs(0) - i*(locs(0)
- locs(8)) / 8;
                                          }
                                  }
                                  //----------------for recieving 9 arguments
                                  else if (argc - count == 9) {

                                          int ArgStart = count;
                                          int ArgEnd = argc;
                                          double data;

                                          if (ArgStart != ArgEnd) {
                                                  for (int i = ArgStart;
i<ArgEnd; i++) { Tcl_GetDouble(interp, argv[i], &data); locs(i - ArgStart) =
data;
                                                  }
                                          }

                                  }
                                  //end of recieving 9 arguments
                                  else {
                                          opserr << "WARNING eleLoad - invalid
input for -beamThermal\n";
                                  }
#ifdef _DEBUG
                                  opserr << "TclSafeBuilder:: locs" <<
locs << endln; #endif for (int i = 0; i<theEleTags.Size(); i++) { theLoad = new
Beam2dThermalAction(eleLoadTag, locs, theSeries, theEleTags(i)); if (theLoad ==
0) { opserr << "WARNING eleLoad - out of memory creating load of type " <<
argv[count]; return TCL_ERROR;
                                          }

                                          // get the current pattern tag if no
tag given in i/p int loadPatternTag = theTclLoadPattern->getTag();

                                          // add the load to the domain
                                          if
(theTclDomain->addElementalLoad(theLoad, loadPatternTag) == false) { opserr <<
"WARNING eleLoad - could not add following load to domain:\n "; opserr <<
theLoad; delete theLoad; return TCL_ERROR;
                                          }
                                          eleLoadTag++;
                                  }//end of for loop
                                  return 0;
                          }//end of <if(strcmp(argv[count+1],"-node") = 0)>
                           //--------------------------end for
beam2DThermalAction with time series ----------------------------------------
                  }
                  else
                  {
                          //(1) 9 temperature points, i.e. 8 layers
                          //(2) 5 temperature points, i.e. 4 layers
                          //(3) 2 temperature points, i.e. 1 layers: linear or
uniform
                          //double t1, locY1, t2, locY2, t3, locY3, t4, locY4,
t5, locY5, t6, locY6, t7, locY7, t8, locY8, t9, locY9; double Temp[9]; double
Loc[9];
                          // 9 temperature points are given,i.e. 8 layers are
defined; Also the 9 corresponding vertical coordinate is given.
                          // the temperature at each fiber is obtained by
interpolating of temperatures at the nearby temperature points. if (argc - count
== 18) { double indata[18]; double BufferData;

                                  for (int i = 0; i<18; i++) {
                                          if (Tcl_GetDouble(interp, argv[count],
&BufferData) != TCL_OK) { opserr << "WARNING eleLoad - invalid data " <<
argv[count] << " for -beamThermal 3D\n"; return TCL_ERROR;
                                          }
                                          indata[i] = BufferData;
                                          count++;
                                  }

                                  for (int i = 0; i<9; i++) {
                                          Temp[i] = indata[2 * i];
                                          Loc[i]  = indata[2 * i + 1];
                                  }

                          }

                          // 5 temperatures are given, i.e. 4 layers are
defined. else if (argc - count == 10) {

                                  double indata[10];
                                  double BufferData;

                                  for (int i = 0; i<10; i++) {
                                          if (Tcl_GetDouble(interp, argv[count],
&BufferData) != TCL_OK) { opserr << "WARNING eleLoad - invalid data " <<
argv[count] << " for -beamThermal 3D\n"; return TCL_ERROR;
                                          }
                                          indata[i] = BufferData;
                                          count++;
                                  }

                                  Temp[0] = indata[0]; Temp[2] = indata[2];
Temp[4] = indata[4]; Temp[6] = indata[6]; Temp[8] = indata[8]; Loc[0]  =
indata[1]; Loc[2]  = indata[3]; Loc[4]  = indata[5]; Loc[6]  = indata[7]; Loc[8]
= indata[9];

                                  for (int i = 1; i<5; i++) {
                                          Temp[2 * i - 1] = (Temp[2 * i - 2] +
Temp[2 * i]) / 2; Loc[2 * i - 1]  = (Loc[2 * i - 2] + Loc[2 * i]) / 2;
                                  }
                          }
                          //End for 5 inputs
                          // two temperature is given,
                          //if the two temperatures are equal,i.e. uniform
Temperature change in element
                          //if the two temperatures are different,i.e. linear
Temperature change in element else if (argc - count == 4) { double indata[4];
                                  double BufferData;

                                  for (int i = 0; i<4; i++) {
                                          if (Tcl_GetDouble(interp, argv[count],
&BufferData) != TCL_OK) { opserr << "WARNING eleLoad - invalid data " <<
argv[count] << " for -beamThermal 3D\n"; return TCL_ERROR;
                                          }
                                          indata[i] = BufferData;
                                          count++;
                                  }

                                  Temp[0] = indata[0]; Temp[8] = indata[2];
                                  Loc[0]  = indata[1]; Loc[8]  = indata[3];
                                  for (int i = 1; i<8; i++) {
                                          Temp[i] = Temp[0] - i*(Temp[0] -
Temp[8]) / 8; Loc[i] = Loc[0] - i*(Loc[0] - Loc[8]) / 8;
                                  }

                          }
                          //end for 2 inputs

                          for (int i = 0; i<theEleTags.Size(); i++) {
                                  theLoad = new Beam2dThermalAction(eleLoadTag,
                                          Temp[0], Loc[0], Temp[1], Loc[1],
                                          Temp[2], Loc[2], Temp[3], Loc[3],
                                          Temp[4], Loc[4], Temp[5], Loc[5],
                                          Temp[6], Loc[6], Temp[7], Loc[7],
                      Temp[8], Loc[8], theEleTags(i));

                                  if (theLoad == 0) {
                                          opserr << "WARNING eleLoad - out of
memory creating load of type beamThermal"; return TCL_ERROR;
                                  }

                                  // get the current pattern tag if no tag given
in i/p int loadPatternTag = theTclLoadPattern->getTag();

                                  // add the load to the domain
                                  if (theTclDomain->addElementalLoad(theLoad,
loadPatternTag) == false) { opserr << "WARNING eleLoad - could not add following
load to domain:\n "; opserr << theLoad; delete theLoad; return TCL_ERROR;
                                  }
                                  eleLoadTag++;
                          }

                          return 0;
                          //-End of Adding BeamThermalAction defined with direct
input.
                  }
                  //end for no sourced pattern
          }
          // End of for the if (ndm==2)
          else if (ndm == 3)
          {
                  //so far three kinds of temperature distribution
                  double t1, locY1, t2, locY2, t3, locY3, t4, locY4, t5, locY5,
                          t6, t7, locZ1, t8, t9, locZ2, t10, t11, locZ3, t12,
t13, locZ4, t14, t15, locZ5;

                  // the temperature at each fiber is obtained by interpolating
of temperatures at the nearby temperature points. if (strcmp(argv[count],
"-source") == 0) { count++; if (strcmp(argv[count], "-node") == 0) { for (int i
= 0; i<theEleTags.Size(); i++) { theLoad = new Beam3dThermalAction(eleLoadTag,
theEleTags(i)); if (theLoad == 0) { opserr << "WARNING eleLoad - out of memory
creating load of type " << argv[count]; return TCL_ERROR;
                                          }
                                          // get the current pattern tag if no
tag given in i/p int loadPatternTag = theTclLoadPattern->getTag();
                                          // add the load to the domain
                                          if
(theTclDomain->addElementalLoad(theLoad, loadPatternTag) == false) { opserr <<
"WARNING eleLoad - could not add following load to domain:\n "; opserr <<
theLoad; delete theLoad; return TCL_ERROR;
                                          }
                                          eleLoadTag++;
                                  }	//end of loop tf all elements defined
                                  return 0;
                          }//end for defing thermal action with nodal input
                          else {
                                  const char *pwd = getInterpPWD(interp);
                                  // simulationInfo.addInputFile(argv[count],
pwd); count++; bool using2Ddata = false;

                                  double RcvLoc1, RcvLoc2, RcvLoc3, RcvLoc4;
                                  TimeSeries* theSeries;

                                  if (argc - count == 4) {
                                          theSeries = new
PathTimeSeriesThermal(eleLoadTag, argv[count - 1], 15); using2Ddata = false;

                                          if (Tcl_GetDouble(interp, argv[count],
&RcvLoc1) != TCL_OK) { opserr << "WARNING eleLoad - invalid single loc  " <<
argv[count] << " for -beamThermal\n"; return TCL_ERROR;
                                          }
                                          if (Tcl_GetDouble(interp, argv[count +
1], &RcvLoc2) != TCL_OK) { opserr << "WARNING eleLoad - invalid single loc  " <<
argv[count + 1] << " for -beamThermal\n"; return TCL_ERROR;
                                          }
                                          if (Tcl_GetDouble(interp, argv[count +
2], &RcvLoc3) != TCL_OK) { opserr << "WARNING eleLoad - invalid single loc  " <<
argv[count + 2] << " for -beamThermal\n"; return TCL_ERROR;
                                          }
                                          if (Tcl_GetDouble(interp, argv[count +
3], &RcvLoc4) != TCL_OK) { opserr << "WARNING eleLoad - invalid single loc  " <<
argv[count + 3] << " for -beamThermal\n"; return TCL_ERROR;
                                          }

                                          //end for recieving input
                                          for (int i = 0; i<theEleTags.Size();
i++) {

                                                  theLoad = new
Beam3dThermalAction(eleLoadTag, RcvLoc1, RcvLoc2, RcvLoc3, RcvLoc4, theSeries,
theEleTags(i));

                                                  if (theLoad == 0) {
                                                          opserr << "WARNING
eleLoad - out of memory creating load of type " << argv[count]; return
TCL_ERROR;
                                                  }

                                                  // get the current pattern tag
if no tag given in i/p int loadPatternTag = theTclLoadPattern->getTag();

                                                  // add the load to the domain
                                                  if
(theTclDomain->addElementalLoad(theLoad, loadPatternTag) == false) { opserr <<
"WARNING eleLoad - could not add following load to domain:\n "; opserr <<
theLoad; delete theLoad; return TCL_ERROR;
                                                  }
                                                  eleLoadTag++;
                                          }
                                          return 0;

                                  }
                                  //end of defining 15 data points with external
file else if (argc - count == 2 || argc - count == 9) {
                                          // for receiving data which has the
similiar structure as 2D beam section Vector locs(9); using2Ddata = true;
                                          TimeSeries* theSeries = new
PathTimeSeriesThermal(eleLoadTag, argv[count - 1], 9); if (argc - count == 2) {

                                                  double RcvLoc1, RcvLoc2;
                                                  if (Tcl_GetDouble(interp,
argv[count], &RcvLoc1) != TCL_OK) { opserr << "WARNING eleLoad - invalid single
loc  " << argv[count] << " for -beamThermal\n"; return TCL_ERROR;
                                                  }
                                                  if (Tcl_GetDouble(interp,
argv[count + 1], &RcvLoc2) != TCL_OK) { opserr << "WARNING eleLoad - invalid
single loc  " << argv[count + 1] << " for -beamThermal\n"; return TCL_ERROR;
                                                  }

                                                  locs(0) = RcvLoc1; locs(8) =
RcvLoc2; for (int i = 1; i<8; i++) { locs(i) = locs(0) - i*(locs(0) - locs(8)) /
8;
                                                  }
                                          }
                                          //end of receiving 2 data points
                                          else {
                                                  double indata[9];
                                                  double BufferData;
                                                  for (int i = 0; i<9; i++) {
                                                          if
(Tcl_GetDouble(interp, argv[count], &BufferData) != TCL_OK) { opserr << "WARNING
eleLoad - invalid data " << argv[count] << " for -beamThermal 3D\n"; return
TCL_ERROR;
                                                          }
                                                          indata[i] =
BufferData; count++;
                                                  }

                                                  for (int i = 0; i<9; i++) {
                                                          locs(i) = indata[i];
                                                  }

                                          }
                                          //end of receiving 9data points

                                          for (int i = 0; i<theEleTags.Size();
i++) { theLoad = new Beam3dThermalAction(eleLoadTag, locs, theSeries,
theEleTags(i)); if (theLoad == 0) { opserr << "WARNING eleLoad - out of memory
creating load of type " << argv[count]; return TCL_ERROR;
                                                  }

                                                  // get the current pattern tag
if no tag given in i/p int loadPatternTag = theTclLoadPattern->getTag();

                                                  // add the load to the domain
                                                  if
(theTclDomain->addElementalLoad(theLoad, loadPatternTag) == false) { opserr <<
"WARNING eleLoad - could not add following load to domain:\n "; opserr <<
theLoad; delete theLoad; return TCL_ERROR;
                                                  }
                                                  eleLoadTag++;
                                          }//end of for loop
                                          return 0;
                                  }

                                  else {
                                          opserr << "WARNING eleLoad - invalid
input for -beamThermal\n";
                                  }

                          }//end for source beam element temperature data
-source -filePath

                  }
                  //-------------------------end for importing temp data from
external files --source ------------------------- else {

                          //double t1, locY1, t2, locY2, t3, locY3, t4, locY4,
t5, locY5,
                          //t6, t7, locZ1, t8, t9, locZ2, t10,t11, locZ3, t12,
t13, locZ4, t14,t15, locZ5;

                          if (argc - count == 25) {
                                  double indata[25];
                                  double BufferData;

                                  for (int i = 0; i<25; i++) {
                                          if (Tcl_GetDouble(interp, argv[count],
&BufferData) != TCL_OK) { opserr << "WARNING eleLoad - invalid data " <<
argv[count] << " for -beamThermal 3D\n"; return TCL_ERROR;
                                          }
                                          indata[i] = BufferData;
                                          count++;
                                  }

                                  for (int i = 0; i<theEleTags.Size(); i++) {
                                          theLoad = new
Beam3dThermalAction(eleLoadTag, indata[0], indata[1], indata[2], indata[3],
indata[4], indata[5], indata[6], indata[7], indata[8], indata[9], indata[10],
indata[11], indata[12], indata[13], indata[14], indata[15], indata[16],
indata[17], indata[18], indata[19], indata[20], indata[21], indata[22],
indata[23], indata[24], theEleTags(i)); if (theLoad == 0) { opserr << "WARNING
eleLoad - out of memory creating load of type " << argv[count]; return
TCL_ERROR;
                                          }
                                          // get the current pattern tag if no
tag given in i/p int loadPatternTag = theTclLoadPattern->getTag();

                                          // add the load to the domain
                                          if
(theTclDomain->addElementalLoad(theLoad, loadPatternTag) == false) { opserr <<
"WARNING eleLoad - could not add following load to domain:\n "; opserr <<
theLoad; delete theLoad; return TCL_ERROR;
                                          }
                                          eleLoadTag++;
                                  }
                                  return 0;
                          }
                          //end of  if (argc-count == 25){
                          else if (argc - count == 4) {

                                  if (Tcl_GetDouble(interp, argv[count], &t1) !=
TCL_OK) { opserr << "WARNING eleLoad - invalid T1 " << argv[count] << " for
-beamThermal\n"; return TCL_ERROR;
                                  }
                                  if (Tcl_GetDouble(interp, argv[count + 1],
&locY1) != TCL_OK) { opserr << "WARNING eleLoad - invalid LocY1 " << argv[count
+ 1] << " for -beamThermal\n"; return TCL_ERROR;
                                  }
                                  if (Tcl_GetDouble(interp, argv[count + 2],
&t5) != TCL_OK) { opserr << "WARNING eleLoad - invalid T1 " << argv[count] << "
for -beamThermal\n"; return TCL_ERROR;
                                  }
                                  if (Tcl_GetDouble(interp, argv[count + 3],
&locY5) != TCL_OK) { opserr << "WARNING eleLoad - invalid LocY1 " << argv[count
+ 1] << " for -beamThermal\n"; return TCL_ERROR;
                                  }

                                  locY2 = locY1 + (locY5 - locY1) / 4;
                                  locY3 = locY1 + (locY5 - locY1) / 2;
                                  locY4 = locY1 + 3 * (locY5 - locY1) / 4;
                                  t2 = t1 + (t5 - t1) / 4;
                                  t3 = t1 + (t5 - t1) / 2;
                                  t4 = t1 + 3 * (t5 - t1) / 4;
                                  locZ1 = locZ2 = locZ3 = locZ4 = locZ5 = 0;
                                  t6 = t7 = t8 = t9 = t10 = 0;
                                  t11 = t12 = t13 = t14 = t15 = 0;

                                  for (int i = 0; i<theEleTags.Size(); i++) {
                                          theLoad = new
Beam3dThermalAction(eleLoadTag, t1, locY1, t2, locY2, t3, locY3, t4, locY4, t5,
locY5, t6, t7, locZ1, t8, t9, locZ2, t10, t11, locZ3, t12, t13, locZ4, t14, t15,
locZ5, theEleTags(i)); if (theLoad == 0) { opserr << "WARNING eleLoad - out of
memory creating load of type " << argv[count]; return TCL_ERROR;
                                          }
                                          // get the current pattern tag if no
tag given in i/p int loadPatternTag = theTclLoadPattern->getTag();

                                          // add the load to the domain
                                          if
(theTclDomain->addElementalLoad(theLoad, loadPatternTag) == false) { opserr <<
"WARNING eleLoad - could not add following load to domain:\n "; opserr <<
theLoad; delete theLoad; return TCL_ERROR;
                                          }
                                          eleLoadTag++;
                                  }
                                  return 0;
                          }
                          //end of  if (argc-count == 4){
                          else {
                                  opserr << "WARNING eleLoad
Beam3dThermalAction: invalid number of temperature aguments,/n looking for
arguments for Temperatures and cordinates.\n";
                          }
                  } //end for not sourced pattern
          }//else if ndm=3
  }//else if '-beamThermal'

   //--Adding identifier for ThermalAction:[END] [SIF]--//



  // Added by Scott R. Hamilton   - Stanford
  else if (strcmp(argv[count],"-beamTemp") == 0) {
    count++;
    if (ndm == 2) {
      double temp1, temp2, temp3, temp4;

      // Four temps given, Temp change at top node 1, bottom node 1, top node 2,
bottom node 2. if (argc-count == 4){ if (Tcl_GetDouble(interp, argv[count],
&temp1) != TCL_OK) { opserr << "WARNING eleLoad - invalid Ttop1 " << argv[count]
<< " for -beamTemp\n"; return TCL_ERROR;
        }

        if (Tcl_GetDouble(interp, argv[count+1],&temp2 ) != TCL_OK) {
          opserr << "WARNING eleLoad - invalid Tbot1 " << argv[count+1] << " for
-beamTemp\n"; return TCL_ERROR;
        }
        if (Tcl_GetDouble(interp, argv[count+2], &temp3) != TCL_OK) {
          opserr << "WARNING eleLoad - invalid Ttop2 " << argv[count+1] << " for
-beamTemp\n"; return TCL_ERROR;
        }
        if (Tcl_GetDouble(interp, argv[count+3], &temp4) != TCL_OK) {
          opserr << "WARNING eleLoad - invalid Tbot2 " << argv[count+1] << " for
-beamTemp\n"; return TCL_ERROR;
        }

        for (int i=0; i<theEleTags.Size(); i++) {
          theLoad = new Beam2dTempLoad(eleLoadTag, temp1, temp2, temp3, temp4,
theEleTags(i));

          if (theLoad == 0) {
            opserr << "WARNING eleLoad - out of memory creating load of type "
<< argv[count] ; return TCL_ERROR;
          }

          // get the current pattern tag if no tag given in i/p
          int loadPatternTag = theTclLoadPattern->getTag();

          // add the load to the domain
          if (theTclDomain->addElementalLoad(theLoad, loadPatternTag) == false)
{ opserr << "WARNING eleLoad - could not add following load to domain:\n ";
            opserr << theLoad;
            delete theLoad;
            return TCL_ERROR;
          }
          eleLoadTag++;
        }

        return 0;

      }
      // Two temps given, temp change at top, temp at bottom of element
      else if (argc-count == 2) {
        if (Tcl_GetDouble(interp, argv[count], &temp1) != TCL_OK) {
          opserr << "WARNING eleLoad - invalid Ttop " << argv[count] << " for
-beamTemp\n"; return TCL_ERROR;
        }

        if (Tcl_GetDouble(interp, argv[count+1],&temp2 ) != TCL_OK) {
          opserr << "WARNING eleLoad - invalid Tbot " << argv[count+1] << " for
-beamTemp\n"; return TCL_ERROR;
        }

        for (int i=0; i<theEleTags.Size(); i++) {
          theLoad = new Beam2dTempLoad(eleLoadTag, temp1, temp2, theEleTags(i));

          if (theLoad == 0) {
            opserr << "WARNING eleLoad - out of memory creating load of type "
<< argv[count] ; return TCL_ERROR;
          }

          // get the current pattern tag if no tag given in i/p
          int loadPatternTag = theTclLoadPattern->getTag();

          // add the load to the domain
          if (theTclDomain->addElementalLoad(theLoad, loadPatternTag) == false)
{ opserr << "WARNING eleLoad - could not add following load to domain:\n ";
            opserr << theLoad;
            delete theLoad;
            return TCL_ERROR;
          }
          eleLoadTag++;
        }
      }
      // One twmp change give, uniform temp change in element
      else if (argc-count == 1) {
        if (Tcl_GetDouble(interp, argv[count],&temp1 ) != TCL_OK) {
          opserr << "WARNING eleLoad - invalid Tbot " << argv[count+1] << " for
-beamTemp\n"; return TCL_ERROR;
        }
        theLoad=0;

        for (int i=0; i<theEleTags.Size(); i++) {
          theLoad = new Beam2dTempLoad(eleLoadTag, temp1, theEleTags(i));

          if (theLoad == 0) {
            opserr << "WARNING eleLoad - out of memory creating load of type "
<< argv[count] ; return TCL_ERROR;
          }

          // get the current pattern tag if no tag given in i/p
          int loadPatternTag = theTclLoadPattern->getTag();

          // add the load to the domain
          if (theTclDomain->addElementalLoad(theLoad, loadPatternTag) == false)
{ opserr << "WARNING eleLoad - could not add following load to domain:\n ";
            opserr << theLoad;
            delete theLoad;
            return TCL_ERROR;
          }
          eleLoadTag++;
        }

        return 0;

      }

      else {
        opserr << "WARNING eleLoad -beamTempLoad invalid number of temperature
aguments,/n looking for 0, 1, 2 or 4 arguments.\n";
      }
    } else {
      opserr << "WARNING eleLoad -beamTempLoad type currently only valid only
for ndm=2\n"; return TCL_ERROR;
    }
  }

  // if get here we have sucessfully created the load and added it to the domain
  return TCL_OK;
}
*/


int
TclCommand_addNodalMass(ClientData clientData, Tcl_Interp *interp, int argc,
                        TCL_Char **argv)
{
    G3_Runtime *rt = G3_getRuntime(interp);
    TclBuilder *theTclBuilder = G3_getModelBuilder(rt);
    Domain     *theTclDomain = G3_getDomain(rt);

  // ensure the destructor has not been called -
  if (theTclBuilder == 0) {
    opserr << "WARNING builder has been destroyed - load \n";
    return TCL_ERROR;
  }

  //  int ndf = theTclBuilder->getNDF();
  int ndf = argc - 2;

  // make sure at least one other argument to contain type of system
  if (argc < (2 + ndf)) {
    opserr << "WARNING bad command - want: mass nodeId " << ndf << " mass values\n"; 
    printCommand(argc, argv); 
    return TCL_ERROR;
  }

  // get the id of the node
  int nodeId;
  if (Tcl_GetInt(interp, argv[1], &nodeId) != TCL_OK) {
    opserr << "WARNING invalid nodeId: " << argv[1];
    opserr << " - mass nodeId " << ndf << " forces\n";
    return TCL_ERROR;
  }

  // check for mass terms
  Matrix mass(ndf,ndf);
  double theMass;
  for (int i=0; i<ndf; i++)
  {
     if (Tcl_GetDouble(interp, argv[i+2], &theMass) != TCL_OK)
     {
          opserr << "WARNING invalid nodal mass term\n";
          opserr << "node: " << nodeId << ", dof: " << i+1 << endln;
          return TCL_ERROR;
      }
      mass(i,i) = theMass;
  }

  if (theTclDomain->setMass(mass, nodeId) != 0) {
    opserr << "WARNING failed to set mass at node " << nodeId << endln;
    return TCL_ERROR;
  }

  // if get here we have sucessfully created the node and added it to the domain
  return TCL_OK;
}



/*


int
TclCommand_addImposedMotionSP(ClientData clientData,
                                   Tcl_Interp *interp,
                                   int argc,
                                   TCL_Char **argv)
{
  // ensure the destructor has not been called -
  if (theTclBuilder == 0) {
    opserr << "WARNING builder has been destroyed - sp \n";
    return TCL_ERROR;
  }

  //  int ndf = theTclBuilder->getNDF();

  // check number of arguments
  if (argc < 4) {
    opserr << "WARNING bad command - want: imposedMotion nodeId dofID
gMotionID\n"; printCommand(argc, argv); return TCL_ERROR;
  }

  // get the nodeID, dofId and value of the constraint
  int nodeId, dofId, gMotionID;

  if (Tcl_GetInt(interp, argv[1], &nodeId) != TCL_OK) {
    opserr << "WARNING invalid nodeId: " << argv[1];
    opserr << " - imposedMotion nodeId dofID gMotionID\n";
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[2], &dofId) != TCL_OK) {
    opserr << "WARNING invalid dofId: " << argv[2] << " -  imposedMotion ";
    opserr << nodeId << " dofID gMotionID\n";
      return TCL_ERROR;
  }
  dofId--; // DECREMENT THE DOF VALUE BY 1 TO GO TO OUR C++ INDEXING

  if (Tcl_GetInt(interp, argv[3], &gMotionID) != TCL_OK) {
    opserr << "WARNING invalid gMotionID: " << argv[3] << " -  imposedMotion ";
    opserr << nodeId << " dofID gMotionID\n";
    return TCL_ERROR;
  }

  bool alt = false;
  if (argc == 5) {
    if (strcmp(argv[4],"-other") == 0)
      alt = true;
  }

  //
  // check valid node & dof
  //

  Node *theNode = theTclDomain->getNode(nodeId);
  if (theNode == 0) {
    opserr << "WARNING invalid node " << argv[2] << " node not found\n ";
    return -1;
  }
  int nDof = theNode->getNumberDOF();
  if (dofId < 0 || dofId >= nDof) {
    opserr << "WARNING invalid dofId: " << argv[2] << " dof specified cannot be
<= 0 or greater than num dof at nod\n "; return -2;
  }


  MultiSupportPattern *thePattern = theTclMultiSupportPattern;
  int loadPatternTag = thePattern->getTag();

  // create a new ImposedMotionSP
  SP_Constraint *theSP;
  if (alt == true) {
    theSP = new ImposedMotionSP1(nodeId, dofId, loadPatternTag, gMotionID);
  }
  else {
    theSP = new ImposedMotionSP(nodeId, dofId, loadPatternTag, gMotionID);
  }

  if (theSP == 0) {
    opserr << "WARNING ran out of memory for ImposedMotionSP ";
    opserr << " -  imposedMotion ";
    opserr << nodeId << " " << dofId++ << " " << gMotionID << endln;
    return TCL_ERROR;
  }
  if (thePattern->addSP_Constraint(theSP) == false) {
    opserr << "WARNING could not add SP_Constraint to pattern ";
    printCommand(argc, argv);
    delete theSP;
    return TCL_ERROR;
  }

  // if get here we have sucessfully created the node and added it to the domain
  return TCL_OK;
}
*/



/*
int
TclCommand_addEqualDOF_MP_Mixed(ClientData clientData, Tcl_Interp *interp,
                                int argc, TCL_Char **argv)
{
        // Ensure the destructor has not been called
        if (theTclBuilder == 0) {
          opserr << "WARNING builder has been destroyed - equalDOF \n";
          return TCL_ERROR;
        }

        // Check number of arguments
        if (argc < 4) {
          opserr << "WARNING bad command - want: equalDOFmixed RnodeID? CnodeID?
numDOF? RDOF1? CDOF1? ... ..."; printCommand (argc, argv); return TCL_ERROR;
        }

        // Read in the node IDs and the DOF
        int RnodeID, CnodeID, dofIDR, dofIDC, numDOF;

        if (Tcl_GetInt (interp, argv[1], &RnodeID) != TCL_OK) {
          opserr << "WARNING invalid RnodeID: " << argv[1]
               << " equalDOF RnodeID? CnodeID? numDOF? RDOF1? CDOF1? ...";
          return TCL_ERROR;
        }
        if (Tcl_GetInt (interp, argv[2], &CnodeID) != TCL_OK) {
          opserr << "WARNING invalid CnodeID: " << argv[2]
               << " equalDOF RnodeID? CnodeID? numDOF? RDOF1? CDOF1? ...";
          return TCL_ERROR;
        }

        if (Tcl_GetInt (interp, argv[3], &numDOF) != TCL_OK) {
          opserr << "WARNING invalid numDOF: " << argv[2]
               << " equalDOF RnodeID? CnodeID? numDOF? RDOF1? CDOF1? ...";
          return TCL_ERROR;
        }

        // The number of DOF to be coupled
        //        int numDOF = argc - 3;

        // The constraint matrix ... U_c = C_cr * U_r
        Matrix Ccr (numDOF, numDOF);
        Ccr.Zero();

        // The vector containing the retained and constrained DOFs
        ID rDOF (numDOF);
        ID cDOF (numDOF);

        int i, j, k;
        // Read the degrees of freedom which are to be coupled
        for (i = 4, j = 5, k = 0; k < numDOF; i+=2, j+=2, k++) {
          if (Tcl_GetInt (interp, argv[i], &dofIDR) != TCL_OK) {
            opserr << "WARNING invalid dofID: " << argv[3]
                 << " equalDOF RnodeID? CnodeID? DOF1? DOF2? ...";
            return TCL_ERROR;
          }
          if (Tcl_GetInt(interp, argv[j], &dofIDC) != TCL_OK) {
            opserr << "WARNING invalid dofID: " << argv[3]
                 << " equalDOF RnodeID? CnodeID? DOF1? DOF2? ...";
            return TCL_ERROR;
          }

          dofIDR -= 1; // Decrement for C++ indexing
          dofIDC -= 1;
          if (dofIDC < 0 || dofIDR < 0) {
            opserr << "WARNING invalid dofID: " << argv[i]
                   << " must be >= 1";
            return TCL_ERROR;
          }
          rDOF(k) = dofIDR;
          cDOF(k) = dofIDC;
          Ccr(k,k) = 1.0;
        }

        // Create the multi-point constraint
        MP_Constraint *theMP = new MP_Constraint (RnodeID, CnodeID, Ccr, cDOF,
rDOF); if (theMP == 0) { opserr << "WARNING ran out of memory for equalDOF
MP_Constraint "; printCommand (argc, argv); return TCL_ERROR;
        }

        // Add the multi-point constraint to the domain
        if (theTclDomain->addMP_Constraint (theMP) == false) {
          opserr << "WARNING could not add equalDOF MP_Constraint to domain ";
          printCommand(argc, argv);
          delete theMP;
          return TCL_ERROR;
        }

        char buffer[80];
        sprintf(buffer, "%d", theMP->getTag());
        Tcl_SetResult(interp, buffer, TCL_VOLATILE);

        return TCL_OK;
}



int
TclCommand_RigidDiaphragm(ClientData clientData, Tcl_Interp *interp, int argc,
TCL_Char **argv)
{
  if (argc < 3) {
      opserr << "WARNING rigidLink perpDirn? rNode? <cNodes?>\n";
      return TCL_ERROR;
  }

  int rNode, perpDirn;
  if (Tcl_GetInt(interp, argv[1], &perpDirn) != TCL_OK) {
      opserr << "WARNING rigidLink perpDirn rNode cNodes - could not read
perpDirn? \n"; return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[2], &rNode) != TCL_OK) {
      opserr << "WARNING rigidLink perpDirn rNode cNodes - could not read rNode
\n"; return TCL_ERROR;
  }

  // read in the constrained Nodes
  int numConstrainedNodes = argc - 3;
  ID constrainedNodes(numConstrainedNodes);
  for (int i=0; i<numConstrainedNodes; i++) {
      int cNode;
      if (Tcl_GetInt(interp, argv[3+i], &cNode) != TCL_OK) {
          opserr << "WARNING rigidLink perpDirn rNode cNodes - could not read a
cNode\n"; return TCL_ERROR;
      }
      constrainedNodes(i) = cNode;
  }

  RigidDiaphragm theLink(*theTclDomain, rNode, constrainedNodes, perpDirn-1);


  return TCL_OK;
}




int
TclCommand_addMP(ClientData clientData, Tcl_Interp *interp, int argc,
                           TCL_Char **argv)
{
  opserr << "WARNING - TclCommand_addMP() not yet implemented\n";
  return TCL_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Added by Scott J. Brandenberg, UC Davis, sjbrandenberg@ucdavis.edu
int
TclCommand_doPySimple1Gen(ClientData clientData, Tcl_Interp *interp, int argc,
                               TCL_Char **argv)
{
        if(argc < 6 || argc > 7){
                opserr << "WARNING PySimple1Gen file1? file2? file3? file4?
file5? <file6?>"; opserr << "Must have either 5 or 6 arguments." << endln;
        }

        PySimple1Gen *thePySimple1Gen;
        thePySimple1Gen = new PySimple1Gen;

        if(argc==6)
                thePySimple1Gen->WritePySimple1(argv[1], argv[2], argv[3],
argv[4], argv[5]); if(argc==7) thePySimple1Gen->WritePySimple1(argv[1], argv[2],
argv[3], argv[4], argv[5], argv[6]);

        delete thePySimple1Gen;

        return TCL_OK;
}

int
TclCommand_doTzSimple1Gen(ClientData clientData, Tcl_Interp *interp, int argc,
                               TCL_Char **argv)
{
        if(argc < 6 || argc > 7){
                opserr << "WARNING TzSimple1Gen file1? file2? file3? file4?
file5? <file6?>"; opserr << "Must have either 5 or 6 arguments." << endln;
        }

        TzSimple1Gen *theTzSimple1Gen;
        theTzSimple1Gen = new TzSimple1Gen;

        if(argc==6)
                theTzSimple1Gen->WriteTzSimple1(argv[1], argv[2], argv[3],
argv[4], argv[5]); if(argc==7) theTzSimple1Gen->WriteTzSimple1(argv[1], argv[2],
argv[3], argv[4], argv[5], argv[6]);

        delete theTzSimple1Gen;

        return TCL_OK;
}
// End Added by Scott J. Brandenberg
///////////////////////////////////////////////////////////////////////////////////////////////////

// Added by Prishati Raychowdhury (UCSD)
int
TclSafeBuilder_doShallowFoundationGen(ClientData clientData, Tcl_Interp
*interp, int argc, TCL_Char **argv)
{
        if(argc != 5){
                opserr << "WARNING ShallowFoundationGen FoundationID?
ConnectingNode? InputDataFile? FoundationMatType?"; opserr << "Must have 4
arguments." << endln;
        }

        ShallowFoundationGen *theShallowFoundationGen;
        theShallowFoundationGen = new ShallowFoundationGen;


      // Checking for error
        int FoundationID; int ConnectingNode; int FoundationMatType;

                if (Tcl_GetInt(interp, argv[1], &FoundationID) != TCL_OK) {
          opserr << "WARNING invalid FoundationID: " << argv[1]
               << ". ShallowFoundationGen FoundationID? ConnectingNode?
InputDataFile? FoundationMatType? "; return TCL_ERROR;
        }
        if (Tcl_GetInt(interp, argv[2], &ConnectingNode) != TCL_OK) {
          opserr << "WARNING invalid ConnectingNode: " << argv[2]
               << ". ShallowFoundationGen FoundationID? ConnectingNode?
InputDataFile? FoundationMatType? "; return TCL_ERROR;
        }
        if (Tcl_GetInt(interp, argv[4], &FoundationMatType) != TCL_OK) {
          opserr << "WARNING invalid FoundationMatType: " << argv[4]
               << ". ShallowFoundationGen FoundationID? ConnectingNode?
InputDataFile? FoundationMatType? "; return TCL_ERROR;
        }

        theShallowFoundationGen->GetShallowFoundation(argv[1], argv[2], argv[3],
argv[4]); delete theShallowFoundationGen;

        return TCL_OK;
}
// End PRC
*/

/*


int
TclSafeBuilder_addRemoHFiber(ClientData clientData, Tcl_Interp *interp,
int argc, TCL_Char **argv)
{
  return TclCommand_addHFiber(clientData, interp, argc,argv,theTclBuilder);

}

int
TclCommand_addRemoLayer(ClientData clientData, Tcl_Interp *interp, int argc,
                           TCL_Char **argv)
{
  return TclCommand_addReinfLayer(clientData, interp, argc,argv,
                                       theTclBuilder);
}


extern int
TclSafeBuilderStiffnessDegradationCommand(ClientData clientData,
                                           Tcl_Interp *interp,
                                           int argc, TCL_Char **argv, Domain
*theDomain);

int
TclCommand_addStiffnessDegradation(ClientData clientData,
                                        Tcl_Interp *interp,
                                        int argc, TCL_Char **argv)
{
  return TclSafeBuilderStiffnessDegradationCommand(clientData, interp,
argc, argv, theTclDomain);
}

extern int
TclSafeBuilderUnloadingRuleCommand(ClientData clientData,
                                    Tcl_Interp *interp,
                                    int argc, TCL_Char **argv, Domain
*theDomain);

int
TclCommand_addUnloadingRule(ClientData clientData,
                                 Tcl_Interp *interp,
                                 int argc, TCL_Char **argv)
{
  return TclSafeBuilderUnloadingRuleCommand(clientData, interp, argc,
argv, theTclDomain);
}

extern int
TclSafeBuilderStrengthDegradationCommand(ClientData clientData,
                                          Tcl_Interp *interp,
                                          int argc, TCL_Char **argv, Domain
*theDomain);

int
TclCommand_addStrengthDegradation(ClientData clientData,
                                       Tcl_Interp *interp,
                                       int argc, TCL_Char **argv)
{
  TclSafeBuilder *theTclBuilder =
      (TclSafeBuilder *)Tcl_GetAssocData(interp, "OPS::theTclBuilder", NULL);
  Domain *theTclDomain = theTclBuilder->getDomain();
  return TclSafeBuilderStrengthDegradationCommand(clientData, interp,
argc, argv, theTclDomain);
}

extern int
TclSafeBuilderHystereticBackboneCommand(ClientData clientData,
                                         Tcl_Interp *interp,
                                         int argc, TCL_Char **argv, Domain
*theDomain);

int
TclCommand_addHystereticBackbone(ClientData clientData,
                                      Tcl_Interp *interp,
                                      int argc,	TCL_Char **argv)
{
  TclSafeBuilder *theTclBuilder =
      (TclSafeBuilder *)Tcl_GetAssocData(interp, "OPS::theTclBuilder", NULL);
  Domain *theTclDomain = theTclBuilder->getDomain();
  return TclSafeBuilderHystereticBackboneCommand(clientData, interp,
argc, argv, theTclDomain);
}

/// added by ZHY
extern int
TclSafeBuilderUpdateMaterialStageCommand(ClientData clientData,
                                          Tcl_Interp *interp,
                                          int argc,
                                          TCL_Char **argv,
                                          TclSafeBuilder *theTclBuilder,
                                          Domain *theDomain);
int
TclCommand_UpdateMaterialStage(ClientData clientData,
                                    Tcl_Interp *interp,
                                    int argc,
                                    TCL_Char **argv)
{
  return TclSafeBuilderUpdateMaterialStageCommand(clientData, interp,
                                                   argc, argv, theTclBuilder,
theTclDomain);
}

/// added by ZHY
extern int
TclCommand_UpdateMaterialsCommand(ClientData clientData,
                                  Tcl_Interp *interp,
                                  int argc,
                                  TCL_Char **argv,
                                  TclSafeBuilder *theTclBuilder,
                                  Domain *theDomain);
static int
TclCommand_UpdateMaterials(ClientData clientData,
                           Tcl_Interp *interp,
                           int argc,
                           TCL_Char **argv)
{
  TclSafeBuilder *theTclBuilder =
      (TclSafeBuilder *)Tcl_GetAssocData(interp, "OPS::theTclBuilder", NULL);
  return TclCommand_UpdateMaterialsCommand(clientData, interp,
                                           argc, argv, theTclBuilder,
theTclDomain);
}

/// added by ZHY
extern int
TclSafeBuilderUpdateParameterCommand(ClientData clientData,
                                          Tcl_Interp *interp,
                                          int argc,
                                          TCL_Char **argv,
                                          TclSafeBuilder
*theTclBuilder); int TclCommand_UpdateParameter(ClientData clientData,
                                    Tcl_Interp *interp,
                                    int argc,
                                    TCL_Char **argv)
{
  return TclSafeBuilderUpdateParameterCommand(clientData, interp,
                                       argc, argv, theTclBuilder);
}

extern int
TclSafeBuilderFrictionModelCommand (ClientData clienData,
                                     Tcl_Interp *interp, int argc, TCL_Char
**argv, Domain *theDomain);

int
TclCommand_addFrictionModel(ClientData clientData,
                    Tcl_Interp *interp, int argc, TCL_Char **argv)
{
  return TclSafeBuilderFrictionModelCommand(clientData, interp, argc,
argv, theTclDomain);
}

int
TclCommand_Package(ClientData clientData, Tcl_Interp *interp, int argc, TCL_Char
**argv)
{

  void *libHandle;
  int (*funcPtr)(ClientData clientData, Tcl_Interp *interp,  int argc,
                 TCL_Char **argv, Domain*, TclSafeBuilder*);

  const char *funcName = 0;
  int res = -1;

  if (argc == 2) {
    res = getLibraryFunction(argv[1], argv[1], &libHandle, (void **)&funcPtr);
  } else if (argc == 3) {
    res = getLibraryFunction(argv[1], argv[2], &libHandle, (void **)&funcPtr);
  }

  if (res == 0) {
    int result = (*funcPtr)(clientData, interp,
                            argc,
                            argv,
                            theTclDomain,
                            theTclBuilder);
  } else {
    opserr << "Error: Could not find function: " << argv[1] << endln;
    return -1;
  }

  return res;
}
*/


//
// BEGIN AUTGEN
//

//
// SectionForceDeformation Operations
//

// Retrieve a SectionForceDeformation instance from the model
// runtime
SectionForceDeformation*
TclSafeBuilder::getSection(const std::string &name)
{
  SectionForceDeformation *instance = m_SectionForceDeformationMap.at(name);
  if (instance) {
    return instance->getCopy();
  } else {
    return nullptr;
  }
}

SectionForceDeformation*
TclSafeBuilder::getSection(int tag)
{
  const std::string &name = std::to_string(tag);
  return this->getSection(name);
}

// Add a new SectionForceDeformation to the model runtime
int
TclSafeBuilder::addSection(const std::string &name, SectionForceDeformation &instance)
{
  m_SectionForceDeformationMap[name] = &instance;
  return 1;
}

// Add a new SectionForceDeformation to the model runtime
int
TclSafeBuilder::addSection(SectionForceDeformation &instance)
{
  const std::string &name = std::to_string(instance.getTag());
  m_SectionForceDeformationMap[name] = &instance;
/*
  opserr << "WARNING (ModelBuilder) Failed to add SectionForceDeformation \n"
         << "         with tag '" << name.c_str() << "' to model.\n";
*/
  return 1;
}

//
// SectionRepres Operations
//

// Retrieve a SectionRepres instance from the model
// runtime
SectionRepres*
TclSafeBuilder::getSectionRepres(const std::string &name)
{
  SectionRepres *instance = m_SectionRepresMap.at(name);
  if (instance) {
    return instance;
  } else {
    return nullptr;
  }
}

SectionRepres*
TclSafeBuilder::getSectionRepres(int tag)
{
  const std::string &name = std::to_string(tag);
  return this->getSectionRepres(name);
}

// Add a new SectionRepres to the model runtime
int
TclSafeBuilder::addSectionRepres(const std::string &name, SectionRepres &instance)
{
  m_SectionRepresMap[name] = &instance;
  return 1;
}

// Add a new SectionRepres to the model runtime
int
TclSafeBuilder::addSectionRepres(SectionRepres &instance)
{
  const std::string &name = std::to_string(instance.getTag());
  m_SectionRepresMap[name] = &instance;
/*
  opserr << "WARNING (ModelBuilder) Failed to add SectionRepres \n"
         << "         with tag '" << name.c_str() << "' to model.\n";
*/
  return 1;
}

//
// NDMaterial Operations
//

// Retrieve a NDMaterial instance from the model
// runtime
NDMaterial*
TclSafeBuilder::getNDMaterial(const std::string &name)
{
  NDMaterial *instance = m_NDMaterialMap.at(name);
  if (instance) {
    return instance->getCopy();
  } else {
    return nullptr;
  }
}

NDMaterial*
TclSafeBuilder::getNDMaterial(int tag)
{
  const std::string &name = std::to_string(tag);
  return this->getNDMaterial(name);
}

// Add a new NDMaterial to the model runtime
int
TclSafeBuilder::addNDMaterial(const std::string &name, NDMaterial &instance)
{
  m_NDMaterialMap[name] = &instance;
  return 1;
}

// Add a new NDMaterial to the model runtime
int
TclSafeBuilder::addNDMaterial(NDMaterial &instance)
{
  const std::string &name = std::to_string(instance.getTag());
  m_NDMaterialMap[name] = &instance;
/*
  opserr << "WARNING (ModelBuilder) Failed to add NDMaterial \n"
         << "         with tag '" << name.c_str() << "' to model.\n";
*/
  return 1;
}

//
// UniaxialMaterial Operations
//

// Retrieve a UniaxialMaterial instance from the model
// runtime
UniaxialMaterial*
TclSafeBuilder::getUniaxialMaterial(const std::string &name)
{
  UniaxialMaterial *instance = m_UniaxialMaterialMap.at(name);
  if (instance) {
    return instance->getCopy();
  } else {
    return nullptr;
  }
}

UniaxialMaterial*
TclSafeBuilder::getUniaxialMaterial(int tag)
{
  const std::string &name = std::to_string(tag);
  return this->getUniaxialMaterial(name);
}

// Add a new UniaxialMaterial to the model runtime
int
TclSafeBuilder::addUniaxialMaterial(const std::string &name, UniaxialMaterial &instance)
{
  m_UniaxialMaterialMap[name] = &instance;
  return 1;
}

// Add a new UniaxialMaterial to the model runtime
int
TclSafeBuilder::addUniaxialMaterial(UniaxialMaterial &instance)
{
  const std::string &name = std::to_string(instance.getTag());
  m_UniaxialMaterialMap[name] = &instance;
/*
  opserr << "WARNING (ModelBuilder) Failed to add UniaxialMaterial \n"
         << "         with tag '" << name.c_str() << "' to model.\n";
*/
  return 1;
}

//
// CrdTransf Operations
//

// Retrieve a CrdTransf instance from the model
// runtime
CrdTransf*
TclSafeBuilder::getCrdTransf(const std::string &name)
{
  CrdTransf *instance = m_CrdTransfMap.at(name);
  if (instance) {
    return instance;
  } else {
    return nullptr;
  }
}

CrdTransf*
TclSafeBuilder::getCrdTransf(int tag)
{
  const std::string &name = std::to_string(tag);
  return this->getCrdTransf(name);
}

// Add a new CrdTransf to the model runtime
int
TclSafeBuilder::addCrdTransf(const std::string name, CrdTransf *instance)
{
  // m_CrdTransfMap[name] = instance;
  m_CrdTransfMap.insert({name, instance});
  return 1;
}

// Add a new CrdTransf to the model runtime
int
TclSafeBuilder::addCrdTransf(CrdTransf *instance)
{
  const key_t name = std::to_string(instance->getTag());
  // m_CrdTransfMap[name]std::stringnstance;
  // m_CrdTransfMap.insert(std::make_pair<key_t,CrdTransf*>(std::move(name), instance);
  return this->addCrdTransf(name, instance);
}

