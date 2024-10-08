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
// Purpose: This file contains the class definition for TimeSeries.
// TimeSeries is an abstract class. A TimeSeries object is used to
// determine the load factor to be applied to the loads in a pettern.
// to the model.
//
// Written: fmk 
// Created: 07/99
// Revision: A
//
#include <TimeSeries.h>
#include <TaggedObject.h>
#include <MapOfTaggedObjects.h>

static MapOfTaggedObjects theTimeSeriesObjects;

void
OPS_ADD_RUNTIME_VXV(OPS_clearAllTimeSeries)
{
  theTimeSeriesObjects.clearAll();
}
    

TimeSeries::TimeSeries(int tag, int classTag)
  :TaggedObject(tag), MovableObject(classTag)
{

}

TimeSeries::TimeSeries(int classTag)
  :TaggedObject(0), MovableObject(classTag)
{

}

TimeSeries::~TimeSeries()
{

}
