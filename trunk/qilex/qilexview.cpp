/***************************************************************************
 *   Copyright (C) 2003 by Leopold Palomo & Xavier Sierra                                   *
 *   lepalom@wol.es & xavsierra@yahoo.es                                                    *
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

 // include files for Qt
#include <qprinter.h>
#include <qpainter.h>

// application specific includes
#include "qilexview.h"
#include "qilexdoc.h"
#include "qilex.h"
#include "contrib/SoCoordinateAxis.h"

//SbVec3f vec_trans;
//static int num_objects = 0;

//This is the constructor of QilexView class that built the scene. First, only there are 
//the axes of the cell
QilexView::QilexView(QWidget *parent, const char *name) : QWidget(parent, name)
{

   p_Viewer = new SoQtExaminerViewer(this);

   if (p_Viewer == NULL)
     exit (1);
   
	SoSeparator *root = new SoSeparator;
	root->ref();
		
	scene = new SoSeparator;
	scene->ref();
	
	nocol = new SoSeparator;
	nocol->ref();
	
	SoCoordinateAxis *AxisW   = new SoCoordinateAxis();
   AxisW->fNDivision = 1;
   AxisW->fDivisionLength = 300;
	
   	
   SoMaterial *axesMat = new SoMaterial;
   axesMat->diffuseColor.setValue(1,0.0,0.0);

   SoSeparator *sepAxes = new SoSeparator;
	
	
   sepAxes->addChild(axesMat);
   sepAxes->addChild(AxisW);

	nocol->addChild(sepAxes);	
	
   selobjects = new SoSelection;
   SoShapeHints *hints = new SoShapeHints;
	
	coords = new SoCoordinate3;
	faces = new SoIndexedFaceSet;
	
	
   hints->vertexOrdering.setValue(SoShapeHints::COUNTERCLOCKWISE);
   hints->shapeType.setValue(SoShapeHints::UNKNOWN_SHAPE_TYPE);
   selobjects->addChild(hints);
	
   selobjects->addChild(coords = new SoCoordinate3);
   selobjects->addChild(faces = new SoIndexedFaceSet);

	SoNodeSensor * sensor = new SoNodeSensor(QilexView::nodechangeCB, scene);
   sensor->attach(scene); 
   		
	scene->addChild(selobjects);
   
	SoEventCallback * ecb = new SoEventCallback;
   ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(), event_cb, this);
   
   scene->addChild(ecb);
	root->addChild(scene);
	root->addChild(nocol);	
   
	
   p_Viewer->setSceneGraph(root);
   p_Viewer->setViewing(FALSE);

}

//Deletes the scene
QilexView::~QilexView()
{
   delete p_Viewer;
}

//Changes the objects of the scene for a new object
void QilexView::setScene(SoSeparator *Scene)
{
     p_Viewer->setSceneGraph(Scene);
}

//Rebuilds the scene adding a new object to it. These new objects belongs to the SoSelection
//list and, therefore, these can be selected
void QilexView::addObjectCell(SoSeparator* escena)
{
   
   selobjects->addChild(escena);
   
   selobjects->addSelectionCallback(QilexView::selectCB, this);
   selobjects->addDeselectionCallback(QilexView::deselectCB, this);  
   p_Viewer->viewAll();


}

void QilexView::addRobotCell(SoSeparator* robot)
{
   
   scene->addChild(robot);
   p_Viewer->viewAll();

}

void QilexView::addNoColObject(SoSeparator *ncol)
{
	nocol->addChild(ncol);
   p_Viewer->viewAll();                       	
}


//If a object is selected, a TransformBox appears around its
void 
QilexView::selectCB(void *, SoPath *p)
{
  SoPath * path = new SoPath(*p);
  path->ref();
  std::cout << path->getLength() << std::endl;
  path->truncate(path->getLength()-1);
  SoSeparator * group = (SoSeparator *) path->getTail();
  SoTransform * transform = (SoTransform *) group->getChild(0);
  if ( !transform->isOfType(SoTransform::getClassTypeId()) ) return;
  path->append(transform);
  
  SoTransformerManip * manip = new SoTransformerManip;
 // SoTransformBoxManip * manip = new SoTransformBoxManip;
  manip->replaceNode(path);
  path->unref();
}

//If a object is deselected, the TransformBox disappears. In addition, the new 
//object's configuration is sent to the transform matrix used for the Inverse Kinematics
void 
QilexView::deselectCB(void *closure, SoPath *p)
{
  QilexView *sv = (QilexView *) closure;
  SbVec3f joinax;
  SbVec3f scalax;
  SbVec3f rotax;
  float angle;
  SoSFRotation rotmanip;
  SoPath * path = new SoPath(*p);
  path->ref();
  path->truncate(path->getLength()-1);
  SoSeparator * group = (SoSeparator *) path->getTail();
  //SoTransformBoxManip * manip = (SoTransformBoxManip *) group->getChild(0);
  //if ( !manip->isOfType(SoTransformBoxManip::getClassTypeId()) ) return;
  
  SoTransformerManip * manip = (SoTransformerManip *) group->getChild(0);
  if ( !manip->isOfType(SoTransformerManip::getClassTypeId()) ) return;
   
  path->append(manip);
  joinax = manip->translation.getValue();
  scalax = manip->scaleFactor.getValue();
  rotmanip = manip->rotation.getValue();
  rotmanip.getValue(rotax,angle);
  //emit sv->new_transf(joinax);
  //emit sv->new_rot(rotax,angle);
  //emit sv->new_scale(scalax);
  emit sv->new_config(joinax,scalax,rotax,angle);
 /* cout << "Translació: " << joinax[0] << "  " << joinax[1] << "  " << joinax[2] << endl;
  cout << "Escalat: " << scalax[0] << "  " << scalax[1] << "  " << scalax[2] << endl;
  cout << "Rotació: " << rotax[0] << "  " << rotax[1] << "  " << rotax[2] << "  " << angle << endl;
  */
  manip->replaceManip(path, new SoTransform);
  path->unref();
  
}

