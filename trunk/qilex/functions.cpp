/***************************************************************************
                          functions.cpp  -  description
                             -------------------
    begin                : Frid May 3 2002
    copyright            : (C) 2002 by Leopold Palomo Avellaneda
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
#include "functions.h"


/* General functions needed for the programs */

// convert radians to degrees
double degree (double prad)
{
  return prad / PI_div180;
}

// convert from degrees to radians

double rad (double pdegree)
{
  return pdegree * PI_div180;
}

// returm the maxim value of two double
// if they are equal return one
double max (double lambda1, double lambda2)
{
  if (lambda1 >= lambda2)
    return lambda1;
  else
    return lambda2;
}

int maxvector (double vector[], int dim)
{
  double pmax = vector[1];
  int pimax = 1;
  
  for (int i = 2; i <= dim; i++)
  {
      if (vector[i] > pmax)
	   {
	      pmax = vector[i];
	      pimax = i;
	   }
  }
  return pimax;
}

int minvector (double vector[], int dim)
{
   double pmin = vector[1];
   int pimin = 1;

   for (int i = 2; i <= dim; i++)
   {
      if (vector[i] < pmin)
	   {
	      pmin = vector[i];
	      pimin = i;
	   }
   }
   return pimin;
}


bool equal_vector( double vector1[], double vector2[], int first_index1, int first_index2, int elements, double tol)
{
   bool equality = true;
   int i= 0;

   while (equality && i < elements)
   {
      equality = (fabs(vector1[first_index1+i] - vector2[first_index2+i])
                  < tol);
      i++;
                  
   }
   return equality;
}



   
