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

 #include <assert.h>
#include <iostream>

// include files for Qt
#include <qdir.h>
#include <qwidget.h>


// application specific includes
#include "qilexdoc.h"
#include "qilex.h"
#include "qilexview.h"
#include "functions.h"
#include <contrib/SoCoordinateAxis.h>

#include <widgets/panel_control_rx.h>

              
QilexDoc::QilexDoc(QWidget *parent, const char *name, QilexView *pview) : QObject(parent, name)
{
   modified = false;
   scene = new SoSeparator;

   view = pview;
   filename = "uname.qlx";
   QFile file;
   if (file.exists(filename) )
   {
      if(!file.remove(filename))
      {
         cout << "There's some problem erasing uname.qlx" << endl;
         exit (1);
      }
   }
}

QilexDoc::~QilexDoc()
{
}
void QilexDoc::newDoc()
{

}

bool QilexDoc::save()
{
  return true;
}

bool QilexDoc::saveAs(const QString &filename)
{
  return true;
}

int QilexDoc::doc_opencell(const QString &pfilename)
{

//   filename = pfilename;
   int error = 0;
   
   QDomDocument xmlDoc;
   QDomNodeList list;
   
   QString data;

      // envies un nom d'un fitxer xml amb la configuració del robot i
      // inicialitza la classe
      // retorna un 0 si tot ha estat bé.
      // retorna un 1 si el fitxer no ha pogut ser validat
      // retorna un 2 si el fitxer estava buit
      // retorna un 3 si el fitxer tenia algun tipus d'error a les dades
      // retorna un 4 si el fitxer no s'ha pogut obrir
   QFile xmlFile(pfilename);

   if ( !xmlFile.open( IO_ReadOnly ) )
   {
        QMessageBox::critical( 0,tr( "Critical Error" ),tr( "Cannot open file %1" ).arg( pfilename ) );
        return 4;
   }
   if( !xmlDoc.setContent( &xmlFile ) )
   {
        QMessageBox::critical( 0,tr( "Critical Error" ),tr( "Parsing error for file %1" ).arg( pfilename ) );
        xmlFile.close();
        return 1;
   }
   xmlFile.close();

    // get the header information from the DOM

   QDomElement qilexelement = xmlDoc.documentElement();
        
   QDomNode node;

   list = xmlDoc.elementsByTagName ( "kineelement");
   
   for (unsigned int i=0; i < list.length(); i++)
   {
      node = list.item(i);
      if ( node.isElement())
      {
         qilexelement = node.toElement();
         error = doc_insert_kinematic_chain( qilexelement);
      }
   }

   list = xmlDoc.elementsByTagName ( "geomelement");

   for (unsigned int i=0; i < list.length(); i++)
   {
      node = list.item(i);
      if ( node.isElement())
      {
         qilexelement = node.toElement();
         error = doc_insert_geometric_object(qilexelement);
      }
   }
   if (error == 0)
      filename = pfilename;
      
   return error;
}


bool QilexDoc::isModified() const
{
  return modified;
}

int QilexDoc::doc_new_kinematic_chain_rx(ct_new_kinematic_chain *data)
{
   int error = 0;
   int tipus = 0;
   void * buffer ; //char *buffer;
   char *buftemp = (char*)malloc(1024);
   
   SoOutput out;

   size_t sizeModel = 0;
      
   SoSeparator *kinechain = new SoSeparator;
   SoSeparator *kinetest = new SoSeparator;

   Rchain_rx *kineengine = new Rchain_rx;

   SoTransform *pos_rot = new SoTransform;
   SbVec3f joinax;

   joinax.setValue(SbVec3f(data->x,data->y,data->z));
   pos_rot->translation.setValue(joinax);
   pos_rot->rotation.setValue(SbVec3f(data->axeX, data->axeY, data->axeZ), (float) rad((double) data->angle));

   kinechain = readFile(data->QsModelFile.latin1(), tipus);

   if (kinechain == NULL) // no object read
   { return 1; }
   else  // ok, there's no object with the same name
   {
      error = kineengine->init_dat(data->QsDatFile.latin1()); //
      
		kineengine->update_conf();/*
		Important part. We assume that we are talking about the rx90 file.
		We don't test de dat file
		*/
			
      if (error == 0)
      {
         kinechain->ref();
         kinetest = (SoSeparator*)SoNode::getByName(data->QsName.latin1());

         if (kinetest==NULL)
         {
            //we need to put it in a buffer to write the xml file
            // if is Ok
            SoOutput out;
            out.setBuffer(buftemp, 1024, reallocCB);

            SoWriteAction wa1(&out);
            wa1.apply(kinechain);

            out.getBuffer(buffer, sizeModel);

            kinechain->insertChild(pos_rot, 0);
         }
         error = doc_insert_kinematic_chain_rx(kineengine, kinechain);
      }
   }         

   if (error==0)
   {
      writeXML_kineelement((char *)buffer, sizeModel, tipus, data, (Rchain*)kineengine);
   }
   return error;
}


int QilexDoc::doc_new_kinematic_chain(ct_new_kinematic_chain *data)
{
   int error = 0;
   int tipus = 0;
   void * buffer ; //char *buffer;
   char *buftemp = (char*)malloc(1024);
   
   SoOutput out;

   size_t sizeModel = 0;
      
   SoSeparator *kinechain = new SoSeparator;
   SoSeparator *kinetest = new SoSeparator;

   Rchain *kineengine = new Rchain;

   SoTransform *pos_rot = new SoTransform;
   SbVec3f joinax;

   joinax.setValue(SbVec3f(data->x,data->y,data->z));
   pos_rot->translation.setValue(joinax);
   pos_rot->rotation.setValue(SbVec3f(data->axeX, data->axeY, data->axeZ), (float) rad((double) data->angle));

   kinechain = readFile(data->QsModelFile.latin1(), tipus);

   if (kinechain == NULL) // no object read
   { return 1; }
   else  // ok, there's no object with the same name
   {
      error = kineengine->init_dat(data->QsDatFile.latin1()); //

      if (error == 0)
      {
         kinechain->ref();
         kinetest = (SoSeparator*)SoNode::getByName(data->QsName.latin1());

         if (kinetest==NULL)
         {
            //we need to put it in a buffer to write the xml file
            // if is Ok
            SoOutput out;
            out.setBuffer(buftemp, 1024, reallocCB);

            SoWriteAction wa1(&out);
            wa1.apply(kinechain);

            out.getBuffer(buffer, sizeModel);

            kinechain->insertChild(pos_rot, 0);
         }
         error = doc_insert_kinematic_chain(kineengine, kinechain);
      }
   }         

   if (error==0)
   {
      writeXML_kineelement((char *)buffer, sizeModel, tipus, data, kineengine);
   }
   return error;
}

