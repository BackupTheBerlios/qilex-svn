/***************************************************************************
                          point.cpp  -  description
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

#include "point.h"

class Qstring;

point::point(){}
point::point(Rhmatrix pmatrix, QString pname)
{
   value = pmatrix;
   name = pname;
}
void point::setName(QString pname)
{
   name = pname;
}
QString point::getName()
{
   return name;
}
void point::setValue (Rhmatrix pmatrix)
{
   value = pmatrix;
}
Rhmatrix point::getValue ()
{
   return value;
}

point::~point()
{
}

void point::operator = (point a)
{
   name = a.name;
   value = a.value;
}
