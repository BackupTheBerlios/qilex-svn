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

 #ifndef PANEL_CONTROL_HAND_H
#define PANEL_CONTROL_HAND_H

#include <stdlib.h>
#include <qwidget.h>
#include <qvariant.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlcdnumber.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qslider.h>
#include <qtextbrowser.h>
//#include <qtextedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qsocket.h>
#include <qtimer.h>
#include <qtextstream.h>

#include "qpanel_joint.h"
#include <simulator/rchain_hand.h>
#include <simulator/rhmatrix.h>

#include "consolewidget.h"
#include "containers.h"


//#include "interpret/qvalparser.h"


const double micro_interval = 1.0;
const double micro_angle = 0.01;


class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QGroupBox;
class QButtonGroup;
class QRadioButton;
class QLCDNumber;
class QLabel;
class QSlider;
//class QTextEdit;
class QVBox;
class QHBox;
class QComboBox;
class Qlayout;
class Qpanel_joint;
class consoleWidget;
class QLineEdit;
class Rchain_hand;
//class QvalParser;

class panel_control_hand : public QWidget
{
    Q_OBJECT

public:
//   panel_control( QWidget* parent = 0, const char* name = 0, int dof = 1);
   panel_control_hand( QWidget* parent = 0, const char* name = 0, Rchain_hand *pkinechain = 0 );
   ~panel_control_hand();

   Qpanel_joint **ldial;

   QLabel* TextLabelf0;
   QLabel* TextLabelf1;
   QLabel* TextLabelf2;
   QLabel* TextLabelf3;

   int grasp_number;
   double **sol;
   double *sol1;

   QCheckBox* check_world;
   QCheckBox* check_tool;
   QPushButton* pushButton;
   QPushButton* kinematic;
   QPushButton* kinematic_where;
   QPushButton* kinematic_retry;
   QPushButton* comunic0;
   QPushButton* comunic1;
   QPushButton* comunic2;
   QPushButton* comunic3;
   QPushButton* comunic_stop;
   QButtonGroup* choice;
   QRadioButton* f0;
   QRadioButton* f1;
   QRadioButton* f2;
   QRadioButton* f3;
   QComboBox* select_f0;
   QComboBox* select_f1;
   QComboBox* select_f2;
   QComboBox* select_f3;
   consoleWidget* TextBrowser;
   
   Rchain_hand* kinechain;
   Rhmatrix* A_hand;
   Rhmatrix* pre;
   Rhmatrix* post;
   void update_limits();
   void init_angles();
   void init_angles1();
   void random_angles();
   void random_angles1();

   bool fi0;
   bool fi1;
   bool fi2;
   bool fi3;

   int haySocket;
   int connect_error;
   bool connect_active;

   QTimer *timer_socket;
   QSocket *socket;
   QString s;
   QString str;

   //bool close(bool autoDelete);
   
   //QvalParser *parser;
   
protected:
   QVBoxLayout* mainlayout;

   QGroupBox* dials_arm;
   QGroupBox* dials_f0;
   QGroupBox* dials_f1;
   QGroupBox* dials_f2;
   QGroupBox* dials_f3;
   QButtonGroup* GroupBox;
   QWidget* megagrup;

   QHBoxLayout* inv_f0;
   QHBoxLayout* inv_f1;
   QHBoxLayout* inv_f2;
   QHBoxLayout* inv_f3;


   signals:
   void stop_kinematics(bool );
/*emet un senyal qun es decideix aturar el càlcul de
  la cinemàtica inversa*/
 
   private:
      QPixmap image0;

   public slots:
      void slotmove_home();
      void slotupdate_fingers(ct_new_grasping_object *data);
      void slotgo_inverse();
      void slotwhere_inverse();
      void slotretry_inverse();
	  void SocketConexionCerradaSlot();
	  void SocketErrorSlot( int e );
	  void CerrarConexionSlot();
	  void socketCerradoSlot();
	  void crearSocketSlot();
	  void enviarDatoAlServidorSlot();
	  void conexionStoppedSlot();
      
   private slots:
      void slot_finger0();
      void slot_finger1();
      void slot_finger2();
      void slot_finger3();
	  virtual void SocketConectadoSlot();
		

};

#endif // PANEL_CONTROL_HAND_H