SoIntersectionDetectionAction::Resp
QilexView::intersectionCB(void *closure, const SoIntersectingPrimitive * pr1, const SoIntersectingPrimitive * pr2)
{
  //QilexView *sv = (QilexView *) closure;
  SbVec3f a1, a2, b1, b2, c1, c2;
  a1 = pr1->xf_vertex[0];
  b1 = pr1->xf_vertex[1];
  c1 = pr1->xf_vertex[2];
  a2 = pr2->xf_vertex[0];
  b2 = pr2->xf_vertex[1];
  c2 = pr2->xf_vertex[2];
  // visualize the intersecting polygons
  const int cnum = coords->point.getNum();
  coords->point.setNum(cnum + 6);
  coords->point.set1Value(cnum + 0, a1);
  coords->point.set1Value(cnum + 1, b1);
  coords->point.set1Value(cnum + 2, c1);
  coords->point.set1Value(cnum + 3, a2);
  coords->point.set1Value(cnum + 4, b2);
  coords->point.set1Value(cnum + 5, c2);
  const int fnum = faces->coordIndex.getNum();
  faces->coordIndex.setNum(fnum + 8);
  faces->coordIndex.set1Value(fnum + 0, cnum + 0);
  faces->coordIndex.set1Value(fnum + 1, cnum + 1);
  faces->coordIndex.set1Value(fnum + 2, cnum + 2);
  faces->coordIndex.set1Value(fnum + 3, -1);
  faces->coordIndex.set1Value(fnum + 4, cnum + 3);
  faces->coordIndex.set1Value(fnum + 5, cnum + 4);
  faces->coordIndex.set1Value(fnum + 6, cnum + 5);
  faces->coordIndex.set1Value(fnum + 7, -1);
  
  //emit sv->new_colision();
 /* cout << "Els punts de la primera cara són:" << endl;
  cout << a1[0] << "  " << a1[1] << "  " << a1[2] << endl;
  cout << b1[0] << "  " << b1[1] << "  " << b1[2] << endl;
  cout << c1[0] << "  " << c1[1] << "  " << c1[2] << endl;
  
  cout << "Els punts de la segona cara són:" << endl;
  cout << a2[0] << "  " << a2[1] << "  " << a2[2] << endl;
  cout << b2[0] << "  " << b2[1] << "  " << b2[2] << endl;
  cout << c2[0] << "  " << c2[1] << "  " << c2[2] << endl;
   */
  return SoIntersectionDetectionAction::NEXT_PRIMITIVE;
}

