/***************************************************************************
                          list_ppoint.h  -  description
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

#ifndef LIST_PPOINT_H
#define LIST_PPOINT_H


/**
  *@author Leopold Palomo Avellaneda
  */

#include <list>
#include <algorithm>       // STL algorithms class library
#include "ppoint.h"
#include "qstring.h"
#include "simulator/rhmatrix.h"

using namespace std;

class ppoint;
class QString;

class list_ppoint {
public: 
   list<ppoint> l_ppoint;
   list<ppoint>::iterator l_iter;
   int dof;
   list_ppoint(int dof);
   list_ppoint();
   ~list_ppoint();
   void init(int dof);
   ppoint manage(QString name);
   void update(ppoint p);
   bool deletep (QString name);
};


#endif
