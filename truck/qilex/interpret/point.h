/***************************************************************************
                          point.h  -  description
                             -------------------
    begin                : dv abr 11 2003
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

#ifndef POINT_H
#define POINT_H


/**
  *@author Leopold Palomo Avellaneda
  */

#include "qstring.h"
#include "../simulator/rhmatrix.h"


class Qstring;
class Rhmatrix;
  
class point {
public: 
   Rhmatrix value;
   QString name;

   void setName(QString name);
   QString getName();

   void setValue (Rhmatrix pmatrix);
   Rhmatrix getValue ();
   point();
   point(Rhmatrix pmatrix, QString pname);   
   ~point();
   void operator = (point a);
};

#endif
