/***************************************************************************
                          rhrteuler.cpp  -  description
                             -------------------
    begin                : dl mai 5 2003
    copyright            : (C) 2003 by Leopold Palomo Avellaneda
    email                : lepalom@wol.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "rhrteuler.h"

Rhrteuler::Rhrteuler(){
   px = py = pz = psalpha = psbeta = psgamma = pcalpha = pcbeta = pcgamma = 0.0;
}
Rhrteuler::~Rhrteuler(){
}

/* Set the matrix as rotation ZYZ and after a traslation XYZ  */
/* alpha=yaw, beta=pitch and gamma=roll   */
/* alpha, beta and gamma in radians  */


Rhmatrix Rhrteuler::tr_ZYZ_XYZ (double x, double y, double z, double alpha, double beta, double gamma)
{
   psalpha = sin (alpha);
   pcalpha = cos (alpha);
   psbeta = sin (beta);
   pcbeta = cos (beta);
   psgamma = sin (gamma);
   pcgamma = cos (gamma);

   px = x;
   py = y;
   pz = z;

   return(tr_ZYZ_XYZ());   

}
Rhmatrix Rhrteuler::tr_ZYZ_XYZ()
{
  Rhmatrix value;
  value.mat[0][0] =  mat[0][0] = pcalpha * pcbeta * pcgamma - psalpha * psgamma;
  value.mat[0][1] =  mat[0][1] = -pcalpha * pcbeta * psgamma - psalpha * pcgamma;
  value.mat[0][2] =  mat[0][2] = pcalpha * psbeta;
  value.mat[0][3] =  mat[0][3] = px;
  value.mat[1][0] =  mat[1][0] = psalpha * pcbeta * pcgamma + pcalpha * psgamma;
  value.mat[1][1] =  mat[1][1] = -psalpha * pcbeta * psgamma + pcalpha * pcgamma;
  value.mat[1][2] =  mat[1][2] = psalpha * psbeta;
  value.mat[1][3] =  mat[1][3] = py;
  value.mat[2][0] =  mat[2][0] = -psbeta * pcgamma;
  value.mat[2][1] =  mat[2][1] = psbeta * psgamma;
  value.mat[2][2] =  mat[2][2] = pcbeta;
  value.mat[2][3] =  mat[2][3] = pz;

  return value;               
}
Rhmatrix Rhrteuler::value ()
{
  Rhmatrix value;
  value.mat[0][0] = mat[0][0];
  value.mat[0][1] = mat[0][1];
  value.mat[0][2] = mat[0][2];
  value.mat[0][3] = mat[0][3];
  value.mat[1][0] = mat[1][0];
  value.mat[1][1] = mat[1][1];
  value.mat[1][2] = mat[1][2];
  value.mat[1][3] = mat[1][3];
  value.mat[2][0] = mat[2][0];
  value.mat[2][1] = mat[2][1];
  value.mat[2][2] = mat[2][2];
  value.mat[2][3] = mat[2][3];

  return value;

}

