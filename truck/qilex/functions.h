/*                        functions.h  -  description
                             -------------------
    begin                : Fri Mai 3 2002
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
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <math.h>
#include <iostream>
#include <iomanip>


const double PI_div180 = 0.01745329251994329547437168059786927187815;	/* Pi /180 */
const double M2PI = 6.283185307179586231995926937088370323181;	/* 2 * Pi */
const double epsilon = 0.000001;
//const double epsilon = 0.000000001;
const double ITERATIONS = 5000;

const double microinterval = 1.0;
const double microangle = 0.01;



// convert from radians to degrees
double degree (double);

// convert from degrees to radians
double rad (double);

// return de position of the max value of a vector
int maxvector (double vector[], int dim);
int minvector (double vector[], int dim);

double max (double lambda1, double lambda2);

// generates a double random number betwen max i min
double grandom (double min, double max);


bool equal_vector( double vector1[], double vector2[], int index1, int index2,
                        int elements, double tol);


#endif
