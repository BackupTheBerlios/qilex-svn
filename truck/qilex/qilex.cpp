/***************************************************************************
 *   Copyright (C) 2003 by Leopold Palomo & Xavier Sierra                  *
 *   lepalom@wol.es & xavsierra@yahoo.es                                   *
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

 #ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <qaccel.h>
#include <qsplitter.h>
#include <qdial.h>
#include <qscrollbar.h>
#include <qslider.h>
#include <qlcdnumber.h>
#include <qhbox.h>

#include "qilex.h"

#include "images/filesave.xpm"
#include "images/fileopen.xpm"
#include "images/filenew.xpm"



QilexApp::QilexApp(QWidget* parent, const char* name)
{
   //setCaption(tr("Qilex-0.4.1 " ));
   setCaption(tr(QString::QString("Qilex-"  VERSION)));
   
   //the init of SoQt
   SoQt::init(parent);
   SoQtComposeRotation::initClass();
   SoCoordinateAxis::initClass();
           
  ///////////////////////////////////////////////////////////////////
  // call inits to invoke all other construction parts

   initActions();
   initMenuBar();
   initToolBar();
   initStatusBar();
   
   initView_Doc();

 
// SoQt .......::initClass();

   viewToolBar->setOn(true);
   viewStatusBar->setOn(true);

   p_scene_loaded = false;


}

QilexApp::~QilexApp()
{
}

/** initializes all QActions of the application */
void QilexApp::initActions()
{

   QPixmap openIcon, saveIcon, newIcon;
   newIcon = QPixmap(filenew);
   openIcon = QPixmap(fileopen);
   saveIcon = QPixmap(filesave);

   new_kinechain = FALSE;

   cellOpen = new QAction(tr("Open Cell"), openIcon, tr("&Open Cell"), 0, this);
   cellOpen->setStatusTip(tr("Opens a created cell"));
   cellOpen->setWhatsThis(tr("Open Cell\n\nOpens a created cell"));
   connect(cellOpen, SIGNAL(activated()), this, SLOT(slotcellOpen()));

   objectInsert = new QAction(tr("Insert Object"), tr("&Insert Object"), 0, this);
   objectInsert->setStatusTip(tr("Insert a created object"));
   objectInsert->setWhatsThis(tr("Insert a created object"));
   connect(objectInsert, SIGNAL(activated()), this, SLOT(slotobjectInsert()));

   Bnew_kinematic_chain = new QAction(tr("New kinematic chain"), tr("&New kinematic chain"), 0, this);
   connect(Bnew_kinematic_chain, SIGNAL(activated()), this, SLOT(slotnew_kinematic_chain()));

   Bnew_geometric_object = new QAction(tr("New geometric object"), tr("&New geometric object"), 0, this);
   connect(Bnew_geometric_object, SIGNAL(activated()), this, SLOT(slotnew_geometric_object()));

   Bnew_grasping_object = new QAction(tr("New grasping object"), tr("&New grasping object"), 0, this);
   connect(Bnew_grasping_object, SIGNAL(activated()), this, SLOT(slotnew_grasping_object()));

   cellSave = new QAction(tr("Save Cell"), saveIcon, tr("&Save Cell"), QAccel::stringToKey(tr("Ctrl+S")), this);
   cellSave->setStatusTip(tr("Saves the actual cell"));
   cellSave->setWhatsThis(tr("Save Cell.\n\nSaves the actual cell"));
   connect(cellSave, SIGNAL(activated()), this, SLOT(slotcellSave()));
   
   cellSaveAs = new QAction(tr("Save Cell As"), tr("Save Cell &as"), 0, this);
   cellSaveAs->setStatusTip(tr("Saves the actual cell under a new filename"));
   cellSaveAs->setWhatsThis(tr("Save As\n\nSaves the actual cell under a new filename"));
   connect(cellSaveAs, SIGNAL(activated()), this, SLOT(slotcellSave()));


   cellClose = new QAction(tr("Close cell"), tr("&Close cell"), QAccel::stringToKey(tr("Ctrl+W")), this);
   cellClose->setStatusTip(tr("Closes the actual cell"));
   cellClose->setWhatsThis(tr("Close Cell\n\nCloses the actual cell"));
   connect(cellClose, SIGNAL(activated()), this, SLOT(slotcellClose()));

   scenePrint = new QAction(tr("Print scene"), tr("&Print scene"), QAccel::stringToKey(tr("Ctrl+P")), this);
   scenePrint->setStatusTip(tr("Prints out the actual view of the scene"));
   scenePrint->setWhatsThis(tr("Print File\n\nPrints out the actual document"));
   connect(scenePrint, SIGNAL(activated()), this, SLOT(slotscenePrint()));

   cellQuit = new QAction(tr("Exit"), tr("E&xit"), QAccel::stringToKey(tr("Ctrl+Q")), this);
   cellQuit->setStatusTip(tr("Quits the application"));
   cellQuit->setWhatsThis(tr("Exit\n\nQuits the application"));
   connect(cellQuit, SIGNAL(activated()), this, SLOT(slotcellQuit()));

   viewToolBar = new QAction(tr("Toolbar"), tr("Tool&bar"), 0, this, 0, true);
   viewToolBar->setStatusTip(tr("Enables/disables the toolbar"));
   viewToolBar->setWhatsThis(tr("Toolbar\n\nEnables/disables the toolbar"));
   connect(viewToolBar, SIGNAL(toggled(bool)), this, SLOT(slotViewToolBar(bool)));
   
   viewStatusBar = new QAction(tr("Statusbar"), tr("&Statusbar"), 0, this, 0, true);
   viewStatusBar->setStatusTip(tr("Enables/disables the statusbar"));
   viewStatusBar->setWhatsThis(tr("Statusbar\n\nEnables/disables the statusbar"));
   connect(viewStatusBar, SIGNAL(toggled(bool)), this, SLOT(slotViewStatusBar(bool)));

   helpAboutApp = new QAction(tr("About"), tr("&About..."), 0, this);
   helpAboutApp->setStatusTip(tr("About the application"));
   helpAboutApp->setWhatsThis(tr("About\n\nAbout the application"));
   connect(helpAboutApp, SIGNAL(activated()), this, SLOT(slotHelpAbout()));

}

