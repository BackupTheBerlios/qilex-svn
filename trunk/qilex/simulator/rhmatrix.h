/***************************************************************************
                          rhmatrix.h  -  description
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

#ifndef RHMATRIX_H
#define RHMATRIX_H

#include <cmath>
#include <sstream>
#include <iomanip>
//#include <strstream.h>
#include <iostream>
//#include <string.h>


/**
  *@author Leopold Palomo
  */

using namespace std;
class Rhmatrix {
public: 
   Rhmatrix();
   ~Rhmatrix();

   double mat[4][4];
   double determinant ();
   Rhmatrix inverse ();
   Rhmatrix identity ();
 //  void printmatrix ();
 //  void printmatrix_scaled (double scale);
   std::string row(const int row);

   Rhmatrix descaled (double scale);
   Rhmatrix operator + (Rhmatrix a);
   Rhmatrix operator - (Rhmatrix a);
   Rhmatrix operator * (Rhmatrix a);
   void operator = (Rhmatrix a);
 
};


#endif
