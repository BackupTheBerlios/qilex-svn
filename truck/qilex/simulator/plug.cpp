/***************************************************************************
                          plug.cpp  -  description
                             -------------------
    begin                : Tue Feb 25 2003
    copyright            : (C) 2003 by Leopold Palomo
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

#include "plug.h"

plug::plug()
{
}
void plug::setIndex(int ind)
{
   index = ind;
}    

plug::~plug(){
}
void plug::setValue(double value)
{
   if (value != pval)
   {
      pval = value;
      emit valueChanged(pval);
      emit valueChangedfull (index + 1 , pval);
   }
}