void QilexApp::initMenuBar()
{
  ///////////////////////////////////////////////////////////////////
  // MENUBAR

  ///////////////////////////////////////////////////////////////////
  // menuBar entry fileMenu

   cellNew = new QPopupMenu();

   /***********************************/
   cellCreateNewObject = new QPopupMenu();
   
   cellMenu=new QPopupMenu();
   
   cellOpen->addTo(cellMenu);
   //objectInsert->addTo(cellNew);
   Bnew_kinematic_chain->addTo(cellCreateNewObject);
   Bnew_geometric_object->addTo(cellCreateNewObject);
   Bnew_grasping_object->addTo(cellCreateNewObject);
   /***********************************/

   
   cellMenu->insertItem(tr("New Cell"), cellNew, 1, -1 ) ;
   cellNew->insertItem(tr("Create New Object"), cellCreateNewObject, -1, -1 ) ;

   cellQuit->addTo(cellMenu);
   
   //////////////////////////////////////////////////////////////////
  // menuBar entry viewMenu
   viewMenu=new QPopupMenu();
   viewMenu->setCheckable(true);
   viewToolBar->addTo(viewMenu);
   viewStatusBar->addTo(viewMenu);

  ///////////////////////////////////////////////////////////////////
    // menuBar entry helpMenu
   helpMenu=new QPopupMenu();
   helpAboutApp->addTo(helpMenu);

  ///////////////////////////////////////////////////////////////////
  // MENUBAR CONFIGURATION
   menuBar()->insertItem(tr("&File"), cellMenu);
   menuBar()->insertItem(tr("&View"), viewMenu);
   menuBar()->insertSeparator();
   menuBar()->insertItem(tr("&Help"), helpMenu);

}
void QilexApp::normalMenuBar()
{
   // p_scene_loaded indica si s'ha carregat una escena o no

   
   if(!p_scene_loaded)
   {
      cellOpen->removeFrom(cellMenu);
      cellQuit->removeFrom(cellMenu);
      cellMenu->removeItem(1) ;
      cellMenu->insertItem(tr("Edit Cell"), cellNew, 1, -1 ) ;
      cellClose->addTo(cellMenu);
      cellMenu->insertSeparator();
      cellSave->addTo(cellMenu);
      cellSaveAs->addTo(cellMenu);
      cellMenu->insertSeparator();
      //scenePrint->addTo(cellMenu);
      cellMenu->insertSeparator();
      cellQuit->addTo(cellMenu);
   
   }
   p_scene_loaded = true;
}  
void QilexApp::initMenuBar_again()
{
   cellMenu->clear() ;

   cellOpen->addTo(cellMenu);
   cellMenu->insertItem(tr("New Cell"), cellNew, 1, -1 ) ;
   cellQuit->addTo(cellMenu);

}



