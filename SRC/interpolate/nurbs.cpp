/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
** ****************************************************************** */
//
// This file contains the implementation for NURBS derivatives
//
// Authors 
//   Vinh Phu Nguyen, nvinhphu@gmail.com
//   Robert Simpson, Cardiff University, UK
//
#include <math.h>
#include <float.h>
#include <stdlib.h>

#include "nurbs.h"
#include <Vector.h>
#include <Matrix.h>

#define TOL 100*DBL_EPSILON


static double** 
init2DArray(int x, int y) {
  double **array = (double **)malloc(x * sizeof(double *));

  for (int c = 0; c < x; c++)
    array[c] = (double*)malloc(y * sizeof(double));

  return array;
}

static void
free2Darray(double **array, int x) {
  for (int c = 0; c < x; c++)
    free(array[c]);

  free(array);
}

int FindSpan(int n, int p, double u, Vector& U)
{
  /* 
   * This function determines the knot span.
     ie. if we have a coordinate u which lies in the range u \in [u_i, u_{i+1})
     we want to find i
     Note that: u_i <= u < (not equal) u_{i+1}!!!
     If we have knot = [0,0.5,1] then u=0.5 has span=1 not 0!!!
  */

  if ( u >= U[n + 1] )
    return n;

  if ( u <= U[p] )
    return p;

  int low  = p,
      high = n + 1, 
      mid  = (low + high) / 2;

  while ( u < U[mid] || u >= U[mid + 1] )
  {
    if ( u < U[mid] )
      high = mid;
    else
      low = mid;
    mid = (low + high) / 2;
  }
  return mid;
}

void 
BasisFuns( int i, double u, int p, Vector& U, Vector& N)
{

  /*
   we can compute the non zero basis functions
     at point u, there are p+1 non zero basis functions
  */

  N[0] = 1.0;

  // double *left  = (double *)malloc(sizeof(double) * (p + 1));
  // double *right = (double *)malloc(sizeof(double) * (p + 1));

  Vector left(p+1);
  Vector right(p+1);

  double saved, temp;

  for (int j = 1; j <= p; ++j) {
    left[j]  = u - U[i + 1 - j];
    right[j] = U[i + j] - u;
    saved = 0.0;
    for (int r = 0; r < j; ++r) {
      temp  = N[r] / ( right[r + 1] + left[j - r] );
      N[r]  = saved + right[r + 1] * temp;
      saved = left[j - r] * temp;
    }
    N[j] = saved;
  }

  // free(left);
  // free(right);

}

// void dersBasisFuns(int i, double u, int p, int order, Vector knot, double **ders)
void dersBasisFuns(int i, double u, int p, int order, Vector& knot, Matrix& ders)
{
  /*
   * Calculate the non-zero derivatives of the b-spline functions
   */

  double saved, temp;
  int j, j1, j2, r;

  // double *left  = (double *)malloc(sizeof(double) * (p + 1));
  // double *right = (double *)malloc(sizeof(double) * (p + 1));

  static Vector right(p+1);
  static Vector left(p+1);
  right.resize(p+1);
  left.resize(p+1);

  // double **ndu  = init2DArray(p + 1, p + 1);
  // double **a    = init2DArray(p + 1, p + 1);

  static Matrix ndu(p+1,p+1);
  static Matrix a(p+1,p+1);
  ndu.resize(p+1,p+1);
  a.resize(p+1,p+1);

  ndu(0,0) = 1.0;
  for ( j = 1; j <= p; j++ ) {
    left[j] = u - knot[i + 1 - j];
    right[j] = knot[i + j] - u;

    saved = 0.0;
    for ( r = 0; r < j; r++ ) {
      ndu(j,r) = right[r + 1] + left[j - r];
      temp = ndu(r,j - 1) / ndu(j,r);

      ndu(r,j) = saved + right[r + 1] * temp;
      saved = left[j - r] * temp;
    }
    ndu(j,j) = saved;
  }

  for (int j = 0; j <= p; j++ )
    ders(0,j) = ndu(j,p);

  if ( order == 0 )
    return;

  for (int r = 0; r <= p; r++ ) {
    int s1 = 0, 
        s2 = 1;

    a(0,0) = 1.0;

    for (int k = 1; k <= order; k++ ) {
      double d = 0.;
      int rk = r - k, 
          pk = p - k;

      if ( r >= k ) {
        a(s2,0) = a(s1,0) / ndu(pk + 1,rk);
        d = a(s2,0) * ndu(rk,pk);
      }

      j1 = rk >= -1 ? 1 : -rk;
      j2 = (r - 1 <= pk) ? k - 1 : p - r;

      for ( j = j1; j <= j2; j++ ) {
        a(s2,j) = (a(s1,j) - a(s1,j - 1)) / ndu(pk + 1,rk + j);
        d += a(s2,j) * ndu(rk + j,pk);
      }
      if ( r <= pk ) {
        a(s2,k) = -a(s1,k - 1) / ndu(pk + 1,r);
        d += a(s2,k) * ndu(r,pk);
      }
      ders(k,r) = d;
      j  = s1; 
      s1 = s2; 
      s2 = j;
    }
  }

  r = p;
  for (int k = 1; k <= order; k++ ) {
    for (int j = 0; j <= p; j++ )
      ders(k,j) *= r;
    r *= (p - k);
  }

  // free(left);
  // free(right);

  // free2Darray(ndu, p + 1);
  // free2Darray(a, p + 1);

}


