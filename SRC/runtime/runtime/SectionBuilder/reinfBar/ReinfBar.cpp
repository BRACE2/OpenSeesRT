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
// File: ReinfBar.C
// Written by Remo M. de Souza
// December 1998

#include <math.h>
#include <string>
#include <Matrix.h>
#include <Vector.h>
#include <OPS_Stream.h>
#include <ReinfBar.h>


ReinfBar::ReinfBar():
                   diameter(0.0), area(0.0), matID(0), posit(2)
{

}


ReinfBar::ReinfBar(double barArea, int materialID, const Vector &position):
                   diameter(0.0), area(barArea),
                   matID(materialID), posit(position)
{
}

ReinfBar::~ReinfBar()
{

}

void ReinfBar::setDiameter (double barDiameter)
{
   double pi = acos(-1.0);
   diameter = barDiameter;
   area = pi * diameter*diameter/4.0;     
}

void ReinfBar::setArea (double barArea)
{
   area = barArea;
}

void ReinfBar::setMaterial (int materialID)
{
   matID = materialID;
}

void ReinfBar::setPosition (const Vector &position)
{
   posit = position;
}

double ReinfBar::getDiameter() const
{
   return diameter;
}

double ReinfBar::getArea() const
{
   return area;
}


const Vector & ReinfBar::getPosition() const
{
   return posit;
}


void ReinfBar::Print(OPS_Stream &s, int flag) const
{
   s << "\nReinforcing Bar area: " << area;
   s << "\nMaterial ID: " << matID;
   s << "\nDiameter: " << diameter;
   s << "\nArea: " << area;
   s << "\nPosition: " << posit;
}

