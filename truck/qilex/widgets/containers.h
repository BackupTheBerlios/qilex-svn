/***************************************************************************
                          containers.h  -  description
                             -------------------
    begin                : Wed Feb 26 2003
    copyright            : (C) 2003 by Leopold Palomo Avellaneda, Xavier Sierra
    email                : lepalom@wol.es, xavsierra@yahoo.es
     ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef CONTAINERS_H
#define CONTAINERS_H
#include <qstring.h>

struct ct_new_kinematic_chain
{
   QString QsModelFile;
   QString QsDatFile;
   QString QsName;

   int simulator_engine;

   float x, y, z, axeX, axeY, axeZ, angle;

   int error_number;
   bool accepted;
};

struct ct_new_geometric_object
{
   QString QsModelFile;
   QString QsName;

   float x, y, z, axeX, axeY, axeZ, angle;
   int error_number;
};

struct ct_grasp_points
{
   int point;
   double px;
   double py;
   double pz;
   double nx;
   double ny;
   double nz;
};

struct ct_new_grasping_object
{
   QString QsModelFile;
   QString QsDatFile;
   QString QsName;
   ct_grasp_points* grasp_points;

   int num_point;
   float x, y, z, axeX, axeY, axeZ, angle;
   int error_number;
   //bool accepted;
};



#endif // CONTAINERS_H