int QilexDoc::doc_new_kinematic_hand(ct_new_kinematic_chain *data)
{
   int error = 0;
   int tipus = 0;
   void * buffer ; //char *buffer;
   char *buftemp = (char*)malloc(1024);

   SoOutput out;

   size_t sizeModel = 0;

   SoSeparator *kinechain = new SoSeparator;
   SoSeparator *kinetest = new SoSeparator;

   Rchain_hand *kineengine = new Rchain_hand();

   SoTransform *pos_rot = new SoTransform;
   SbVec3f joinax;

   joinax.setValue(SbVec3f(data->x,data->y,data->z));
   pos_rot->translation.setValue(joinax);
   pos_rot->rotation.setValue(SbVec3f(data->axeX, data->axeY, data->axeZ), (float) rad((double) data->angle));

   kinechain = readFile(data->QsModelFile.latin1(), tipus);

   if (kinechain == NULL) // no object read
   { return 1; }
   else  // ok, there's no object with the same name
   {
      error = kineengine->init_dat(data->QsDatFile.latin1()); //

      if (error == 0)
      {
         kinechain->ref();
         kinetest = (SoSeparator*)SoNode::getByName(data->QsName.latin1());

         if (kinetest==NULL)
         {
            //we need to put it in a buffer to write the xml file
            // if is Ok
            SoOutput out;
            out.setBuffer(buftemp, 1024, reallocCB);

            SoWriteAction wa1(&out);
            wa1.apply(kinechain);

            out.getBuffer(buffer, sizeModel);

            kinechain->insertChild(pos_rot, 0);
         }
         error = doc_insert_kinematic_hand(kineengine, kinechain);
      }
   }

   if (error==0)
   {
      writeXML_kineelement((char *)buffer, sizeModel, tipus, data, kineengine);
   }
   return error;
}

int QilexDoc::doc_new_geometric_object(ct_new_geometric_object *data)
{
   int error = 0;
   int tipus = 0;

   void *buffer; //char *buffer;
   char *buftemp = (char*)malloc(1024);
   size_t sizeModel = 0;;

   SoSeparator *object = new SoSeparator;
   SoSeparator *objecttest = new SoSeparator;


   SoTransform *pos_rot = new SoTransform;
   SbVec3f joinax;


   joinax.setValue(SbVec3f(data->x,data->y,data->z));
   pos_rot->translation.setValue(joinax);
   pos_rot->recenter(joinax);
   pos_rot->rotation.setValue(SbVec3f(data->axeX, data->axeY, data->axeZ), (float) rad((double) data->angle));
   object = readFile(data->QsModelFile.latin1(), tipus);


   if (object == NULL) // no object read
   {
      error = 1 ;
   }
   else  // ok, there's no object with the same name
   {
      object->ref();
      objecttest = (SoSeparator*)SoNode::getByName(data->QsName.latin1());

      if (objecttest==NULL)
      {

         SoOutput out;
         out.setBuffer(buftemp, 1024, reallocCB);

         SoWriteAction wa1(&out);
         wa1.apply(object);

         out.getBuffer(buffer, sizeModel);

         object->setName(data->QsName.latin1());
         object->insertChild(pos_rot, 0);

         view->addObjectCell(object);
         error = 0;

         writeXML_geomelement((char *)buffer, sizeModel, tipus, data);
      }
      else
      {
         object->unref();
         error = 3;
      }
   }

   return error;
}

