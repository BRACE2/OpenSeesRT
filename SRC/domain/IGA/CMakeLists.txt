#==============================================================================
# 
#        OpenSees -- Open System For Earthquake Engineering Simulation
#                Pacific Earthquake Engineering Research Center
#
#==============================================================================

target_sources(OPS_Domain
    PRIVATE
#   gaussQuadrature.cpp
    IGASurfacePatch.cpp
    IGAFollowerLoad.cpp
    NurbsDers.cpp
    PUBLIC
#   gaussQuadrature.h
    IGASurfacePatch.h
    IGAFollowerLoad.h
    NurbsDers.h
)

target_include_directories(OPS_Domain PUBLIC ${CMAKE_CURRENT_LIST_DIR})
