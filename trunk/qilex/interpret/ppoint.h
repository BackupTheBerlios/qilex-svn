/***************************************************************************
                          ppoint.h  -  description
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

#ifndef PPOINT_H
#define PPOINT_H


/**
  *@author Leopold Palomo Avellaneda
  */

#include <qstring.h>

class Qstring;

class ppoint {
public: 
   int dof;
   double *q;
   ppoint(int pdof);
   ppoint();   
   ~ppoint();
   QString name;
   void setName(QString name);
   QString getName();
   void operator = (ppoint a);
   
};

#endif
