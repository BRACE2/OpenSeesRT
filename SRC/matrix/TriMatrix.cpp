//===----------------------------------------------------------------------===//
//
//        OpenSees - Open System for Earthquake Engineering Simulation
//
//===----------------------------------------------------------------------===//
//
#include "TriMatrix.h"


TriDiagonalMatrixF::TriDiagonalMatrixF(int n)
{
  length = n;
  A = new double[n];
  B = new double[n];
  C = new double[n];
}


TriDiagonalMatrixF::~TriDiagonalMatrixF()
{
  delete[] A;
  delete[] B;
  delete[] C;
}

/// The width and height of this matrix.
int TriDiagonalMatrixF::N()
{
   return (A != 0 ? length : 0); 
}

/// Indexer. Setter throws an exception if you try to set any not on the super, main, or sub diagonals.
double TriDiagonalMatrixF::GetMat(int row, int col)
{ 
  int di = row - col;

  if (di == 0)
    return B[row];

  else if (di == -1)
    return C[row];

  else if (di == 1)
    return A[row];

  else 
    return 0;
}

void TriDiagonalMatrixF::SetMat(int row, int col, double value)
{
  int di = row - col;

  if (di == 0)
    B[row] = value;

  else if (di == -1)
    C[row] = value;

  else if (di == 1)
    A[row] = value;
}
    

// Solve the system of equations this*x=d given the specified d.
//
// Uses the Thomas algorithm described in the wikipedia article: http://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm
// Not optimized. Not destructive.
//
// <param name="d">Right side of the equation.</param>
double* 
TriDiagonalMatrixF::Solve(double* d, int dLength)
{
  int n = N();

  // cPrime
  double* cPrime = new double[n];
  cPrime[0] = C[0] / B[0];

  for (int i = 1; i < n; i++)
    cPrime[i] = C[i] / (B[i] - cPrime[i-1] * A[i]);

  // dPrime
  double* dPrime = new double[n];
  dPrime[0] = d[0] / B[0];

  for (int i = 1; i < n; i++)
    dPrime[i] = (d[i] - dPrime[i-1]*A[i]) / (B[i] - cPrime[i - 1] * A[i]);


  // Back substitution
  double* x = new double[n];
  x[n - 1] = dPrime[n - 1];

  for (int i = n-2; i >= 0; i--)
    x[i] = dPrime[i] - cPrime[i] * x[i + 1];
  
  delete[] cPrime;
  delete[] dPrime;
  return x;
}
