/***************************************************************************
                          qpanel_joint.cpp  -  description
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


#include "qpanel_joint.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qlcdnumber.h>
#include <qslider.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

Qpanel_joint::Qpanel_joint( QWidget* parent,  const char* name)
    : QWidget( parent, name)
{
   if ( !name )
      setName( "qpanel_joint" );

   format = 1; // mode degree, default

   layout = new QHBoxLayout( this, 3, 3, "layout"); 

   label_joint = new QLabel( this , "label_joint" );
   label_joint->setText( tr( "joint" ) );
   layout->addWidget( label_joint );

   slider = new QSlider( this , "slider" );
   slider->setMinValue( -720 );
   slider->setMaxValue( 720 );
   slider->setOrientation( QSlider::Horizontal );
   layout->addWidget( slider );

   joint_value = new QLCDNumber( this, "joint_value" );
   joint_value->setMode( QLCDNumber::DEC );
   joint_value->setSegmentStyle( QLCDNumber::Flat);
   layout->addWidget( joint_value );

   unit = new QLabel( this, "unit" );
   unit->setText( tr( "deg" ) );
   layout->addWidget( unit );

   connect( slider, SIGNAL( valueChanged(int) ), this, SLOT( setValue_int(int) ) );
}
void Qpanel_joint::setformat(int fmt)
{
   switch( fmt )
   {
      case 0 :       //mm lineal
                  format = fmt;
                  unit->setText( tr( "mm" ) );

                  break;

      case 1 :   //radians//degree no lineal
                  unit->setText( tr( "deg" ) );
                  format = fmt;

                  break;

      default  :   break;
   }
}

void Qpanel_joint::setValue_int(int i_val)
{
   if (i_value != i_val)
   {
      i_value = i_val;
      if(format == 1) //revolute
      {
         d_value = rad((double)i_value/ 2); //passem a radians amb definició de 0.5 graus
         emit valueChange(d_value);
         joint_value->display(degree(d_value));
      }
      else
      {
         d_value = i_val / 2;
         emit valueChange(d_value);
         joint_value->display(d_value);
      }
   }
}

void Qpanel_joint::setValue(double value)
{
   if (d_value != value)
   {
      d_value = value;
      if(format == 1) //revolute
      {
         i_value = (int)(2 * degree(d_value)); //passem a graus amb def 0.5
         slider->setValue(i_value); 
         joint_value->display(degree(d_value));
      }
      else
      {
         i_value = (int) (2 * d_value);
         slider->setValue(i_value);
         joint_value->display(d_value);
      }
   }
}

Qpanel_joint::~Qpanel_joint(){
}
void Qpanel_joint::setMinValue(double val)
{
   if (format==0)
   {
      slider->setMinValue((int)( val*2 ));
   }
   else
   {
      slider->setMinValue((int)( degree(val*2) ));
   }
}
void Qpanel_joint::setMaxValue(double val)
{
   if (format==0)
   {
      slider->setMaxValue((int)( val*2 ));
   }
   else
   {
      slider->setMaxValue((int)( degree(val*2) ));
   }


}
void Qpanel_joint::setLabelJoin(int i)
{
   QString data;
   data.setNum(i,10);
   label_joint->setText(label_joint->text().append(data));
}

void Qpanel_joint::setRankJoin(double min, double max)
{
   if (format==0)
   {
      slider->setMinValue((int)( min * 2));
      slider->setMaxValue((int)( max * 2));
   }
   else
   {
      slider->setMinValue((int)( degree(min) * 2 ));
      slider->setMaxValue((int)( degree(max) * 2 ));
   }
}


