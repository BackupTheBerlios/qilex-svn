/***************************************************************************
                          soqtcomposerotation.h  -  description
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

#ifndef SOQTCOMPOSEROTATION_H
#define SOQTCOMPOSEROTATION_H


#include <qobject.h>
#include <Inventor/SbVec3f.h>
#include <Inventor/engines/SoCompose.h>
#include <Inventor/engines/SoSubEngine.h>
#include <Inventor/engines/SoComposeRotation.h>
#include <Inventor/lists/SoEngineOutputList.h>

#include <Inventor/fields/SoMFRotation.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFVec3f.h>

#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoMaterial.h>


/**
  *@author Leopold Palomo Avellaneda
  */

class SoQtComposeTranslation : public QObject, public SoEngine{
   Q_OBJECT

   SO_ENGINE_HEADER(SoQtComposeTranslation);
   
   public: 


   //Input fields
      int index; //z , default D-H

      float axis[3];
      SoSFFloat delta;
           
      SoEngineOutput translation; //SoTranslation
      
      float oldvalue;

      SoQtComposeTranslation();
      ~SoQtComposeTranslation();

      static void initClass();

   public slots:
      void setValue_delta(double);
   signals:
      void valueNew(double);  
   private:
      virtual void evaluate();
};          

#endif