int QilexDoc::doc_new_grasping_object(ct_new_grasping_object *data)
{
   int error = 0;
   int tipus = 0;

   void *buffer; //char *buffer;
   char *buftemp = (char*)malloc(1024);
   size_t sizeModel = 0;;

   SoSeparator *object = new SoSeparator;
   SoSeparator *objecttest = new SoSeparator;


   SoTransform *pos_rot = new SoTransform;
   SbVec3f joinax;
   SbVec3f joingrasp0;
   SbVec3f joingrasp1;
   SbVec3f joingrasp2;
   SbVec3f joingrasp3;

   joinax.setValue(SbVec3f(data->x,data->y,data->z));
   pos_rot->translation.setValue(joinax);
   pos_rot->recenter(joinax);
   pos_rot->rotation.setValue(SbVec3f(data->axeX, data->axeY, data->axeZ), (float) rad((double) data->angle));
   object = readFile(data->QsModelFile.latin1(), tipus);


   if (object == NULL) // no object read
   {
      error = 1 ;
   }
   else  // ok, there's no object with the same name
   {
      error = read_grasp_points(data);

      SoMaterial *bronze = new SoMaterial;
      bronze->ambientColor.setValue(0.33,0.22,0.27);
      bronze->diffuseColor.setValue(0.78,0.57,0.11);
      bronze->specularColor.setValue(0.99,0.94,0.81);
      bronze->shininess=0.28;

      SoSphere *grasp_sphere = new SoSphere;
      grasp_sphere->radius=7.0;

      SoFont *font = new SoFont;
      font->size.setValue(28);
      font->name.setValue("Times-Roman");
      SoSeparator *grasp_sep0 = new SoSeparator;
      SoTransform *grasp_transf0 = new SoTransform;
      SoSeparator *text0 = new SoSeparator;
      SoText2 *label_text0 = new SoText2;
      SoSeparator *grasp_sep1 = new SoSeparator;
      SoTransform *grasp_transf1 = new SoTransform;
      SoSeparator *text1 = new SoSeparator;
      SoText2 *label_text1 = new SoText2;
      SoSeparator *grasp_sep2 = new SoSeparator;
      SoTransform *grasp_transf2 = new SoTransform;
      SoSeparator *text2 = new SoSeparator;
      SoText2 *label_text2 = new SoText2;
      SoSeparator *grasp_sep3 = new SoSeparator;
      SoTransform *grasp_transf3 = new SoTransform;
      SoSeparator *text3 = new SoSeparator;
      SoText2 *label_text3 = new SoText2;

      //for (int i=0;i<data->num_point;i++)
      //{
       joingrasp0.setValue(SbVec3f(data->grasp_points[0].px,data->grasp_points[0].py,data->grasp_points[0].pz));
       grasp_transf0->translation.setValue(joingrasp0);
       grasp_transf0->recenter(joingrasp0);
       label_text0->string=" 1";
       text0->addChild(font);
       text0->addChild(label_text0);
       grasp_sep0->addChild(bronze);
       grasp_sep0->addChild(grasp_transf0);
       grasp_sep0->addChild(grasp_sphere);
       grasp_sep0->addChild(text0);
       //grasp_sep0->addChild(line0);
       joingrasp1.setValue(SbVec3f(data->grasp_points[1].px,data->grasp_points[1].py,data->grasp_points[1].pz));
       grasp_transf1->translation.setValue(joingrasp1);
       grasp_transf1->recenter(joingrasp1);
       label_text1->string=" 2";
       text1->addChild(font);
       text1->addChild(label_text1);
       grasp_sep1->addChild(bronze);
       grasp_sep1->addChild(grasp_transf1);
       grasp_sep1->addChild(grasp_sphere);
       grasp_sep1->addChild(text1);
       joingrasp2.setValue(SbVec3f(data->grasp_points[2].px,data->grasp_points[2].py,data->grasp_points[2].pz));
       grasp_transf2->translation.setValue(joingrasp2);
       grasp_transf2->recenter(joingrasp2);
       label_text2->string=" 3";
       text2->addChild(font);
       text2->addChild(label_text2);
       grasp_sep2->addChild(bronze);
       grasp_sep2->addChild(grasp_transf2);
       grasp_sep2->addChild(grasp_sphere);
       grasp_sep2->addChild(text2);
       joingrasp3.setValue(SbVec3f(data->grasp_points[3].px,data->grasp_points[3].py,data->grasp_points[3].pz));
       grasp_transf3->translation.setValue(joingrasp3);
       grasp_transf3->recenter(joingrasp3);
       label_text3->string=" 4";
       text3->addChild(font);
       text3->addChild(label_text3);
       grasp_sep3->addChild(bronze);
       grasp_sep3->addChild(grasp_transf3);
       grasp_sep3->addChild(grasp_sphere);
       grasp_sep3->addChild(text3);
       //object->addChild(grasp_sep);
      //}
      if (error == 0)
      {
      object->ref();
      objecttest = (SoSeparator*)SoNode::getByName(data->QsName.latin1());

      if (objecttest==NULL)
      {

         SoOutput out;
         out.setBuffer(buftemp, 1024, reallocCB);

         SoWriteAction wa1(&out);
         wa1.apply(object);

         out.getBuffer(buffer, sizeModel);

         object->setName(data->QsName.latin1());
	 //grasp_object->addChild(model_object);
	 object->addChild(grasp_sep0);
	 object->addChild(grasp_sep1);
	 object->addChild(grasp_sep2);
	 object->addChild(grasp_sep3);
         object->insertChild(pos_rot, 0);

         view->addObjectCell(object);
         error = 0;

         //writeXML_geomelement((char *)buffer, sizeModel, tipus, data);
	 //S'ha de canviar!!!!!
      }
      else
      {
         object->unref();
         error = 3;
      }
      }
   }
   return error;
}

SoSeparator* QilexDoc::readFile(const char *filename, int &tipus)
{
   // Open the input file
   SoInput mySceneInput;
   if (!mySceneInput.openFile(filename)) {
      fprintf(stderr, "Cannot open file %s\n", filename);
      return NULL;
   }

   // Read the whole file into the database
   SoSeparator *myGraph = SoDB::readAll(&mySceneInput);
   tipus = (int)mySceneInput.isFileVRML2();

   if (myGraph == NULL) {
      fprintf(stderr, "Problem reading file\n");
      return NULL;
   }
   mySceneInput.closeFile();
   return myGraph;
}
int QilexDoc::writeXML_kineelement(const char *buffer, size_t size, int tipus, ct_new_kinematic_chain *data, Rchain *kinechain)
{
   bool error=false;
   double scale;
   size_t pos;
   
   QFile file( filename );
   error = file.open(IO_ReadWrite);
   pos = file.size();
   
   if (error && pos == 0)
   {
      file.close();
      error = writeXMLheader();
   }

   if(!error)
   {
      if ( file.open(IO_ReadWrite))
      {
         pos = file.size();
         pos = pos - 13;

         file.at(pos);  //remove </qilexfile>

         QTextStream stream( &file );
        
         stream <<"<kineelement name=\"" << data->QsName.ascii() << "\"" ;
         stream <<" pos_x=\"" << data->x <<"\"";
         stream <<" pos_y=\"" << data->y <<"\"";
         stream <<" pos_z=\"" << data->z <<"\"";
         stream <<" pos_rx=\"" << data->axeX <<"\"";
         stream <<" pos_ry=\"" << data->axeY <<"\"";
         stream <<" pos_rz=\"" << data->axeZ <<"\"";
         stream <<" pos_angle=\"" << data->angle <<"\"" << ">" << endl;
         stream <<"<model3d format=\"";

         if(tipus==0)
         {
            stream << "vrml1\" size=\"" << size << "\">";
            stream << buffer ;

         }
         else{
            stream << "vrml2\" size=\"" << size << "\">";
            stream << buffer;

         }
         stream <<"</model3d>" << endl;
         stream <<"<kinechain name=\"" << kinechain->name << "\"";
         stream << " dof=\"" << kinechain->dof << "\">" << "\n";
         stream <<"  <manufacturer>" << kinechain->manufacturer <<"</manufacturer>" <<"\n";

         scale = kinechain->scale();
         
         for(int i=1;i<=kinechain->dof;i++)
         {
            stream <<"<joint id=\"" << i << "\"" << " sigma=\"" << kinechain->dh_parameters[i].sigma << "\"";
            stream <<" theta=\"" << degree(kinechain->dh_parameters[i].theta) << "\"";
            stream <<" alpha=\"" << degree(atan2(kinechain->dh_parameters[i].salpha,kinechain->dh_parameters[i].calpha)) << "\"";
            stream <<" ai=\"" << kinechain->dh_parameters[i].ai / scale << "\"";
            stream <<" di=\"" << kinechain->dh_parameters[i].di / scale << "\"";

            if (kinechain->dh_parameters[i].sigma == 1)
            {
               stream <<" low_rank=\"" << degree(kinechain->dh_parameters[i].lower_value) << "\"";
               stream <<" up_rank=\""  << degree(kinechain->dh_parameters[i].upper_value) << "\"";
               stream << " home=\"" << degree(kinechain->home[i]) << "\"" ;
					stream << " max_speed=\"" << degree(kinechain->dh_parameters[i].m_speed) << "\"";
            	stream << " max_acc=\"" << degree(kinechain->dh_parameters[i].m_acc) << "\"";
            
            }
            else{             
              stream <<" low_rank=\"" << kinechain->dh_parameters[i].lower_value << "\"";
              stream <<" up_rank=\""  << kinechain->dh_parameters[i].upper_value << "\"";
              stream << " home=\"" << kinechain->home[i] << "\"" << ">" << endl;
				  stream << " max_speed=\"" << kinechain->dh_parameters[i].m_speed << "\"";
              stream << " max_acc=\"" << kinechain->dh_parameters[i].m_acc << "\"";
            
            }
            stream << ">" << endl;
            stream << "</joint>" << endl;
         }
         stream <<"</kinechain>" << endl;
         stream <<"</kineelement>" << "\n";
         stream <<"</qilexfile>" << "\n";
         file.close();
      }

   }
   else
   {
      return 1;
   }
   return 0; 
}

