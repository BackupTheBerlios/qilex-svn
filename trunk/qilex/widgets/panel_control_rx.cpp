/***************************************************************************
                          panelControlRx.cpp  -  description
                             -------------------
    begin                : Wed Jan 31 11:38:17 CET 2003
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

#include "panel_control_rx.h"
#include "../images/gohome.xpm"

/* 
 *  Constructs a slider_rot which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
panelControlRx::panelControlRx( QWidget* parent,  const char* name, Rchain_rx* pkinechain)
    : QWidget( parent, name)                                       
{
   if ( !name )
      setName( "joint_widget" );

       if ( !name )
	setName( "panel_joint" );

   setGeometry( QRect( 10, 10, 303, 433 ) );
   mainlayout = new QVBoxLayout( this, 11, 6, "mainlayout");

   if (pkinechain != NULL)
   {
      kinechain = pkinechain;
   }
   else
   {
      cerr << "kinechain is null" << endl;
      exit (1);
   }
   parser = new QvalParser(kinechain);
    
   dials = new QGroupBox( this, "dials" );
//   dials->setColumnLayout(dof, Qt::Vertical );
   dials->setColumns(kinechain->dof);
   dials->setOrientation(Qt::Vertical);
   dials->layout()->setSpacing( 3 );
   dials->layout()->setMargin( 3 );
   dials->setTitle(tr("Joint's Values"));
  
   ldial = new Qpanel_joint*[kinechain->dof];

   for(int i = 0; i< kinechain->dof ; i++)
   {
      ldial[i] = new Qpanel_joint(dials);
      ldial[i]->setLabelJoin(i + 1) ;
   }  
   
   mainlayout->addWidget( dials );
  
   GroupBox = new QGroupBox( this, "GroupBox" );
   GroupBox->setColumns(7);
   GroupBox->setOrientation(Qt::Horizontal);
   GroupBox->layout()->setSpacing( 3 );
   GroupBox->layout()->setMargin( 5 );
   GroupBox->setMaximumHeight (125 ); //setGeometry( QRect( 12, 115, 299, 140 ) );
 
   QWidget* pWidgetX = new QWidget( GroupBox, "widgetX" );
   QWidget* pWidgetY = new QWidget( GroupBox, "widgetY" );
   QWidget* pWidgetZ = new QWidget( GroupBox, "widgetZ" );
   QWidget* pWidgetRX = new QWidget( GroupBox, "widgetRX" );
   QWidget* pWidgetRY = new QWidget( GroupBox, "widgetRY" );
   QWidget* pWidgetRZ = new QWidget( GroupBox, "widgetRZ" );
   QWidget* pWidgetBut = new QWidget( GroupBox, "widgetBut" );

   layoutX = new QVBoxLayout( pWidgetX, 0, 3, "layoutX");
   layoutY = new QVBoxLayout( pWidgetY, 0, 3, "layoutY");
   layoutZ = new QVBoxLayout( pWidgetZ, 0, 3, "layoutZ");

   layoutRX = new QVBoxLayout( pWidgetRX, 0, 3, "layoutRX");
   layoutRY = new QVBoxLayout( pWidgetRY, 0, 3, "layoutRY");
   layoutRZ = new QVBoxLayout( pWidgetRZ, 0, 3, "layoutRZ");

   layoutBut = new QVBoxLayout( pWidgetBut, 0, 3, "layoutBut");

   TextLabelX = new QLabel( pWidgetX, "TextLabelX" );
   QFont TextLabelX_font(  TextLabelX->font() );
   TextLabelX_font.setBold( TRUE );
   TextLabelX->setText( trUtf8( "X" ) );
   TextLabelX->setFont( TextLabelX_font );
   layoutX->addWidget( TextLabelX );


   Xinc = new QPushButton( pWidgetX, "pushXinc" );
   Xinc->setText("+");
   Xinc->setMinimumSize(10,10);
   layoutX->addWidget( Xinc );

   Xdec = new QPushButton( pWidgetX, "pushXdec" );
   Xdec->setText("-");
   Xdec->setMinimumSize(10,10);
   layoutX->addWidget( Xdec );


   TextLabelY = new QLabel( pWidgetY, "TextLabelY" );
   QFont TextLabelY_font(  TextLabelY->font() );
   TextLabelY_font.setBold( TRUE );
   TextLabelY->setFont( TextLabelY_font );
   TextLabelY->setText( trUtf8( "Y" ) );
   layoutY->addWidget( TextLabelY );

   Yinc = new QPushButton( pWidgetY, "pushYinc" );
   Yinc->setText("+");
   Yinc->setMinimumSize(10,10);
   layoutY->addWidget( Yinc );

   Ydec = new QPushButton( pWidgetY, "pushYdec" );
   Ydec->setText("-");
   Ydec->setMinimumSize(10,10);
   layoutY->addWidget( Ydec );


   TextLabelZ = new QLabel( pWidgetZ, "TextLabelZ" );
   QFont TextLabelZ_font(  TextLabelZ->font() );
   TextLabelZ_font.setBold( TRUE );
   TextLabelZ->setFont( TextLabelZ_font );
   TextLabelZ->setText( trUtf8( "Z" ) );
   layoutZ->addWidget( TextLabelZ );


   Zinc = new QPushButton( pWidgetZ, "pushZinc" );
   Zinc->setText("+");
   Zinc->setMinimumSize(10,10);
   layoutZ->addWidget( Zinc );

   Zdec = new QPushButton( pWidgetZ, "pushZdec" );
   Zdec->setText("-");
   Zdec->setMinimumSize(10,10);
   layoutZ->addWidget( Zdec );


   TextLabelRX = new QLabel( pWidgetRX, "TextLabelRX" );
   QFont TextLabelRX_font(  TextLabelRX->font() );
   TextLabelRX_font.setBold( TRUE );
   TextLabelRX->setFont( TextLabelRX_font );
   TextLabelRX->setText( trUtf8( "RX" ) );
   layoutRX->addWidget( TextLabelRX );


   RXinc = new QPushButton( pWidgetRX, "pushRXinc" );
   RXinc->setText("+");
   RXinc->setMinimumSize(10,10);
   layoutRX->addWidget( RXinc );

   RXdec = new QPushButton( pWidgetRX, "pushRXdec" );
   RXdec->setText("-");
   RXdec->setMinimumSize(10,10);
   layoutRX->addWidget( RXdec );


   TextLabelRY = new QLabel( pWidgetRY, "TextLabelRY" );
   QFont TextLabelRY_font(  TextLabelRY->font() );
   TextLabelRY_font.setBold( TRUE );
   TextLabelRY->setFont( TextLabelRY_font );
   TextLabelRY->setText( trUtf8( "RY" ) );
   layoutRY->addWidget( TextLabelRY );

   
   RYinc = new QPushButton( pWidgetRY, "pushRYinc" );
   RYinc->setText("+");
   RYinc->setMinimumSize(10,10);
   layoutRY->addWidget( RYinc );

   RYdec = new QPushButton( pWidgetRY, "pushRYdec" );
   RYdec->setText("-");
   RYdec->setMinimumSize(10,10);
   layoutRY->addWidget( RYdec );


   TextLabelRZ = new QLabel( pWidgetRZ, "TextLabelRZ" );
   QFont TextLabelRZ_font(  TextLabelRZ->font() );
   TextLabelRZ_font.setBold( TRUE );
   TextLabelRZ->setFont( TextLabelRZ_font );
   TextLabelRZ->setText( trUtf8( "RZ" ) );
   layoutRZ->addWidget( TextLabelRZ );


   RZinc = new QPushButton( pWidgetRZ, "pushRZinc" );
   RZinc->setText("+");
   RZinc->setMinimumSize(10,10);
   layoutRZ->addWidget( RZinc );

   RZdec = new QPushButton( pWidgetRZ, "pushRZdec" );
   RZdec->setText("-");
   RZdec->setMinimumSize(10,10);
   layoutRZ->addWidget( RZdec );


   check_world = new QCheckBox( pWidgetBut, "check_world" );
   check_world->setText( trUtf8( "World" ) );
   layoutBut->addWidget( check_world );
     

   check_tool = new QCheckBox( pWidgetBut, "check_tool" );
   check_tool->setText( trUtf8( "Tool" ) );
   layoutBut->addWidget( check_tool );

	check_net = new QCheckBox( pWidgetBut, "check_net" );
   check_net->setText( trUtf8( "Net Server" ) );
   layoutBut->addWidget( check_net );
	
   pushButton = new QPushButton( pWidgetBut, "pushButton" );

   QPixmap buttonhome(gohome); 

   pushButton->setPixmap(buttonhome);
   layoutBut->addWidget( pushButton );

   TextBrowser = new consoleWidget( this, "Terminal" );

   TextBrowser->setMinimumSize(330,300);

   mainlayout->addWidget(GroupBox);
   mainlayout->addWidget(TextBrowser);

   connect(pushButton, SIGNAL(clicked()), this, SLOT(slotmove_home()));

   slot_setworld();

   connect(check_world, SIGNAL(clicked()), this, SLOT(slot_setworld()));
   connect(check_tool, SIGNAL(clicked()), this, SLOT(slot_settool()));
   connect(check_net, SIGNAL(clicked()), this, SLOT(slot_netserver()));
	                                                          
   connect(Xinc,SIGNAL(clicked()), this, SLOT(slot_buttonXinc()));
   connect(Xdec,SIGNAL(clicked()), this, SLOT(slot_buttonXdec()));
   connect(Yinc,SIGNAL(clicked()), this, SLOT(slot_buttonYinc()));
   connect(Ydec,SIGNAL(clicked()), this, SLOT(slot_buttonYdec()));
   connect(Zinc,SIGNAL(clicked()), this, SLOT(slot_buttonZinc()));
   connect(Zdec,SIGNAL(clicked()), this, SLOT(slot_buttonZdec()));
   
   connect(RXinc,SIGNAL(clicked()), this, SLOT(slot_buttonRXinc()));
   connect(RXdec,SIGNAL(clicked()), this, SLOT(slot_buttonRXdec()));
   connect(RYinc,SIGNAL(clicked()), this, SLOT(slot_buttonRYinc()));
   connect(RYdec,SIGNAL(clicked()), this, SLOT(slot_buttonRYdec()));
   connect(RZinc,SIGNAL(clicked()), this, SLOT(slot_buttonRZinc()));
   connect(RZdec,SIGNAL(clicked()), this, SLOT(slot_buttonRZdec()));

   connect(kinechain,SIGNAL(messageChanged(QString)),  TextBrowser, SLOT(insertLineAtEnd(QString)));
   kinechain->setconsole(TextBrowser);
   connect(TextBrowser,SIGNAL(keyboard_input(QString)), parser, SLOT(set_stdin(QString)));
   connect(parser, SIGNAL (set_stdout(QString )), TextBrowser, SLOT(insertLineAtEnd(QString)));
	
	netserver = new netthread();
	netserver->init(this);

//  resize( QSize(336, 499).expandedTo(minimumSizeHint()) );

}
panelControlRx::~panelControlRx()
{
    // no need to delete child widgets, Qt does it all for us
    delete kinechain;
    delete parser;
	 delete netserver;
}

void panelControlRx::slotmove_home()
{
   kinechain->do_ready();
}
void panelControlRx::update_limits()
{
   double min, max;
   for(int i=0; i<kinechain->dof;i++)
   {
      kinechain->limits(i, min, max);
      ldial[i]->setRankJoin(min,max);
   }
}

bool panelControlRx::close(bool autoDelete)
{
  //hide(); //cal posar quelcom
  return FALSE;
}

void panelControlRx::slot_buttonXinc()
{
   emit newValueX(microinterval);
}
void panelControlRx::slot_buttonYinc()
{
   emit newValueY(microinterval);
}
void panelControlRx::slot_buttonZinc()
{
   emit newValueZ(microinterval);
}
void panelControlRx::slot_buttonRXinc()
{
   emit newValueRX(microangle);
}
void panelControlRx::slot_buttonRYinc()
{
   emit newValueRY(microangle);
}
void panelControlRx::slot_buttonRZinc()
{
   emit newValueRZ(microangle);
}



void panelControlRx::slot_buttonXdec()
{
   emit newValueX(-microinterval);
}
void panelControlRx::slot_buttonYdec()
{
   emit newValueY(-microinterval);
}
void panelControlRx::slot_buttonZdec()
{
   emit newValueZ(-microinterval);
}
void panelControlRx::slot_buttonRXdec()
{
   emit newValueRX(-microangle);
}
void panelControlRx::slot_buttonRYdec()
{
   emit newValueRY(-microangle);
}
void panelControlRx::slot_buttonRZdec()
{
   emit newValueRZ(-microangle);
}

void panelControlRx::slot_setworld()
{
   check_world->setChecked(true);
   check_tool->setChecked(false); 
	check_net->setChecked(false); 
	netserver->exit();		
	//comprobar que existe i si existe
	//mutex server .....
	//must add some more code

   disconnect( this, SIGNAL(newValueX(double)), 0, 0 );
   disconnect( this, SIGNAL(newValueY(double)), 0, 0 );
   disconnect( this, SIGNAL(newValueZ(double)), 0, 0 );

   disconnect( this, SIGNAL(newValueRX(double)), 0, 0 );
   disconnect( this, SIGNAL(newValueRY(double)), 0, 0 );
   disconnect( this, SIGNAL(newValueRZ(double)), 0, 0 );

   connect(this, SIGNAL(newValueX(double)), kinechain, SLOT(slot_world_X (double )));
   connect(this, SIGNAL(newValueY(double)), kinechain, SLOT(slot_world_Y (double )));
   connect(this, SIGNAL(newValueZ(double)), kinechain, SLOT(slot_world_Z (double )));

   connect(this, SIGNAL(newValueRX(double)), kinechain, SLOT(slot_world_RX (double )));
   connect(this, SIGNAL(newValueRY(double)), kinechain, SLOT(slot_world_RY (double )));
   connect(this, SIGNAL(newValueRZ(double)), kinechain, SLOT(slot_world_RZ (double )));

}

void panelControlRx::slot_settool()
{
   check_world->setChecked(false);
   check_tool->setChecked(true);
	check_net->setChecked(false); //must add some more code
   //comprobar que existe i si existe
		//mutex server .....
		//must add some more code
	netserver->exit();	
	
   disconnect( this, SIGNAL(newValueX(double)), 0, 0 );
   disconnect( this, SIGNAL(newValueY(double)), 0, 0 );
   disconnect( this, SIGNAL(newValueZ(double)), 0, 0 );

   disconnect( this, SIGNAL(newValueRX(double)), 0, 0 );
   disconnect( this, SIGNAL(newValueRY(double)), 0, 0 );
   disconnect( this, SIGNAL(newValueRZ(double)), 0, 0 );

   connect(this, SIGNAL(newValueX(double)), kinechain, SLOT(slot_tool_X (double )));
   connect(this, SIGNAL(newValueY(double)), kinechain, SLOT(slot_tool_Y (double )));
   connect(this, SIGNAL(newValueZ(double)), kinechain, SLOT(slot_tool_Z (double )));

   connect(this, SIGNAL(newValueRX(double)), kinechain, SLOT(slot_tool_RX (double )));
   connect(this, SIGNAL(newValueRY(double)), kinechain, SLOT(slot_tool_RY (double )));
   connect(this, SIGNAL(newValueRZ(double)), kinechain, SLOT(slot_tool_RZ (double )));


}                   

void panelControlRx::slot_netserver()
{
   check_world->setChecked(false);
   check_tool->setChecked(false); 
	check_net->setChecked(true); //must add some more code
	netserver->start();
	
	double q[6];
	q[0] = 0; q[1] = rad(-65); q[2] = rad(195);
	q[3] = 0; q[4] = rad(-45); q[5] =0;
   kinechain->move(q);
	

   disconnect( this, SIGNAL(newValueX(double)), 0, 0 );
   disconnect( this, SIGNAL(newValueY(double)), 0, 0 );
   disconnect( this, SIGNAL(newValueZ(double)), 0, 0 );

   disconnect( this, SIGNAL(newValueRX(double)), 0, 0 );
   disconnect( this, SIGNAL(newValueRY(double)), 0, 0 );
   disconnect( this, SIGNAL(newValueRZ(double)), 0, 0 );
	
}




void panelControlRx::slot_updatetext(QString &line)
{
   TextBrowser->insertLineAtEnd(line);
}
void panelControlRx::move_pickpoint(Rhmatrix point)
{
     kinechain->move(point);
}



