/***************************************************************************
                          panelControlRx.h  -  description
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

#ifndef PANEL_CONTROL_RX_H
#define PANEL_CONTROL_RX_H

#include <qvariant.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlcdnumber.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qtextbrowser.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qlineedit.h>

#include "qpanel_joint.h"
#include "simulator/rchain_rx.h"

#include "consolewidget.h"
#include "interpret/qvalparser.h"
#include "netthread.h"


class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QGroupBox;
class QLCDNumber;
class QLabel;
class QSlider;
class QTextEdit;
class QVBox;
class Qlayout;
class Qpanel_joint;
class consoleWidget;
class QLineEdit;
class Rchain;
class QvalParser;
class netthread;
      
class panelControlRx : public QWidget
{ 
    Q_OBJECT

public:
//   panelControlRx( QWidget* parent = 0, const char* name = 0, int dof = 1);
   panelControlRx( QWidget* parent = 0, const char* name = 0, Rchain_rx *pkinechain = 0 );
   ~panelControlRx();

   Qpanel_joint **ldial;

   QLabel* TextLabelX;
   QLabel* TextLabelY;
   QLabel* TextLabelZ;
   QLabel* TextLabelRX;
   QLabel* TextLabelRY;
   QLabel* TextLabelRZ;


   QPushButton* Xinc; 
   QPushButton* Yinc; 
   QPushButton* Zinc; 
   QPushButton* RXinc; 
   QPushButton* RYinc; 
   QPushButton* RZinc; 

   QPushButton* Xdec;
   QPushButton* Ydec;
   QPushButton* Zdec;
   QPushButton* RXdec;
   QPushButton* RYdec;
   QPushButton* RZdec;

   
   QCheckBox* check_world;
   QCheckBox* check_tool;
	QCheckBox* check_net;
		
   QPushButton* pushButton;
   consoleWidget* TextBrowser;
   
   Rchain_rx* kinechain;
   void update_limits();

   bool close(bool autoDelete);
	static void* server (void* data);
	
   
	netthread *netserver;
   QvalParser *parser;
   
protected:
   QVBoxLayout* mainlayout;

   QGroupBox* dials;
   QGroupBox* GroupBox;

   QVBoxLayout* layoutX;
   QVBoxLayout* layoutY;
   QVBoxLayout* layoutZ;
   QVBoxLayout* layoutRX;
   QVBoxLayout* layoutRY;
   QVBoxLayout* layoutRZ;
   QVBoxLayout* layoutBut;


   signals:
    /* ca */
   void newValueX (double );
/* emet un signal qual l'slider X canvia de valor */

   void newValueY (double );
/* emet un signal qual l'slider Y canvia de valor */

   void newValueZ (double );
/* emet un signal qual l'slider Z canvia de valor */

   void newValueRX (double );
/* emet un signal qual l'slider RX canvia de valor,
   correspon a una rotació en radians */

   void newValueRY (double );
/* emet un signal qual l'slider RY canvia de valor,
   correspon a una rotació en radians */

   void newValueRZ (double );
/* emet un signal qual l'slider RZ canvia de valor,
   correspon a una rotació en radians */
 
   private:
      QPixmap image0;
		int threadId;
   	//pthread_t pthreadServer;
  	
      
   public slots:
      void slotmove_home();
      void slot_updatetext(QString &line);
		void move_pickpoint(Rhmatrix point);

      
   private slots:
      void slot_buttonXinc();
      void slot_buttonYinc();
      void slot_buttonZinc();
      void slot_buttonRXinc();
      void slot_buttonRYinc();
      void slot_buttonRZinc();

      void slot_buttonXdec();
      void slot_buttonYdec();
      void slot_buttonZdec();
      void slot_buttonRXdec();
      void slot_buttonRYdec();
      void slot_buttonRZdec();

      void slot_setworld();
      void slot_settool();
		void slot_netserver();


};

#endif // PANEL_CONTROL_H