int QilexDoc::writeXML_kineelement(const char *buffer, size_t size, int tipus, ct_new_kinematic_chain *data, Rchain_hand *kinechain)
{
   //Això s'ha d'omplir pel cas de l'estructura Rchain_hand
   return 0;
}


int QilexDoc::writeXML_geomelement(const char *buffer, size_t size, int tipus, ct_new_geometric_object *data)
{
   bool error=false;
   QFile file( filename );
   if ( !file.open( IO_ReadOnly ) )
   {
      error = writeXMLheader();
   }
   if(!error)
   {
      QFile file( filename);
      if ( file.open(IO_ReadWrite))
      {
         size_t pos = file.size();
         cout << pos;
         pos = pos - 13;

         bool status = file.at(pos);
         status = file.isSequentialAccess ();

         QTextStream stream( &file );

         stream <<"<geomelement name=\"" << data->QsName.ascii() << "\"" ;
         stream <<" pos_x=\"" << data->x <<"\"";
         stream <<" pos_y=\"" << data->y <<"\"";
         stream <<" pos_z=\"" << data->z <<"\"";
         stream <<" pos_rx=\"" << data->axeX <<"\"";
         stream <<" pos_ry=\"" << data->axeY <<"\"";
         stream <<" pos_rz=\"" << data->axeZ <<"\"";
         stream <<" pos_angle=\"" << data->angle <<"\"" << ">" << endl;
         stream <<"<model3d format=\"";
         if(tipus==0)
         {
            stream << "vrml1\" size=\"" << size << "\">" << buffer;
         }
         else{
            stream << "vrml2\" size=\"" << size << "\">" << buffer;
         }  
         stream <<"</model3d>" << endl;
         stream <<"</geomelement>" << "\n";
         stream <<"</qilexfile>" << "\n";

         file.close();
      }
 
   }
   else
   {
   }
   return 1;
}