void QilexApp::initToolBar()
{
  ///////////////////////////////////////////////////////////////////
  // TOOLBAR
  fileToolbar = new QToolBar(this, "file operations");
//  fileNew->addTo(fileToolbar);
//  fileOpen->addTo(fileToolbar);
//  fileSave->addTo(fileToolbar);
  fileToolbar->addSeparator();
  QWhatsThis::whatsThisButton(fileToolbar);

}

void QilexApp::initStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  //STATUSBAR
  statusBar()->message(tr("Ready."), 2000);
}

void QilexApp::initView_Doc()
{
  ////////////////////////////////////////////////////////////////////
  // set the main widget here

   view = new QilexView(this, "The view");
   view->setMinimumSize( 420, 300 );
   setCentralWidget(view);

   doc = new QilexDoc(this,"The Cell", view);

}


bool QilexApp::queryExit()
{
  int exit=QMessageBox::information(this, tr("Quit..."),
                                    tr("Do your really want to quit?"),
                                    QMessageBox::Ok, QMessageBox::Cancel);

  if (exit==1)
  {

  }
  else
  {

  };

  return (exit==1);
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////


void QilexApp::slotcellOpen()
{
   statusBar()->message(tr("Opening a cell..."));

   //QFileDialog fd(QString::null, "(*.qlx)", 0, 0, TRUE );
  // QString fileName = QFileDialog::getOpenFileName(0,0,this);
   QString fileName = QFileDialog::getOpenFileName("","Cell file (*.qlx)",this,"Open file dialog", "Choose a file" );
   openAcell(fileName);
}
void QilexApp::openAcell(const QString &fileName)
{
   int error = 0;
   
   if (fileName.isEmpty())
   {
      statusBar()->message(tr("Opening aborted"), 2000);
      error = 1;
   }
   else
   {
      statusBar()->message(tr("Opening file..."));
      error = doc->doc_opencell(fileName);
   // cal posar els menus correctament
   // i inicialitzar el document
   }
   if (error == 0)
   {
     normalMenuBar();
     statusBar()->message(fileName + tr("has been opened"), 2000);
   }
   else
   {
      statusBar()->message(tr("Opening aborted"), 2000);
   }

   statusBar()->message(tr("Ready."));
}
void QilexApp::slotobjectInsert()
{
   statusBar()->message(tr("Opening a cell..."));

   QString fileName = QFileDialog::getOpenFileName("","Cell file (*.qlx)",this,"Open file dialog", "Choose a file" );
  // QFileDialog fd(QString::null, "(*.qlx)", 0, 0, TRUE );
  // QString fileName = QFileDialog::getOpenFileName(0,0,this);

   if (!fileName.isEmpty())
   {
   }
   //inserta objecte a l'escena
   statusBar()->message(tr("Ready."));
}

void QilexApp::slotnew_kinematic_chain()
{

   int error = 1;

   ct_new_kinematic_chain *data = new ct_new_kinematic_chain;

   new_kinematic_chain *dialog_nkc = new new_kinematic_chain(this, "The dialog", true, 0, data);

   statusBar()->message(tr("Opening file..."));

   while (error!=0 && dialog_nkc->exec() == QDialog::Accepted)
   {
      if(data->simulator_engine==0)
      {
      error = doc->doc_new_kinematic_chain(data);
      }
      if(data->simulator_engine==1)
      {
      //De moment Rchain i Rchain_hand fan el mateix
      //La funció que es cridarà és doc_new_kinematic_hand
      error = doc->doc_new_kinematic_hand(data);
      }
		if(data->simulator_engine==2)
      {
      //De moment Rchain i Rchain_hand fan el mateix
      //La funció que es cridarà és doc_new_kinematic_hand
      error = doc->doc_new_kinematic_chain_rx(data);
      }
		switch (error)
      {
         case 0:
            QMessageBox::information( 0,tr( "Information" ),
                  tr( "Your new kinematic chain %1 have been inserted in the cell" ).arg( data->QsName ) );            break;
         case 1:
            QMessageBox::critical( 0,tr( "Critical Error" ),
                  tr( "Cannot open file %1" ).arg( data->QsModelFile ) );break;
         case 2:
            QMessageBox::critical( 0,tr( "Critical Error" ),
                  tr( "Cannot open file %1" ).arg( data->QsDatFile ) );break;
         case 3:
            QMessageBox::critical( 0,tr( "Critical Error" ),
                  tr( "There's another object in the cell with the same name :%1" ).arg( data->QsName ) );break;
         case 4:
            QMessageBox::critical( 0,tr( "Critical Error" ),
                  tr( "The dat file %1 is not a valid dat file" ).arg( data->QsDatFile ) );break;
         case 5:
            QMessageBox::critical( 0,tr( "Critical Error" ),
                tr( "The inventor file %1 isn't a correct file for qilex" ).arg( data->QsModelFile ) );break;
         default: break;
      }  
   }
   if (error == 0)
   {
     normalMenuBar();
     statusBar()->message(tr("A new kinematic chain has been inserted"), 2000);
   }
   else
   {
      statusBar()->message(tr("Opening aborted"), 2000);
   }

   if(data->simulator_engine==1)
      {
      new_kinechain = TRUE;
     connect(this,SIGNAL(new_grasp_value(ct_new_grasping_object *)),doc->panel_hand,SLOT(slotupdate_fingers(ct_new_grasping_object *)));
     }
   delete data;
 //  delete dialog_nkc;
   
}
void QilexApp::slotcellSave()
{
  statusBar()->message(tr("Saving file..."));
//  doc->save();
  statusBar()->message(tr("Ready."));
}

void QilexApp::slotcellSaveAs()
{
  statusBar()->message(tr("Saving file under new filename..."));
  QString fn = QFileDialog::getSaveFileName(0, 0, this);
  if (!fn.isEmpty())
  {
//    doc->saveAs(fn);
  }
  else
  {
    statusBar()->message(tr("Saving aborted"), 2000);
  }

  statusBar()->message(tr("Ready."));
}

void QilexApp::slotcellClose()
{
  statusBar()->message(tr("Closing file..."));

  statusBar()->message(tr("Ready."));
}

void QilexApp::slotscenePrint()
{
  statusBar()->message(tr("Printing..."));
  QPrinter printer;
  if (printer.setup(this))
  {
    QPainter painter;
    painter.begin(&printer);

    ///////////////////////////////////////////////////////////////////
    // TODO: Define printing by using the QPainter methods here

    painter.end();
  }

  statusBar()->message(tr("Ready."));
}

void QilexApp::slotcellQuit()
{
  statusBar()->message(tr("Exiting application..."));
  ///////////////////////////////////////////////////////////////////
  // exits the Application
  qApp->quit();


  statusBar()->message(tr("Ready."));
}

void QilexApp::slotEditCut()
{
  statusBar()->message(tr("Cutting selection..."));

  statusBar()->message(tr("Ready."));
}

void QilexApp::slotEditCopy()
{
  statusBar()->message(tr("Copying selection to clipboard..."));

  statusBar()->message(tr("Ready."));
}

void QilexApp::slotEditPaste()
{
  statusBar()->message(tr("Inserting clipboard contents..."));

  statusBar()->message(tr("Ready."));
}


void QilexApp::slotViewToolBar(bool toggle)
{
  statusBar()->message(tr("Toggle toolbar..."));
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off

  if (toggle== false)
  {
    fileToolbar->hide();
  }
  else
  {
    fileToolbar->show();
  };

  statusBar()->message(tr("Ready."));
}

void QilexApp::slotViewStatusBar(bool toggle)
{
  statusBar()->message(tr("Toggle statusbar..."));
  ///////////////////////////////////////////////////////////////////
  //turn Statusbar on or off

  if (toggle == false)
  {
    statusBar()->hide();
  }
  else
  {
    statusBar()->show();
  }

  statusBar()->message(tr("Ready."));
}

void QilexApp::slotHelpAbout()
{
  QMessageBox::about(this,tr("About..."),
                      tr("Qilex\nVersion "  "\n(c) 2004 by Leopold Palomo Avellaneda & Xavier Sierra Contreras") );
}

void QilexApp::slotnew_geometric_object()
{

   int error = 1;

   ct_new_geometric_object *data = new ct_new_geometric_object;

   new_geometric_object *dialog_nkc = new new_geometric_object(this, "The dialog", true, 0, data);

   statusBar()->message(tr("Opening file..."));

   while (error!=0 && dialog_nkc->exec() == QDialog::Accepted)
   {
      error = doc->doc_new_geometric_object(data);

      switch (error)
      {
         case 0:
            QMessageBox::information( 0,tr( "Information" ),
                  tr( "Your new geometric object %1 have been inserted in the cell" ).arg( data->QsName ) );            break;
         case 1:
            QMessageBox::critical( 0,tr( "Critical Error" ),
                  tr( "Cannot open file %1" ).arg( data->QsModelFile ) );break;
         case 3:
            QMessageBox::critical( 0,tr( "Critical Error" ),
                  tr( "There's another object in the cell with the same name :%1" ).arg( data->QsName ) );break;
         default: break;
      }
   }
   if (error == 0)
   {
     normalMenuBar();
     statusBar()->message(tr("A new geometric object has been inserted"), 2000);
   }
   else
   {
      statusBar()->message(tr("Opening aborted"), 2000);
   }
}

void QilexApp::slotnew_grasping_object()
{
int error = 1;

   ct_new_grasping_object *data = new ct_new_grasping_object;

   new_grasping_object *dialog_nkc = new new_grasping_object(this, "The dialog", true, 0, data);

   statusBar()->message(tr("Opening file..."));

   while (error!=0 && dialog_nkc->exec() == QDialog::Accepted)
   {
      error = doc->doc_new_grasping_object(data);

      switch (error)
      {
         case 0:
            QMessageBox::information( 0,tr( "Information" ),
                  tr( "Your new grasping object %1 have been inserted in the cell" ).arg( data->QsName ) );            break;
         case 1:
            QMessageBox::critical( 0,tr( "Critical Error" ),
                  tr( "Cannot open file %1" ).arg( data->QsModelFile ) );break;
         case 3:
            QMessageBox::critical( 0,tr( "Critical Error" ),
                  tr( "There's another object in the cell with the same name :%1" ).arg( data->QsName ) );break;
         default: break;
      }
   }
   if (error == 0)
   {

     normalMenuBar();
     statusBar()->message(tr("A new grasping object has been inserted"), 2000);
     if(new_kinechain)
     {
     emit new_grasp_value(data);
     }
   }
   else
   {
      statusBar()->message(tr("Opening aborted"), 2000);
   }
}
