/***************************************************************************
                          rhrteuler.h  -  description
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

#ifndef RHRTEULER_H
#define RHRTEULER_H

#include <simulator/rhmatrix.h>
#include <cmath>
#include <functions.h>


/**
  *@author Leopold Palomo Avellaneda
  */

class Rhrteuler : public Rhmatrix  {
   double px, py, pz, psalpha, psbeta, psgamma, pcalpha, pcbeta, pcgamma;
public: 
   Rhrteuler();
   ~Rhrteuler();
   Rhmatrix tr_ZYZ_XYZ (double x, double y, double z, double alpha, double beta, double gamma);
   Rhmatrix tr_ZYZ_XYZ();
   Rhmatrix value ();
};

#endif
