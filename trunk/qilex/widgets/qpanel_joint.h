/***************************************************************************
                          qpanel_joint.h  -  description
                             -------------------
    begin                : Thu Jan 30 2003
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


#ifndef QPANEL_JOINT_H
#define QPANEL_JOINT_H

#include <qwidget.h>
#include <qvariant.h>
#include <qlcdnumber.h>
#include <qslider.h>
#include <qlayout.h>
#include "../functions.h"
/**
  *@author Leopold Palomo Avellaneda
  */

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLCDNumber;
class QLabel;
class QSlider;


class Qpanel_joint : public QWidget  {
   Q_OBJECT

   public:
	   Qpanel_joint(QWidget *parent=0, const char *name=0);
      ~Qpanel_joint();

      QLabel* label_joint;
      QSlider* slider;
      QLCDNumber* joint_value;
      QLabel* unit;
      void setLabelJoin(int i);
      void setRankJoin(double, double);
      void setformat(int fmt);
      void setMinValue(double);
      void setMaxValue(double);
   protected:
      QHBoxLayout* layout;

   public slots:
      void setValue(double);
   private slots:
      void setValue_int(int);
   signals:
      void valueChange(double);

   private:
      int i_value;
      double d_value;
      int format; //0 rads ; 1 degree ; 2 mm


};

#endif // QPANEL_JOINT_H
