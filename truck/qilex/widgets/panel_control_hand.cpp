/***************************************************************************
 *   Copyright (C) 2003 by Xavier Sierra                                   *
 *   xavsierra@yahoo.es                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

 #include "panel_control_hand.h"
#include "images/gohome.xpm"
#include <qtextstream.h>
#include "functions.h"

/* 
 *  Constructs a slider_rot which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
panel_control_hand::panel_control_hand( QWidget* parent,  const char* name, Rchain_hand *pkinechain)
    : QWidget( parent, name)
{
   setCaption(tr("Qilex0.4 " ));
   grasp_number=0;
   if ( !name )
      setName( "joint_widget" );

       if ( !name )
	setName( "panel_joint" );

   setGeometry( QRect( 10, 10, 303, 840 ) );
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
//   parser = new QvalParser(kinechain);

   choice = new QButtonGroup (this,"Fingers");
   choice->setGeometry( 10, 10, 100, 100);
   choice->setTitle(tr("Fingers"));

   f0 = new QRadioButton("F1", choice);
   f0->move( 20, 20 );
   f0->setChecked(TRUE);
   f1 = new QRadioButton("F2", choice);
   f1->move( 80, 20 );
   f2 = new QRadioButton("F3", choice);
   f2->move( 140, 20 );
   f3 = new QRadioButton("F4", choice);
   f3->move( 200, 20 );

   choice->insert( f0,0 );
   choice->insert( f1,1 );
   choice->insert( f2,2 );
   choice->insert( f3,3 );

   dials_arm = new QGroupBox( this, "dials_arm" );
   dials_arm->setColumns(6);
   dials_arm->setOrientation(Qt::Vertical);
   dials_arm->layout()->setSpacing( 3 );
   dials_arm->layout()->setMargin( 3 );
   dials_arm->setTitle(tr("Arm's Joints"));

   dials_f0 = new QGroupBox( this, "dials_finger" );
   dials_f0->setColumns(4);
   dials_f0->setOrientation(Qt::Vertical);
   dials_f0->layout()->setSpacing( 3 );
   dials_f0->layout()->setMargin( 3 );
   dials_f0->setTitle(tr("Finger1's Joints"));

   dials_f1 = new QGroupBox( this, "dials_finger" );
   dials_f1->setColumns(4);
   dials_f1->setOrientation(Qt::Vertical);
   dials_f1->layout()->setSpacing( 3 );
   dials_f1->layout()->setMargin( 3 );
   dials_f1->setTitle(tr("Finger2's Joints"));

   dials_f2 = new QGroupBox( this, "dials_finger" );
   dials_f2->setColumns(4);
   dials_f2->setOrientation(Qt::Vertical);
   dials_f2->layout()->setSpacing( 3 );
   dials_f2->layout()->setMargin( 3 );
   dials_f2->setTitle(tr("Finger3's Joints"));

   dials_f3 = new QGroupBox( this, "dials_finger" );
   dials_f3->setColumns(4);
   dials_f3->setOrientation(Qt::Vertical);
   dials_f3->layout()->setSpacing( 3 );
   dials_f3->layout()->setMargin( 3 );
   dials_f3->setTitle(tr("Finger4's Joints"));

   ldial = new Qpanel_joint*[22];

   for(int i = 0; i< kinechain->n_same ; i++)
   {
      ldial[i] = new Qpanel_joint(dials_arm);
      ldial[i]->setLabelJoin(i + 1) ;
   }
   for(int i = kinechain->n_same; i< kinechain->n_same+4 ; i++)
   {
      ldial[i] = new Qpanel_joint(dials_f0);
      ldial[i]->setLabelJoin(i + 1) ;
   }
   for(int i=kinechain->n_same+4; i< kinechain->n_same+8 ; i++)
   {
      ldial[i] = new Qpanel_joint(dials_f1);
      ldial[i]->setLabelJoin(i + 1 - 4) ;
   }
   for(int i=kinechain->n_same+8; i< kinechain->n_same+12 ; i++)
   {
      ldial[i] = new Qpanel_joint(dials_f2);
      ldial[i]->setLabelJoin(i + 1 - 8) ;
   }
   for(int i = kinechain->n_same+12; i< kinechain->n_joint ; i++)
   {
      ldial[i] = new Qpanel_joint(dials_f3);
      ldial[i]->setLabelJoin(i + 1 - 12) ;
   }

   mainlayout->addWidget( dials_arm );
   mainlayout->addWidget( choice );
   mainlayout->insertWidget(2, dials_f0 );
   dials_f0->setShown(TRUE);
   mainlayout->insertWidget(2, dials_f1 );
   dials_f1->setHidden(TRUE);
   mainlayout->insertWidget(2, dials_f2 );
   dials_f2->setHidden(TRUE);
   mainlayout->insertWidget(2, dials_f3 );
   dials_f3->setHidden(TRUE);

   connect( f0, SIGNAL( clicked() ), this, SLOT( slot_finger0() ) );
   connect( f1, SIGNAL( clicked() ), this, SLOT( slot_finger1() ) );
   connect( f2, SIGNAL( clicked() ), this, SLOT( slot_finger2() ) );
   connect( f3, SIGNAL( clicked() ), this, SLOT( slot_finger3() ) );

   GroupBox = new QButtonGroup( this, "GroupBox" );
   GroupBox->setGeometry(10,10,100,100);
   GroupBox->setTitle(tr("Inverse kinematics"));

   kinematic = new QPushButton ( "kinematic", GroupBox );
   kinematic->setText("GO");
   kinematic->setGeometry(10,25,60,20);
   GroupBox->insert( kinematic );

   kinematic_retry = new QPushButton ( "kinematic_retry", GroupBox );
   kinematic_retry->setText("RETRY");
   kinematic_retry->setGeometry(10,50,60,20);
   GroupBox->insert( kinematic_retry );

   kinematic_where = new QPushButton ( "kinematic_stop", GroupBox );
   kinematic_where->setText("WHERE");
   kinematic_where->setGeometry(10,75,60,20);
   GroupBox->insert( kinematic_where );

   pushButton = new QPushButton( "pushButton", GroupBox );
   QPixmap buttonhome(gohome);
   pushButton->setPixmap(buttonhome);
   pushButton->setGeometry(10,100,60,20);
   GroupBox->insert( pushButton );

   TextLabelf0 = new QLabel( "TextLabelf0", GroupBox );
   TextLabelf0->setGeometry(90,25,30,20);
   TextLabelf0->setText( "F1" );

   TextLabelf1 = new QLabel( "TextLabelf1", GroupBox );
   TextLabelf1->setGeometry(90,50,30,20);
   TextLabelf1->setText( "F2" );

   TextLabelf2 = new QLabel( "TextLabelf2", GroupBox );
   TextLabelf2->setGeometry(90,75,30,20);
   TextLabelf2->setText( "F3" );

   TextLabelf3 = new QLabel( "TextLabelf3", GroupBox );
   TextLabelf3->setGeometry(90,100,30,20);
   TextLabelf3->setText( "F4" );

   select_f0 = new QComboBox (GroupBox, "select_f0" );
   select_f0->setGeometry(110,25,60,20);

   select_f1 = new QComboBox (GroupBox, "select_f1" );
   select_f1->setGeometry(110,50,60,20);

   select_f2 = new QComboBox (GroupBox, "select_f2" );
   select_f2->setGeometry(110,75,60,20);

   select_f3 = new QComboBox (GroupBox, "select_f3" );
   select_f3->setGeometry(110,100,60,20);

   select_f0->insertItem("None",0);
   select_f1->insertItem("None",0);
   select_f2->insertItem("None",0);
   select_f3->insertItem("None",0);
   
   comunic0 = new QPushButton ("sim_to_hand", GroupBox);
   comunic0->setText("Qilex->Hand");
   comunic0->setGeometry(180,25,95,20);
   GroupBox->insert( comunic0 );

   comunic1 = new QPushButton ("hand_to_sim", GroupBox);
   comunic1->setText("Hand->Qilex");
   comunic1->setGeometry(180,50,95,20);
   GroupBox->insert( comunic1 );

   comunic2 = new QPushButton ("sim_to_glove", GroupBox);
   comunic2->setText("Qilex->Glove");
   comunic2->setGeometry(180,75,95,20);
   GroupBox->insert( comunic2 );

   comunic3 = new QPushButton ("glove_to_sim", GroupBox);
   comunic3->setText("Glove->Qilex");
   comunic3->setGeometry(180,100,95,20);
   GroupBox->insert( comunic3 );

   comunic_stop = new QPushButton ("stop_comunications", GroupBox);
   comunic_stop->setText("STOP");
   comunic_stop->setGeometry(280,25,45,95);
   GroupBox->insert( comunic_stop );

   mainlayout->insertWidget(6,GroupBox);

   haySocket = 0;
   connect_error = 1;

   connect(pushButton, SIGNAL(clicked()), this, SLOT(slotmove_home()));
   connect(kinematic, SIGNAL(clicked()), this, SLOT(slotgo_inverse()));
   connect(kinematic_where, SIGNAL(clicked()), this, SLOT(slotwhere_inverse()));
   connect(kinematic_retry, SIGNAL(clicked()), this, SLOT(slotretry_inverse()));
   
   connect(comunic0, SIGNAL(clicked()), this, SLOT(crearSocketSlot()));
   connect(comunic_stop, SIGNAL(clicked()), this, SLOT(conexionStoppedSlot()));

   TextBrowser = new consoleWidget( this, "Terminal" );
   TextBrowser->setMinimumSize(330,120);
   mainlayout->addWidget(TextBrowser);

   connect(kinechain,SIGNAL(messageChanged(QString)),  TextBrowser, SLOT(insertLineAtEnd(QString)));
   kinechain->setconsole(TextBrowser);
}
panel_control_hand::~panel_control_hand()
{
    // no need to delete child widgets, Qt does it all for us
    delete kinechain;
    //delete parser;
}

void panel_control_hand::update_limits()
{
 double min, max;

 for(int i=0; i<kinechain->n_joint; i++)
 {
  if(i<6)
  {
   kinechain->limits(i,min,max,0);
  }
  if(i>5 && i<10)
  {
   kinechain->limits(i,min,max,0);
  }
  if(i>9 && i<14)
  {
   kinechain->limits(i-4,min,max,1);
  }
  if(i>13 && i<18)
  {
   kinechain->limits(i-8,min,max,2);
  }
  if(i>17 && i<22)
  {
   kinechain->limits(i-12,min,max,3);
  }
   ldial[i]->setRankJoin(min,max);
  }
}

void panel_control_hand::slot_finger0()
{
 if (dials_f1->isShown())
 {
 dials_f1->setHidden(TRUE);
 }
 if (dials_f2->isShown())
 {
 dials_f2->setHidden(TRUE);
 }
 if (dials_f3->isShown())
 {
 dials_f3->setHidden(TRUE);
 }
 dials_f0->setShown(TRUE);
}

void panel_control_hand::slot_finger1()
{
 if (dials_f0->isShown())
 {
 dials_f0->setHidden(TRUE);
 }
 if (dials_f2->isShown())
 {
 dials_f2->setHidden(TRUE);
 }
 if (dials_f3->isShown())
 {
 dials_f3->setHidden(TRUE);
 }
 dials_f1->setShown(TRUE);
}

void panel_control_hand::slot_finger2()
{
 if (dials_f1->isShown())
 {
 dials_f1->setHidden(TRUE);
 }
 if (dials_f0->isShown())
 {
 dials_f0->setHidden(TRUE);
 }
 if (dials_f3->isShown())
 {
 dials_f3->setHidden(TRUE);
 }
 dials_f2->setShown(TRUE);
}

void panel_control_hand::slot_finger3()
{
 if (dials_f1->isShown())
 {
 dials_f1->setHidden(TRUE);
 }
 if (dials_f2->isShown())
 {
 dials_f2->setHidden(TRUE);
 }
 if (dials_f0->isShown())
 {
 dials_f0->setHidden(TRUE);
 }
 dials_f3->setShown(TRUE);
}

void panel_control_hand::slotmove_home()
{
   kinechain->do_ready();
}

void panel_control_hand::slotupdate_fingers(ct_new_grasping_object *data)
{
  double X,Y,Z,W;
  std::ostringstream pcout;
  A_hand = new Rhmatrix[data->num_point];
  pre = new Rhmatrix[1];
  post = new Rhmatrix[data->num_point];

  grasp_number = data->num_point;

  select_f0->insertItem("point 1",1);
  select_f1->insertItem("point 1",1);
  select_f2->insertItem("point 1",1);
  select_f3->insertItem("point 1",1);
  select_f0->insertItem("point 2",2);
  select_f1->insertItem("point 2",2);
  select_f2->insertItem("point 2",2);
  select_f3->insertItem("point 2",2);
  select_f0->insertItem("point 3",3);
  select_f1->insertItem("point 3",3);
  select_f2->insertItem("point 3",3);
  select_f3->insertItem("point 3",3);
  select_f0->insertItem("point 4",4);
  select_f1->insertItem("point 4",4);
  select_f2->insertItem("point 4",4);
  select_f3->insertItem("point 4",4);

//inicialització del vector sol, que conté els angles solució de la cadena cinemàtica
  sol = new double*[kinechain->n_chain];
  for(int k=0;k<kinechain->n_chain;k++)
  {
  sol[k] = new double[kinechain->dof[0]+1];
  }
  sol1 = new double[kinechain->dof[0]+1];

//definició del sistema de referència de cada punt de grasping respecte del sistema de l'objecte
   for (int i=0;i<data->num_point;i++)
   {
    post[i].mat[3][0]=0.0;
    post[i].mat[3][1]=0.0;
    post[i].mat[3][2]=0.0;
    post[i].mat[3][3]=1.0;
    post[i].mat[0][3]=data->grasp_points[i].px;
    post[i].mat[1][3]=data->grasp_points[i].py;
    post[i].mat[2][3]=data->grasp_points[i].pz;
    post[i].mat[0][2]=data->grasp_points[i].nx;
    post[i].mat[1][2]=data->grasp_points[i].ny;
    post[i].mat[2][2]=data->grasp_points[i].nz;
    post[i].mat[0][0]=-(data->grasp_points[i].ny/data->grasp_points[i].nx)*sqrt(1.0/(1.0+(data->grasp_points[i].ny/data->grasp_points[i].nx)*(data->grasp_points[i].ny/data->grasp_points[i].nx)));
    post[i].mat[1][0]=sqrt(1.0/(1.0+(data->grasp_points[i].ny/data->grasp_points[i].nx)*(data->grasp_points[i].ny/data->grasp_points[i].nx)));
    post[i].mat[2][0] = 0.0;
    post[i].mat[0][1] = post[i].mat[1][2]*post[i].mat[2][0] - post[i].mat[1][0]*post[i].mat[2][2];
    post[i].mat[1][1] = post[i].mat[0][0]*post[i].mat[2][2] - post[i].mat[0][2]*post[i].mat[2][0];
    post[i].mat[2][1] = post[i].mat[0][2]*post[i].mat[1][0] - post[i].mat[0][0]*post[i].mat[1][2];
   }

//definició del sistema de referència de l'objecte respecte del sistema del món
    X = data->axeX*sin(rad(data->angle/2.0));
    Y = data->axeY*sin(rad(data->angle/2.0));
    Z = data->axeZ*sin(rad(data->angle/2.0));
    W = cos(rad(data->angle/2.0));
    pre[0].mat[3][0]=0.0;
    pre[0].mat[3][1]=0.0;
    pre[0].mat[3][2]=0.0;
    pre[0].mat[3][3]=1.0;
    pre[0].mat[0][3]=data->x;
    pre[0].mat[1][3]=data->y;
    pre[0].mat[2][3]=data->z;
    pre[0].mat[0][0]=1.0-2.0*Y*Y-2.0*Z*Z;
    pre[0].mat[0][1]=2.0*X*Y-2.0*Z*W;
    pre[0].mat[0][2]=2.0*X*Z+2.0*Y*W;
    pre[0].mat[1][0]=2.0*X*Y+2.0*Z*W;
    pre[0].mat[1][1]=1.0-2.0*X*X-2.0*Z*Z;
    pre[0].mat[1][2]=2.0*Y*Z-2.0*X*W;
    pre[0].mat[2][0]=2.0*X*Z-2.0*Y*W;
    pre[0].mat[2][1]=2.0*Y*Z+2.0*X*W;
    pre[0].mat[2][2]=1.0-2.0*X*X-2.0*Y*Y;

//Composició de les dues transformacions anteriors
//Sistema de referència de cada grasping point en funció de les coordenades del món
   for(int i=0;i<data->num_point;i++)
   {
    A_hand[i] = pre[0]*post[i];
   }

   pcout << "----------------------------------------------------" << std::endl;
   pcout << "There are " << grasp_number << " grasping points" << std::endl;
   pcout << "----------------------------------------------------" << std::endl;

   for(int k=0;k<data->num_point;k++)
   {
	   pcout << "The trasnformation matrix of point " << k+1 << " is: " << std::endl;
   pcout << std::setiosflags (ios::fixed | ios::showpoint);
   pcout << std::endl;
   for (int i = 0; i < 4; i++)
   {
      for (int j = 0; j < 4; j++)
      {
		  pcout << std::setw (10) << std::setprecision (5) << A_hand[k].mat[i][j];
      }
      pcout << std::endl;
   }
   pcout << std::endl;
   }
   kinechain->send_stream(pcout);

}

void panel_control_hand::slotgo_inverse()
{

 Rhmatrix *A_prov = new Rhmatrix[grasp_number];
 int num_iter;
 fi0=FALSE;
 fi1=FALSE;
 fi2=FALSE;
 fi3=FALSE;

 //NO S'HA SELECCIONAT CAP PUNT DE GRASPING
 if(select_f0->currentItem()==0 && select_f1->currentItem()==0 && select_f2->currentItem()==0 && select_f3->currentItem()==0)
 {
   std::ostringstream pcout;
   pcout << "----------------------------------------------------" << std::endl;
   pcout << "No grasping points have been selected " << std::endl;
   pcout << "----------------------------------------------------" << std::endl;
   kinechain->send_stream(pcout);
 }

 //TOTS ELS DITS TENEN ASSIGNAT UN PUNT DE GRASPING
 if(select_f0->currentItem()!=0 && select_f1->currentItem()!=0 && select_f2->currentItem()!=0 && select_f3->currentItem()!=0)
 {
  A_prov[0] = A_hand[select_f0->currentItem()-1];
  A_prov[1] = A_hand[select_f1->currentItem()-1];
  A_prov[2] = A_hand[select_f2->currentItem()-1];
  A_prov[3] = A_hand[select_f3->currentItem()-1];

  init_angles();
  num_iter = kinechain->kineinverse_hand(A_prov,sol);
 }

 //NOMÉS EL PRIMER DIT TÉ ASSIGNAT UN PUNT DE GRASPING
if(select_f0->currentItem()!=0 && select_f1->currentItem()==0 && select_f2->currentItem()==0 && select_f3->currentItem()==0)
 {
  A_prov[0] = A_hand[select_f0->currentItem()-1];

  init_angles1();
  num_iter = kinechain->kineinverse(A_prov[0],sol1,0);
 }

 //NOMÉS EL SEGON DIT TÉ ASSIGNAT UN PUNT DE GRASPING
 if(select_f0->currentItem()==0 && select_f1->currentItem()!=0 && select_f2->currentItem()==0 && select_f3->currentItem()==0)
 {
  A_prov[0] = A_hand[select_f1->currentItem()-1];

  init_angles1();
  num_iter = kinechain->kineinverse(A_prov[0],sol1,1);
 }

 //NOMÉS EL TERCER DIT TÉ ASSIGNAT UN PUNT DE GRASPING
 if(select_f0->currentItem()==0 && select_f1->currentItem()==0 && select_f2->currentItem()!=0 && select_f3->currentItem()==0)
 {
  A_prov[0] = A_hand[select_f2->currentItem()-1];

  init_angles1();
  num_iter = kinechain->kineinverse(A_prov[0],sol1,2);
 }

 //NOMÉS EL QUART DIT TÉ ASSIGNAT UN PUNT DE GRASPING
 if(select_f0->currentItem()==0 && select_f1->currentItem()==0 && select_f2->currentItem()==0 && select_f3->currentItem()!=0)
 {
  A_prov[0] = A_hand[select_f3->currentItem()-1];

  init_angles1();
  num_iter = kinechain->kineinverse(A_prov[0],sol1,3);
 }

 //EL PRIMER I EL SEGON DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()!=0 && select_f1->currentItem()!=0 && select_f2->currentItem()==0 && select_f3->currentItem()==0)
 {
  A_prov[0] = A_hand[select_f0->currentItem()-1];
  A_prov[1] = A_hand[select_f1->currentItem()-1];
  fi0 = TRUE;
  fi1 = TRUE;
  fi2 = FALSE;
  fi3 = FALSE;
  init_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,2);
 }

 //EL PRIMER I EL TERCER DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()!=0 && select_f1->currentItem()==0 && select_f2->currentItem()!=0 && select_f3->currentItem()==0)
 {
  A_prov[0] = A_hand[select_f0->currentItem()-1];
  A_prov[2] = A_hand[select_f2->currentItem()-1];
  fi0 = TRUE;
  fi1 = FALSE;
  fi2 = TRUE;
  fi3 = FALSE;
  init_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,2);
 }

 //EL PRIMER I EL QUART DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()!=0 && select_f1->currentItem()==0 && select_f2->currentItem()==0 && select_f3->currentItem()!=0)
 {
  A_prov[0] = A_hand[select_f0->currentItem()-1];
  A_prov[3] = A_hand[select_f3->currentItem()-1];
  fi0 = TRUE;
  fi1 = FALSE;
  fi2 = FALSE;
  fi3 = TRUE;
  init_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,2);
 }

 //EL SEGON I EL TERCER DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()==0 && select_f1->currentItem()!=0 && select_f2->currentItem()!=0 && select_f3->currentItem()==0)
 {
  A_prov[1] = A_hand[select_f1->currentItem()-1];
  A_prov[2] = A_hand[select_f2->currentItem()-1];
  fi0 = FALSE;
  fi1 = TRUE;
  fi2 = TRUE;
  fi3 = FALSE;
  init_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,2);
 }

 //EL SEGON I EL QUART DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()==0 && select_f1->currentItem()!=0 && select_f2->currentItem()==0 && select_f3->currentItem()!=0)
 {
  A_prov[1] = A_hand[select_f1->currentItem()-1];
  A_prov[3] = A_hand[select_f3->currentItem()-1];
  fi0 = FALSE;
  fi1 = TRUE;
  fi2 = FALSE;
  fi3 = TRUE;
  init_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,2);
 }

 //EL TERCER I EL QUART DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()==0 && select_f1->currentItem()==0 && select_f2->currentItem()!=0 && select_f3->currentItem()!=0)
 {
  A_prov[2] = A_hand[select_f2->currentItem()-1];
  A_prov[3] = A_hand[select_f3->currentItem()-1];
  fi0 = FALSE;
  fi1 = FALSE;
  fi2 = TRUE;
  fi3 = TRUE;
  init_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,2);
 }

 //EL PRIMER, EL SEGON I EL TERCER DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()!=0 && select_f1->currentItem()!=0 && select_f2->currentItem()!=0 && select_f3->currentItem()==0)
 {
  A_prov[0] = A_hand[select_f0->currentItem()-1];
  A_prov[1] = A_hand[select_f1->currentItem()-1];
  A_prov[2] = A_hand[select_f2->currentItem()-1];
  fi0 = TRUE;
  fi1 = TRUE;
  fi2 = TRUE;
  fi3 = FALSE;
  init_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,3);
 }

 //EL PRIMER, EL SEGON I EL QUART DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()!=0 && select_f1->currentItem()!=0 && select_f2->currentItem()==0 && select_f3->currentItem()!=0)
 {
  A_prov[0] = A_hand[select_f0->currentItem()-1];
  A_prov[1] = A_hand[select_f1->currentItem()-1];
  A_prov[3] = A_hand[select_f3->currentItem()-1];
  fi0 = TRUE;
  fi1 = TRUE;
  fi2 = FALSE;
  fi3 = TRUE;
  init_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,3);
 }

 //EL PRIMER, EL TERCER I EL QUART DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()!=0 && select_f1->currentItem()==0 && select_f2->currentItem()!=0 && select_f3->currentItem()!=0)
 {
  A_prov[0] = A_hand[select_f0->currentItem()-1];
  A_prov[2] = A_hand[select_f2->currentItem()-1];
  A_prov[3] = A_hand[select_f3->currentItem()-1];
  fi0 = TRUE;
  fi1 = FALSE;
  fi2 = TRUE;
  fi3 = TRUE;
  init_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,3);
 }

 //EL SEGON, EL TERCER I EL QUART DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()==0 && select_f1->currentItem()!=0 && select_f2->currentItem()!=0 && select_f3->currentItem()!=0)
 {
  A_prov[1] = A_hand[select_f1->currentItem()-1];
  A_prov[2] = A_hand[select_f2->currentItem()-1];
  A_prov[3] = A_hand[select_f3->currentItem()-1];
  fi0 = FALSE;
  fi1 = TRUE;
  fi2 = TRUE;
  fi3 = TRUE;
  init_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,3);
 }

  delete[] A_prov;
}

void panel_control_hand::slotwhere_inverse()
{
  //emit stop_kinematics(TRUE);
  for (int k=0;k<kinechain->n_chain;k++)
  {
  kinechain->show_tcp(k);
  }
}

void panel_control_hand::slotretry_inverse()
{
  Rhmatrix *A_prov = new Rhmatrix[grasp_number];
 int num_iter;
 fi0=FALSE;
 fi1=FALSE;
 fi2=FALSE;
 fi3=FALSE;

 //NO S'HA SELECCIONAT CAP PUNT DE GRASPING
 if(select_f0->currentItem()==0 && select_f1->currentItem()==0 && select_f2->currentItem()==0 && select_f3->currentItem()==0)
 {
   std::ostringstream pcout;
   pcout << "----------------------------------------------------" << std::endl;
   pcout << "No grasping points have been selected " << std::endl;
   pcout << "----------------------------------------------------" << std::endl;
   kinechain->send_stream(pcout);
 }

 //TOTS ELS DITS TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()!=0 && select_f1->currentItem()!=0 && select_f2->currentItem()!=0 && select_f3->currentItem()!=0)
 {
  A_prov[0] = A_hand[select_f0->currentItem()-1];
  A_prov[1] = A_hand[select_f1->currentItem()-1];
  A_prov[2] = A_hand[select_f2->currentItem()-1];
  A_prov[3] = A_hand[select_f3->currentItem()-1];

  random_angles();
  num_iter = kinechain->kineinverse_hand_retry(A_prov,sol);
  //connect(this,SIGNAL(stop_kinematics(bool)),kinechain,SLOT(slot_stop_kinematics(bool)));
 }

 //NOMÉS EL PRIMER DIT TÉ ASSIGNAT UN PUNT DE GRASPING
 if(select_f0->currentItem()!=0 && select_f1->currentItem()==0 && select_f2->currentItem()==0 && select_f3->currentItem()==0)
 {
  A_prov[0] = A_hand[select_f0->currentItem()-1];

  random_angles1();
  num_iter = kinechain->kineinverse(A_prov[0],sol1,0);
 }

 //NOMÉS EL SEGON DIT TÉ ASSIGNAT UN PUNT DE GRASPING
 if(select_f0->currentItem()==0 && select_f1->currentItem()!=0 && select_f2->currentItem()==0 && select_f3->currentItem()==0)
 {
  A_prov[0] = A_hand[select_f1->currentItem()-1];

  random_angles1();
  num_iter = kinechain->kineinverse(A_prov[0],sol1,1);
 }

 //NOMÉS EL TERCER DIT TÉ ASSIGNAT UN PUNT DE GRASPING
 if(select_f0->currentItem()==0 && select_f1->currentItem()==0 && select_f2->currentItem()!=0 && select_f3->currentItem()==0)
 {
  A_prov[0] = A_hand[select_f2->currentItem()-1];

  random_angles1();
  num_iter = kinechain->kineinverse(A_prov[0],sol1,2);
 }

 //NOMÉS EL QUART DIT TÉ ASSIGNAT UN PUNT DE GRASPING
 if(select_f0->currentItem()==0 && select_f1->currentItem()==0 && select_f2->currentItem()==0 && select_f3->currentItem()!=0)
 {
  A_prov[0] = A_hand[select_f3->currentItem()-1];

  random_angles1();
  num_iter = kinechain->kineinverse(A_prov[0],sol1,3);
 }

 //EL PRIMER I EL SEGON DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()!=0 && select_f1->currentItem()!=0 && select_f2->currentItem()==0 && select_f3->currentItem()==0)
 {
  A_prov[0] = A_hand[select_f0->currentItem()-1];
  A_prov[1] = A_hand[select_f1->currentItem()-1];
  fi0 = TRUE;
  fi1 = TRUE;
  fi2 = FALSE;
  fi3 = FALSE;
  random_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,2);
 }

 //EL PRIMER I EL TERCER DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()!=0 && select_f1->currentItem()==0 && select_f2->currentItem()!=0 && select_f3->currentItem()==0)
 {
  A_prov[0] = A_hand[select_f0->currentItem()-1];
  A_prov[2] = A_hand[select_f2->currentItem()-1];
  fi0 = TRUE;
  fi1 = FALSE;
  fi2 = TRUE;
  fi3 = FALSE;
  random_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,2);
 }

 //EL PRIMER I EL QUART DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()!=0 && select_f1->currentItem()==0 && select_f2->currentItem()==0 && select_f3->currentItem()!=0)
 {
  A_prov[0] = A_hand[select_f0->currentItem()-1];
  A_prov[3] = A_hand[select_f3->currentItem()-1];
  fi0 = TRUE;
  fi1 = FALSE;
  fi2 = FALSE;
  fi3 = TRUE;
  random_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,2);
 }

 //EL SEGON I EL TERCER DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()==0 && select_f1->currentItem()!=0 && select_f2->currentItem()!=0 && select_f3->currentItem()==0)
 {
  A_prov[1] = A_hand[select_f1->currentItem()-1];
  A_prov[2] = A_hand[select_f2->currentItem()-1];
  fi0 = FALSE;
  fi1 = TRUE;
  fi2 = TRUE;
  fi3 = FALSE;
  random_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,2);
 }

 //EL SEGON I EL QUART DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()==0 && select_f1->currentItem()!=0 && select_f2->currentItem()==0 && select_f3->currentItem()!=0)
 {
  A_prov[1] = A_hand[select_f1->currentItem()-1];
  A_prov[3] = A_hand[select_f3->currentItem()-1];
  fi0 = FALSE;
  fi1 = TRUE;
  fi2 = FALSE;
  fi3 = TRUE;
  random_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,2);
 }

 //EL TERCER I EL QUART DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()==0 && select_f1->currentItem()==0 && select_f2->currentItem()!=0 && select_f3->currentItem()!=0)
 {
  A_prov[2] = A_hand[select_f2->currentItem()-1];
  A_prov[3] = A_hand[select_f3->currentItem()-1];
  fi0 = FALSE;
  fi1 = FALSE;
  fi2 = TRUE;
  fi3 = TRUE;
  random_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,2);
 }


 //EL PRIMER, EL SEGON I EL TERCER DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()!=0 && select_f1->currentItem()!=0 && select_f2->currentItem()!=0 && select_f3->currentItem()==0)
 {
  A_prov[0] = A_hand[select_f0->currentItem()-1];
  A_prov[1] = A_hand[select_f1->currentItem()-1];
  A_prov[2] = A_hand[select_f2->currentItem()-1];
  fi0 = TRUE;
  fi1 = TRUE;
  fi2 = TRUE;
  fi3 = FALSE;
  random_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,3);
 }

 //EL PRIMER, EL SEGON I EL QUART DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()!=0 && select_f1->currentItem()!=0 && select_f2->currentItem()==0 && select_f3->currentItem()!=0)
 {
  A_prov[0] = A_hand[select_f0->currentItem()-1];
  A_prov[1] = A_hand[select_f1->currentItem()-1];
  A_prov[3] = A_hand[select_f3->currentItem()-1];
  fi0 = TRUE;
  fi1 = TRUE;
  fi2 = FALSE;
  fi3 = TRUE;
  random_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,3);
 }

 //EL PRIMER, EL TERCER I EL QUART DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()!=0 && select_f1->currentItem()==0 && select_f2->currentItem()!=0 && select_f3->currentItem()!=0)
 {
  A_prov[0] = A_hand[select_f0->currentItem()-1];
  A_prov[2] = A_hand[select_f2->currentItem()-1];
  A_prov[3] = A_hand[select_f3->currentItem()-1];
  fi0 = TRUE;
  fi1 = FALSE;
  fi2 = TRUE;
  fi3 = TRUE;
  random_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,3);
 }

 //EL SEGON, EL TERCER I EL QUART DIT TENEN ASSIGNATS PUNTS DE GRASPING
 if(select_f0->currentItem()==0 && select_f1->currentItem()!=0 && select_f2->currentItem()!=0 && select_f3->currentItem()!=0)
 {
  A_prov[1] = A_hand[select_f1->currentItem()-1];
  A_prov[2] = A_hand[select_f2->currentItem()-1];
  A_prov[3] = A_hand[select_f3->currentItem()-1];
  fi0 = FALSE;
  fi1 = TRUE;
  fi2 = TRUE;
  fi3 = TRUE;
  random_angles();
  num_iter = kinechain->kineinverse_two_chains(A_prov,sol,fi0,fi1,fi2,fi3,3);
 }

  delete[] A_prov;
}

void panel_control_hand::init_angles()
{
  sol[0][0] = rad(0.0);
  sol[0][1] = rad(0.0);
  sol[0][2] = rad(-90.0);
  sol[0][3] = rad(90.0);
  sol[0][4] = rad(0.0);
  sol[0][5] = rad(0.0);
  sol[0][6] = rad(90.0);
  sol[0][7] = rad(90.0);
  sol[0][8] = rad(45.0);
  sol[0][9] = rad(45.0);
  sol[0][10] = rad(45.0);
  sol[0][11] = rad(45.0);
  sol[0][12] = rad(-90.0);
  sol[0][13] = rad(-90.0);

  sol[1][0] = rad(0.0);
  sol[1][1] = rad(0.0);
  sol[1][2] = rad(-90.0);
  sol[1][3] = rad(90.0);
  sol[1][4] = rad(0.0);
  sol[1][5] = rad(0.0);
  sol[1][6] = rad(90.0);
  sol[1][7] = rad(90.0);
  sol[1][8] = rad(45.0);
  sol[1][9] = rad(45.0);
  sol[1][10] = rad(45.0);
  sol[1][11] = rad(45.0);
  sol[1][12] = rad(-90.0);
  sol[1][13] = rad(-90.0);

  sol[2][0] = rad(0.0);
  sol[2][1] = rad(0.0);
  sol[2][2] = rad(-90.0);
  sol[2][3] = rad(90.0);
  sol[2][4] = rad(0.0);
  sol[2][5] = rad(0.0);
  sol[2][6] = rad(90.0);
  sol[2][7] = rad(90.0);
  sol[2][8] = rad(45.0);
  sol[2][9] = rad(45.0);
  sol[2][10] = rad(45.0);
  sol[2][11] = rad(45.0);
  sol[2][12] = rad(-90.0);
  sol[2][13] = rad(-90.0);

  sol[3][0] = rad(0.0);
  sol[3][1] = rad(0.0);
  sol[3][2] = rad(-90.0);
  sol[3][3] = rad(90.0);
  sol[3][4] = rad(0.0);
  sol[3][5] = rad(0.0);
  sol[3][6] = rad(56.3);
  sol[3][7] = rad(-56.3);
  sol[3][8] = rad(45.0);
  sol[3][9] = rad(45.0);
  sol[3][10] = rad(45.0);
  sol[3][11] = rad(45.0);
  sol[3][12] = rad(-90.0);
  sol[3][13] = rad(-90.0);
}

void panel_control_hand::init_angles1()
{
  sol1[0] = rad(0.0);
  sol1[1] = rad(0.0);
  sol1[2] = rad(-90.0);
  sol1[3] = rad(90.0);
  sol1[4] = rad(0.0);
  sol1[5] = rad(0.0);
  sol1[6] = rad(90.0);
  sol1[7] = rad(90.0);
  sol1[8] = rad(45.0);
  sol1[9] = rad(45.0);
  sol1[10] = rad(45.0);
  sol1[11] = rad(45.0);
  sol1[12] = rad(-90.0);
  sol1[13] = rad(-90.0);
 }

void panel_control_hand::random_angles()
{

  sol[0][0] = rad(0.0);
  sol[0][1] = -160.0 + 0.5*320.0;
  sol[0][2] = -227.5 + 0.4*275.0;
  sol[0][3] = -52.5 + 0.6*285.0;
  sol[0][4] = -270.0 + 0.2*540.0;
  sol[0][5] = -105.0 + 0.1*225.0;
  sol[0][6] = kinechain->status_join[0][6];
  sol[0][7] = kinechain->status_join[0][7];
  sol[0][8] = kinechain->status_join[0][8];
  sol[0][9] = kinechain->status_join[0][9];
  sol[0][10] = kinechain->status_join[0][10];
  sol[0][11] = kinechain->status_join[0][11];
  sol[0][12] = kinechain->status_join[0][12];
  sol[0][13] = kinechain->status_join[0][13];

}

void panel_control_hand::random_angles1()
{
  sol1[0] = rad(0.0);
  sol1[1] = -160.0 + 0.1*320.0;
  sol1[2] = -227.5 + 0.7*275.0;
  sol1[3] = -52.5 + 0.2*285.0;
  sol1[4] = -270.0 + 0.3*540.0;
  sol1[5] = -105.0 + 0.8*225.0;
  sol1[6] = kinechain->status_join[0][6];
  sol1[7] = kinechain->status_join[0][7];
  sol1[8] = kinechain->status_join[0][8];
  sol1[9] = kinechain->status_join[0][9];
  sol1[10] = kinechain->status_join[0][10];
  sol1[11] = kinechain->status_join[0][11];
  sol1[12] = kinechain->status_join[0][12];
  sol1[13] = kinechain->status_join[0][13];
 }

void panel_control_hand::SocketConectadoSlot()
{
	std::ostringstream pcout;
    // Banderas de hay Socket y error
    haySocket = 1;
    connect_error = 0;
	connect_active = TRUE;

	pcout << "Socket connectat" << std::endl;
    kinechain->send_stream (pcout);
    
    //enviarDatoAlServidorSlot();    
}


void panel_control_hand::SocketConexionCerradaSlot()
{
	std::ostringstream pcout;

	pcout << "Connexió finalitzada pel servidor" << std::endl;
    kinechain->send_stream(pcout);
    haySocket = 0;
}


void panel_control_hand::SocketErrorSlot( int e )
{
    std::ostringstream pcout;

    if ( e == 0)
		pcout << "Error en el servidor remot" << std::endl;
    else
        pcout << "Error número " << e << std::endl;
    
    // Bandera de error 
    connect_error = 1;
}

void panel_control_hand::CerrarConexionSlot()
{
    socket->close();
    
    // Si hay bandera de error
    if ( connect_error == 0 )
    {
    if ( socket->state() == QSocket::Closing ) 
    {
        // Tenemos un cierre retrasado.
        connect( socket, SIGNAL(delayedCloseFinished()), SLOT(socketCerradoSlot()) );
    } else 
    {
        // El socket esta cerrado.
        socketCerradoSlot();
    }
    // Banderas hay o no socket error
    haySocket = 0;
    connect_error = 1;
    }
}

void panel_control_hand::socketCerradoSlot()
{
	std::ostringstream pcout;

    pcout << "Connexió finalitzada" << std::endl;
    kinechain->send_stream (pcout);
}

void panel_control_hand::crearSocketSlot() {
	std::ostringstream pcout;
    // Creamdo el socket y conxión de varias de sus señales

    if (haySocket==0)
    {
    socket = new QSocket( this );
	timer_socket = new QTimer( this );
	timer_socket->start(400, TRUE);

    connect( socket, SIGNAL(connected()), SLOT(SocketConectadoSlot()) );
    connect( socket, SIGNAL(connectionClosed()), SLOT(SocketConexionCerradaSlot()) );
    connect( socket, SIGNAL(error(int)), SLOT(SocketErrorSlot(int)) );
	connect( timer_socket, SIGNAL(timeout()), this, SLOT(enviarDatoAlServidorSlot()) );

    //  IP remota y puerto que utilizara el Socket
    pcout << "Connectant al servidor" << std::endl;
	kinechain->send_stream (pcout);
    socket->connectToHost( "147.83.37.84", 4242 );
    }
    else
    {
    connect_active = TRUE;
    timer_socket->start(400, TRUE);
    pcout << "Connexió reiniciada" << std::endl;
    kinechain->send_stream (pcout);
    }

}

void panel_control_hand::enviarDatoAlServidorSlot()    
{
    // Detener el timer del Socket
    //timerSocket->stop();
    
    // Velocidad y aceleración
    QTextOStream( &str ) << 1000.0 << " # " ;
    s.append( str );
    str = ' \0 ';
    QTextOStream( &str ) << 1000.0 << " # " ;
    s.append( str );
    str = ' \0 ';
    
    
    // Convertir el float a string para enviarlo por el socket
    QTextOStream( &str ) << degree(kinechain->list_plug[14].pval) - 90.0 << " # " ;
    s.append( str );
    str = ' \0 ';
    QTextOStream( &str ) << degree(kinechain->list_plug[15].pval) + 7.5 << " # " ;
    s.append( str );
    str = ' \0 ';
    QTextOStream( &str ) << degree(kinechain->list_plug[16].pval) << " # " ;
    s.append( str );
    str = ' \0 ';
    QTextOStream( &str ) << degree(kinechain->list_plug[17].pval) + 16.65 << " # " ;
    s.append( str );
    str = ' \0 ';
    
    QTextOStream( &str ) << degree(kinechain->list_plug[10].pval) - 90.0 << " # " ;
    s.append( str );
    str = ' \0 ';
    QTextOStream( &str ) << degree(kinechain->list_plug[11].pval) + 7.5 << " # " ;
    s.append( str );
    str = ' \0 ';
    QTextOStream( &str ) << degree(kinechain->list_plug[12].pval) << " # " ;
    s.append( str );
    str = ' \0 ';
    QTextOStream( &str ) << degree(kinechain->list_plug[13].pval) + 16.65 << " # " ;
    s.append( str );
    str = ' \0 ';

    QTextOStream( &str ) << degree(kinechain->list_plug[6].pval) - 90.0 << " # " ;
    s.append( str );
    str = ' \0 ';
    QTextOStream( &str ) << degree(kinechain->list_plug[7].pval) + 7.5 << " # " ;
    s.append( str );
    str = ' \0 ';
    QTextOStream( &str ) << degree(kinechain->list_plug[8].pval) << " # " ;
    s.append( str );
    str = ' \0 ';
    QTextOStream( &str ) << degree(kinechain->list_plug[9].pval) + 16.65 << " # " ;
    s.append( str );
    str = ' \0 ';
    
    QTextOStream( &str ) << degree(kinechain->list_plug[18].pval) - 45.44<< " # " ;
    s.append( str );
    str = ' \0 ';
    QTextOStream( &str ) << degree(kinechain->list_plug[19].pval) + 7.5 << " # " ;
    s.append( str );
    str = ' \0 ';
    QTextOStream( &str ) << degree(kinechain->list_plug[20].pval) << " # " ;
    s.append( str );
    str = ' \0 ';
    QTextOStream( &str ) << degree(kinechain->list_plug[21].pval) + 12.73 << " # " ;
    s.append( str );
    str = ' \0 ';

    // Enviar el caracter que indica el final de los datos
    QTextOStream( &str ) << " @ " ;
    s.append( str );
    str = ' \0 ';
    
    // Escribimos al servidor
    QTextStream os(socket);
    os << s << "\n";
    s = ' \0 ';
    
    // Verificamos si existe el socket y las restricciones e iniciamos o no el timer
    if ( haySocket == 1 && connect_active == TRUE )
    timer_socket->start(400, TRUE);
}

void panel_control_hand::conexionStoppedSlot()
{
  std::ostringstream pcout;

  connect_active = FALSE;
  pcout << "Connexió aturada" << std::endl;
  kinechain->send_stream (pcout);
}
