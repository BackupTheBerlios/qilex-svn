/***************************************************************************
                          ppoint.cpp  -  description
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

#include "ppoint.h"

ppoint::ppoint(int pdof)
{
   dof = pdof;
   q = new double[pdof];
}
ppoint::ppoint()
{
   dof = 0;
}

ppoint::~ppoint()
{
   if (dof != 0)
      delete[] q;   
}
void ppoint::setName(QString pname)
{
   name = pname;
}

QString ppoint::getName()
{
   return name;
}
void ppoint::operator = (ppoint a)
{
   int i=0;
   while(i<dof && i<a.dof)
   {
      q[i] = a.q[i];
      i++;
   }
   name = a.name;
}
