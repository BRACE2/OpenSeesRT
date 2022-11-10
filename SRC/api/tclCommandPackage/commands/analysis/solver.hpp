
// standard library
#include <array>
#include <string>
#include <unordered_map>

// framework
#include <g3_api.h>
#include <runtimeAPI.h>
#include <analysisAPI.h>
#include <OPS_Globals.h>
#include <packages.h>

// analysis
#include <StaticAnalysis.h>
#include <DirectIntegrationAnalysis.h>
#include <VariableTimeStepDirectIntegrationAnalysis.h>

// system of eqn and solvers
#include <BandSPDLinSOE.h>
#include <BandSPDLinLapackSolver.h>

#include <BandGenLinSOE.h>
#include <BandGenLinLapackSolver.h>

#include <ConjugateGradientSolver.h>

#include <FullGenLinSOE.h>
#include <FullGenLinLapackSolver.h>

#include <ProfileSPDLinSOE.h>
#include <ProfileSPDLinDirectSolver.h>
#include <DistributedProfileSPDLinSOE.h>

#include <DiagonalSOE.h>
#include <DiagonalDirectSolver.h>

#include <SProfileSPDLinSolver.h>
#include <SProfileSPDLinSOE.h>

#include <SparseGenColLinSOE.h>

#include <SparseGenRowLinSOE.h>
// #include <SymSparseLinSOE.h>
// #include <SymSparseLinSolver.h>
#include <EigenSOE.h>
#include <EigenSolver.h>
#include <ArpackSOE.h>
#include <ArpackSolver.h>
#include <SymArpackSOE.h>
#include <SymArpackSolver.h>
#include <BandArpackSOE.h>
#include <BandArpackSolver.h>
#include <SymBandEigenSOE.h>
#include <SymBandEigenSolver.h>
#include <FullGenEigenSOE.h>
#include <FullGenEigenSolver.h>

#ifdef _THREADS
#  include <ThreadedSuperLU.h>
#else
#  include <SuperLU.h>
#endif

#ifdef _CUSP
#  include <CuSPSolver.h>
#endif

#ifdef _CULAS4
#include <CulaSparseSolverS4.h>
#endif

#ifdef _CULAS5
#include <CulaSparseSolverS5.h>
#endif

#ifdef _MUMPS
#  include <MumpsSOE.h>
#  include <MumpsSolver.h>
#  if defined(_PARALLEL_PROCESSING) || defined(_PARALLEL_INTERPRETERS)
#    include <MumpsParallelSOE.h>
#    include <MumpsParallelSolver.h>
#  endif
#endif

#if 1 || defined(_PETSC)
LinearSOE *TclCommand_newPetscSOE(int, TCL_Char**);
#endif

#ifdef _CUDA
#  include <BandGenLinSOE_Single.h>
#  include <BandGenLinLapackSolver_Single.h>
#endif

#if defined(_PARALLEL_PROCESSING)
//  parallel soe & solvers
#  include <DistributedBandSPDLinSOE.h>
#  include <DistributedSparseGenColLinSOE.h>
#  include <DistributedSparseGenRowLinSOE.h>
#  include <DistributedBandGenLinSOE.h>
#  include <DistributedDiagonalSOE.h>
#  include <DistributedDiagonalSolver.h>

#  include <MPIDiagonalSOE.h>
#  include <MPIDiagonalSolver.h>

#  define MPIPP_H
#  include <DistributedSuperLU.h>
#  include <DistributedProfileSPDLinSOE.h>
#endif

typedef LinearSOE*(G3_SysOfEqnSpecifier)(G3_Runtime*, int, G3_Char**);

// Specifiers defined in solver.cpp
G3_SysOfEqnSpecifier specify_SparseSPD;
G3_SysOfEqnSpecifier specifySparseGen;
G3_SysOfEqnSpecifier G3Parse_newMumpsLinearSOE;

