/***************************************************************************
 *   Copyright (C) 2003 by Leopold Palomo & Xavier Sierra                  *
 *   lepalom@wol.es & xavsierra@yahoo.es                                   *
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

 #ifndef QILEX_H
#define QILEX_H
 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// include files for Qt
#include <qapplication.h> 
#include <qmainwindow.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qstatusbar.h>
#include <qwhatsthis.h>
#include <qstring.h>
#include <qpixmap.h>
//#include <qmsgbox.h>
#include <qmessagebox.h> 

#include <qfiledialog.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qslider.h>
#include <qgroupbox.h>
#include <qvbox.h>
#include <qsplitter.h>


// application specific includes
#include "qilexview.h"
#include "qilexdoc.h"

//widgets
#include <widgets/new_kinematic_chain.h>
#include <widgets/new_geometric_object.h>
#include <widgets/new_grasping_object.h>
#include <interpret/qvalparser.h>
#include <widgets/panel_control.h>
#include <widgets/panel_control_rx.h>
#include <widgets/panel_control_hand.h>

#include <widgets/containers.h>

#include <soqthybrid/soqtcomposerotation.h>
#include <contrib/SoCoordinateAxis.h>

// application inventor specific includes
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/engines/SoCompose.h>

#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoTransformation.h>
#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/SbLinear.h>
#include <Inventor/lists/SoEngineList.h>



// forward declaration of the Qilex classes
class QilexDoc;
class QilexView;
/*class new_kinematic_chain;*/

class QilexApp : public QMainWindow
{
  Q_OBJECT

  friend class QilexView;

  public:
    /** construtor of QilexApp, calls all init functions to create the application.
     */
    QilexApp(QWidget* parent=0, const char* name=0);
    ~QilexApp();
    /** opens a file specified by commandline option
     */
    QilexDoc *getDocument() const;

        /** initializes all QActions of the application */
    void initActions();
    /** initMenuBar creates the menu_bar and inserts the menuitems */
    void initMenuBar();
    /** normalMenuBar creates the normalmenu_bar for normal operation with the cell */
    void normalMenuBar();

    void initMenuBar_again();

    bool new_kinechain;

    /** this creates the toolbars. Change the toobar look and add new toolbars in this
     * function */
    void initToolBar();
    /** setup the statusbar */
    void initStatusBar();
    /** setup the document*/
    void initDoc();
    /** setup the mainview*/
    void initView_Doc();

    /** overloaded for Message box on last window exit */
    bool queryExit();

    void completeView(int dof);
    void openAcell(const QString &fileName);  

   public slots:
          /** generate a new document in the actual view */

    /** open a document */
    void slotcellOpen();
    /** save a document */
    void slotcellSave();
    /** save a document under a different filename*/
    void slotcellSaveAs();
    /** close the actual file */
    void slotcellClose();
    /** print the actual file */
    void slotscenePrint();
    /** exits the application */
    void slotcellQuit();
    /** put the marked text/object into the clipboard and remove
     * it from the document */
    void slotEditCut();
    /** put the marked text/object into the clipboard*/
    void slotEditCopy();
    /** paste the clipboard into the document*/
    void slotEditPaste();
    /** toggle the toolbar*/
    void slotViewToolBar(bool toggle);
    /** toggle the statusbar*/
    void slotViewStatusBar(bool toggle);

    /** shows an about dlg*/
    void slotHelpAbout();

    void slotobjectInsert();

    void slotnew_kinematic_chain();
    void slotnew_geometric_object();
    void slotnew_grasping_object();

  signals:
    void new_grasp_value (ct_new_grasping_object *);

  private:
    /** view is the main widget which represents your working area. The View
     * class should handle all events of the view widget.  It is kept empty so
     * you can create your view according to your application's needs by
     * changing the view class.
     */
     QilexView *view;
    /** doc represents your actual document and is created only once. It keeps
     * information such as filename and does the serialization of your files.
     */
      QilexDoc *doc;



    /** file_menu contains all items of the menubar entry "File" */
      QPopupMenu *cellMenu;
    /** view_menu contains all items of the menubar entry "View" */
      QPopupMenu *viewMenu;
    /** view_menu contains all items of the menubar entry "Help" */
      QPopupMenu *helpMenu;

      QPopupMenu *cellNew;

      QPopupMenu *cellCreateNewObject;


      QToolBar *fileToolbar;


      QAction *cellOpen;          
      QAction *objectInsert;
      
      QAction *Bnew_kinematic_chain;
      QAction *Bnew_geometric_object;
      QAction *Bnew_grasping_object;
                   
      QAction *cellSave;
      QAction *cellSaveAs;

      QAction *cellClose;
      QAction *scenePrint;
      QAction *cellQuit;

      QAction *viewToolBar;
      QAction *viewStatusBar;

      QAction *helpAboutApp;

      bool p_scene_loaded;
        
};
 
#endif // QILEX_H
