#include "qpanel_join.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qlcdnumber.h>
#include <qslider.h>
#include <qlayout.h>



Qpanel_join::Qpanel_join( QWidget* parent,  const char* name)
    : QWidget( parent, name)
{
   if ( !name )
      setName( "qpanel_join" );

   resize( 294, 10 ); 
      
   join = new QLabel( this, "join" );
   join->setGeometry( QRect( 0, 10, 33, 21 ) ); 
   join->setText( trUtf8( "join" ) );

   unit = new QLabel( this, "unit" );
   unit->setGeometry( QRect( 260, 10, 31, 21 ) ); 
   unit->setText( trUtf8( "deg" ) );

   slider = new QSlider( this, "slider" );
   slider->setGeometry( QRect( 50, 10, 120, 20 ) ); 
   slider->setMinValue( -720 );
   slider->setMaxValue( 720 );
   slider->setOrientation( QSlider::Horizontal );

   join_value = new QLCDNumber( this, "join_value" );
   join_value->setGeometry( QRect( 190, 10, 64, 23 ) ); 
   join_value->setMode( QLCDNumber::DEC );
   join_value->setSegmentStyle( QLCDNumber::Flat);   

   connect( slider, SIGNAL( valueChanged(int) ), this, SLOT( ChangeValue_int(int) ) );
   connect( this, SIGNAL( ValueChange_int(int) ), slider , SLOT( setValue(int) ) );
   connect( this, SIGNAL( ValueChange2(double) ), join_value, SLOT( display(double) ) );
}

Qpanel_join::~Qpanel_join(){
}
void Qpanel_join::ValueChange_out(double d_val)
{
   if (d_val != d_value)
   {
      d_value = d_val;
      i_value = (int) (degree(d_val) * 2);
      emit ValueChange_int(i_value);
      emit ValueChange_in(d_value);
      emit ValueChange2((double)i_value);
   }
}
void Qpanel_join::ChangeValue_int(int i_val)
{
   if (i_val != i_value)
   {
      i_value = i_val;
      d_value = (int) rad((double)i_val) / 2;
      emit ValueChange_int(i_value);
      emit ValueChange_in(d_value);
      emit ValueChange2((double)(i_value) / 2)));
   }
}
void Qpanel_join::setMinValue(int val)
{
   slider->setMinValue( val );
}
void Qpanel_join::setMaxValue(int val)
{
   slider->setMaxValue(val);
}
void Qpanel_join::setLabelJoin(int i)
{
   QString data;
   data.setNum(i,10);
   join->setText(join->text().append(data));
}
void Qpanel_join::setRankJoin(double max, double min)
{
   slider->setMinValue((int) min * 2);
   slider->setMaxValue((int) max * 2);
}