bool QilexDoc::writeXMLheader()
{
   bool error = false;
   QFile file( filename);
   if ( file.open( IO_WriteOnly ) )
   {
      QTextStream stream( &file );

      stream << "<?xml version=\"1.0\" encoding=\"iso8859-15\"?>" << "\n";
      stream <<"<!DOCTYPE qilexfile ["  << "\n";
      stream <<"<!ELEMENT qilexfile (kineelement+, geomelement*)>"  << "\n";
      stream <<"<!ELEMENT kineelement (model3d, kinechain)>"  << "\n";
      stream <<"<!ATTLIST kineelement name CDATA #REQUIRED"  << "\n";
      stream <<"                      kineengine CDATA #REQUIRED"  << "\n";
      stream <<"                      pos_x CDATA #REQUIRED"  << "\n";
      stream <<"                      pos_y CDATA #REQUIRED"  << "\n";
      stream <<"                      pos_z CDATA #REQUIRED"  << "\n";
      stream <<"                      pos_rx CDATA #REQUIRED"  << "\n";
      stream <<"                      pos_ry CDATA #REQUIRED"  << "\n";
      stream <<"                      pos_rz CDATA #REQUIRED"  << "\n";
      stream <<"                      pos_angle CDATA #REQUIRED>"  << "\n";
      stream <<"<!ELEMENT model3d (#PCDATA)>"  << "\n";
      stream <<"<!ATTLIST model3d format CDATA #REQUIRED"  << "\n";
      stream <<"                  size CDATA #REQUIRED >"  << "\n";
      stream <<"<!ELEMENT kinechain (manufacturer,joint+)>"  << "\n";
      stream <<"<!-- name of the robot ex: puma 560 -->"  << "\n";
      stream <<"<!-- number of degrees of freedom ex: 6 -->"  << "\n";
      stream <<"<!ATTLIST kinechain name CDATA #REQUIRED"  << "\n";
      stream <<"                      dof CDATA #REQUIRED >"  << "\n";
      stream <<"<!-- manufacturer of the robot, not required ex: unimation -->"  << "\n";
      stream <<"<!ELEMENT manufacturer (#PCDATA)>"  << "\n";
      stream <<"<!-- Denavit-Hartenberg parameters of the robot -->"  << "\n";
      stream <<"<!-- and a little more data of the robot -->"  << "\n";
      stream <<"<!ELEMENT joint EMPTY>"  << "\n";
      stream <<"<!-- id of the join. Must be a number: 1...dof -->"  << "\n";
      stream <<"<!-- sigma: 0 join prismatic, 1 join revolute -->"  << "\n";
      stream <<"<!-- theta of the join  (degree)-->"  << "\n";
      stream <<"<!-- alpha of the join  (degree)-->"  << "\n";
      stream <<"<!-- ai of the join  (better mm)-->"  << "\n";
      stream <<"<!-- di of the join  (better mm)-->"  << "\n";
      stream <<"<!-- low_rank = the lowest value of the join (degree or mm)  -->"  << "\n";
      stream <<"<!-- up_rank = the highest value of the join (degree or mm)  -->"  << "\n";
      stream <<"<!-- max_speed = the max speed of the join (degree/s or mm/s) (not required default=30) -->"  << "\n";
      stream <<"<!-- max_accc = the max acceleration speed of the join (degree/s² or mm/s²) (not required default=1) -->"  << "\n";
      stream <<"<!-- value in home position of the join  (not required default=0)-->"  << "\n";
      stream <<"<!ATTLIST joint"  << "\n";
      stream <<"                id ID #REQUIRED"  << "\n";
      stream <<"               sigma (0 | 1) #IMPLIED"  << "\n";
      stream <<"                theta CDATA #REQUIRED"  << "\n";
      stream <<"                alpha CDATA #REQUIRED"  << "\n";
      stream <<"                ai CDATA #REQUIRED"  << "\n";
      stream <<"                di CDATA #REQUIRED"  << "\n";
      stream <<"                low_rank CDATA #REQUIRED"  << "\n";
      stream <<"                up_rank CDATA #REQUIRED"  << "\n";
      stream <<"                max_speed CDATA \"30\""  << "\n";
      stream <<"                max_acc CDATA \"1\""  << "\n";
      stream <<"                home CDATA \"0\">"  << "\n";
      stream <<"<!ELEMENT geomelement (model3d)>"  << "\n";
      stream <<"<!ATTLIST geomelement name CDATA #REQUIRED"  << "\n";
      stream <<"                      pos_x CDATA #REQUIRED"  << "\n";
      stream <<"                      pos_y CDATA #REQUIRED"  << "\n";
      stream <<"                      pos_z CDATA #REQUIRED"  << "\n";
      stream <<"                      pos_rx CDATA #REQUIRED"  << "\n";
      stream <<"                      pos_ry CDATA #REQUIRED"  << "\n";
      stream <<"                      pos_rz CDATA #REQUIRED"  << "\n";
      stream <<"                      pos_angle CDATA #REQUIRED>"  << "\n";
      stream <<"]>"  << "\n";
      stream <<"<qilexfile>" << "\n";
      stream <<"</qilexfile>" << "\n";
      file.close();
   }
   else
   {
      error = true;
   }
   return error;
}

int QilexDoc::doc_insert_kinematic_chain(QDomElement kine_element)
{
   int error = 0;
   const char * buffer;

   QDomNodeList list;
   
   Rchain *kineengine = new Rchain;
   
   SoSeparator *kinechain = new SoSeparator;
   SoSeparator *kinetest = new SoSeparator;

   //Rchain *kineengine = new Rchain;
   SoTransform *pos_rot = new SoTransform;
   SbVec3f joinax;

   float joinangle;
   float pos_x, pos_y, pos_z, pos_rx, pos_ry, pos_rz;

   QString data, name;

   QDomNode node;
   QDomElement element;
   
   name = kine_element.attribute ("name", QString::null);
   data = kine_element.attribute ("kineengine", QString::null);
   // here put some stuff to select the kinechain engine

   data = kine_element.attribute ("pos_x", QString::null);
   pos_x = data.toFloat();

   data = kine_element.attribute ("pos_y", QString::null);
   pos_y = data.toFloat();
      
   data = kine_element.attribute ("pos_z", QString::null);
   pos_z = data.toFloat();

   data = kine_element.attribute ("pos_rx", QString::null);
   pos_rx = data.toFloat();

   data = kine_element.attribute ("pos_ry", QString::null);
   pos_ry = data.toFloat();

   data = kine_element.attribute ("pos_rz", QString::null);
   pos_rz = data.toFloat();

   data = kine_element.attribute ("pos_angle", QString::null);
   joinangle = data.toFloat();
      
   joinax.setValue(SbVec3f( pos_x, pos_y, pos_z));
   pos_rot->translation.setValue(joinax);
   pos_rot->rotation.setValue(SbVec3f(pos_rx, pos_ry, pos_rz), (float) rad((double)joinangle));

   list = kine_element.elementsByTagName ("kinechain");

   if (list.length() == 1)
   {
      node = list.item(0);
      element = node.toElement();
      error = kineengine->read_element_xml (element);
   }
   else
   { error =4;} // assigno un nombre diferrent de 0
      
   list = kine_element.elementsByTagName ("model3d");
   if (list.length() == 1)
   {
      node = list.item(0);
      element = node.toElement();
            
      data = element.attribute ("format", QString::null);
      // some stuff to take care about the format
            
      data = element.attribute ("size", QString::null);
      size_t size = (size_t)data.toULong(0,10);
      buffer = new char[size];
   
      data = element.text();
      buffer = data.ascii();

  /*    char *buffer2 = new char[size];
      for(unsigned i=0;i<size;i++)
         buffer2[i] = buffer[i];
    */                 
      SoInput input;
      input.setBuffer((void *)buffer, size);

      if (input.isValidBuffer())
      {
         kinechain = SoDB::readAll(&input);
         
         if (kinechain == NULL)
            error = 10;
      }
      else
         {error = 8;} //  assigno un nombre diferent de 0
   }
   else
   { error =9; }// assigno un nombre diferent de 0
      
   if (error == 0)
   {
      kinechain->ref();
      kinetest = (SoSeparator*)SoNode::getByName(name.latin1());

      if (kinetest==NULL)
      {
         //we need to put it in a buffer to write the xml file
         // if is Ok
         kinechain->insertChild(pos_rot, 0);
         kinechain->setName(name.latin1());
         
         error = doc_insert_kinematic_chain(kineengine, kinechain);
      }
   }
   else {error = 5;}
   
   
   return error;
}


