/***************************************************************************
                          list_ppoint.cpp  -  description
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

#include "list_ppoint.h"

list_ppoint::list_ppoint(int pdof)
{
   dof = pdof;
}
list_ppoint::~list_ppoint(){
}
list_ppoint::list_ppoint()
{
   dof = 0;
}
void list_ppoint::init(int pdof)
{
   dof = pdof;
}

ppoint list_ppoint::manage(QString name)
{
   ppoint p(dof);

   l_iter = l_ppoint.begin();

   while(l_iter != l_ppoint.end() && (*l_iter).name != name)
   {
      l_iter++;
   }
   if (l_iter != l_ppoint.end())
   {
      p = (*l_iter);
   }
   return(p);
}
bool list_ppoint::deletep (QString name)
{
   bool result = true;

   l_iter = l_ppoint.begin();

   while(l_iter != l_ppoint.end() && (*l_iter).name != name )
   {
      l_iter++;
   }
   if (l_iter == l_ppoint.end())
   {
      result = false;
   }
   else
   {
      l_ppoint.erase(l_iter);
   }
   return(result);
}
void list_ppoint::update(ppoint p)
{
   l_iter = l_ppoint.begin();

   while(l_iter != l_ppoint.end() && (*l_iter).name != p.name )
   {
      l_iter++;
   }
   if (l_iter == l_ppoint.end())
   {
      l_ppoint.push_back(p);
   }
   else
   {
      (*l_iter).name = p.name;
      int i=0;
      while(i<dof && i<p.dof)
      {
         (*l_iter).q[i] = p.q[i];
         i++;
      }
   }
}
