/***************************************************************************
                          rhmatrix.cpp  -  description
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

 
#include "rhmatrix.h"


/***********************************************************************

Class Rhmatrix

***********************************************************************/
/* Determinant of a homegenous matrix  */
double Rhmatrix::determinant ()
{
  double det;
  det =
    mat[0][0] * mat[1][1] * mat[2][2] - mat[0][0] * mat[2][1] * mat[1][2] -
    mat[1][0] * mat[0][1] * mat[2][2] + mat[1][0] * mat[0][2] * mat[2][1] +
    mat[2][0] * mat[0][1] * mat[1][2] - mat[2][0] * mat[0][2] * mat[1][1];
  return det;
}

// returns the identity matrix
Rhmatrix Rhmatrix::identity ()
{
  Rhmatrix ID;
  ID.mat[0][0] = 1;
  ID.mat[0][1] = 0;
  ID.mat[0][2] = 0;
  ID.mat[0][3] = 0;

  ID.mat[1][0] = 0;
  ID.mat[1][1] = 1;
  ID.mat[1][2] = 0;
  ID.mat[1][3] = 0;

  ID.mat[2][0] = 0;
  ID.mat[2][1] = 0;
  ID.mat[2][2] = 1;
  ID.mat[2][3] = 0;

  return (ID);
}

/* Inverse of homegeneous matrix*/

Rhmatrix Rhmatrix::inverse ()
{
  Rhmatrix Minverse;
  Minverse.mat[0][0] = mat[0][0];
  Minverse.mat[0][1] = mat[1][0];
  Minverse.mat[0][2] = mat[2][0];

  Minverse.mat[1][0] = mat[0][1];
  Minverse.mat[1][1] = mat[1][1];
  Minverse.mat[1][2] = mat[2][1];

  Minverse.mat[2][0] = mat[0][2];
  Minverse.mat[2][1] = mat[1][2];
  Minverse.mat[2][2] = mat[2][2];

  Minverse.mat[0][3] =
    -mat[0][0] * mat[0][3] - mat[1][0] * mat[1][3] - mat[2][0] * mat[2][3];
  Minverse.mat[1][3] =
    -mat[0][1] * mat[0][3] - mat[1][1] * mat[1][3] - mat[2][1] * mat[2][3];
  Minverse.mat[2][3] =
    -mat[0][2] * mat[0][3] - mat[1][2] * mat[1][3] - mat[2][2] * mat[2][3];

  return Minverse;
}


/* Print the values of the h. matrix */
/*
void Rhmatrix::printmatrix ()
{
  cout << setiosflags (ios::fixed | ios::showpoint);
  cout << endl;
  for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
	cout << setw (10) << setprecision (5) << mat[i][j];
      cout << endl;
    }
  cout << endl;
}


void Rhmatrix::printmatrix_scaled (double scale)
{
  cout << setiosflags (ios::fixed | ios::showpoint);
  cout << endl;
  for (int i = 0; i < 3; i++)
  {
      for (int j = 0; j < 3; j++)
      {
         cout << setw (10) << setprecision (5) << mat[i][j];
      }
      cout << setw (10) << setprecision (5) << scale * mat[i][3];
      cout << endl;
  }
  for (int j = 0; j < 4; j++)
    cout << setw (10) << setprecision (5) << mat[3][j];

  cout << endl;
}

*/
Rhmatrix Rhmatrix::descaled (double scale)
{
  Rhmatrix pm;

  for (int i = 0; i < 3; i++)
  {
      for (int j = 0; j < 3; j++)
      {
         pm.mat[i][j] = mat[i][j];
      }
      pm.mat[i][3] =  scale * mat[i][3];

  }
   return pm;
}

 
/* Redefinition of the operators */
Rhmatrix Rhmatrix::operator * (Rhmatrix b)
{
  Rhmatrix c;
  c.mat[0][0] =
    mat[0][0] * b.mat[0][0] + mat[0][1] * b.mat[1][0] +
    mat[0][2] * b.mat[2][0];
  c.mat[1][0] =
    mat[1][0] * b.mat[0][0] + mat[1][1] * b.mat[1][0] +
    mat[1][2] * b.mat[2][0];
  c.mat[2][0] =
    mat[2][0] * b.mat[0][0] + mat[2][1] * b.mat[1][0] +
    mat[2][2] * b.mat[2][0];

  c.mat[0][1] =
    mat[0][0] * b.mat[0][1] + mat[0][1] * b.mat[1][1] +
    mat[0][2] * b.mat[2][1];
  c.mat[1][1] =
    mat[1][0] * b.mat[0][1] + mat[1][1] * b.mat[1][1] +
    mat[1][2] * b.mat[2][1];
  c.mat[2][1] =
    mat[2][0] * b.mat[0][1] + mat[2][1] * b.mat[1][1] +
    mat[2][2] * b.mat[2][1];

  c.mat[0][2] =
    mat[0][0] * b.mat[0][2] + mat[0][1] * b.mat[1][2] +
    mat[0][2] * b.mat[2][2];
  c.mat[1][2] =
    mat[1][0] * b.mat[0][2] + mat[1][1] * b.mat[1][2] +
    mat[1][2] * b.mat[2][2];
  c.mat[2][2] =
    mat[2][0] * b.mat[0][2] + mat[2][1] * b.mat[1][2] +
    mat[2][2] * b.mat[2][2];

  c.mat[0][3] =
    mat[0][0] * b.mat[0][3] + mat[0][1] * b.mat[1][3] +
    mat[0][2] * b.mat[2][3] + mat[0][3];
  c.mat[1][3] =
    mat[1][0] * b.mat[0][3] + mat[1][1] * b.mat[1][3] +
    mat[1][2] * b.mat[2][3] + mat[1][3];
  c.mat[2][3] =
    mat[2][0] * b.mat[0][3] + mat[2][1] * b.mat[1][3] +
    mat[2][2] * b.mat[2][3] + mat[2][3];

  return c;
}

Rhmatrix Rhmatrix::operator + (Rhmatrix b)
{
  Rhmatrix c;
  for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
	{
	  c.mat[i][j] = mat[i][j] + b.mat[i][j];
	}
    }
  return c;
}

Rhmatrix Rhmatrix::operator - (Rhmatrix b)
{
  Rhmatrix c;
  for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
	{
	  c.mat[i][j] = mat[i][j] - b.mat[i][j];
	}
    }
  return c;
}

// això és pot millorar, no cal copiar
// l'última fila

void Rhmatrix::operator = (Rhmatrix a)
{
  for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
	{
	  mat[i][j] = a.mat[i][j];
	}
    }
}

/* class constructor*/

Rhmatrix::Rhmatrix ()
{
  for (int i = 0; i < 3; i++)
    mat[3][i] = 0;
  mat[3][3] = 1;
}
Rhmatrix::~Rhmatrix()
{
}
std::string Rhmatrix::row (const int row)
{
   std::ostringstream oss;
   //ostrstream oss;
   if (row < 4)
   {
      for (int j = 0; j < 4; j++)
      {
         oss << setw (10) << setprecision (5) << mat[row][j] << " " ;
      }
      oss << endl;
   }
   else
   {
      oss << endl;
   }
   return oss.str();
}