int QilexDoc::doc_insert_geometric_object(QDomElement geom_element)
{
   int error = 0;
   const char * buffer;
    
   SbVec3f joinax;
   SoTransform *pos_rot = new SoTransform;

   float joinangle;
   float pos_x, pos_y, pos_z, pos_rx, pos_ry, pos_rz;

   QString data, name;

   QDomNode node;
   QDomElement element;
   QDomNodeList list;

   SoSeparator *geomelement = new SoSeparator;
   SoSeparator *geomtest = new SoSeparator;
   
   name = geom_element.attribute ("name", QString::null);

   data = geom_element.attribute ("pos_x", QString::null);
   pos_x = data.toFloat();

   data = geom_element.attribute ("pos_y", QString::null);
   pos_y = data.toFloat();

   data = geom_element.attribute ("pos_z", QString::null);
   pos_z = data.toFloat();

   data = geom_element.attribute ("pos_rx", QString::null);
   pos_rx = data.toFloat();

   data = geom_element.attribute ("pos_ry", QString::null);
   pos_ry = data.toFloat();

   data = geom_element.attribute ("pos_rz", QString::null);
   pos_rz = data.toFloat();

   data = geom_element.attribute ("pos_angle", QString::null);
   joinangle = data.toFloat();

   joinax.setValue(SbVec3f( pos_x, pos_y, pos_z));
   pos_rot->translation.setValue(joinax);
   pos_rot->rotation.setValue(SbVec3f(pos_rx, pos_ry, pos_rz), (float) rad((double)joinangle));
   
   list = geom_element.elementsByTagName ("model3d");
   if (list.length() == 1)
   {
      node = list.item(0);
      element = node.toElement();

      data = element.attribute ("format", QString::null);
      // some stuff to take care about the format

      data = element.attribute ("size", QString::null);
      size_t size = (size_t)data.toULong(0,10);
      buffer = new char[size];

      data = element.text();
      buffer = data.ascii();

      SoInput input;
      input.setBuffer((void*) buffer, size);

      if (input.isValidBuffer())
      {
         geomelement = SoDB::readAll(&input);
         if (geomelement == NULL)
            error = 10;
      }
      else
         {error = 8;} //   assigno un nombre diferent de 0
   }
   else
   { error =9; }// assigno un nombre diferent de 0

   if (error == 0)
   {
      geomelement->ref();
      geomtest = (SoSeparator*)SoNode::getByName(name.latin1());

      if (geomtest==NULL)
      {
         //we need to put it in a buffer to write the xml file
         // if is Ok
         geomelement->insertChild(pos_rot, 0);
         geomelement->setName(name.latin1());
         view->addObjectCell(geomelement);
      }
   }
   return error; 
}



int QilexDoc::doc_insert_kinematic_chain(Rchain *kineengine, SoSeparator *kinechain)
{
   int error = 0;
   int i;

   SbVec3f joinax;

   float joinangle;
   
   SbName joints[] = {"joint1", "joint2", "joint3", "joint4","joint5", "joint6",
                   "joint7", "joint8", "joint9", "joint10","joint11", "joint12",
                   "joint13", "joint14", "joint15", "joint16","joint17", "joint18",
                   "joint19", "joint20", "joint21", "joint22","joint23", "joint24",  };
   
   SoEngineList compR(kineengine->dof);
   SoNodeList Rots(kineengine->dof);

   SoSearchAction lookingforjoints;
   SoTransform *pjoint = new SoTransform;

      // Identifie the rotations and assing the job
   i = 0;

   while (i < kineengine->dof && error == 0)
   {
      lookingforjoints.setName(joints[i]);
      lookingforjoints.setType(SoTransform::getClassTypeId());
      lookingforjoints.setInterest(SoSearchAction::FIRST);
      lookingforjoints.apply(kinechain);

      //  assert(lookingforjoints.getPath() != NULL);
      SoNode * pnode = lookingforjoints.getPath()->getTail();;

      pjoint = (SoTransform *) pnode;

      if(NULL != pjoint)
      {
         Rots.append((SoTransform *) pjoint);
         compR.append(new SoQtComposeRotation);
         // cal comprobar si l'articulació es de rotació o translació: arreglar...
         ((SoTransform *) Rots[i])->rotation.getValue(joinax, joinangle);
         ((SoQtComposeRotation *) compR[i])->axis.setValue(joinax);

         ((SoTransform *) Rots[i])->rotation.connectFrom(&((SoQtComposeRotation *) compR[i])->rotation);
      }
      else
      {
         error = 5; // not a valid Model3d file
      }
      i++ ;
   }

   if (error == 0)
   {
      SoSeparator *axisworld = new SoSeparator;
      axisworld->unrefNoDelete();
		
		SoCoordinateAxis *AxisW   = new SoCoordinateAxis();
	   AxisW->fNDivision = 1;
      AxisW->fDivisionLength = 200;
		axisworld->addChild(AxisW);

      //kinechain->insertChild(AxisW,1);
      view->addNoColObject(axisworld);
		
    /*  lookingforjoints.setName("tool");
      lookingforjoints.setType(SoSeparator::getClassTypeId());
      lookingforjoints.setInterest(SoSearchAction::FIRST);
      lookingforjoints.apply(kinechain);

      if(lookingforjoints.getPath() != NULL)
      {
         SoNode * pnode = lookingforjoints.getPath()->getTail();;
      	
			SoCoordinateAxis *AxisT   = new SoCoordinateAxis();
			AxisT->fNDivision = 1;
      	AxisT->fDivisionLength = 200;

			SoSeparator *axistool = new SoSeparator;
         axistool->ref();
         axistool = (SoSeparator *) pnode;
         axistool->addChild(AxisT);
			view->addNoColObject(axistool);			
      }
      */ 
      panel_control *panel = new panel_control(0, "Panel", kineengine);

      for (int i = 0; i < kineengine->dof; i++)
      {
         // connect(panel->ldial[i], SIGNAL(valueChange(double)),((SoQtComposeRotation *) compR[i]), SLOT(setValue_angle(double)));
         connect(panel->ldial[i], SIGNAL(valueChange(double)),&panel->kinechain->list_plug[i], SLOT(setValue(double)));
         connect(&panel->kinechain->list_plug[i], SIGNAL(valueChanged(double)),((SoQtComposeRotation *) compR[i]), SLOT(setValue_angle(double)));
         connect(&panel->kinechain->list_plug[i], SIGNAL(valueChanged(double)),panel->ldial[i], SLOT(setValue(double)));
      }

      view->addRobotCell(kinechain);

      panel->show();
      panel->update_limits();
      //panel->kinechain->setconsole_mode(true); Ja estava comentada
      panel->kinechain->setconsole_mode(false);
      panel->kinechain->do_ready();
		connect(view, SIGNAL(pick_point(Rhmatrix)),panel, SLOT(move_pickpoint(Rhmatrix )));
      //panel->kinechain->setconsole_mode(false); Ja estava comentada
   }
   return error;
}