SbBool
QilexView::filterCB(void *, const SoPath * p1, const SoPath * p2)
{
  // the overlayed polygons must be ignored...
  if ( p1->getTail() == faces || p2->getTail() == faces ) return FALSE;
  return TRUE;
}

void
QilexView::nodechangeCB(void * closure, SoSensor * s)
{
  SoNodeSensor * sensor = (SoNodeSensor *) s;
  sensor->detach();
  SoSelection * root = (SoSelection *) closure;
  /*int i;
  for ( i = 0; i < num_objects; i++ ) {
    SoSeparator * group = (SoSeparator *) root->getChild(i);
    SoMaterial * material = (SoMaterial *) group->getChild(1);
    material->diffuseColor.setValue(1.0f, 1.0f, 0.0f);
  }*/
  coords->point.setNum(0);
  faces->coordIndex.setNum(0);
  SoIntersectionDetectionAction ida;
  ida.setFilterCallback(QilexView::filterCB, NULL);
  ida.addIntersectionCallback(QilexView::intersectionCB, NULL);
  ida.setManipsEnabled(FALSE);
  ida.setDraggersEnabled(FALSE);
  ida.setIntersectionEpsilon(0.01);
  //ida.setTypeEnabled(SoMaterial::getClassTypeId(),FALSE);
  //ida.setTypeEnabled(SoCoordinate3::getClassTypeId(),FALSE);
  //ida.setTypeEnabled(SoIndexedFaceSet::getClassTypeId(),FALSE);
  ida.apply(root);
  sensor->attach(root);
}

void QilexView::event_cb(void * ud, SoEventCallback * n)
{
  const SoMouseButtonEvent * mbe = (SoMouseButtonEvent *)n->getEvent();

  if (mbe->getButton() == SoMouseButtonEvent::BUTTON2 &&
      mbe->getState() == SoButtonEvent::DOWN) {
    QilexView *pView = (QilexView *)ud;
	 SoQtExaminerViewer * viewer = pView->p_Viewer;

    SoRayPickAction rp(viewer->getViewportRegion());
    rp.setPoint(mbe->getPosition());
    rp.apply(viewer->getSceneManager()->getSceneGraph());

    SoPickedPoint * point = rp.getPickedPoint();
    if (point == NULL) {
      (void)fprintf(stderr, "\n** miss! **\n\n");
      return;
    }
    n->setHandled();

    (void)fprintf(stdout, "\n");

    SbVec3f v = point->getPoint();
    SbVec3f nv = point->getNormal();
    (void)fprintf(stderr, "point=<%f, %f, %f>, normvec=<%f, %f, %f>\n",
                  v[0], v[1], v[2], nv[0], nv[1], nv[2]);

    Rhmatrix pickpoint = pView->fillMatrix((double)-nv[0], (double)-nv[1], (double)-nv[2] ,(double)v[0], (double)v[1], (double)v[2], 0.0);

	 const SoDetail *detail = point->getDetail();
    if (detail->getTypeId() == SoFaceDetail::getClassTypeId())
    {     	      // safe downward cast, know the type
    		SoFaceDetail * facedetail = (SoFaceDetail *)detail;
         std::cerr << "Number of vertex in the face: " << facedetail->getNumPoints() << std::endl;
			int nVertex = facedetail->getNumPoints();
			
			SoCoordinate3 *coordNode;						
			int closestIndex;
			bool found = findClosestVertex(point, coordNode, closestIndex);
			if (found)
			{
				std::cerr << "Vertex Found " << std::endl;
				std::cerr << "Closed Index: " << closestIndex << std::endl;
				SbVec3f ptr = coordNode->point[closestIndex];
				SbMatrix mat = point->getObjectToWorld();
				mat.multVecMatrix(ptr, ptr);
							//	SbVec3f closestVertext = coordNode[closestIndex].point[0];
				(void)fprintf(stderr, "Vertex=<%f, %f, %f>\n", ptr[0], ptr[1], ptr[2]);
			
			}
				
			
	 }
      	         
    emit pView->pick_point(pickpoint);
  }
}


