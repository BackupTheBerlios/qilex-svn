/***************************************************************************
 *   Copyright (C) 2003 by Leopold Palomo & Xavier Sierra                                   *
 *  lepalom@wol.es &  xavsierra@yahoo.es                                                    *
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

 #ifndef QILEXVIEW_H
#define QILEXVIEW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

// include files for QT
#include <qwidget.h>
#include <qtimer.h>

// application specific includes
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>

 // Coin-Inventor includes
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoEnvironment.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/manips/SoTransformBoxManip.h>
#include <Inventor/manips/SoCenterballManip.h> 
#include <Inventor/manips/SoTrackballManip.h> 
#include <Inventor/manips/SoTransformerManip.h> 
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/details/SoDetail.h>
#include <Inventor/details/SoFaceDetail.h>
       
					 
#include <Inventor/sensors/SoNodeSensor.h>
#include <Inventor/collision/SoIntersectionDetectionAction.h>
#include <Inventor/SoInput.h>

#include <simulator/rhmatrix.h>


#include "qilexdoc.h"


class QilexDoc;
class SoNode;

static SoCoordinate3 * coords; 
static SoIndexedFaceSet * faces;
		

/** The QilexView class provides the view widget for the QilexApp instance. **/

class QilexView : public QWidget
{
   Q_OBJECT

   friend class QilexDoc;

   public:
    /** Constructor for the main view */
    QilexView(QWidget *parent = 0, const char *name=0);
    /** Destructor for the main view */
    ~QilexView();

    /** returns a pointer to the document connected to the view instance. Mind that this method requires a QilexApp instance as a parent
     * widget to get to the window document pointer by calling the QilexApp::getDocument() method.
     *
     * @see QilexApp#getDocument
     */
      QilexDoc *getDocument() const;

      SoQtExaminerViewer *p_Viewer;
      SoSelection *selobjects;
      		
		SoSeparator *scene;
      SoSeparator *nocol;
		
      //0 if it's a kinematic chain, 1 if it's a geometric object and 2 if it's a grasping object
      int ke;
      
      void setScene(SoSeparator*);
      void addObjectCell(SoSeparator*);
		void addRobotCell(SoSeparator*);      
		void addNoColObject(SoSeparator*); 
				                                            	
		static void event_cb(void * ud, SoEventCallback * n);
		
      
   signals:
    void new_colision();
    void new_transf(SbVec3f);
    void new_scale(SbVec3f);
    void new_rot(SbVec3f,float);
    void new_config(SbVec3f,SbVec3f,SbVec3f,float);
	 void pick_point(Rhmatrix );
	 
   protected:
      QilexDoc * cell;
      
    /** contains the implementation for printing functionality */
    void print(QPrinter *pPrinter);
	
   private:
  
    static void selectCB(void*, SoPath * p);
    static void deselectCB(void* closure, SoPath * p);
    static SoIntersectionDetectionAction::Resp intersectionCB(void *, const SoIntersectingPrimitive * pr1, 
    const SoIntersectingPrimitive * pr2);
    static SbBool filterCB(void *, const SoPath * p1, const SoPath * p2);
    static void nodechangeCB(void * closure, SoSensor * s);
	 Rhmatrix fillMatrix(double nx, double ny, double nz, double x, double y, double z );
	 Rhmatrix fillMatrix (double nx, double ny, double nz, double px, double py, double pz, double alpha);
	 static SbBool findClosestVertex(const SoPickedPoint *pickedPoint, SoCoordinate3 *&coordNode, int &closestIndex);
    

};

#endif // QILEXVIEW_H
