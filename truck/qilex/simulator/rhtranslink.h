/***************************************************************************
                          rhtranslink.h  -  description
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

#ifndef RHTRANSLINK_H
#define RHTRANSLINK_H

#include "rhmatrix.h"

/**
  *@author Leopold Palomo
  */

class Rhtranslink : public Rhmatrix  {
   double psthetai, pcthetai, psalphai, pcalphai, pai, pdi, pS;
public:
   void transformation (double sthetai, double cthetai,
	             double salphai, double calphai, double ai,
				    double di);
   void update (unsigned int sigma, double value);
   void scale_dist (double sc);
   Rhmatrix value ();
   Rhtranslink ();
	~Rhtranslink();
};

#endif
