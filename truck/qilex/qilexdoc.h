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

 #ifndef QILEXDOC_H
#define QILEXDOC_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

// include files for QT
#include <qobject.h>
#include <qstring.h>
#include <qdom.h>
#include <qfile.h>
#include <qtextstream.h>

// application specific includes
#include "qilex.h"

// Coin-Inventor includes
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h> 
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoLabel.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/Qt/SoQt.h>
#include <Inventor/lists/SoEngineList.h>
#include <Inventor/nodes/SoFont.h>




#include <simulator/rchain.h> // simulator engines
#include <simulator/rchain_rx.h> // simulator engines
#include <simulator/rchain_hand.h> // simulator hand engines


#include "widgets/containers.h"
#include "widgets/panel_control_hand.h"
#include "widgets/panel_control_rx.h"


#include "functions.h"
#include <contrib/SoCoordinateAxis.h>

#include <sstream>


using namespace std;

// forward declaration of the Qilex classes
class QilexView;

/**	QilexDoc provides a document object for a document-view model.**/


class QilexDoc : public QObject
{
   Q_OBJECT
      public:
      /** Constructor for the fileclass of the application */
      QilexDoc(QWidget *parent, const char *name=0, QilexView *pview=0);
      /** Destructor for the fileclass of the application */
      ~QilexDoc();
      void newDoc();
      bool save();
      bool saveAs(const QString &filename);
      bool isModified() const;
      QString filename;
      int doc_new_kinematic_chain(ct_new_kinematic_chain *data);
      int doc_new_kinematic_chain_rx(ct_new_kinematic_chain *data);
		int doc_new_kinematic_hand(ct_new_kinematic_chain *data);
      int doc_new_geometric_object(ct_new_geometric_object *data);
      int doc_new_grasping_object(ct_new_grasping_object *data);
      int doc_opencell(const QString &filename);
      int read_grasp_points(ct_new_grasping_object *data);
      int read_grasp_xml (QDomElement kine_element, ct_new_grasping_object *grasp);
      panel_control_hand *panel_hand;

   public slots:
      /** calls repaint() on all views connected to the document object and is called by the view by which the document has been changed.
      * As this view normally repaints itself, it is excluded from the paintEvent.
      */
    signals:
      void documentChanged();

   protected:
      bool modified;
      SoSeparator *scene;
      //Rchain *l_rchain;
   private:
      QilexView *view;
      SoSeparator* readFile(const char *filename, int &tipus);
      int writeXML_kineelement(const char *buffer, size_t size, int tipus, ct_new_kinematic_chain *data, Rchain *kinechain);
      int writeXML_kineelement(const char *buffer, size_t size, int tipus, ct_new_kinematic_chain *data, Rchain_hand *kinechain);
      int writeXML_geomelement(const char *buffer, size_t size, int tipus, ct_new_geometric_object *data);

      bool writeXMLheader();
      int doc_insert_kinematic_chain(QDomElement qilexelement);
		int doc_insert_kinematic_chain_rx(Rchain_rx *kineengine, SoSeparator *kinechain);
      int doc_insert_geometric_object(QDomElement qilexelement);

      int doc_insert_kinematic_chain(Rchain *kineengine, SoSeparator *kinechain);
      int doc_insert_kinematic_hand(Rchain_hand *kineengine, SoSeparator *kinechain);
};
void *reallocCB(void * ptr, size_t newsize);
#endif // QILEXDOC_H