Rhmatrix QilexView::fillMatrix(double nx, double ny, double nz, double x, double y, double z )
{
	return (fillMatrix(nx, ny, nz, x, y, z , 0.0));
}

//function that fill a RhMatrix from a normal vector

Rhmatrix QilexView::fillMatrix (double nx, double ny, double nz, double px, double py, double pz, double alpha)
{
	Rhmatrix pos;
	double d, mod, x, y ,z, tx, ty, tz;
	
	d = - (nx * px + ny * py + nz * pz);
	
	
	pos.mat[0][3] = px;
	pos.mat[1][3] = py;
	pos.mat[2][3] = pz;
	
	if (fabs(nx) > 1e-8)
	{	
		y = 1.0; z = 1.0;
		x = - (d + nz + ny) / nx;
	}
	else if (fabs(ny) > 1e-8)
		{
			x = 1.0; z = 1.0;
			y = - (d + nz + nx) / ny;
		}
		else
		{
		 	x = 1.0; y = 1.0;
			z = - (d + nx + ny) / nz;
		}
	
	x = x -px; y = y - py; z = z - pz;	
	mod = sqrt (x*x + y*y + z*z); 
	x = x/mod; y = y/mod; z = z/mod;
	
	pos.mat[0][0] = x;
	pos.mat[1][0] = y;
	pos.mat[2][0] = z;	
		
	tx = ny * z - nz * y;
	ty = nz * x - nx * z;
	tz = nx * y - ny * x;
	mod = sqrt (tx*tx + ty*ty + tz*tz); 
	
	pos.mat[0][1] = tx/mod;
	pos.mat[1][1] = ty/mod;
	pos.mat[2][1] = tz/mod;

	mod = sqrt (nx*nx + ny*ny + nz*nz); 
	pos.mat[0][2] = nx/mod;
	pos.mat[1][2] = ny/mod;
	pos.mat[2][2] = nz/mod;	
	
	
	return pos;
}
  //   This function finds the closest vertex to an intersection
//   point on a shape made of faces, passed in the
//   "pickedPoint" argument. It returns the SoCoordinate3 node
//   containing the vertex's coordinates in the "coordNode"
//   argument and the index of the vertex in that node in the
//   "closestIndex" argument. If the shape is not made of faces
//   or there were any other problems, this returns FALSE.

SbBool QilexView::findClosestVertex(const SoPickedPoint *pickedPoint, SoCoordinate3 *&coordNode, int &closestIndex)
{
   const SoDetail *pickDetail = pickedPoint->getDetail();

   if (pickDetail != NULL && pickDetail->getTypeId() ==
                             SoFaceDetail::getClassTypeId()) {
      // Picked object is made of faces
      SoFaceDetail *faceDetail = (SoFaceDetail *) pickDetail;

      // Find the coordinate node that is used for the faces.
      // Assume that it's the last SoCoordinate3 node traversed
      // before the picked shape.
      SoSearchAction  mySearchAction;
      mySearchAction.setType(SoCoordinate3::getClassTypeId());
      mySearchAction.setInterest(SoSearchAction::LAST);
      mySearchAction.apply(pickedPoint->getPath());

      if (mySearchAction.getPath() != NULL) {  // We found one
         coordNode = (SoCoordinate3 *)
                     mySearchAction.getPath()->getTail();

         // Get the intersection point in the object space
         // of the picked shape
         SbVec3f objIntersect = pickedPoint->getObjectPoint();

         // See which of the points of the face is the closest
         // to the intersection point
         float minDistance = 1e12;
         closestIndex = -1;
         for (int i = 0; i < faceDetail->getNumPoints(); i++) {
            int pointIndex =
                  faceDetail->getPoint(i)->getCoordinateIndex();
            float curDistance = (coordNode->point[pointIndex] -
                  objIntersect).length();
            if (curDistance < minDistance) {
               closestIndex = pointIndex;
               minDistance = curDistance;
            }
         }

         if (closestIndex >= 0)
            return TRUE;
      }
   }

   return FALSE;
}
