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
// File: QuadPatch.C
// Written by Remo M. de Souza
// December 1998
//
#include <string>
#include <Matrix.h>
#include <Patch.h>
#include <QuadPatch.h>
#include <QuadCell.h>
#include <OPS_Stream.h>



QuadPatch::QuadPatch():
                       matID(0), nDivIJ(1), nDivJK(1), vertCoord(4,2)
{

}


QuadPatch::QuadPatch(int materialID, int numSubdivIJ, int numSubdivJK,
                     const Matrix &vertexCoords):
                       matID(materialID), 
                       nDivIJ(numSubdivIJ), nDivJK(numSubdivJK),
                       vertCoord(vertexCoords)

{

}  


QuadPatch::~QuadPatch()
{

}

void QuadPatch::setMaterialID(int materialID)
{
   matID = materialID;
}


void QuadPatch::setDiscretization(int numSubdivIJ, int numSubdivJK)
{
   nDivIJ = numSubdivIJ;
   nDivJK = numSubdivJK;
}

void QuadPatch::setVertCoords(const Matrix &vertexCoords)
{
   vertCoord = vertexCoords;
}

int QuadPatch::getMaterialID() const
{
   return matID;
}
 
void QuadPatch::getDiscretization(int &numSubdivIJ, int &numSubdivJK) const
{
   numSubdivIJ = nDivIJ;
   numSubdivJK = nDivJK;
}

const Matrix & QuadPatch::getVertCoords() const
{
   return vertCoord;
}

int QuadPatch::getNumCells() const
{
   return nDivIJ * nDivJK;
}

Cell **
QuadPatch::getCells() const
{
   double deltaXi;
   double deltaEta; 
   Matrix cellVertCoord(4,2);
   Vector N(4);
   double xi, eta;
   int i, j, k, r, s;
   int numCells;
   Cell **cells;
  
   if (nDivIJ > 0  && nDivJK > 0)
   {
      numCells  = this->getNumCells();

      cells = new Cell*  [numCells];
      
      if (!cells)
         return 0;

      deltaXi  = 2.0 / nDivIJ;
      deltaEta = 2.0 / nDivJK;

      k = 0;
      for (j = 0; j < nDivJK; j++)
         for (i = 0; i < nDivIJ; i++)
         {
            // compute natural coordinates

            cellVertCoord(0,0) = -1.0 + deltaXi  * i;
            cellVertCoord(0,1) = -1.0 + deltaEta * j;
            cellVertCoord(1,0) = -1.0 + deltaXi  * (i+1);
            cellVertCoord(1,1) = cellVertCoord(0,1);
            cellVertCoord(2,0) = cellVertCoord(1,0);
            cellVertCoord(2,1) = -1.0 + deltaEta * (j+1);
            cellVertCoord(3,0) = cellVertCoord(0,0);
            cellVertCoord(3,1) = cellVertCoord(2,1);

            // map to cartesian coordinates using bilinear
            // shape functions

            for (r = 0; r < 4; r++)
            {
               xi  = cellVertCoord(r,0);
               eta = cellVertCoord(r,1);
 
               N(0) = (1.0 - xi)*(1.0 - eta)/4.0;
               N(1) = (1.0 + xi)*(1.0 - eta)/4.0;
               N(2) = (1.0 + xi)*(1.0 + eta)/4.0;
               N(3) = (1.0 - xi)*(1.0 + eta)/4.0;

               cellVertCoord(r,0) = 0.0;
               cellVertCoord(r,1) = 0.0;

               for (s = 0; s < 4; s++)
               {
                  cellVertCoord(r,0) += N(s) * vertCoord(s,0);
                  cellVertCoord(r,1) += N(s) * vertCoord(s,1);
               }
            }  

            cells[k] = new QuadCell(cellVertCoord); 
            //opserr << "\ncreating cells Cell " << k << " :" << cells[k];
            k++; 
         }
   }
   else
      return 0;

   return cells;
}


Patch * 
QuadPatch::getCopy() const
{
   QuadPatch *theCopy = new QuadPatch (matID, nDivIJ, nDivJK, vertCoord);
   return theCopy;
}
 
void QuadPatch::Print(OPS_Stream &s, int flag) const
{
   s << "\nPatch Type: QuadPatch";
   s << "\nMaterial Id: " << matID;
   s << "\nNumber of subdivisions in the IJ direction: " << nDivIJ;
   s << "\nNumber of subdivisions in the JK direction: " << nDivJK;
   s << "\nVertex Coordinates: " << vertCoord;
}


OPS_Stream &operator<<(OPS_Stream &s, QuadPatch &quadPatch)
{
   quadPatch.Print(s);
   return s;
}