// Helpers to automatically create constructors for systems/solvers 
// that do not take arguments when they are constructed.
template <typename Solver, typename SOE>
LinearSOE *simple_soe(G3_Runtime*, int, G3_Char**) {return new SOE(*(new Solver()));}
#define G3_SOE(Solver, SOE) simple_soe<Solver, SOE>
#define SP_SOE(Solver, SOE) nullptr
#define MP_SOE(Solver, SOE) nullptr

typedef LinearSOE*(*fn)(G3_Runtime*, int, G3_Char**);
struct soefps {fn ss, sp, mp;};

std::unordered_map<std::string, struct soefps> soe_table = {
  {"BandSPD", {
     G3_SOE(BandSPDLinLapackSolver,      BandSPDLinSOE),
     SP_SOE(BandSPDLinLapackSolver,      DistributedBandSPDLinSOE),
     MP_SOE(BandSPDLinLapackSolver,      DistributedBandSPDLinSOE)}},

  {"BandGeneral", { // BandGen, BandGEN
     G3_SOE(BandGenLinLapackSolver,      BandGenLinSOE),
     SP_SOE(BandGenLinLapackSolver,      DistributedBandGenLinSOE),
     MP_SOE(BandGenLinLapackSolver,      DistributedBandGenLinSOE)}},

  {"BandSPD", {
     G3_SOE(BandSPDLinLapackSolver,      BandSPDLinSOE),
     SP_SOE(BandSPDLinLapackSolver,      DistributedBandSPDLinSOE),
     MP_SOE(BandSPDLinLapackSolver,      DistributedBandSPDLinSOE)}},

  {"SparseGen", {
     specifySparseGen, 
     nullptr, 
     nullptr}},
  {"SuperLU", {
     specifySparseGen, 
     nullptr, 
     nullptr}},

  {"SparseSYM", {
     specify_SparseSPD, nullptr, nullptr}},
  {"SparseSPD", {
     // Legacy specifier
     specify_SparseSPD, nullptr, nullptr}},

  {"Diagonal", {
     G3_SOE(DiagonalDirectSolver,        DiagonalSOE),
     SP_SOE(DistributedDiagonalSolver,   DistributedDiagonalSOE),
     MP_SOE(DistributedDiagonalSolver,   DistributedDiagonalSOE)}},

  {"MPIDiagonal", {
     G3_SOE(DiagonalDirectSolver,        DiagonalSOE),
     SP_SOE(MPIDiagonalSolver,           MPIDiagonalSOE),
     MP_SOE(MPIDiagonalSolver,           MPIDiagonalSOE)}},

  {"SProfileSPD", {
     G3_SOE(SProfileSPDLinSolver,        SProfileSPDLinSOE),
     SP_SOE(SProfileSPDLinSolver,        SProfileSPDLinSOE),
     MP_SOE(SProfileSPDLinSolver,        SProfileSPDLinSOE)}},

  {"ProfileSPD", {
     G3_SOE(ProfileSPDLinDirectSolver,   ProfileSPDLinSOE),
     SP_SOE(ProfileSPDLinDirectSolver,   DistributedProfileSPDLinSOE),
     MP_SOE(ProfileSPDLinDirectSolver,   DistributedProfileSPDLinSOE)}},

  {"ParallelProfileSPD", {
     nullptr, nullptr,
     MP_SOE(ProfileSPDLinDirectSolver,   DistributedProfileSPDLinSOE)}},

  {"FullGeneral", {
     G3_SOE(FullGenLinLapackSolver,      FullGenLinSOE),
     SP_SOE(FullGenLinLapackSolver,      FullGenLinSOE),
     MP_SOE(FullGenLinLapackSolver,      FullGenLinSOE)}},

#ifdef _CUDA
  {"BandGeneral_Single", {  // "BandGEN_Single", "BandGen_Single"
     G3_SOE(BandGenLinLapackSolver_Single,    BandGenLinSOE_Single),
     SP_SOE(BandGenLinLapackSolver_Single,    BandGenLinSOE_Single),
     MP_SOE(BandGenLinLapackSolver_Single,    BandGenLinSOE_Single)}}
  }
#endif
};

