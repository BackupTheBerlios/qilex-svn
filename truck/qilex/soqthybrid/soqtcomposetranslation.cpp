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

#include "soqtcomposetranslation.h"

SO_ENGINE_SOURCE(SoQtComposeTranslation);


SoQtComposeTranslation::SoQtComposeTranslation()
{

   SO_ENGINE_CONSTRUCTOR(SoQtComposeTranslation);

   SO_ENGINE_ADD_INPUT(delta,(0.0));
 //  SO_ENGINE_ADD_INPUT(axis, (0.0f,0.0f,0.0f));

   SO_ENGINE_ADD_OUTPUT(translation, SoSFVec3f);
   index = 2;
   axis[0] = axis[1] = axis[2] = 0;
}
SoQtComposeTranslation::~SoQtComposeTranslation(){
}

void SoQtComposeTranslation::evaluate()
{
  SO_ENGINE_OUTPUT(translation, SoSFVec3f, setValue(axis));
}
void SoQtComposeTranslation::setValue_delta(double d_delta)
{
   if (oldvalue != d_delta)
   {
      oldvalue = (float)d_delta;
      axis[index] = (float)d_delta;
      emit valueNew(d_delta);
   }
}
void SoQtComposeTranslation::initClass()
{
   SO_ENGINE_INIT_CLASS(SoQtComposeTranslation,SoEngine,"Engine");
}
