/***************************************************************************
                          list_point.cpp  -  description
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

#include "list_point.h"

list_point::list_point(){
}
list_point::~list_point(){
}

point list_point::manage(QString name)
{
   point p;
  
   l_iter = l_point.begin();
   
   while(l_iter != l_point.end() && (*l_iter).name != name )
   {
      l_iter++;
   }
   if (l_iter != l_point.end())
   {
      p = (*l_iter);
   } 
   return(p);   

}
bool list_point::deletep (QString name)
{
   bool result = true;
   
   l_iter = l_point.begin();

   while(l_iter != l_point.end() && (*l_iter).name != name)
   {
      l_iter++;
   }
   if (l_iter == l_point.end())
   {
      result = false;
   }
   else
   {
      l_point.erase(l_iter);
   }
   return(result);
}
void list_point::update(point p)
{

   l_iter = l_point.begin();
   
   while(l_iter != l_point.end() && (*l_iter).name != p.name)
   {
      l_iter++;
   }
   if (l_iter == l_point.end())
   {
      l_point.push_back(p);
   }
   else
   {
      (*l_iter).name = p.name;
      (*l_iter).value = p.value;      
   }
}
