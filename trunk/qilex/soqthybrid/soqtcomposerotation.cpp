/***************************************************************************
                          soqtcomposerotation.cpp  -  description
                             -------------------
    begin                : Wed Jan 29 2003
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

#include "soqtcomposerotation.h"

SO_ENGINE_SOURCE(SoQtComposeRotation);


SoQtComposeRotation::SoQtComposeRotation()
{

   SO_ENGINE_CONSTRUCTOR(SoQtComposeRotation);

   SO_ENGINE_ADD_INPUT(axis, (0.0f,0.0f,0.0f));
   SO_ENGINE_ADD_INPUT(angle,(0.0f));
   SO_ENGINE_ADD_OUTPUT(rotation, SoMFRotation);
   

}
SoQtComposeRotation::~SoQtComposeRotation(){
}

void SoQtComposeRotation::evaluate()
{
  SO_ENGINE_OUTPUT(rotation, SoMFRotation, setValue(axis.getValue(), angle.getValue()));
}
void SoQtComposeRotation::setValue_angle(double d_angle)
{
   if (oldvalue != d_angle)
   {
      oldvalue = d_angle;
      angle.setValue((float)d_angle);
      emit valueNew(d_angle);
   }
}
void SoQtComposeRotation::initClass()
{
   SO_ENGINE_INIT_CLASS(SoQtComposeRotation,SoEngine,"Engine");
}
