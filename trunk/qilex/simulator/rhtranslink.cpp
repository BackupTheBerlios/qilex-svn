/***************************************************************************
                          rhtranslink.cpp  -  description
                             -------------------
    begin                : Mon Nov 4 2002
    copyright            : (C) 2002 by Leopold Palomo
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

#include "rhtranslink.h"
#include "rhmatrix.h"
#include "../functions.h"
#include <math.h>


/***************************************************************************
Class Rhtranslink

 ***************************************************************************/

/*
  The matrix is in the form

  i      |cos(thetai)   -cos(alphai)*sin(thetai)   sin(alphai)*sin(thetai)     ai*cos(thetai)  |
  T    = |sin(thetai)    cos(thetai)*cos(thetai)   -sin(alphai)*cos(thetai)   ai*sin(thetai)   |
  i-1    |    0                  sin(alphai)             cos(alphai)                di         |
         |    0                      0                         0                    1          |

Ti en funció de thetai, ai, di, on salphai:=sin(alphai) i calphai:=cos(alphai)*/

void Rhtranslink::transformation (double sthetai, double cthetai, double salphai,
			     double calphai, double ai, double di)
{

// update the values or the private vars
  psthetai = sthetai;
  pcthetai = cthetai;
  psalphai = salphai;
  pcalphai = calphai;
  pai = ai;
  pdi = di;

  mat[0][0] = cthetai;
  mat[0][1] = -calphai * sthetai;
  mat[0][2] = salphai * sthetai;
  mat[0][3] = ai * cthetai;
  mat[1][0] = sthetai;
  mat[1][1] = calphai * cthetai;
  mat[1][2] = -salphai * cthetai;
  mat[1][3] = ai * sthetai;
  mat[2][0] = 0.0;
  mat[2][1] = salphai;
  mat[2][2] = calphai;
  mat[2][3] = di;
}

/* function to update the transformation */

void Rhtranslink::update (unsigned int sigma, double value)
{
  if (sigma == 0)		// prismatic, only di
    {
      mat[2][3] = value;
    }
  else				// revolute, uptade theta and theirs multiplications
    {				// value MUST be in radians
      psthetai = sin (value);
      pcthetai = cos (value);
      mat[0][0] = pcthetai;
      mat[0][1] = -pcalphai * psthetai;
      mat[0][2] = psalphai * psthetai;
      mat[0][3] = pai * pcthetai;
      mat[1][0] = psthetai;
      mat[1][1] = pcalphai * pcthetai;
      mat[1][2] = -psalphai * pcthetai;
      mat[1][3] = pai * psthetai;
    }
}

/* return the value of the Rhmatrix inside the htrans*/

Rhmatrix Rhtranslink::value ()
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

void Rhtranslink::scale_dist (double sc)
{
  pai = pai * sc;
  pdi = pdi * sc;

  pS = sc;
//  mat[0][0] = pcthetai;
//  mat[0][1] = -pcalphai * psthetai;
//  mat[0][2] = psalphai * psthetai;
  mat[0][3] = pai * pcthetai;
//  mat[1][0] = psthetai;
//  mat[1][1] = pcalphai * pcthetai;
//  mat[1][2] = -psalphai * pcthetai;
  mat[1][3] = pai * psthetai;
//  mat[2][0] = 0.0;
//  mat[2][1] = psalphai;
//  mat[2][2] = pcalphai;
  mat[2][3] = pdi;
}

// constructor

Rhtranslink::Rhtranslink ()
{
  psthetai = 0;
  pcthetai = 1;
  psalphai = 0;
  pcalphai = 1;
  pai = 0;
  pdi = 0;
  pS = 1;
  transformation (psthetai, pcthetai, psalphai, pcalphai, pai, pdi);
}

Rhtranslink::~Rhtranslink ()
{
}