int QilexDoc::doc_insert_kinematic_hand(Rchain_hand *kineengine, SoSeparator *kinechain)
{
   int error = 0;
   int i;

   SbVec3f joinax;

   float joinangle;

   SbName joints[] = {"joint1", "joint2", "joint3", "joint4","joint5", "joint6",
                   "joint7", "joint8", "joint9", "joint10","joint11", "joint12",
                   "joint13", "joint14", "joint15", "joint16","joint17", "joint18",
                   "joint19", "joint20", "joint21", "joint22","joint23", "joint24",  };

   SbName tools[] = {"tool1", "tool2", "tool3", "tool4", "tool5", "tool6", };

   SoEngineList compR(kineengine->n_joint);
   SoNodeList Rots(kineengine->n_joint);

   SoSearchAction lookingforjoints;
   SoTransform *pjoint = new SoTransform;

      // Identifie the rotations and assing the job
   i = 0;

  while (i < kineengine->n_joint && error == 0)
   {
      lookingforjoints.setName(joints[i]);
      lookingforjoints.setType(SoTransform::getClassTypeId());
      lookingforjoints.setInterest(SoSearchAction::FIRST);
      lookingforjoints.apply(kinechain);

      //  assert(lookingforjoints.getPath() != NULL);
      SoNode * pnode = lookingforjoints.getPath()->getTail();;

      pjoint = (SoTransform *) pnode;

      if(NULL != pjoint)
      {
         Rots.append((SoTransform *) pjoint);
         compR.append(new SoQtComposeRotation);
         // cal comprobar si l'articulació es de rotació o translació: arreglar...
         ((SoTransform *) Rots[i])->rotation.getValue(joinax, joinangle);
         ((SoQtComposeRotation *) compR[i])->axis.setValue(joinax);

         ((SoTransform *) Rots[i])->rotation.connectFrom(&((SoQtComposeRotation *) compR[i])->rotation);
      }
      else
      {
         error = 5; // not a valid Model3d file
      }
      i++ ;
   }

   if (error == 0)
   {
      SoCoordinateAxis *AxisW   = new SoCoordinateAxis();
      SoCoordinateAxis *AxisT   = new SoCoordinateAxis();

      AxisW->fNDivision = 1;
      AxisW->fDivisionLength = 200;
      AxisT->fNDivision = 1;
      AxisT->fDivisionLength = 100;

      kinechain->insertChild(AxisW,1);

      i = 0;
      while (i < kineengine->n_chain && error==0)
      {
      lookingforjoints.setName(tools[i]);
      lookingforjoints.setType(SoSeparator::getClassTypeId());
      lookingforjoints.setInterest(SoSearchAction::FIRST);
      lookingforjoints.apply(kinechain);

      if(lookingforjoints.getPath() != NULL)
      {
         SoNode * pnode = lookingforjoints.getPath()->getTail();;

         SoSeparator *axistool = new SoSeparator;
         axistool->ref();
         axistool = (SoSeparator *) pnode;
         axistool->addChild(AxisT);
      }
      i++;
      }

      panel_hand = new panel_control_hand(0, "Panel", kineengine);

     for (int i = 0; i < kineengine->n_joint; i++)
      {
         // connect(panel->ldial[i], SIGNAL(valueChange(double)),((SoQtComposeRotation *) compR[i]), SLOT(setValue_angle(double)));
         connect(panel_hand->ldial[i], SIGNAL(valueChange(double)),&panel_hand->kinechain->list_plug[i], SLOT(setValue(double)));
         connect(&panel_hand->kinechain->list_plug[i], SIGNAL(valueChanged(double)),((SoQtComposeRotation *) compR[i]), SLOT(setValue_angle(double)));
         connect(&panel_hand->kinechain->list_plug[i], SIGNAL(valueChanged(double)),panel_hand->ldial[i], SLOT(setValue(double)));
      }

      view->addRobotCell(kinechain);

      panel_hand->show();
      panel_hand->update_limits();
      panel_hand->kinechain->setconsole_mode(false);
      panel_hand->kinechain->do_ready();
   }
   return error;
}


/************************************************/
/************************************************/
/************************************************/
/************************************************/
/************************************************/
void *reallocCB(void * ptr, size_t newsize)
{
   return realloc(ptr, newsize);
}

//   SoWriteAction writeAction;
//   writeAction.apply(scene);
int QilexDoc::read_grasp_xml (QDomElement kine_element, ct_new_grasping_object* grasp)
{
   QDomNodeList list;
   QDomElement element;
   QDomNode node;
   QString data;
   bool ok;
   int int_number = 0;
   int error = 0;

   if ( kine_element.tagName () == "kineobject" )
   {
      data = kine_element.attribute ("num_point", QString::null);
      if (data.isNull())
      {
         error = 3;
      }
      else
      {
         int_number = data.toInt( &ok, 10 );
      }
   }
   else
      error = 3;

   grasp->grasp_points = new ct_grasp_points[int_number];
   grasp->num_point = int_number;
   list = kine_element.elementsByTagName ("point");

   if (list.count()==(unsigned int)int_number)
   {
      for (int i=0;i<int_number;i++)
      {
         grasp->grasp_points[i].point = i;
         node = list.item(i);
         if ( node.isElement())
         {   element = node.toElement();

	    data = element.attribute ("x", QString::null);
            grasp->grasp_points[i].px = data.toDouble();

            data = element.attribute ("y", QString::null);
            grasp->grasp_points[i].py = data.toDouble();

            data = element.attribute ("z", QString::null);
            grasp->grasp_points[i].pz = data.toDouble();

            data = element.attribute ("nx", QString::null);
            grasp->grasp_points[i].nx = data.toDouble();

            data = element.attribute ("ny", QString::null);
            grasp->grasp_points[i].ny = data.toDouble();

            data = element.attribute ("nz", QString::null);
            grasp->grasp_points[i].nz = data.toDouble();

         }
         else
            error = 3;
      }
   }
   else
      error = 3;

   return error;
}

