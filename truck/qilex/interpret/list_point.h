/***************************************************************************
                          list_point.h  -  description
                             -------------------
    begin                : dg mai 4 2003
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
/**
  *@author Leopold Palomo Avellaneda
  */

 
#ifndef LIST_POINT_H
#define LIST_POINT_H

#include <list>
#include <algorithm>       // STL algorithms class library
#include "point.h"
#include "qstring.h"
#include "simulator/rhmatrix.h"

using namespace std;

class point;
class Rhmatrix;
class QString;

class list_point {
public: 
   list<point> l_point;
   list<point>::iterator l_iter;

   list_point();
   ~list_point();
   point manage(QString name);
   void update(point p);
   bool deletep (QString name);
};

#endif