double
OneBasisFun(int p, int m, Vector U, int i, double u)
{
  /*
    Compute an individual B-spline function
  */

  if ((i == 0 && u == U[0] ) ||
      (i == (m - p - 1) && u == U[m]))
    return 1.0;

  if (u < U[i] || u >= U[i + p + 1])
    return 0.0;

  double *N = (double*)malloc(sizeof(double) * (p + 1));

  for (int j = 0; j <= p; j++) {
    if (u >= U[i + j] && u < U[i + j + 1])
      N[j] = 1.0;
    else
      N[j] = 0.0;
  }

  for (int k = 1; k <= p; k++) {
    double saved, Uleft, Uright, temp;

    if (N[0] == 0.0) 
      saved = 0.0;
    else 
      saved = ((u - U[i]) * N[0]) / (U[i + k] - U[i]);

    for (int j = 0; j < (p - k + 1); j++) {
      Uleft = U[i + j + 1];
      Uright = U[i + j + k + 1];
      if (N[j + 1] == 0.0) {
        N[j] = saved; 
        saved = 0.0;
      } else {
        temp = N[j + 1] / (Uright - Uleft);
        N[j] = saved + (Uright - u) * temp;
        saved = (u - Uleft) * temp;
      }
    }
  }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
  double Nip = N[0];
#pragma GCC diagnostic pop
  free(N);

  return Nip;

}


void dersOneBasisFuns(int p, int m, Vector U, int i, double u, int order, double* ders)
{
  /*
    Compute the derivatives for basis function Nip
  */

  double **N = init2DArray(order + 1, order + 1);
  double *ND = (double*)malloc((order + 1) * sizeof(double));

  double saved, temp;

  if (u < U[i] || u >= U[i + p + 1]) {
    for (int k = 0; k <= order; k++)
      ders[k] = 0.0;
    return;
  }

  for (int j = 0; j <= p; j++) {
    if (u >= U[i + j] && u < U[i + j + 1])
      N[j][0] = 1.0;
    else
      N[j][0] = 0.0;
  }

  for (int k = 1; k <= p; k++)
  {
    if (N[0][k - 1] == 0.0)
      saved = 0.0;
    else
      saved = ((u - U[i]) * N[0][k - 1]) / ( U[i + k] - U[i] );

    for (int j = 0; j < (p - k + 1); j++) {
      double Uleft = U[i + j + 1];
      double Uright = U[i + j + k + 1];
      if (N[j + 1][k - 1] == 0.0) {
        N[j][k] = saved; 
        saved = 0.0;
      } else {
        temp = N[j + 1][k - 1] / (Uright - Uleft);
        N[j][k] = saved + (Uright - u) * temp;
        saved = (u - Uleft) * temp;
      }
    }
  }

  ders[0] = N[0][p];

  for (int k = 1; k <= order; k++) {
    for (int j = 0; j <= k; j++)
      ND[j] = N[j][p - k];

    for (int jj = 1; jj <= k; jj++) {

      if (ND[0] == 0.0)
        saved = 0.0;
      else
        saved = ND[0] / ( U[i + p - k + jj] - U[i]);

      for (int j = 0; j < (k - jj + 1); j++) {
        double Uleft = U[i + j + 1];
        double Uright = U[i + j + p + jj];

        if (ND[j + 1] == 0.0) {
          ND[j] = (p - k + jj) * saved; 
          saved = 0.0;
        } else {
          temp = ND[j + 1] / (Uright - Uleft);
          ND[j] = (p - k + jj) * (saved - temp);
          saved = temp;
        }
      }
    }
    ders[k] = ND[0];
  }

  free2Darray(N, order + 1);
  free(ND);
}