int QilexDoc::read_grasp_points (ct_new_grasping_object *data)
{
      // envies un nom d'un fitxer xml amb la configuració del robot i
      // inicialitza la classe
      // retorna un 0 si tot ha estat bé.
      // retorna un 1 si el fitxer no ha pogut ser validat
      // retorna un 2 si el fitxer estava buit
      // retorna un 3 si el fitxer tenia algun tipus d'error a les dades
      // retorna un 4 si el fitxer no s'ha pogut obrir

   int error=0;

   QDomDocument xmlDoc;

   QFile xmlFile(data->QsDatFile.latin1());

   if ( !xmlFile.open( IO_ReadOnly ) )
   {
//      QMessageBox::critical( 0,tr( "Critical Error" ),tr( "Cannot open file %1" ).arg( fileName ) );
        error = 4;
   }
   if( !xmlDoc.setContent( &xmlFile ) )
   {
  //      QMessageBox::critical( 0,tr( "Critical Error" ),tr( "Parsing error for file %1" ).arg( fileName ) );
        xmlFile.close();
        error = 1;
   }
   xmlFile.close();

    // get the header information from the DOM
   QDomElement element = xmlDoc.documentElement();

//   node = root.firstChild();
   error = read_grasp_xml (element, data);
   return error;
}


int QilexDoc::doc_insert_kinematic_chain_rx(Rchain_rx *kineengine, SoSeparator *kinechain)
{
   int error = 0;
   int i;

   SbVec3f joinax;

   float joinangle;
   
   SbName joints[] = {"joint1", "joint2", "joint3", "joint4","joint5", "joint6", };
   
   SoEngineList compR(kineengine->dof);
   SoNodeList Rots(kineengine->dof);

   SoSearchAction lookingforjoints;
   SoTransform *pjoint = new SoTransform;

      // Identifie the rotations and assing the job
   i = 0;

   while (i < kineengine->dof && error == 0)
   {
      lookingforjoints.setName(joints[i]);
      lookingforjoints.setType(SoTransform::getClassTypeId());
      lookingforjoints.setInterest(SoSearchAction::FIRST);
      lookingforjoints.apply(kinechain);

      //  assert(lookingforjoints.getPath() != NULL);
      SoNode * pnode = lookingforjoints.getPath()->getTail();;

      pjoint = (SoTransform *) pnode;

      if(NULL != pjoint)
      {
         Rots.append((SoTransform *) pjoint);
         compR.append(new SoQtComposeRotation);
         // cal comprobar si l'articulació es de rotació o translació: arreglar...
         ((SoTransform *) Rots[i])->rotation.getValue(joinax, joinangle);
         ((SoQtComposeRotation *) compR[i])->axis.setValue(joinax);

         ((SoTransform *) Rots[i])->rotation.connectFrom(&((SoQtComposeRotation *) compR[i])->rotation);
      }
      else
      {
         error = 5; // not a valid Model3d file
      }
      i++ ;
   }

   if (error == 0)
   {
      SoSeparator *axisworld = new SoSeparator;
      axisworld->unrefNoDelete();
		
		SoCoordinateAxis *AxisW   = new SoCoordinateAxis();
	   AxisW->fNDivision = 1;
      AxisW->fDivisionLength = 200;
		axisworld->addChild(AxisW);

      //kinechain->insertChild(AxisW,1);
      view->addNoColObject(axisworld);
		
    /*  lookingforjoints.setName("tool");
      lookingforjoints.setType(SoSeparator::getClassTypeId());
      lookingforjoints.setInterest(SoSearchAction::FIRST);
      lookingforjoints.apply(kinechain);

      if(lookingforjoints.getPath() != NULL)
      {
         SoNode * pnode = lookingforjoints.getPath()->getTail();;
      	
			SoCoordinateAxis *AxisT   = new SoCoordinateAxis();
			AxisT->fNDivision = 1;
      	AxisT->fDivisionLength = 200;

			SoSeparator *axistool = new SoSeparator;
         axistool->ref();
         axistool = (SoSeparator *) pnode;
         axistool->addChild(AxisT);
			view->addNoColObject(axistool);			
      }
      */ 
      panelControlRx *panel = new panelControlRx(0, "Panel", kineengine);

      for (int i = 0; i < kineengine->dof; i++)
      {
         // connect(panel->ldial[i], SIGNAL(valueChange(double)),((SoQtComposeRotation *) compR[i]), SLOT(setValue_angle(double)));
         connect(panel->ldial[i], SIGNAL(valueChange(double)),&panel->kinechain->list_plug[i], SLOT(setValue(double)));
         connect(&panel->kinechain->list_plug[i], SIGNAL(valueChanged(double)),((SoQtComposeRotation *) compR[i]), SLOT(setValue_angle(double)));
         connect(&panel->kinechain->list_plug[i], SIGNAL(valueChanged(double)),panel->ldial[i], SLOT(setValue(double)));
      }

      view->addRobotCell(kinechain);

      panel->show();
      panel->update_limits();
      //panel->kinechain->setconsole_mode(true); Ja estava comentada
      panel->kinechain->setconsole_mode(false);
      panel->kinechain->do_ready();
		connect(view, SIGNAL(pick_point(Rhmatrix)),panel, SLOT(move_pickpoint(Rhmatrix )));
      //panel->kinechain->setconsole_mode(false); Ja estava comentada
   }
   return error;
}
