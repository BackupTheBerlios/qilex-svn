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


#include <math.h>
#include <sstream>
#include <iomanip.h>
#include <string.h>

#include "rchain_hand.h"
#include "functions.h"

/************************************************************************

Class Rchain_hand
(generic open chain)

************************************************************************/


/***********************************************************************
INICIALITZACIÓ DE LA CLASSE I LECTURA DE DADES
***********************************************************************/

Rchain_hand::Rchain_hand ()
{
   Tworld = Tworld.identity();
   ptimer = new QTimer( this );

}

Rchain_hand::~Rchain_hand ()
{
  int l;

  for (l=0;l<n_chain;l++)
  {
   delete [] status_join[l];
   delete [] pqf[l];
   delete [] pqi[l];
   delete [] pslope_path[l];
              
   delete[] home[l];
   delete[] pT[l];
   delete[] ppremult_T[l];
   delete[] ppostmult_T[l];
   
   delete[] ptkT[l];
   delete[] ptkpremult_T[l];
   delete[] ptkpostmult_T[l];


   delete[] dh_parameters[l];
  }
  delete[] list_plug;
  delete[] dof;
  delete[] pS,
  delete[] Ttool;
}

void Rchain_hand::num_chains(int cadenes)
{
  int l;

   n_chain = cadenes;
   dof = new int[n_chain];
   status_join = new double*[n_chain];
   pqi = new double*[n_chain];
   pqf = new double*[n_chain];
   pslope_path = new double*[n_chain];
   home = new double*[n_chain];
   dh_parameters = new dh_parameters_joint2*[n_chain];
   pT = new Rhtranslink*[n_chain];
   ppremult_T = new Rhmatrix*[n_chain];
   ppostmult_T = new Rhmatrix*[n_chain];
   ptkT = new Rhtranslink*[n_chain];
   ptkpremult_T = new Rhmatrix*[n_chain];
   ptkpostmult_T = new Rhmatrix*[n_chain];
   pS = new double[n_chain];
   Ttool = new Rhmatrix[n_chain];
   for(l=0;l<n_chain;l++)
   {
     pS[l]=1.0;
     Ttool[l] = Ttool[l].identity();
   }
}
   
/** Write property of int n.  and init the class with the vars and cubic arrays*/
void Rchain_hand::init (int pnewVal, int k)
{
   dof[k] = pnewVal;

   nsolmax = 16;

   status_join[k] = new double[pnewVal+1];
   pqi[k] = new double[pnewVal+1];
   pqf[k] = new double[pnewVal+1];
   pslope_path[k] = new double[pnewVal+1];
  
   home[k] = new double[pnewVal+1];


   name = new char[40];

   dh_parameters[k] = new dh_parameters_joint2[pnewVal+1];

   //iniciate = true;
   console_mode = false;



   // Why pnewVal + 2?
   // pnewVal is the number of link
   // 1 the world transform
   // and 2 is the tool transform
   // ergo I need to keep n + 2 transforms
   //

  
   pT[k] = new Rhtranslink[pnewVal+2];

   ppremult_T[k] = new Rhmatrix[pnewVal+2];
   ppostmult_T[k] = new Rhmatrix[pnewVal+2];


   // private temporal vars

   ptkT[k] = new Rhtranslink[pnewVal + 2];
   ptkpremult_T[k] = new Rhmatrix[pnewVal + 2];
   ptkpostmult_T[k] = new Rhmatrix[pnewVal + 2];

}

/* You must call this routine in order of the joins. So, first the join 1, after 2, ... till n
  and the value of the i, the join */ 

void Rchain_hand::set_dh_pararmeters (int sigma, double theta, double alphai,
			       double ai, double di, double low_rank,
			       double up_rank, double maxspeed, double maxacc,
			       int i, int k)
{

   double rad_theta = rad (theta);
   double rad_alpha = rad (alphai);
   double temp = 0;
   //double *vec_speed;
   //double *vec_acc;
   //int l;
   //ostringstream pcout;
   
   //pcout << endl << "Aquest són els valors del l'articulació: " << i << endl;
   dh_parameters[k][i].sigma = sigma;
   dh_parameters[k][i].theta = rad_theta;
   dh_parameters[k][i].stheta = sin (rad_theta);
   dh_parameters[k][i].ctheta = cos (rad_theta);
   dh_parameters[k][i].di = di;

   if (sigma == 1)
      status_join[k][i] = rad_theta;
   else
      status_join[k][i] = di;

   dh_parameters[k][i].salpha = sin (rad_alpha);
   dh_parameters[k][i].calpha = cos (rad_alpha);
   dh_parameters[k][i].ai = ai;

   // low_rank and up_rank must be in radians if the join is rotation

   if (sigma == 1)
   {
      dh_parameters[k][i].lower_value = rad (low_rank);
      dh_parameters[k][i].upper_value = rad (up_rank);
      dh_parameters[k][i].m_speed = rad(maxspeed);
      dh_parameters[k][i].m_acc = rad(maxacc);
   }
   else
   {
      dh_parameters[k][i].lower_value = low_rank;
      dh_parameters[k][i].upper_value = up_rank;
      dh_parameters[k][i].m_speed = maxspeed;
      dh_parameters[k][i].m_acc = maxacc;
   }

   dh_parameters[k][i].rank = fabs(dh_parameters[k][i].upper_value - dh_parameters[k][i].lower_value);

  // create transform matrix T[i]

   pT[k][i].transformation (dh_parameters[k][i].stheta, dh_parameters[k][i].ctheta,
			dh_parameters[k][i].salpha, dh_parameters[k][i].calpha,
			dh_parameters[k][i].ai, dh_parameters[k][i].di);

  // update and scale
   if (i == dof[k])
   {
      for (int n = 1; n <= i; n++)
      {
	      temp = temp + dh_parameters[k][n].di + dh_parameters[k][n].ai;
      }

      // maximum distante betwend each the links
      pS[k] = pS[k] / (temp*0.06);

      //initial transformation; identity
      pT[k][0].transformation(0, 1, 0, 1, 0, 0);
      //initial transformation tool; identity
      pT[k][ dof[k] + 1].transformation(0, 1, 0, 1, 0, 0);
            
      ppremult_T[k][0] = pT[k][0].value();
      Tworld = pT[k][0].value();
      ppostmult_T[k][dof[k] + 1] = pT[k][dof[k] + 1].value();
      Ttool[k] = pT[k][dof[k] + 1].value();

    //  pcout << "Ppostmult_T[dof + 1]" << endl;
      
    //  printmatrix(ppostmult_T[dof + 1]);

      
      for (int n = 1; n <= i + 1; n++)
      {
	      pT[k][n].scale_dist (pS[k]);
	      pupdate_T_premult (ppremult_T[k], pT[k], n, k);

         if (n<=dof[k])
         {
          dh_parameters[k][n].ai = dh_parameters[k][n].ai * pS[k];
   	      dh_parameters[k][n].di = dh_parameters[k][n].di * pS[k];
         }
	   }
      for (int n = i ; n >= 0; n--)
      {
         pupdate_T_postmult (ppostmult_T[k], pT[k], n);
      }
      //pcout << "Tworld" << endl;
      //printmatrix(Tworld);
      //pcout << "Ttool" << endl;
      //printmatrix(Ttool[k]);
      
      /*vec_speed = new double[dof[k]+1];
      vec_acc = new double[dof[k]+1];
      vec_speed[0] = 2000;
      vec_acc[0] = 500;
      l=1;
      while (l<=dof[k])
      {
        vec_speed[l]=dh_parameters[k][l].m_speed;
        vec_acc[l]=dh_parameters[k][l].m_acc;
        l++;
      }*/

      //pmax_speed[k] = dh_parameters[k][minvector(vec_speed, dof[k])].m_speed;
      //pmax_acceleration[k] = dh_parameters[k][minvector(vec_acc, dof[k])].m_acc;
      //pcurrent_speed[k] = pmax_speed[k] * 0.5;
      //pcurrent_acceleration[k] = pmax_acceleration[k] * 0.5;


     // delete[] vec_speed;
      //delete[] vec_acc;

   }
   //send_stream(pcout);
      
}

int Rchain_hand::init_dat (const char *robot_file)
{
      // envies un nom d'un fitxer xml amb la configuració del robot i
      // inicialitza la classe
      // retorna un 0 si tot ha estat bé.
      // retorna un 1 si el fitxer no ha pogut ser validat
      // retorna un 2 si el fitxer estava buit
      // retorna un 3 si el fitxer tenia algun tipus d'error a les dades
      // retorna un 4 si el fitxer no s'ha pogut obrir

   int error;
   int art;

   QDomDocument xmlDoc;

   QFile xmlFile(robot_file);

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
   error = read_element_xml (element);

   art = 0;
   for(int k=0;k<n_chain;k++)
   {
    art = art + (dof[k] - n_same - 3);
   }
   n_joint = n_same + art;

   list_plug = new plug[n_joint];
   for(int i=0;i<n_joint;i++)
   {
    list_plug[i].setIndex(i);
   }

   delta = new double[n_chain];
   delta[0] = 0.0;
   delta[1] = home[0][6] - home[1][6];
   delta[2] = home[0][6] - home[2][6];
   delta[3] = home[0][6] - home[3][6];

   return error;
}
int Rchain_hand::read_element_xml (QDomElement kine_element)
{
   QDomNodeList list, list0;
   QDomElement element, element0;
   QDomNode node0, node;
   QString data;
   bool ok;
   double sigma, theta, alpha, ai, di, low_rank, up_rank, maxspeed, maxacc;
   double phome;
   int int_number1, int_number2, int_number3;
   int error = 0;

   if ( kine_element.tagName () == "kinehand" )
   {
      name = kine_element.attribute ("name", QString::null);
      data = kine_element.attribute ("chains", QString::null);
      if (data.isNull())
      {
         error = 2;
      }
      else
      {
         int_number1 = data.toInt( &ok, 10 );
         if (ok)
         {
            num_chains(int_number1);
         }
         else
         {
            error = 3;
         }
      }
      data = kine_element.attribute ("robjoint", QString::null);
      if (data.isNull())
      {
        error = 3;
      }
      else
      {
        int_number2 = data.toInt( &ok, 10 );
        if (ok)
        {
          n_same = int_number2;
        }
        else
        {
          error = 3;
        }
      } 
   }
   else
   {
     error = 2;
   }
   
   list0 = kine_element.elementsByTagName ("kinechain");
   if(list0.count()==(unsigned int)n_chain)
   {
   for(int k=0;k<n_chain;k++)
   {
     node0 = list0.item(k);
     if (node0.isElement())
     {
       element0 = node0.toElement();
       data = element0.attribute ("dof", QString::null);
       if (data.isNull())
       {
         error = 3;
       }
       else
       {
         int_number3 = data.toInt( &ok, 10);
         if(ok)
         {
           dof[k] = int_number3;
           init(dof[k],k);
         }
         else
         {
           error = 3;
         }
       }
     }
         
   list = element0.elementsByTagName ("joint");

   if (list.count()==(unsigned int)dof[k])
   {
      for (int i=0;i<dof[k];i++)
      {
         node = list.item(i);
         if ( node.isElement())
         {   element = node.toElement();

            data = element.attribute ("sigma", QString::null);
            sigma = data.toDouble();

            data = element.attribute ("theta", QString::null);
            theta = data.toDouble();

            data = element.attribute ("alpha", QString::null);
            alpha = data.toDouble();

            data = element.attribute ("ai", QString::null);
            ai = data.toDouble();

            data = element.attribute ("di", QString::null);
            di = data.toDouble();

            data = element.attribute ("low_rank", QString::null);
            low_rank = data.toDouble();

            data = element.attribute ("up_rank", QString::null);
            up_rank = data.toDouble();

            data = element.attribute ("max_speed", QString::null);
            maxspeed = data.toDouble();

            data = element.attribute ("max_acc", QString::null);
            maxacc = data.toDouble();

            data = element.attribute ("home", QString::null);
            phome = data.toDouble();

            set_dh_pararmeters (sigma, theta, alpha, ai, di, low_rank,
            up_rank, maxspeed, maxacc, i+1, k);

            if (sigma == 1)
               home[k][i+1] = rad(phome);
            else
               home[k][i+1] = phome;
         }
         else
            error = 3;
      }
   }
   else
      error = 2;
   }
   }
   return error;       
}



/****************************************************************************
ACTUALITZACIÓ DE MATRIUS I VARIABLES EN CORRESPONDÈNCIA AMB EL PANELL DE CONTROL
****************************************************************************/


void Rchain_hand::do_ready()
{
   move_path_join (home, 0);
   /*for(int k=0; k<n_chain; k++)
   {
   show_tcp(k);
   }*/
}

void Rchain_hand::move_path_join (double **qf, int movement)
{
   double *distance;
   double dist_max;

   distance = new double[n_chain+1];
   distance[0] = 0.0;

   p_speed = rad(100.0);
   p_acceleration = rad(2.5);

   //assign the values of the positions
   for (int k=0; k<n_chain; k++)
   {
      pslope_path[k][0] = 0.0;
      for (int i=1; i<=dof[k];i++)
      {
        pqi[k][i] = status_join[k][i];
        pqf[k][i] = qf[k][i];
        pslope_path[k][i] = fabs(pqi[k][i] - pqf[k][i]);
      }
   }
   for(int k=0;k<n_chain;k++)
   {
   distance[k+1] = pslope_path[k][maxvector(pslope_path[k], dof[k])];
   }

   dist_max = distance[maxvector(distance,n_chain)];

   ptime_f = dist_max / p_speed;

   ptime = 0;

   for (int k=0; k<n_chain;k++)
   {
   for (int i = 1; i<=dof[k]; i++)
   {
      pslope_path[k][i] = (pqf[k][i] - pqi[k][i])/ptime_f;
   }
   }
   if (movement == 0) // linear movement
   {
      connect( ptimer, SIGNAL(timeout()), this, SLOT(slot_genpas_linear()) );
   }
   ptimer->start( 40, FALSE );

}

void Rchain_hand::slot_genpas_linear()
{

   double value;

   if (ptime >= ptime_f) // last movement
   {
      for(int i=1; i<=n_same;i++)
      {
        pupdate_join_arm (i, pqf[0][i]);
      }
      for(int k=0; k<n_chain;k++)
      {
      for (int i=n_same+1; i<=dof[k];i++)
      {
         pupdate_join (i, pqf[k][i],k);
      }
      }
      disconnect(ptimer, SIGNAL(timeout()), this, SLOT(slot_genpas_linear()) );
      ptimer->stop();
   }
   else
   {
      for (int i=1;i<=n_same;i++)
      {
         value = pslope_path[0][i]*ptime + pqi[0][i];
	 pupdate_join_arm (i, value);
      }
      for (int k=0;k<n_chain;k++)
      {
      for (int i=n_same+1; i<=dof[k];i++)
      {
         value = pslope_path[k][i] * ptime + pqi[k][i];
         pupdate_join (i, value,k);
      }
      }
      ptime = ptime + 0.040; // incrementa el timer
   }

}



/* procedure to update the "premult" vector from a "T" vector*/

void Rchain_hand::pupdate_T_premult (Rhmatrix ppremult_Tt[], Rhtranslink pTt[], int n, int k)
{
  int cont = n + 1;

  // if n = 0, it's the world transformation

  //we know that you only update a join between
  // 1 and dof
  // 0 is the world transform
  //dof + 1 is the tools transform

  ppremult_Tt[n] = ppremult_Tt[n - 1] * pTt[n].value ();

  while (cont <= dof[k] + 1)
  {
      ppremult_Tt[cont] = ppremult_Tt[cont - 1] * pTt[cont].value ();
      cont++;
  }
}

void
Rchain_hand::pupdate_T_postmult (Rhmatrix ppostmult_Tt[], Rhtranslink pTt[],
			       int n)
{
  //we know that you only update a join between
  // 1 and dof
  // 0 is the world transform
  //dof + 1 is the tool transform
  

  int cont = n - 1;

  ppostmult_Tt[n] = pTt[n].value () * ppostmult_Tt[n + 1];

  while (cont >= 0)
  {
      ppostmult_Tt[cont] = pTt[cont].value () * ppostmult_Tt[cont + 1];
      cont--;
  }
}

void Rchain_hand::print_config (int n, int k)
{
   std::ostringstream pcout;
   if (n > 0)
   {
      pcout << "-----------------------------------------------------" << endl;
      pcout << "Value of T matrix " << endl;
      send_stream(pcout);
      printmatrix(pT[k][n], (1 / pS[k]));
      pcout << "Value of premultT matrix " << endl;
      send_stream(pcout);
      printmatrix(ppremult_T[k][n],(1 / pS[k]));
      send_stream(pcout);
      pcout << "Value of postmultT matrix " << endl;
      send_stream(pcout);
      printmatrix(ppostmult_T[k][n], (1 / pS[k]));
      pcout << "Value of the articulation: " << degree (status_join[k][n]) << endl;
      pcout << "----------------------------------------------------" << endl;
      send_stream(pcout);
   }
}

void Rchain_hand::tcp (int k)
{
   std::ostringstream pcout;
   pcout << "----------------------------------------------------" << endl;
   pcout << "Value of the TCP: " << endl;
   send_stream(pcout);
   printmatrix(ppremult_T[k][dof[k]+1], (1 / pS[k]));
   //printmatrix(ppostmult_T[k][0], (1 / pS[k]));
   pcout << "----------------------------------------------------" << endl;
   send_stream(pcout);
}
void Rchain_hand::tcp_arm (int k)
{
  std::ostringstream pcout;
   pcout << "----------------------------------------------------" << endl;
   pcout << "Value of the TCP: " << endl;
   send_stream(pcout);
   printmatrix(ppremult_T[k][5], (1 / pS[k]));
   //printmatrix(ppostmult_T[k][5], (1 / pS[k]));
   pcout << "----------------------------------------------------" << endl;
   send_stream(pcout);
}
  
Rhmatrix Rchain_hand::mat_tcp(int k)
{
   return ppremult_T[k][dof[k]+1].descaled(1 / pS[k]);
}
Rhmatrix Rchain_hand::mat_tcp_arm(int k)
{                            
   return ppremult_T[k][5].descaled(1 / pS[k]);
}

// this function return the scale how is working the class.
double Rchain_hand::scale (int k)
{
  return pS[k];
}

void Rchain_hand::send_stream(std::ostringstream &oss)
{
   if (widget)
   {
      console->insertLineAtEnd(QString::QString(oss.str().c_str()));
      oss.flush();
   }
   else
   {
      cout << oss ;
      oss.flush();
   }
}

void Rchain_hand::setconsole_mode(bool mode)
{
   // mode de funcionament
   // si fals, mode de botonera
   // per tant l'actualització de les articulacions
   // venen del panell

   // si cert, mode consola
   // l'actualització de les articulacions, parteix del motor de la simulaci
   console_mode = mode;

   if(mode)
   {
      disconnect();

   }
   else
   {
      disconnect();
      for(int i=0; i<n_joint; i++)
      {
         connect(&list_plug[i], SIGNAL(valueChangedfull(int, double)), this, SLOT(setValue(int, double)));
      }
   }
}
void Rchain_hand::pupdate_join (int join, double value, int k)
{
  if ((dh_parameters[k][join].lower_value <= value) && (value <= dh_parameters[k][join].upper_value))
   {
    if (join<=10)
    {
     list_plug[join-1+(k*4)].setValue(value);
    }
   status_join[k][join] = value;
   pT[k][join].update (int (dh_parameters[k][join].sigma), value);
   pupdate_T_premult (ppremult_T[k], pT[k], join, k);
   pupdate_T_postmult (ppostmult_T[k], pT[k], join);
   }
   else
   {
     if (ptimer->isActive())
      {
         ptimer->stop();
      }
    }
}
void Rchain_hand::pupdate_join_arm (int join, double value)
{
  double value3=0.0;
  if ((dh_parameters[0][join].lower_value <= value) && (value <= dh_parameters[0][join].upper_value))
   {
   for (int k=0;k<n_chain;k++)
   {
    if (k==3 && join==6)
    {
     value3 = value - delta[3];
     status_join[k][join] = value3;
     pT[k][join].update (int (dh_parameters[k][join].sigma), value3);
     pupdate_T_premult (ppremult_T[k], pT[k], join, k);
     pupdate_T_postmult (ppostmult_T[k], pT[k], join);
    }
    else
    {
     status_join[k][join] = value;
     pT[k][join].update (int (dh_parameters[k][join].sigma), value);
     pupdate_T_premult (ppremult_T[k], pT[k], join, k);
     pupdate_T_postmult (ppostmult_T[k], pT[k], join);
   }
   }
   list_plug[join-1].setValue(value);
   }
   else
   {
     if (ptimer->isActive())
      {
         ptimer->stop();
      }
    }
}

void Rchain_hand::mhome (int k)
{
   for (int l = 1; l <= dof[k]; l++)
   {
      if (dh_parameters[k][l].sigma == 0)
      {
         pupdate_join (l, home[k][l], k);
      }
      else
      {
         pupdate_join (l, home[k][l], k);
      }
   }
}

void Rchain_hand::show_status (int k)
{
   std::ostringstream pcout;
   
   pcout << setiosflags (ios::fixed | ios::showpoint);
   pcout << endl;
   pcout << setw (10) << setprecision (5) << endl;


   for (int l = 1; l <= dof[k]; l++)
   {
      if (dh_parameters[k][l].sigma == 0)
      {
         pcout << endl << "Join: " << l  << "  Current value:"
         << status_join[k][l] << endl;
      }
      else
      {
         pcout << endl << "Join: " << l << "  Current value:"
         << status_join[k][l] << endl;
      }
      pcout << "Limit inferior : " << dh_parameters[k][l].lower_value << endl;
      pcout << "Limit superior : " << dh_parameters[k][l].upper_value << endl;
   }
   send_stream(pcout);
   tcp (k);
}

// cal revisar aquesta funció

void Rchain_hand::rank (int n, int k)
{
   int l = n;
   n--;
   cout << "movent l'articulació : " << n  << "; " << n << endl;
   cout << "Limit inferior : " << dh_parameters[k][n].lower_value << endl;
   cout << "Limit superior : " << dh_parameters[k][n].upper_value << endl;

   move_join (l, dh_parameters[k][l].lower_value, k);
   cout << dh_parameters[k][n].lower_value << dh_parameters[k][n].upper_value << endl;
   for (double inc = dh_parameters[k][n].lower_value; inc <= dh_parameters[k][n].upper_value;inc = inc + 0.05)
   {
      move_join (l, inc, k);
   }
}

void Rchain_hand::setconsole (  consoleWidget *pconsole)
{
   console = pconsole;
   widget = true;
}



/****************************************************************************
FUNCIONS USADES DURANT EL CÀLCUL DE LA CINEMÀTICA INVERSA
****************************************************************************/



double Rchain_hand::pF_objective (Rhmatrix Ti, Rhmatrix Th_i, double &qi, int i, int k)
{

   double F = 0;
   double t_NUM, t_DEN, t_indep, t_dm, pqi;
   double Flow = 0, Fupp =0;
   double C;

   
   C = (dh_parameters[k][i].upper_value-dh_parameters[k][i].lower_value)/3.0;
                 
   t_indep = ptindep (Ti, Th_i, dh_parameters[k][i].salpha,
                  dh_parameters[k][i].calpha, dh_parameters[k][i].ai);
   t_dm = -dh_parameters[k][i].salpha * Ti.mat[1][3]
            - dh_parameters[k][i].calpha * Ti.mat[2][3]
            + Th_i.mat[2][3];

   if (dh_parameters[k][i].sigma == 0)
   {
      F = qi * qi - 2 * t_dm * qi + t_indep;

      if (!(dh_parameters[k][i].lower_value <= t_dm && dh_parameters[k][i].upper_value >= t_dm))
      	
      qi = t_dm;
   }
   else
   {
      t_DEN = pDEN_theta (Ti, Th_i, dh_parameters[k][i].salpha, dh_parameters[k][i].calpha,
		    dh_parameters[k][i].ai);
      t_NUM = pNUM_theta (Ti, Th_i, dh_parameters[k][i].salpha, dh_parameters[k][i].calpha,
		    dh_parameters[k][i].ai);

      F = 2 * (t_DEN * cos (qi) + t_NUM * sin (qi)) + t_indep +
            	dh_parameters[k][i].di * dh_parameters[k][i].di - 2 * dh_parameters[k][i].di * t_dm;

      
    
      qi = atan2 (t_NUM, t_DEN);

      if ((-t_DEN*cos(qi)-t_NUM*sin(qi))<0)
      {
        qi = qi + M_PI;
      } 

     
      if (dh_parameters[k][i].lower_value <= qi && dh_parameters[k][i].upper_value >= qi)
      {
      }
      else
      {
         if (qi >= 0)
         {
            pqi = -(M2PI - qi);
         }
         else
         {
            pqi = M2PI + qi;
         }
 	      if (dh_parameters[k][i].lower_value <= pqi && dh_parameters[k][i].upper_value >= pqi)
	      {
	         qi = pqi;
	      }
	      else
	      {
           Flow = 2 * (t_DEN * cos (dh_parameters[k][i].lower_value+C) + t_NUM * sin (dh_parameters[k][i].lower_value+C)) + t_indep +
            	dh_parameters[k][i].di * dh_parameters[k][i].di - 2 * dh_parameters[k][i].di * t_dm;
           Fupp = 2 * (t_DEN * cos (dh_parameters[k][i].upper_value-C) + t_NUM * sin (dh_parameters[k][i].upper_value-C)) + t_indep +
            	dh_parameters[k][i].di * dh_parameters[k][i].di - 2 * dh_parameters[k][i].di * t_dm;

           if (Flow < Fupp) {
             qi = dh_parameters[k][i].lower_value + C;
             F = Flow;
           }
           else
           {
             qi = dh_parameters[k][i].upper_value - C;
             F = Fupp;
           }
           }
       }
   }
   return (fabs (F));

}

double Rchain_hand::pF_objective_without_ranks (Rhmatrix Ti, Rhmatrix Th_i, double &qi, int i, int k)
{

   double F = 0;
   double t_NUM, t_DEN, t_indep, t_dm;
   double C;


   C = (dh_parameters[k][i].upper_value-dh_parameters[k][i].lower_value)/4.0;

   t_indep = ptindep (Ti, Th_i, dh_parameters[k][i].salpha,
                  dh_parameters[k][i].calpha, dh_parameters[k][i].ai);
   t_dm = -dh_parameters[k][i].salpha * Ti.mat[1][3]
            - dh_parameters[k][i].calpha * Ti.mat[2][3]
            + Th_i.mat[2][3];

   if (dh_parameters[k][i].sigma == 0)
   {
      F = qi * qi - 2 * t_dm * qi + t_indep;

      if (!(dh_parameters[k][i].lower_value <= t_dm && dh_parameters[k][i].upper_value >= t_dm))
      	
      qi = t_dm;
   }
   else
   {
      t_DEN = pDEN_theta (Ti, Th_i, dh_parameters[k][i].salpha, dh_parameters[k][i].calpha,
		    dh_parameters[k][i].ai);
      t_NUM = pNUM_theta (Ti, Th_i, dh_parameters[k][i].salpha, dh_parameters[k][i].calpha,
		    dh_parameters[k][i].ai);

      F = 2 * (t_DEN * cos (qi) + t_NUM * sin (qi)) + t_indep +
            	dh_parameters[k][i].di * dh_parameters[k][i].di - 2 * dh_parameters[k][i].di * t_dm;



      qi = atan2 (t_NUM, t_DEN);

      if ((-t_DEN*cos(qi)-t_NUM*sin(qi))<0)
      {
        qi = qi + M_PI;
      }

   }
   return (fabs (F));

}

double Rchain_hand::pF_objective_arm (Rhmatrix *pTh[], Rhmatrix *pkpostmult_T[], double **qi, int i)
{

   double *t_NUM, *t_DEN, *t_indep, *t_dm, *qi_prov;
   double *Flow, *Fupp, *F;
   double pqi, qi_p;
   double Flow_tot = 0, Fupp_tot =0, Ftot = 0;
   double C;
   int l;
   double Numerador=0, Denominador=0;

   t_NUM = new double[n_chain];
   t_DEN = new double[n_chain];
   t_indep = new double[n_chain];
   t_dm = new double[n_chain];
   Flow = new double[n_chain];
   Fupp = new double[n_chain];
   F = new double[n_chain];
   qi_prov = new double[n_chain];

   C = (dh_parameters[0][i].upper_value-dh_parameters[0][i].lower_value)/3.0;

   for(int k=0;k<n_chain;k++)
   {
   t_indep[k] = ptindep (pkpostmult_T[k][i+1], pTh[k][i], dh_parameters[k][i].salpha,
                  dh_parameters[k][i].calpha, dh_parameters[k][i].ai);
   t_dm[k] = -dh_parameters[k][i].salpha * pkpostmult_T[k][i+1].mat[1][3]
            - dh_parameters[k][i].calpha * pkpostmult_T[k][i+1].mat[2][3]
            + pTh[k][i].mat[2][3];

   if (dh_parameters[k][i].sigma == 0)
   {
      F[k] = qi[k][i] * qi[k][i] - 2 * t_dm[k] * qi[k][i] + t_indep[k];

      if (!(dh_parameters[k][i].lower_value <= t_dm[k] && dh_parameters[k][i].upper_value >= t_dm[k]))

      qi[k][i] = t_dm[k];
   }
   else
   {
      t_DEN[k] = pDEN_theta (pkpostmult_T[k][i+1], pTh[k][i], dh_parameters[k][i].salpha, dh_parameters[k][i].calpha,
		    dh_parameters[k][i].ai);
      t_NUM[k] = pNUM_theta (pkpostmult_T[k][i+1], pTh[k][i], dh_parameters[k][i].salpha, dh_parameters[k][i].calpha,
		    dh_parameters[k][i].ai);

      F[k] = 2 * (t_DEN[k] * cos (qi[k][i]) + t_NUM[k] * sin (qi[k][i])) + t_indep[k] +
            	dh_parameters[k][i].di * dh_parameters[k][i].di - 2 * dh_parameters[k][i].di * t_dm[k];

    }
    }

      for(int k=0;k<n_chain;k++)
      {
        Ftot = Ftot + F[k];
      }

      if(i!=6)
      {
      for(l=0;l<n_chain;l++)
      {
        Numerador = Numerador + t_NUM[l];
        Denominador = Denominador + t_DEN[l];
      }

      qi_p = atan2 (Numerador, Denominador);

      if ((-Denominador*cos(qi_p)-Numerador*sin(qi_p))<0)
      {
        qi_p = qi_p + M_PI;
      }
      }
      else
      {
        for(l=0;l<n_chain;l++)
        {
          qi_prov[l] = atan2(t_NUM[l],t_DEN[l]);
        }
        qi_prov[3] = qi_prov[3] + 33.7;
        qi_p = (qi_prov[0]+qi_prov[1]+qi_prov[2]+qi_prov[3])/4.0;

      }

      if (dh_parameters[0][i].lower_value <= qi_p && dh_parameters[0][i].upper_value >= qi_p)
      {
      }
      else
      {
         if (qi_p >= 0)
         {
            pqi = -(M2PI - qi_p);
         }
         else
         {
            pqi = M2PI + qi_p;
         }
 	      if (dh_parameters[0][i].lower_value <= pqi && dh_parameters[0][i].upper_value >= pqi)
	      {
	         qi_p = pqi;
	      }
	      else
	      {
          for(int k=0;k<n_chain;k++)
          {
           Flow[k] = 2 * (t_DEN[k] * cos (dh_parameters[k][i].lower_value+C) + t_NUM[k] * sin (dh_parameters[k][i].lower_value+C)) + t_indep[k] +
            	dh_parameters[k][i].di * dh_parameters[k][i].di - 2 * dh_parameters[k][i].di * t_dm[k];
           Fupp[k] = 2 * (t_DEN[k] * cos (dh_parameters[k][i].upper_value-C) + t_NUM[k] * sin (dh_parameters[k][i].upper_value-C)) + t_indep[k] +
            	dh_parameters[k][i].di * dh_parameters[k][i].di - 2 * dh_parameters[k][i].di * t_dm[k];
           }
           for(int k=0;k<n_chain;k++)
           {
             Flow_tot = Flow_tot + Flow[k];
             Fupp_tot = Fupp_tot + Fupp[k];
           }

           if (Flow_tot < Fupp_tot) {
             qi_p = dh_parameters[0][i].lower_value + C;
             Ftot = Flow_tot;
           }
           else
           {
             qi_p = dh_parameters[0][i].upper_value - C;
             Ftot = Fupp_tot;
           }
           }
       }
       if(i!=6)
       {
       for(int k=0;k<n_chain;k++)
       {
         qi[k][i] = qi_p;
       }
       }
       else
       {
        for(int k=0;k<n_chain-1;k++)
       {
         qi[k][i] = qi_p;
       }
       qi[3][i] = qi_p -33.7;
       }

      delete[] t_NUM;
      delete[] t_DEN;
      delete[] t_indep;
      delete[] t_dm;
      delete[] Flow;
      delete[] Fupp;
      delete[] F;
      delete[] qi_prov;

   return (fabs (Ftot));

}

double Rchain_hand::pF_objective_arm_without_ranks (Rhmatrix *pTh[], Rhmatrix *pkpostmult_T[], double **qi, int i)
{

   double *t_NUM, *t_DEN, *t_indep, *t_dm, *qi_prov;
   double *Flow, *Fupp, *F;
   double qi_p;
   double Ftot = 0;
   double C;
   int l;
   double Numerador=0, Denominador=0;

   t_NUM = new double[n_chain];
   t_DEN = new double[n_chain];
   t_indep = new double[n_chain];
   t_dm = new double[n_chain];
   Flow = new double[n_chain];
   Fupp = new double[n_chain];
   F = new double[n_chain];
   qi_prov = new double[n_chain];

   C = (dh_parameters[0][i].upper_value-dh_parameters[0][i].lower_value)/4.0;

   for(int k=0;k<n_chain;k++)
   {
   t_indep[k] = ptindep (pkpostmult_T[k][i+1], pTh[k][i], dh_parameters[k][i].salpha,
                  dh_parameters[k][i].calpha, dh_parameters[k][i].ai);
   t_dm[k] = -dh_parameters[k][i].salpha * pkpostmult_T[k][i+1].mat[1][3]
            - dh_parameters[k][i].calpha * pkpostmult_T[k][i+1].mat[2][3]
            + pTh[k][i].mat[2][3];

   if (dh_parameters[k][i].sigma == 0)
   {
      F[k] = qi[k][i] * qi[k][i] - 2 * t_dm[k] * qi[k][i] + t_indep[k];

      if (!(dh_parameters[k][i].lower_value <= t_dm[k] && dh_parameters[k][i].upper_value >= t_dm[k]))
      	
      qi[k][i] = t_dm[k];
   }
   else
   {
      t_DEN[k] = pDEN_theta (pkpostmult_T[k][i+1], pTh[k][i], dh_parameters[k][i].salpha, dh_parameters[k][i].calpha,
		    dh_parameters[k][i].ai);
      t_NUM[k] = pNUM_theta (pkpostmult_T[k][i+1], pTh[k][i], dh_parameters[k][i].salpha, dh_parameters[k][i].calpha,
		    dh_parameters[k][i].ai);

      F[k] = 2 * (t_DEN[k] * cos (qi[k][i]) + t_NUM[k] * sin (qi[k][i])) + t_indep[k] +
            	dh_parameters[k][i].di * dh_parameters[k][i].di - 2 * dh_parameters[k][i].di * t_dm[k];

    }
    }

      for(int k=0;k<n_chain;k++)
      {
        Ftot = Ftot + F[k];
      }
      if(i!=6)
      {
      for(l=0;l<n_chain;l++)
      {
        Numerador = Numerador + t_NUM[l];
        Denominador = Denominador + t_DEN[l];
      }

      qi_p = atan2 (Numerador, Denominador);

      if ((-Denominador*cos(qi_p)-Numerador*sin(qi_p))<0)
      {
        qi_p = qi_p + M_PI;
      }
      }
      else
      {
        for(l=0;l<n_chain;l++)
        {
          qi_prov[l] = atan2(t_NUM[l],t_DEN[l]);
        }
        qi_prov[3] = qi_prov[3] + 33.7;
        qi_p = (qi_prov[0]+qi_prov[1]+qi_prov[2]+qi_prov[3])/4.0;
        
      }

       if(i!=6)
       {
       for(int k=0;k<n_chain;k++)
       {
         qi[k][i] = qi_p;
       }
       }
       else
       {
        for(int k=0;k<n_chain-1;k++)
       {
         qi[k][i] = qi_p;
       }
       qi[3][i] = qi_p -33.7;
       } 

      delete[] t_NUM;
      delete[] t_DEN;
      delete[] t_indep;
      delete[] t_dm;
      delete[] Flow;
      delete[] Fupp;
      delete[] F;
      delete[] qi_prov;

   return (fabs (Ftot));

}

double Rchain_hand::pF_objective_joint_six (Rhmatrix *pTh[], Rhmatrix *pkpostmult_T[], double **qi, int i)
{

   double *t_NUM, *t_DEN, *t_indep, *t_dm;
   double *Flow, *Fupp, *F;
   double qi_p;
   double Ftot = 0;
   int l;
   double Numerador=0, Denominador=0;

   t_NUM = new double[n_chain];
   t_DEN = new double[n_chain];
   t_indep = new double[n_chain];
   t_dm = new double[n_chain];
   Flow = new double[n_chain];
   Fupp = new double[n_chain];
   F = new double[n_chain];


   for(int k=0;k<n_chain;k++)
   {
   t_indep[k] = ptindep (pkpostmult_T[k][i+1], pTh[k][i], dh_parameters[k][i].salpha,
                  dh_parameters[k][i].calpha, dh_parameters[k][i].ai);
   t_dm[k] = -dh_parameters[k][i].salpha * pkpostmult_T[k][i+1].mat[1][3]
            - dh_parameters[k][i].calpha * pkpostmult_T[k][i+1].mat[2][3]
            + pTh[k][i].mat[2][3];

   if (dh_parameters[k][i].sigma == 0)
   {
      F[k] = qi[k][i] * qi[k][i] - 2 * t_dm[k] * qi[k][i] + t_indep[k];

      if (!(dh_parameters[k][i].lower_value <= t_dm[k] && dh_parameters[k][i].upper_value >= t_dm[k]))

      qi[k][i] = t_dm[k];
   }
   else
   {
      t_DEN[k] = pDEN_theta (pkpostmult_T[k][i+1], pTh[k][i], dh_parameters[k][i].salpha, dh_parameters[k][i].calpha,
		    dh_parameters[k][i].ai);
      t_NUM[k] = pNUM_theta (pkpostmult_T[k][i+1], pTh[k][i], dh_parameters[k][i].salpha, dh_parameters[k][i].calpha,
		    dh_parameters[k][i].ai);

      F[k] = 2 * (t_DEN[k] * cos (qi[k][i]) + t_NUM[k] * sin (qi[k][i])) + t_indep[k] +
            	dh_parameters[k][i].di * dh_parameters[k][i].di - 2 * dh_parameters[k][i].di * t_dm[k];

    }
    }

      for(int k=0;k<n_chain;k++)
      {
        Ftot = Ftot + F[k];
      }

      /*for(l=0;l<n_chain-1;l++)
      {
        Numerador = Numerador + t_NUM[l];
        Denominador = Denominador + t_DEN[l];
      }

      Numerador = Numerador+(t_NUM[3]*cos(delta))+(t_DEN[3]*sin(delta));
      Denominador = Denominador+(-t_NUM[3]*sin(delta))+(t_DEN[3]*cos(delta));*/

      for(l=0;l<n_chain;l++)
      {
        Numerador = Numerador + t_NUM[l]*cos(delta[l]) + t_DEN[l]*sin(delta[l]);
        Denominador = Denominador + t_DEN[l]*cos(delta[l]) - t_NUM[l]*sin(delta[l]);
      }

      qi_p = atan2(Numerador,Denominador);

      if ((-Numerador*cos(qi_p)-Denominador*sin(qi_p))<0)
      {
        qi_p = qi_p + M_PI;
      }

       for(int k=0;k<n_chain;k++)
       {
         qi[k][i] = qi_p - delta[k];
       }
       //qi[3][i] = qi_p - delta[3];


      delete[] t_NUM;
      delete[] t_DEN;
      delete[] t_indep;
      delete[] t_dm;
      delete[] Flow;
      delete[] Fupp;
      delete[] F;

   return (fabs (Ftot));

}

double Rchain_hand::pF_calculate (Rhmatrix Ti, Rhmatrix Th_i, double &qi, int i, int k)
{

   double F = 0;
   double t_NUM, t_DEN, t_indep, t_dm;


  t_indep = ptindep (Ti, Th_i, dh_parameters[k][i].salpha,
                  dh_parameters[k][i].calpha, dh_parameters[k][i].ai);
   t_dm = -dh_parameters[k][i].salpha * Ti.mat[1][3]
            - dh_parameters[k][i].calpha * Ti.mat[2][3]
            + Th_i.mat[2][3];

   if (dh_parameters[k][i].sigma == 0)
   {
      F = qi * qi - 2 * t_dm * qi + t_indep;

      if (!(dh_parameters[k][i].lower_value <= t_dm && dh_parameters[k][i].upper_value >= t_dm))
      	
      qi = t_dm;
   }
   else
   {
      t_DEN = pDEN_theta (Ti, Th_i, dh_parameters[k][i].salpha, dh_parameters[k][i].calpha,
		    dh_parameters[k][i].ai);
      t_NUM = pNUM_theta (Ti, Th_i, dh_parameters[k][i].salpha, dh_parameters[k][i].calpha,
		    dh_parameters[k][i].ai);

      F = 2 * (t_DEN * cos (qi) + t_NUM * sin (qi)) + t_indep +
            	dh_parameters[k][i].di * dh_parameters[k][i].di - 2 * dh_parameters[k][i].di * t_dm;

   }
   return (fabs (F));

}


double Rchain_hand::pNUM_theta (Rhmatrix Ti, Rhmatrix Th_i, double salphai,
		       double calphai, double ai)
{
  return (calphai * Th_i.mat[0][3] * Ti.mat[1][3] -
	  Ti.mat[2][3] * salphai * Th_i.mat[0][3] -
	  Ti.mat[2][2] * salphai * Th_i.mat[0][2] -
	  salphai * Th_i.mat[0][0] * Ti.mat[2][0] -
	  Ti.mat[2][1] * salphai * Th_i.mat[0][1] +
	  calphai * Th_i.mat[0][2] * Ti.mat[1][2] +
	  calphai * Th_i.mat[0][1] * Ti.mat[1][1] +
	  calphai * Th_i.mat[0][0] * Ti.mat[1][0] - ai * Th_i.mat[1][3] -
	  Th_i.mat[1][3] * Ti.mat[0][3] - Th_i.mat[1][2] * Ti.mat[0][2] -
	  Th_i.mat[1][1] * Ti.mat[0][1] - Th_i.mat[1][0] * Ti.mat[0][0]);
}


double Rchain_hand::pDEN_theta (Rhmatrix Ti, Rhmatrix Th_i, double salphai,
		       double calphai, double ai)
{
  return (-ai * Th_i.mat[0][3] - Th_i.mat[0][3] * Ti.mat[0][3] -
	  Th_i.mat[0][2] * Ti.mat[0][2] - Th_i.mat[0][1] * Ti.mat[0][1] -
	  Ti.mat[0][0] * Th_i.mat[0][0] +
	  Ti.mat[2][3] * salphai * Th_i.mat[1][3] +
	  Ti.mat[2][2] * salphai * Th_i.mat[1][2] +
	  Ti.mat[2][1] * salphai * Th_i.mat[1][1] +
	  salphai * Th_i.mat[1][0] * Ti.mat[2][0] -
	  calphai * Th_i.mat[1][3] * Ti.mat[1][3] -
	  calphai * Th_i.mat[1][2] * Ti.mat[1][2] -
	  calphai * Th_i.mat[1][1] * Ti.mat[1][1] -
	  calphai * Th_i.mat[1][0] * Ti.mat[1][0]);
}

double Rchain_hand::ptindep (Rhmatrix Ti, Rhmatrix Th_i, double salphai, double calphai,
		    double ai)
{
  return (Th_i.mat[0][0] * Th_i.mat[0][0] + Th_i.mat[0][1] * Th_i.mat[0][1] +
	  Th_i.mat[0][2] * Th_i.mat[0][2] + Th_i.mat[0][3] * Th_i.mat[0][3] +
	  Th_i.mat[1][3] * Th_i.mat[1][3] + Th_i.mat[2][0] * Th_i.mat[2][0] +
	  Th_i.mat[2][1] * Th_i.mat[2][1] + Th_i.mat[2][2] * Th_i.mat[2][2] +
	  Th_i.mat[2][3] * Th_i.mat[2][3] + Ti.mat[1][1] * Ti.mat[1][1] +
	  Ti.mat[1][0] * Ti.mat[1][0] + Ti.mat[2][3] * Ti.mat[2][3] +
	  ai * ai + Ti.mat[0][1] * Ti.mat[0][1] +
	  Ti.mat[0][2] * Ti.mat[0][2] + Ti.mat[0][3] * Ti.mat[0][3] +
	  Ti.mat[0][0] * Ti.mat[0][0] + Ti.mat[1][2] * Ti.mat[1][2] +
	  Ti.mat[1][3] * Ti.mat[1][3] + Ti.mat[2][0] * Ti.mat[2][0] +
	  Ti.mat[2][1] * Ti.mat[2][1] + Ti.mat[2][2] * Ti.mat[2][2] +
	  Th_i.mat[1][0] * Th_i.mat[1][0] + Th_i.mat[1][1] * Th_i.mat[1][1] +
	  Th_i.mat[1][2] * Th_i.mat[1][2] + 2.0 * Ti.mat[0][3] * ai -
	  2.0 * calphai * Ti.mat[2][0] * Th_i.mat[2][0] -
	  2.0 * calphai * Ti.mat[2][2] * Th_i.mat[2][2] -
	  2.0 * salphai * Ti.mat[1][3] * Th_i.mat[2][3] -
	  2.0 * calphai * Ti.mat[2][3] * Th_i.mat[2][3] -
	  2.0 * salphai * Ti.mat[1][0] * Th_i.mat[2][0] -
	  2.0 * salphai * Ti.mat[1][1] * Th_i.mat[2][1] -
	  2.0 * calphai * Ti.mat[2][1] * Th_i.mat[2][1] -
	  2.0 * salphai * Ti.mat[1][2] * Th_i.mat[2][2]);
}

double Rchain_hand::pdm (Rhmatrix Ti, Rhmatrix Th_i, double salphai, double calphai)
{
  return (-salphai * Ti.mat[1][3] - calphai * Ti.mat[2][3] + Th_i.mat[2][3]);
}






void Rchain_hand::settranform_Tworld (Rhmatrix T_world)
{
   Tworld = T_world;

   //update with scaled value

   for (int i=0;i<n_chain;i++)
   {
   pT[i][0].mat[0][3] = T_world.mat[0][3]*pS[i];
   pT[i][0].mat[1][3] = T_world.mat[1][3]*pS[i];
   pT[i][0].mat[2][3] = T_world.mat[2][3]*pS[i];

   pT[i][0].mat[0][2] = T_world.mat[0][2];
   pT[i][0].mat[1][2] = T_world.mat[1][2];
   pT[i][0].mat[2][2] = T_world.mat[2][2];

   pT[i][0].mat[0][1] = T_world.mat[0][1];
   pT[i][0].mat[1][1] = T_world.mat[1][1];
   pT[i][0].mat[2][1] = T_world.mat[2][1];

   pT[i][0].mat[0][0] = T_world.mat[0][0];
   pT[i][0].mat[1][0] = T_world.mat[1][0];
   pT[i][0].mat[2][0] = T_world.mat[2][0];

   ppremult_T[i][0] = pT[i][0].value();
    // update de transpremult vector
   pupdate_T_premult (ppremult_T[i], pT[i], 1, i);

   // update de postmult vector
   pupdate_T_postmult (ppostmult_T[i], pT[i], dof[i]);
   }   
}
void Rchain_hand::settranform_Ttool (Rhmatrix T_tool, int k)
{
   Ttool[k] = T_tool;

   //update with scaled value

   pT[k][dof[k]+1].mat[0][3] = T_tool.mat[0][3]*pS[k];
   pT[k][dof[k]+1].mat[1][3] = T_tool.mat[1][3]*pS[k];
   pT[k][dof[k]+1].mat[2][3] = T_tool.mat[2][3]*pS[k];

   pT[k][dof[k]+1].mat[0][2] = T_tool.mat[0][2];
   pT[k][dof[k]+1].mat[1][2] = T_tool.mat[1][2];
   pT[k][dof[k]+1].mat[2][2] = T_tool.mat[2][2];

   pT[k][dof[k]+1].mat[0][1] = T_tool.mat[0][1];
   pT[k][dof[k]+1].mat[1][1] = T_tool.mat[1][1];
   pT[k][dof[k]+1].mat[2][1] = T_tool.mat[2][1];

   pT[k][dof[k]+1].mat[0][0] = T_tool.mat[0][0];
   pT[k][dof[k]+1].mat[1][0] = T_tool.mat[1][0];
   pT[k][dof[k]+1].mat[2][0] = T_tool.mat[2][0];


   ppostmult_T[k][dof[k] + 1] = pT[k][dof[k] + 1].value();

    // update de transpremult vector
   pupdate_T_premult (ppremult_T[k], pT[k], dof[k], k);

   // update de postmult vector
   pupdate_T_postmult (ppostmult_T[k], pT[k], 1);

}

double Rchain_hand::pdistance (Rhmatrix Ttcp, Rhmatrix Th)
{
   return(
         //orientation part
      .5 *(pow(-Ttcp.mat[0][0]+Th.mat[0][0],2.0)
         + pow(-Ttcp.mat[0][1]+Th.mat[0][1],2.0)
         + pow(-Ttcp.mat[0][2]+Th.mat[0][2],2.0)
         + pow(-Ttcp.mat[1][0]+Th.mat[1][0],2.0)
         + pow(-Ttcp.mat[1][1]+Th.mat[1][1],2.0)
         + pow(-Ttcp.mat[1][2]+Th.mat[1][2],2.0)
         + pow(-Ttcp.mat[2][0]+Th.mat[2][0],2.0)
         + pow(-Ttcp.mat[2][1]+Th.mat[2][1],2.0)
         + pow(-Ttcp.mat[2][2]+Th.mat[2][2],2.0)
         )// translation part
         + pow(-Ttcp.mat[0][3]+Th.mat[0][3],2.0)
         + pow(-Ttcp.mat[1][3]+Th.mat[1][3],2.0)
         + pow(-Ttcp.mat[2][3]+Th.mat[2][3],2.0));

}


void Rchain_hand::setValue(int i, double val)
{
   if (i >=1 && i<=6)
   {
   if (val != status_join[0][i])
   {
      if ((dh_parameters[0][i].lower_value <= val) && (val <= dh_parameters[0][i].upper_value))
      {
            pupdate_join_arm (i, val);
      }
      else
      {
         cout << "Join out of range " << endl;
      }
   }
   }
   if (i>=7 && i<=10)
   {
   if (val != status_join[0][i])
   {
      if ((dh_parameters[0][i].lower_value <= val) && (val <= dh_parameters[0][i].upper_value))
      {
            pupdate_join (i, val,0);
      }
      else
      {
         cout << "Join out of range " << endl;
      }
   }
   }
   if (i>=11 && i<=14)
   {
   if (val != status_join[1][i-4])
   {
      if ((dh_parameters[1][i-4].lower_value <= val) && (val <= dh_parameters[1][i-4].upper_value))
      {
            pupdate_join (i-4, val,1);
      }
      else
      {
         cout << "Join out of range " << endl;
      }
   }
   }
   if (i>=15 && i<=18)
   {
   if (val != status_join[2][i-8])
   {
      if ((dh_parameters[2][i-8].lower_value <= val) && (val <= dh_parameters[2][i-8].upper_value))
      {
            pupdate_join (i-8, val,2);
      }
      else
      {
         cout << "Join out of range " << endl;
      }
   }
   }
   if (i>=19 && i<=22)
   {
   if (val != status_join[3][i-12])
   {
      if ((dh_parameters[3][i-12].lower_value <= val) && (val <= dh_parameters[3][i-12].upper_value))
      {
            pupdate_join (i-12, val,3);
      }
      else
      {
         cout << "Join out of range " << endl;
      }
   }
   }
}


void Rchain_hand::move_join (int join, double value, int k)
{
  if (dh_parameters[k][join].sigma == 1)	// rotation
    value = rad (value);

  //if ((dh_parameters[k][join].lower_value <= value) && (value <= dh_parameters[k][join].upper_value))
    pupdate_join (join, value, k);
  //else
    //cout << "Join out of range " << endl;
}

void Rchain_hand::move_inc_join (int join, double value, int k)
{
   if (dh_parameters[k][join].sigma == 1) // rotation
   {
      value = status_join[k][join] + rad (value);
   }
   /*if ((dh_parameters[k][join].lower_value <= value) && (value <= dh_parameters[k][join].upper_value))
   {*/
      pupdate_join (join, value, k);
   /*}
   else
   {
      cout << "Join out of range " << endl;
   }*/
}


void Rchain_hand::limits(int join, double &min, double &max, int k)
{
   if(join<=dof[k])
   {
    min = dh_parameters[k][join+1].lower_value;
    max = dh_parameters[k][join+1].upper_value;
    }
}


void Rchain_hand::show_tcp(int k)
{

   tcp(k);

   /*for(int i=0;i<4;i++)
   {
      setMessage(QString::QString(ppremult_T[dof+1].row(i).c_str()));
   } */
   emit messageChanged("Rchain_hand ha anat a home");
}

void Rchain_hand::setMessage(QString message)
{
   message_out = message;
   emit messageChanged(message);
   console->insertLineAtEnd(message);

}

void Rchain_hand::printmatrix(Rhmatrix &matrix)
{
   std::ostringstream pcout;

   pcout << setiosflags (ios::fixed | ios::showpoint);
   pcout << endl;
   for (int i = 0; i < 4; i++)
   {
      for (int j = 0; j < 4; j++)
      {
         pcout << setw (10) << setprecision (5) << matrix.mat[i][j];
      }
      pcout << endl;
   }
   pcout << endl;
   send_stream(pcout);
}

void Rchain_hand::printmatrix(Rhmatrix &matrix, double scale)
{
   std::ostringstream pcout;

   pcout << setiosflags (ios::fixed | ios::showpoint);
   pcout << endl;

   for (int i = 0; i < 3; i++)
   {
      for (int j = 0; j < 3; j++)
      {
         pcout << setw (10) << setprecision (5) << matrix.mat[i][j];
      }
      pcout << setw (10) << setprecision (5) << scale * matrix.mat[i][3];
      pcout << endl;
   }
   for (int j = 0; j < 4; j++)
   {
      pcout << setw (10) << setprecision (5) << matrix.mat[3][j];
   }
   pcout << endl;
   send_stream(pcout);

}


/***********************************************************************
FUCIONS GENERALS PER AL CÀLCUL DE LA CINEMÀTICA INVERSA
***********************************************************************/


int Rchain_hand::kineinverse_hand(Rhmatrix A_hand[], double **q)
{
   std::ostringstream pcout;
   int iter_done;
   int piterations;
   double pepsilon;

   Rhtranslink **pkT;
   Rhmatrix **pkpremult_T;
   Rhmatrix **pkpostmult_T;

   piterations = 50000;
   pepsilon =  0.00000000001;
   stop_kinematics = FALSE;

   pkT = new Rhtranslink*[n_chain];
   pkpremult_T = new Rhmatrix*[n_chain];
   pkpostmult_T = new Rhmatrix*[n_chain];

   for(int k=0;k<n_chain;k++)
   {
   pkT[k] = new Rhtranslink[dof[k] + 2];
   pkpremult_T[k] = new Rhmatrix[dof[k] + 2];
   pkpostmult_T[k] = new Rhmatrix[dof[k] + 2];

   A_hand[k].mat[0][3]=A_hand[k].mat[0][3]*pS[k];
   A_hand[k].mat[1][3]=A_hand[k].mat[1][3]*pS[k];
   A_hand[k].mat[2][3]=A_hand[k].mat[2][3]*pS[k];

   // variables temporals, potser aniria més ràpid declarant-les globals a la clase
   for (int i=1;i<=dof[k];i++)
   {
      q[k][i] = status_join[k][i];
      pkT[k][i]= pT[k][i];
      pkpremult_T[k][i] = ppremult_T[k][i];
      pkpostmult_T[k][i] = ppostmult_T[k][i];
   }

   pkT[k][0]= pT[k][0];
   pkpremult_T[k][0] = ppremult_T[k][0];
   pkpostmult_T[k][0] = ppostmult_T[k][0];

   pkT[k][dof[k]+1]= pT[k][dof[k]+1];
   pkpremult_T[k][dof[k]+1] = ppremult_T[k][dof[k]+1];
   pkpostmult_T[k][dof[k]+1] = ppostmult_T[k][dof[k]+1];
   }

   if(n_chain==3 || n_chain==4)
   {

     iter_done = pkineinverse_iter_hand_1(q, pkpremult_T, pkT, pkpostmult_T, A_hand);
   }

   iter_done = pkineinverse_iter_hand_2(q, pkpremult_T, pkT, pkpostmult_T, A_hand,
                   piterations, pepsilon);

   for(int k=0;k<n_chain;k++)
   {
       if(q[k][13]>dh_parameters[k][13].upper_value)
       {
         q[k][13] = q[k][13] - M2PI;
       }
   }


   /*for(int k=0;k<n_chain;k++)
   {
   delete[] pkT[k];
   delete[] pkpremult_T[k];
   delete[] pkpostmult_T[k];
   }*/

  if(!(stop_kinematics))
  {
   if (iter_done==piterations)
   {
      pcout << "No solution found" << endl;
   }
   else
   {
      pcout << "El nombre d'iteracions és: " << iter_done << endl;
      pcout << "Solution found!" << endl;
      move_path_join (q, 0);
   }
  }
  else
  {
    pcout << "Inverse kinematics stopped" << endl;
  }
   send_stream (pcout);
   return iter_done;
}

//Calcula la cinemàtica inversa calculant les qi com la artangent de la suma dels quatre numeradors
//dividit per la suma dels quatre denominadors. En aquest cas, però, es realitza només una aproximació.
int Rchain_hand::pkineinverse_iter_hand_1(double **q, Rhmatrix *pkpremult_T[], Rhtranslink *pkT[],
                  Rhmatrix *pkpostmult_T[], Rhmatrix SA_hand[])
{
  // q[] must have the current values of the articulations

   int iter=0, i, j;
   int *last_move_arm;
   int *imax_arm;
   double Ftotal=20;
   double Fmin=50;
   double *Fmax;
   double **F, **Fprov;
   double **pq, **qprov, **qmin;  //status temporal de les articulacions
   Rhmatrix **pTh;
   double *Farm;

   imax_arm = new int[n_chain];
   last_move_arm = new int[n_chain];
   Fmax = new double[n_chain];
   F = new double*[n_chain];
   pq = new double*[n_chain];
   qmin = new double*[n_chain];
   Fprov = new double*[n_chain];
   qprov = new double*[n_chain];
   Farm = new double[n_same+1];
   pTh = new Rhmatrix*[n_chain];

   for(int k=0;k<n_chain;k++)
   {
   imax_arm[k] = 1;
   last_move_arm[k] = 1;
   Fmax[k] = 1.0;
   F[k] = new double[dof[k]+1];
   pq[k] = new double[dof[k]+1];
   qmin[k] = new double[dof[k]+1];
   Fprov[k] = new double[n_same+1];
   qprov[k] = new double[n_same+1];
   pTh[k] = new Rhmatrix[dof[k]+1];
   }

   for (int k=0;k<n_chain;k++)
   {
   for (i=1;i<=dof[k];i++)
   {
      pq[k][i] = q[k][i];
      pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
      pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
      pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
      q[k][i] = pq[k][i];
   }
   }

   for(int k=0;k<n_chain;k++)
   {
     for(i=n_same+1;i<=dof[k];i++)
     {
       F[k][i] = 0.0;
     }
   }


// Main Bucle to find the inverse
   while (Ftotal > 7.5 && iter < 3000 && !(stop_kinematics))
   {
      for(int k=0;k<n_chain;k++)
      {
      for(i=1;i<=n_same;i++)
      {
         pTh[k][i] = pkpremult_T[k][i-1].inverse() * SA_hand[k];
         pq[k][i] = q[k][i];
      }
      }

      for(i=1;i<=n_same;i++)
      {
        if(i>=1 && i<=5)
        {
         Farm[i] = pF_objective_arm(pTh, pkpostmult_T, pq, i);
        }
        else
        {
         Farm[i] = pF_objective_joint_six(pTh,pkpostmult_T, pq, i);
        }
      }



      for(int k=0;k<n_chain;k++)
      {
        for(i=1;i<=n_same;i++)
        {
          Fprov[k][i] = Farm[i]/n_chain;
          qprov[k][i] = pq[k][i];
        }
      }
      for(int k=0;k<n_chain;k++)
      {
        for(i=1;i<=n_same;i++)
        {
          pq[k][i] = qprov[k][i];
          F[k][i] = Fprov[k][i];
        }
        imax_arm[k]=maxvector(Fprov[k],n_same);
      }


      if((imax_arm[0]==last_move_arm[0]))
      {
         for(int k=0;k<n_chain;k++) //Bucle fictici per provar si funciona el programa
         {
         for(i=1;i<=n_same;i++)
         {
            pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
            pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
            pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
            q[k][i] = pq[k][i];
         }
         }
         for(int k=0;k<n_chain;k++)
         {
         for(i=1;i<=n_same;i++)
         {
            pTh[k][i] = pkpremult_T[k][i-1].inverse() * SA_hand[k];
            pq[k][i] = q[k][i];
         }
         }

         for(i=1;i<=n_same;i++)
          {
            if(i>=1 && i<=5)
            {
              Farm[i] = pF_objective_arm(pTh, pkpostmult_T, pq, i);
            }
            else
            {
              Farm[i] = pF_objective_joint_six(pTh, pkpostmult_T, pq, i);
            }
          }
         for(int k=0;k<n_chain;k++)
          {
          for(i=1;i<=n_same;i++)
          {
          Fprov[k][i] = Farm[i]/n_chain;
          qprov[k][i] = pq[k][i];
          }
        }
        for(int k=0;k<n_chain;k++)
        {
        for(i=1;i<=n_same;i++)
        {
          pq[k][i] = qprov[k][i];
          F[k][i] = Fprov[k][i];
        }
        imax_arm[k]=maxvector(Fprov[k],n_same);
        }

        for(int k=0;k<n_chain;k++)
        {
          for(i=1;i<=n_same;i++)
          {
            pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
            pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
            pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
            q[k][i] = pq[k][i];
          }
         last_move_arm[k] = imax_arm[k];

        }
      }
      else
      {
        for(int k=0;k<n_chain;k++)
        {
         pkT[k][imax_arm[k]].update(int(dh_parameters[k][imax_arm[k]].sigma), pq[k][imax_arm[k]]);
         pupdate_T_premult(pkpremult_T[k], pkT[k], imax_arm[k], k);
         pupdate_T_postmult(pkpostmult_T[k], pkT[k], imax_arm[k]);
         q[k][imax_arm[k]] = pq[k][imax_arm[k]];

         last_move_arm[k] = imax_arm[k];
        }
      }

      for(int k=0;k<n_chain;k++)
      {
        Fmax[k] = F[k][imax_arm[k]];
      }

      Ftotal = 0;
      for(int k=0;k<n_chain;k++)
      {
        Ftotal = Ftotal + Fmax[k];
      }
      Ftotal = Ftotal/n_chain;

      if(Ftotal<Fmin)
      {
        Fmin = Ftotal;
        for(int k=0;k<n_chain;k++)
        {
          for(j=1;j<=dof[k];j++)
          {
            qmin[k][j] = q[k][j];
          }
        }
      }

      iter++;
      }

      if(iter==3000)
      {
        for(int k=0;k<n_chain;k++)
        {
          for(j=1;j<=dof[k];j++)
          {
            q[k][j] = qmin[k][j];
          }
        }
      }
   for(int k=0;k<n_chain;k++)
   {
   delete[] F[k];
   delete[] pq[k];
   delete[] Fprov[k];
   delete[] qprov[k];
   delete[] qmin[k];
   delete[] pTh[k];
   }
   delete[] last_move_arm;
   delete[] imax_arm;
   delete[] Fmax;
   delete[] Farm;
   return iter;

}


//Calcula la cinemàtica inversa calculant les qi com la artangent de la suma dels quatre numeradors
//dividit per la suma dels quatre denominadors
int Rchain_hand::pkineinverse_iter_hand_2(double **q, Rhmatrix *pkpremult_T[], Rhtranslink *pkT[],
                  Rhmatrix *pkpostmult_T[], Rhmatrix SA_hand[], int iterations, double pepsilon)
{
  // q[] must have the current values of the articulations

   int iter=1, i;
   int *last_move_arm, *last_move_finger;
   int *imax_arm, *imax_finger;
   double Ftotal=1;
   double *Fmax;
   double **F, **Fprov, **Freduit;
   double **pq, **qprov;  //status temporal de les articulacions
   Rhmatrix **pTh;
   double *Farm;
   std::ostringstream pcout;

   imax_arm = new int[n_chain];
   imax_finger = new int[n_chain];
   last_move_arm = new int[n_chain];
   last_move_finger = new int[n_chain];
   Fmax = new double[n_chain];
   F = new double*[n_chain];
   pq = new double*[n_chain];
   Fprov = new double*[n_chain];
   qprov = new double*[n_chain];
   Freduit = new double*[n_chain];
   Farm = new double[n_same+1];
   pTh = new Rhmatrix*[n_chain];

   for(int k=0;k<n_chain;k++)
   {
   imax_arm[k] = 1;
   imax_finger[k] = 1;
   last_move_arm[k] = 1;
   last_move_finger[k] = 1;
   Fmax[k] = 1.0;
   F[k] = new double[dof[k]+1];
   pq[k] = new double[dof[k]+1];
   Fprov[k] = new double[n_same+1];
   qprov[k] = new double[n_same+1];
   Freduit[k] = new double[dof[k]+1-n_same];
   pTh[k] = new Rhmatrix[dof[k]+1];
   }

   for (int k=0;k<n_chain;k++)
   {
   for (i=1;i<=dof[k];i++)
   {
      pq[k][i] = q[k][i];
      pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
      pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
      pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
      q[k][i] = pq[k][i];
   }
   }


// Main Bucle to find the inverse
   while (Ftotal > pepsilon && iter < iterations && !(stop_kinematics))
   {
     if((iter%3)==0)
     {
      for(int k=0;k<n_chain;k++)
      {
      for(i=1;i<=n_same;i++)
      {
         pTh[k][i] = pkpremult_T[k][i-1].inverse() * SA_hand[k];
         pq[k][i] = q[k][i];
      }
      }

      for(i=1;i<=n_same;i++)
      {
        if(i>=1 && i<=5)
        {
         /*Farm[i] = pF_objective_arm_without_ranks(pTh, pkpostmult_T, pq, i);
        }
        if(i==5)
        {*/
          Farm[i] = pF_objective_arm(pTh, pkpostmult_T, pq, i);
        }
        if(i==6)
        {
          Farm[i] = pF_objective_joint_six(pTh, pkpostmult_T, pq, i);
        }
      }

      for(int k=0;k<n_chain;k++)
      {
        for(i=1;i<=n_same;i++)
        {
          Fprov[k][i] = Farm[i]/n_chain;
          qprov[k][i] = pq[k][i];
        }
      }
      for(int k=0;k<n_chain;k++)
      {
        for(i=1;i<=n_same;i++)
        {
          pq[k][i] = qprov[k][i];
          F[k][i] = Fprov[k][i];
        }
        imax_arm[k]=maxvector(Fprov[k],n_same);
      }


      if((imax_arm[0]==last_move_arm[0]))
      {
         for(int k=0;k<n_chain;k++) //Bucle fictici per provar si funciona el programa
         {
         for(i=1;i<=n_same;i++)
         {
            pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
            pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
            pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
            q[k][i] = pq[k][i];
         }
         }
         for(int k=0;k<n_chain;k++)
         {
         for(i=1;i<=n_same;i++)
         {
            pTh[k][i] = pkpremult_T[k][i-1].inverse() * SA_hand[k];
            pq[k][i] = q[k][i];
         }
         }

         for(i=1;i<=n_same;i++)
          {
            if(i>=1 && i<=5)
            {
              /*Farm[i] = pF_objective_arm_without_ranks(pTh, pkpostmult_T, pq, i);
            }
            if(i==5)
            {*/
              Farm[i] = pF_objective_arm(pTh, pkpostmult_T, pq, i);
            }
            if(i==6)
            {
              Farm[i] = pF_objective_joint_six(pTh, pkpostmult_T, pq, i);
            }
          }
         for(int k=0;k<n_chain;k++)
          {
          for(i=1;i<=n_same;i++)
          {
          Fprov[k][i] = Farm[i]/n_chain;
          qprov[k][i] = pq[k][i];
          }
        }
        for(int k=0;k<n_chain;k++)
        {
        for(i=1;i<=n_same;i++)
        {
          pq[k][i] = qprov[k][i];
          F[k][i] = Fprov[k][i];
        }
        imax_arm[k]=maxvector(Fprov[k],n_same);
        }

        for(int k=0;k<n_chain;k++)
        {
          for(i=1;i<=n_same;i++)
          {
            pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
            pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
            pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
            q[k][i] = pq[k][i];
          }
         last_move_arm[k] = imax_arm[k];

        }
      }
      else
      {
        for(int k=0;k<n_chain;k++)
        {
         pkT[k][imax_arm[k]].update(int(dh_parameters[k][imax_arm[k]].sigma), pq[k][imax_arm[k]]);
         pupdate_T_premult(pkpremult_T[k], pkT[k], imax_arm[k], k);
         pupdate_T_postmult(pkpostmult_T[k], pkT[k], imax_arm[k]);
         q[k][imax_arm[k]] = pq[k][imax_arm[k]];

         last_move_arm[k] = imax_arm[k];
        }
      }

      }//If del braç
      //Cinemàtica inversa dels dits
      
      for(int k=0;k<n_chain;k++)
      {
      for(i=n_same+1;i<=dof[k];i++)
      {
         pTh[k][i] = pkpremult_T[k][i-1].inverse() * SA_hand[k];
         pq[k][i] = q[k][i];
         if(i>=7 && i<=12)
         {
         F[k][i] = pF_objective(pkpostmult_T[k][i+1], pTh[k][i], pq[k][i], i, k);
         }
         else
         {
         F[k][i] = pF_objective_without_ranks(pkpostmult_T[k][i+1], pTh[k][i], pq[k][i], i, k);
         }

      }
      }

      for(int k=0;k<n_chain;k++)
      {
        for(i=n_same+1;i<=dof[k];i++)
        {
          Freduit[k][i-n_same] = F[k][i];
        }
        imax_finger[k] = maxvector(Freduit[k],dof[k]-n_same)+n_same;
      }

      for(int k=0;k<n_chain;k++) //Bucle fictici per provar si funciona el programa
      {
      if((imax_finger[k]==last_move_finger[k]))
      {
         for(i=n_same+1;i<=dof[k];i++)
         {
            pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
            pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
            pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
            q[k][i] = pq[k][i];
         }
         for(i=n_same+1;i<=dof[k];i++)
         {
            pTh[k][i] = pkpremult_T[k][i-1].inverse() * SA_hand[k];
            pq[k][i] = q[k][i];
            if(i>=7 && i<=12)
            {
            F[k][i] = pF_objective(pkpostmult_T[k][i+1], pTh[k][i], pq[k][i], i, k);
            }
            else
            {
            F[k][i] = pF_objective_without_ranks(pkpostmult_T[k][i+1], pTh[k][i], pq[k][i], i, k);
            }
            Freduit[k][i-1] = F[k][i];
            pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
            pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
            pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
            q[k][i] = pq[k][i];
         }
         imax_finger[k] = maxvector(Freduit[k],dof[k]-n_same)+n_same;
         last_move_finger[k] = imax_finger[k];

      }
      else
      {
         pkT[k][imax_finger[k]].update(int(dh_parameters[k][imax_finger[k]].sigma), pq[k][imax_finger[k]]);
         pupdate_T_premult(pkpremult_T[k], pkT[k], imax_finger[k], k);
         pupdate_T_postmult(pkpostmult_T[k], pkT[k], imax_finger[k]);
         q[k][imax_finger[k]] = pq[k][imax_finger[k]];
      }
      }

      for(int k=0;k<n_chain;k++)
      {
      if(F[k][imax_arm[k]]>=F[k][imax_finger[k]])
      {
        Fmax[k] = F[k][imax_arm[k]];
      }
      else
      {
        Fmax[k] = F[k][imax_finger[k]];
      }
      }
      Ftotal = 0;
      for(int k=0;k<n_chain;k++)
      {
        Ftotal = Ftotal + Fmax[k];
      }
      Ftotal = Ftotal/n_chain;

      iter++;
      }
      pcout << "F màxima: " << Ftotal << endl;
      send_stream(pcout);
     /* cout << Ftotal << endl;
      for(int k=0;k<n_chain;k++)
      {
        for(i=1;i<=dof[k];i++)
        {
          cout << q[k][i];
        }
      }
      cout << endl;*/

   for(int k=0;k<n_chain;k++)
   {
   delete[] F[k];
   delete[] pq[k];
   delete[] Fprov[k];
   delete[] qprov[k];
   delete[] pTh[k];
   delete[] Freduit[k];
   }
   delete[] last_move_arm;
   delete[] last_move_finger;
   delete[] imax_arm;
   delete[] imax_finger;
   delete[] Fmax;
   delete[] Farm;
   return iter;

}

int Rchain_hand::pkineinverse_iter(double *q[], Rhmatrix *pkpremult_T[], Rhtranslink *pkT[],
                  Rhmatrix *pkpostmult_T[], Rhmatrix SA_hand[], int iterations, double pepsilon)
{
   // q[] must have the current values of the articulations

   int iter=0, i;
   int *last_move;
   int *imax;

   double *Fmax, *last_Fmax;
   double **F;
   double **pq;  //status temporal de les articulacions
   Rhmatrix *pTh;

   imax = new int[n_chain];
   last_move = new int[n_chain];
   Fmax = new double[n_chain];
   last_Fmax = new double[n_chain];
   F = new double*[n_chain];
   pq = new double*[n_chain];

   for(int k=0;k<n_chain;k++)
   {
   imax[k] = 1;
   last_move[k] = 1;
   Fmax[k] = 1.0;
   F[k] = new double[dof[k]+1];
   pq[k] = new double[dof[k]+1];
   }

   pTh = new Rhmatrix[n_chain];

   for (int k=0;k<n_chain;k++)
   {
   for (i=1;i<=dof[k];i++)
   {
      pkT[k][i].update(int(dh_parameters[k][i].sigma),q[k][i]);
      pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
      pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
   }
   }

   for (int k=0;k<n_chain;k++)
   {
   for (i=1;i<=dof[k];i++)
   {
      pq[k][i] = q[k][i];
   }
   }

// Main Bucle to find the inverse
   for(int k=0;k<n_chain;k++)
   {
   while (Fmax[k] > pepsilon && iter < iterations)
   {
      for(int k=0;k<n_chain;k++)
      {
      for(i=1;i<=dof[k];i++)
      {
         pTh[k] = pkpremult_T[k][i-1].inverse() * SA_hand[k];
         pq[k][i] = q[k][i];
         F[k][i] = pF_objective(pkpostmult_T[k][i+1], pTh[k], pq[k][i], i, k);
      }
      imax[k]=maxvector(F[k],dof[k]);
      }

      for(int k=0;k<n_chain;k++) //Bucle fictici per provar si funciona el programa
      {
      if((imax[k]==last_move[k]))
      {
         for(i=1;i<=dof[k];i++)
         {
            pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
            pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
            pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
            q[k][i] = pq[k][i];
         }
         for(i=1;i<=dof[k];i++)
         {
            pTh[k] = pkpremult_T[k][i-1].inverse() * SA_hand[k];
            pq[k][i] = q[k][i];
            F[k][i] = pF_objective(pkpostmult_T[k][i+1], pTh[k], pq[k][i], i, k);
            pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
            pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
            pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
            q[k][i] = pq[k][i];
         }
         imax[k]=maxvector(F[k],dof[k]);
         last_move[k] = imax[k];
         last_Fmax[k] = Fmax[k];
         Fmax[k] = F[k][imax[k]];

      }
      else
      {
         pkT[k][imax[k]].update(int(dh_parameters[k][imax[k]].sigma), pq[k][imax[k]]);
         pupdate_T_premult(pkpremult_T[k], pkT[k], imax[k], k);
         pupdate_T_postmult(pkpostmult_T[k], pkT[k], imax[k]);
         q[k][imax[k]] = pq[k][imax[k]];
      }
      Fmax[k] = F[k][imax[k]];
      last_move[k] = imax[k];
      last_Fmax[k] = Fmax[k];
      }//Final del bucle fictici per provar si funciona el programa amb una cadena

      iter++;
      }
    }
   for(int k=0;k<n_chain;k++)
   {
   delete[] F[k];
   delete[] pq[k];
   }
   delete[] last_move;
   delete[] imax;
   delete[] Fmax;
   delete[] last_Fmax;
   delete[] pTh;
   return iter;

}

int Rchain_hand::pkineinverse_iter_without_ranks(double *q[], Rhmatrix *pkpremult_T[], Rhtranslink *pkT[],
                  Rhmatrix *pkpostmult_T[], Rhmatrix SA_hand[], int iterations, double pepsilon)
{
   // q[] must have the current values of the articulations

   int iter=0, i;
   int *last_move;
   int *imax;

   double *Fmax, *last_Fmax;
   double **F;
   double **pq;  //status temporal de les articulacions
   Rhmatrix *pTh;

   imax = new int[n_chain];
   last_move = new int[n_chain];
   Fmax = new double[n_chain];
   last_Fmax = new double[n_chain];
   F = new double*[n_chain];
   pq = new double*[n_chain];

   for(int k=0;k<n_chain;k++)
   {
   imax[k] = 1;
   last_move[k] = 1;
   Fmax[k] = 1.0;
   F[k] = new double[dof[k]+1];
   pq[k] = new double[dof[k]+1];
   }

   pTh = new Rhmatrix[n_chain];

   for (int k=0;k<n_chain;k++)
   {
   for (i=1;i<=dof[k];i++)
   {
      pkT[k][i].update(int(dh_parameters[k][i].sigma),q[k][i]);
      pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
      pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
   }
   }

   for (int k=0;k<n_chain;k++)
   {
   for (i=1;i<=dof[k];i++)
   {
      pq[k][i] = q[k][i];
   }
   }

// Main Bucle to find the inverse
   for(int k=0;k<n_chain;k++)
   {
   while (Fmax[k] > pepsilon && iter < iterations)
   {
      for(int k=0;k<n_chain;k++)
      {
      for(i=1;i<=dof[k];i++)
      {
         pTh[k] = pkpremult_T[k][i-1].inverse() * SA_hand[k];
         pq[k][i] = q[k][i];
         F[k][i] = pF_objective_without_ranks(pkpostmult_T[k][i+1], pTh[k], pq[k][i], i, k);
      }
      imax[k]=maxvector(F[k],dof[k]);
      }

      for(int k=0;k<n_chain;k++) //Bucle fictici per provar si funciona el programa
      {
      if((imax[k]==last_move[k]))
      {
         for(i=1;i<=dof[k];i++)
         {
            pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
            pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
            pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
            q[k][i] = pq[k][i];
         }
         for(i=1;i<=dof[k];i++)
         {
            pTh[k] = pkpremult_T[k][i-1].inverse() * SA_hand[k];
            pq[k][i] = q[k][i];
            F[k][i] = pF_objective_without_ranks(pkpostmult_T[k][i+1], pTh[k], pq[k][i], i, k);
            pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
            pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
            pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
            q[k][i] = pq[k][i];
         }
         imax[k]=maxvector(F[k],dof[k]);
         last_move[k] = imax[k];
         last_Fmax[k] = Fmax[k];
         Fmax[k] = F[k][imax[k]];

      }
      else
      {
         pkT[k][imax[k]].update(int(dh_parameters[k][imax[k]].sigma), pq[k][imax[k]]);
         pupdate_T_premult(pkpremult_T[k], pkT[k], imax[k], k);
         pupdate_T_postmult(pkpostmult_T[k], pkT[k], imax[k]);
         q[k][imax[k]] = pq[k][imax[k]];
      }
      Fmax[k] = F[k][imax[k]];
      last_move[k] = imax[k];
      last_Fmax[k] = Fmax[k];
      }//Final del bucle fictici per provar si funciona el programa amb una cadena

      /*cout << iter;
      cout << " ";
      //cout << "Les F's són:" << endl;
      for(j=1;j<=dof[0];j++)
      {
        cout <<  F[0][j];
        cout << " ";
      }
      //cout << endl;
      //cout << "Les q's són:" << endl;
      for(j=1;j<=dof[0];j++)
      {
        cout <<  q[0][j];
        cout << " ";
      }
      cout << endl;*/
      //cout << "------------------------------------------------------------------------" << endl;
      iter++;
      }
    }
   for(int k=0;k<n_chain;k++)
   {
   delete[] F[k];
   delete[] pq[k];
   }
   delete[] last_move;
   delete[] imax;
   delete[] Fmax;
   delete[] last_Fmax;
   delete[] pTh;
   return iter;

}

void Rchain_hand::slot_stop_kinematics(bool stop)
{
  stop_kinematics = stop;
}

int Rchain_hand::kineinverse_hand_retry(Rhmatrix A_hand[], double **q)
{
   std::ostringstream pcout;
   int iter_done;
   int piterations;
   double pepsilon;

   Rhtranslink **pkT;
   Rhmatrix **pkpremult_T;
   Rhmatrix **pkpostmult_T;

   piterations = 50000;
   pepsilon =  0.00000000001;
   stop_kinematics = FALSE;

   pkT = new Rhtranslink*[n_chain];
   pkpremult_T = new Rhmatrix*[n_chain];
   pkpostmult_T = new Rhmatrix*[n_chain];

   for(int k=0;k<n_chain;k++)
   {
   pkT[k] = new Rhtranslink[dof[k] + 2];
   pkpremult_T[k] = new Rhmatrix[dof[k] + 2];
   pkpostmult_T[k] = new Rhmatrix[dof[k] + 2];

   A_hand[k].mat[0][3]=A_hand[k].mat[0][3]*pS[k];
   A_hand[k].mat[1][3]=A_hand[k].mat[1][3]*pS[k];
   A_hand[k].mat[2][3]=A_hand[k].mat[2][3]*pS[k];

   // variables temporals, potser aniria més ràpid declarant-les globals a la clase
   for (int i=1;i<=dof[k];i++)
   {
      q[k][i] = status_join[k][i];
      if (i==6)
      {
       q[k][i] = q[k][i] - delta[k];
       }
      pkT[k][i]= pT[k][i];
      pkpremult_T[k][i] = ppremult_T[k][i];
      pkpostmult_T[k][i] = ppostmult_T[k][i];
   }

   pkT[k][0]= pT[k][0];
   pkpremult_T[k][0] = ppremult_T[k][0];
   pkpostmult_T[k][0] = ppostmult_T[k][0];

   pkT[k][dof[k]+1]= pT[k][dof[k]+1];
   pkpremult_T[k][dof[k]+1] = ppremult_T[k][dof[k]+1];
   pkpostmult_T[k][dof[k]+1] = ppostmult_T[k][dof[k]+1];
   }

   if(n_chain==3 || n_chain==4)
   {

     iter_done = pkineinverse_iter_hand_1(q, pkpremult_T, pkT, pkpostmult_T, A_hand);
   }

   iter_done = pkineinverse_iter_hand_2(q, pkpremult_T, pkT, pkpostmult_T, A_hand,
                   piterations, pepsilon);

   for(int k=0;k<n_chain;k++)
   {
       if(q[k][13]>dh_parameters[k][13].upper_value)
       {
         q[k][13] = q[k][13] - M2PI;
       }
   }


   /*for(int k=0;k<n_chain;k++)
   {
   delete[] pkT[k];
   delete[] pkpremult_T[k];
   delete[] pkpostmult_T[k];
   }*/

  if(!(stop_kinematics))
  {
   if (iter_done==piterations)
   {
      pcout << "No solution found" << endl;
   }
   else
   {
      pcout << "El nombre d'iteracions és: " << iter_done << endl;
      pcout << "Solution found!" << endl;
      move_path_join (q, 0);
   }
  }
  else
  {
    pcout << "Inverse kinematics stopped" << endl;
  }
   send_stream (pcout);
   return iter_done;
}

/* The inverse function. Returns 0 if position not allowed
   Returns 1 if position is allowed and q with the values of the joins  */
 /* Versió amb variables temporals
 funciona força bé, ja que només actualitza una articulació a l'hora,
 la que té la funció objectiu pitjor */


int Rchain_hand::kineinverse(Rhmatrix A_hand, double *q, int k)
{
   std::ostringstream pcout;
   int iter_done=0;
   int piterations;
   double pepsilon;
   double **pq;

   Rhtranslink *pkT;
   Rhmatrix *pkpremult_T;
   Rhmatrix *pkpostmult_T;
   stop_kinematics = FALSE;

   pq = new double*[n_chain];
   for (int j=0; j<n_chain; j++)
   {
    pq[j] = new double[dof[j]+1];
   }
   for (int j=0; j<n_chain;j++)
   {
    pq[j][0] = 0.0;
   }

   piterations = 50000;
   pepsilon =  0.000000001;

   pkT = new Rhtranslink[dof[k] + 2];
   pkpremult_T = new Rhmatrix[dof[k] + 2];
   pkpostmult_T = new Rhmatrix[dof[k] + 2];

   A_hand.mat[0][3]=A_hand.mat[0][3]*pS[k];
   A_hand.mat[1][3]=A_hand.mat[1][3]*pS[k];
   A_hand.mat[2][3]=A_hand.mat[2][3]*pS[k];

   // variables temporals, potser aniria més ràpid declarant-les globals a la clase

   for (int i=1;i<=dof[k];i++)
   {
      //q[i] = status_join[k][i];
      pkT[i]= pT[k][i];
      pkpremult_T[i] = ppremult_T[k][i];
      pkpostmult_T[i] = ppostmult_T[k][i];
   }

   pkT[0]= pT[k][0];
   pkpremult_T[0] = ppremult_T[k][0];
   pkpostmult_T[0] = ppostmult_T[k][0];

   pkT[dof[k]+1]= pT[k][dof[k]+1];
   pkpremult_T[dof[k]+1] = ppremult_T[k][dof[k]+1];
   pkpostmult_T[dof[k]+1] = ppostmult_T[k][dof[k]+1];

   iter_done = pkineinverse_iter_one_chain(q, pkpremult_T, pkT, pkpostmult_T, A_hand,
                   piterations, pepsilon,k);

   for (int j=0; j<n_chain; j++)
   {
    for (int i=0; i<=n_same; i++)
    {
     pq[j][i] = q[i];
    }
   }

   if (k==0)
   {
   for (int i=n_same+1; i<=dof[k]; i++)
   {
    pq[0][i] = q[i];
    pq[1][i] = status_join[1][i];
    pq[2][i] = status_join[2][i];
    pq[3][i] = status_join[3][i];
   }
   }
   if (k==1)
   {
   for (int i=n_same+1; i<=dof[k]; i++)
   {
    pq[0][i] = status_join[0][i];
    pq[1][i] = q[i];
    pq[2][i] = status_join[2][i];
    pq[3][i] = status_join[3][i];
   }
   }
   if (k==2)
   {
   for (int i=n_same+1; i<=dof[k]; i++)
   {
    pq[0][i] = status_join[0][i];
    pq[1][i] = status_join[1][i];
    pq[2][i] = q[i];
    pq[3][i] = status_join[3][i];
   }
   }
   if (k==3)
   {
    pq[0][6] = q[6]+delta[3];
    pq[1][6] = q[6]+delta[3];
    pq[2][6] = q[6]+delta[3];
    pq[3][6] = q[6]+delta[3];
   for (int i=n_same+1; i<=dof[k]; i++)
   {
    pq[0][i] = status_join[0][i];
    pq[1][i] = status_join[1][i];
    pq[2][i] = status_join[2][i];
    pq[3][i] = q[i];
   }
   }

   if(!(stop_kinematics))
  {
   if (iter_done==piterations)
   {
      pcout << "No solution found" << endl;
   }
   else
   {
      pcout << "El nombre d'iteracions és: " << iter_done << endl;
      pcout << "Solution found!" << endl;
      move_path_join (pq, 0);
   }
  }
  else
  {
    pcout << "Inverse kinematics stopped" << endl;
  }
   send_stream (pcout);
   return iter_done;

}

int Rchain_hand::pkineinverse_iter_one_chain(double *q, Rhmatrix *pkpremult_T, Rhtranslink *pkT, Rhmatrix *pkpostmult_T, Rhmatrix SA_hand, int piterations, double pepsilon, int k)
{
   int iter=0, i;
   int last_move;
   int imax;

   double Fmax, last_Fmax;
   double *F;
   double *pq;  //status temporal de les articulacions
   Rhmatrix pTh;

   imax = 1;
   last_move = 1;
   Fmax = 1.0;
   F = new double[dof[k]+1];
   pq = new double[dof[k]+1];


   for (i=1;i<=dof[k];i++)
   {
      pq[i] = q[i];
   }


// Main Bucle to find the inverse
     for(i=1;i<=dof[k];i++)
     {
      pkT[i].update(int(dh_parameters[k][i].sigma),pq[i]);
      pupdate_T_premult(pkpremult_T, pkT, i, k);
      pupdate_T_postmult(pkpostmult_T, pkT, i);
      q[i] = pq[i];
     }
   while (Fmax > pepsilon && iter < piterations)
   {

      for(i=1;i<=dof[k];i++)
      {
         pTh = pkpremult_T[i-1].inverse() * SA_hand;
         pq[i] = q[i];
         F[i] = pF_objective(pkpostmult_T[i+1], pTh, pq[i], i, k);
      }
      imax=maxvector(F,dof[k]);


      if((imax==last_move))
      {
         for(i=1;i<=dof[k];i++)
         {
            pkT[i].update(int(dh_parameters[k][i].sigma),pq[i]);
            pupdate_T_premult(pkpremult_T, pkT, i, k);
            pupdate_T_postmult(pkpostmult_T, pkT, i);
            q[i] = pq[i];
         }
         for(i=1;i<=dof[k];i++)
         {
            pTh = pkpremult_T[i-1].inverse() * SA_hand;
            pq[i] = q[i];
            F[i] = pF_objective(pkpostmult_T[i+1], pTh, pq[i], i, k);
            pkT[i].update(int(dh_parameters[k][i].sigma),pq[i]);
            pupdate_T_premult(pkpremult_T, pkT, i, k);
            pupdate_T_postmult(pkpostmult_T, pkT, i);
            q[i] = pq[i];
         }
         imax=maxvector(F,dof[k]);
         last_move = imax;
         last_Fmax = Fmax;
         Fmax = F[imax];

      }
      else
      {
         pkT[imax].update(int(dh_parameters[k][imax].sigma), pq[imax]);
         pupdate_T_premult(pkpremult_T, pkT, imax, k);
         pupdate_T_postmult(pkpostmult_T, pkT, imax);
         q[imax] = pq[imax];
      }
      Fmax = F[imax];
      last_move = imax;
      last_Fmax = Fmax;

      iter++;
      }

   delete[] F;
   delete[] pq;

   return iter;

}

int Rchain_hand::kineinverse_two_chains(Rhmatrix A_hand[], double **q, bool finger1, bool finger2, bool finger3, bool finger4, int chains_selected)
{
   std::ostringstream pcout;
   int iter_done;
   int piterations;
   double pepsilon;

   Rhtranslink **pkT;
   Rhmatrix **pkpremult_T;
   Rhmatrix **pkpostmult_T;

   piterations = 50000;
   pepsilon =  0.00000000001;
   stop_kinematics = FALSE;

   pkT = new Rhtranslink*[n_chain];
   pkpremult_T = new Rhmatrix*[n_chain];
   pkpostmult_T = new Rhmatrix*[n_chain];

   for(int k=0;k<n_chain;k++)
   {
   pkT[k] = new Rhtranslink[dof[k] + 2];
   pkpremult_T[k] = new Rhmatrix[dof[k] + 2];
   pkpostmult_T[k] = new Rhmatrix[dof[k] + 2];

   A_hand[k].mat[0][3]=A_hand[k].mat[0][3]*pS[k];
   A_hand[k].mat[1][3]=A_hand[k].mat[1][3]*pS[k];
   A_hand[k].mat[2][3]=A_hand[k].mat[2][3]*pS[k];

   // variables temporals, potser aniria més ràpid declarant-les globals a la clase
   for (int i=1;i<=dof[k];i++)
   {
      //q[k][i] = status_join[k][i];
      pkT[k][i]= pT[k][i];
      pkpremult_T[k][i] = ppremult_T[k][i];
      pkpostmult_T[k][i] = ppostmult_T[k][i];
   }

   pkT[k][0]= pT[k][0];
   pkpremult_T[k][0] = ppremult_T[k][0];
   pkpostmult_T[k][0] = ppostmult_T[k][0];

   pkT[k][dof[k]+1]= pT[k][dof[k]+1];
   pkpremult_T[k][dof[k]+1] = ppremult_T[k][dof[k]+1];
   pkpostmult_T[k][dof[k]+1] = ppostmult_T[k][dof[k]+1];
   }

   if(chains_selected==3 || chains_selected==4)
   {

     iter_done = pkineinverse_iter_three_chains(q, pkpremult_T, pkT, pkpostmult_T, A_hand,
                  finger1, finger2, finger3, finger4);
   }

   iter_done = pkineinverse_iter_two_chains(q, pkpremult_T, pkT, pkpostmult_T, A_hand,
                   piterations, pepsilon, finger1, finger2, finger3, finger4);

   for(int k=0;k<n_chain;k++)
   {
       if(q[k][13]>dh_parameters[k][13].upper_value)
       {
         q[k][13] = q[k][13] - M2PI;
       }
   }


   /*for(int k=0;k<n_chain;k++)
   {
   delete[] pkT[k];
   delete[] pkpremult_T[k];
   delete[] pkpostmult_T[k];
   }*/
  if (finger1)
  {
   for (int i=1; i<=n_same; i++)
   {
    q[1][i] = q[0][i];
    q[2][i] = q[0][i];
    q[3][i] = q[0][i];
   }
  }
  if (finger2)
  {
   for (int i=1; i<=n_same; i++)
   {
    q[0][i] = q[1][i];
    q[2][i] = q[1][i];
    q[3][i] = q[1][i];
   }
  }
  if (finger3)
  {
   for (int i=1; i<=n_same; i++)
   {
    q[0][i] = q[2][i];
    q[1][i] = q[2][i];
    q[3][i] = q[2][i];
   }
  }


  if(!(stop_kinematics))
  {
   if (iter_done==piterations)
   {
      pcout << "No solution found" << endl;
   }
   else
   {
      pcout << "El nombre d'iteracions és: " << iter_done << endl;
      pcout << "Solution found!" << endl;
      move_path_join (q, 0);
   }
  }
  else
  {
    pcout << "Inverse kinematics stopped" << endl;
  }
   send_stream (pcout);
   return iter_done;
}

int Rchain_hand::pkineinverse_iter_three_chains(double **q, Rhmatrix *pkpremult_T[], Rhtranslink *pkT[],
                  Rhmatrix *pkpostmult_T[], Rhmatrix SA_hand[], bool finger1, bool finger2, bool finger3, bool finger4)
{
  // q[] must have the current values of the articulations

   int iter=0, i, j;
   int *last_move_arm;
   int *imax_arm;
   double Ftotal=20;
   double Fmin=50;
   double *Fmax;
   double **F, **Fprov;
   double **pq, **qprov, **qmin;  //status temporal de les articulacions
   Rhmatrix **pTh;
   double *Farm;

   imax_arm = new int[n_chain];
   last_move_arm = new int[n_chain];
   Fmax = new double[n_chain];
   F = new double*[n_chain];
   pq = new double*[n_chain];
   qmin = new double*[n_chain];
   Fprov = new double*[n_chain];
   qprov = new double*[n_chain];
   Farm = new double[n_same+1];
   pTh = new Rhmatrix*[n_chain];

   for(int k=0;k<n_chain;k++)
   {
   imax_arm[k] = 1;
   last_move_arm[k] = 2;
   Fmax[k] = 1.0;
   F[k] = new double[dof[k]+1];
   pq[k] = new double[dof[k]+1];
   qmin[k] = new double[dof[k]+1];
   Fprov[k] = new double[n_same+1];
   qprov[k] = new double[n_same+1];
   pTh[k] = new Rhmatrix[dof[k]+1];
   }

   for (int k=0;k<n_chain;k++)
   {
   if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
   {
   for (i=1;i<=dof[k];i++)
   {
      pq[k][i] = q[k][i];
      pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
      pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
      pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
      q[k][i] = pq[k][i];
   }
   }
   }

   for(int k=0;k<n_chain;k++)
   {
   if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
   {
     for(i=n_same+1;i<=dof[k];i++)
     {
       F[k][i] = 0.0;
     }
   }
   }


// Main Bucle to find the inverse
   while (Ftotal > 7.5 && iter < 3000 && !(stop_kinematics))
   {
      for(int k=0;k<n_chain;k++)
      {
      if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
      {
      for(i=1;i<=n_same;i++)
      {
         pTh[k][i] = pkpremult_T[k][i-1].inverse() * SA_hand[k];
         pq[k][i] = q[k][i];
      }
      }
      }

      for(i=1;i<=n_same;i++)
      {
        if(i>=1 && i<=5)
        {
         Farm[i] = pF_objective_arm(pTh, pkpostmult_T, pq, i);
        }
        else
        {
         Farm[i] = pF_objective_joint_six(pTh,pkpostmult_T, pq, i);
        }
      }



      for(int k=0;k<n_chain;k++)
      {
      if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
      {
        for(i=1;i<=n_same;i++)
        {
          Fprov[k][i] = Farm[i]/n_chain;
          qprov[k][i] = pq[k][i];
        }
      }
      }
      for(int k=0;k<n_chain;k++)
      {
      if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
      {
        for(i=1;i<=n_same;i++)
        {
          pq[k][i] = qprov[k][i];
          F[k][i] = Fprov[k][i];
        }
        imax_arm[k]=maxvector(Fprov[k],n_same);
      }
      }


      if((imax_arm[0]==last_move_arm[0]) || (imax_arm[1]==last_move_arm[1]) || (imax_arm[2]==last_move_arm[2]))
      {
         for(int k=0;k<n_chain;k++) //Bucle fictici per provar si funciona el programa
         {
	 if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
         {
         for(i=1;i<=n_same;i++)
         {
            pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
            pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
            pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
            q[k][i] = pq[k][i];
         }
	 }
         }
         for(int k=0;k<n_chain;k++)
         {
	 if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
         {
         for(i=1;i<=n_same;i++)
         {
            pTh[k][i] = pkpremult_T[k][i-1].inverse() * SA_hand[k];
            pq[k][i] = q[k][i];
         }
	 }
         }

         for(i=1;i<=n_same;i++)
          {
            if(i>=1 && i<=5)
            {
              Farm[i] = pF_objective_arm(pTh, pkpostmult_T, pq, i);
            }
            else
            {
              Farm[i] = pF_objective_joint_six(pTh, pkpostmult_T, pq, i);
            }
          }
         for(int k=0;k<n_chain;k++)
          {
	  if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
          {
          for(i=1;i<=n_same;i++)
          {
          Fprov[k][i] = Farm[i]/n_chain;
          qprov[k][i] = pq[k][i];
          }
	  }
        }
        for(int k=0;k<n_chain;k++)
        {
	if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
        {
        for(i=1;i<=n_same;i++)
        {
          pq[k][i] = qprov[k][i];
          F[k][i] = Fprov[k][i];
        }
        imax_arm[k]=maxvector(Fprov[k],n_same);
        }
	}

        for(int k=0;k<n_chain;k++)
        {
	if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
        {
          for(i=1;i<=n_same;i++)
          {
            pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
            pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
            pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
            q[k][i] = pq[k][i];
          }
         last_move_arm[k] = imax_arm[k];
        }
        }
      }
      else
      {
        for(int k=0;k<n_chain;k++)
        {
	if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
        {
         pkT[k][imax_arm[k]].update(int(dh_parameters[k][imax_arm[k]].sigma), pq[k][imax_arm[k]]);
         pupdate_T_premult(pkpremult_T[k], pkT[k], imax_arm[k], k);
         pupdate_T_postmult(pkpostmult_T[k], pkT[k], imax_arm[k]);
         q[k][imax_arm[k]] = pq[k][imax_arm[k]];

         last_move_arm[k] = imax_arm[k];
        }
	}
      }

      for(int k=0;k<n_chain;k++)
      {
      if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
      {
        Fmax[k] = F[k][imax_arm[k]];
      }
      }

      Ftotal = 0;
      for(int k=0;k<n_chain;k++)
      {
      if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
      {
        Ftotal = Ftotal + Fmax[k];
      }
      }
      Ftotal = Ftotal/n_chain;

      if(Ftotal<Fmin)
      {
        Fmin = Ftotal;
        for(int k=0;k<n_chain;k++)
        {
	if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
        {
          for(j=1;j<=dof[k];j++)
          {
            qmin[k][j] = q[k][j];
          }
        }
	}
      }

      iter++;
      }

      if(iter==3000)
      {
        for(int k=0;k<n_chain;k++)
        {
	if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
        {
          for(j=1;j<=dof[k];j++)
          {
            q[k][j] = qmin[k][j];
          }
        }
	}
      }
   for(int k=0;k<n_chain;k++)
   {
   delete[] F[k];
   delete[] pq[k];
   delete[] Fprov[k];
   delete[] qprov[k];
   delete[] qmin[k];
   delete[] pTh[k];
   }
   delete[] last_move_arm;
   delete[] imax_arm;
   delete[] Fmax;
   delete[] Farm;
   return iter;

}



int Rchain_hand::pkineinverse_iter_two_chains(double **q, Rhmatrix *pkpremult_T[], Rhtranslink *pkT[],
                  Rhmatrix *pkpostmult_T[], Rhmatrix SA_hand[], int iterations, double pepsilon, bool finger1, bool finger2, bool finger3, bool finger4)
{
  // q[] must have the current values of the articulations

   int iter=1, i;
   int *last_move_arm, *last_move_finger;
   int *imax_arm, *imax_finger;
   double Ftotal=1;
   double *Fmax;
   double **F, **Fprov, **Freduit;
   double **pq, **qprov;  //status temporal de les articulacions
   Rhmatrix **pTh;
   double *Farm;
   std::ostringstream pcout;

   imax_arm = new int[n_chain];
   imax_finger = new int[n_chain];
   last_move_arm = new int[n_chain];
   last_move_finger = new int[n_chain];
   Fmax = new double[n_chain];
   F = new double*[n_chain];
   pq = new double*[n_chain];
   Fprov = new double*[n_chain];
   qprov = new double*[n_chain];
   Freduit = new double*[n_chain];
   Farm = new double[n_same+1];
   pTh = new Rhmatrix*[n_chain];

   for(int k=0;k<n_chain;k++)
   {
   imax_arm[k] = 1;
   imax_finger[k] = 1;
   last_move_arm[k] = 2;
   last_move_finger[k] = 1;
   Fmax[k] = 1.0;
   F[k] = new double[dof[k]+1];
   pq[k] = new double[dof[k]+1];
   Fprov[k] = new double[n_same+1];
   qprov[k] = new double[n_same+1];
   Freduit[k] = new double[dof[k]+1-n_same];
   pTh[k] = new Rhmatrix[dof[k]+1];
   }

   for (int k=0;k<n_chain;k++)
   {
   if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
   {
   for (i=1;i<=dof[k];i++)
   {
      pq[k][i] = q[k][i];
      pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
      pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
      pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
      q[k][i] = pq[k][i];
   }
   }
   }


// Main Bucle to find the inverse
   while (Ftotal > pepsilon && iter < iterations && !(stop_kinematics))
   {
     if((iter%3)==0)
     {
      for(int k=0;k<n_chain;k++)
      {
      if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
      {
      for(i=1;i<=n_same;i++)
      {
         pTh[k][i] = pkpremult_T[k][i-1].inverse() * SA_hand[k];
         pq[k][i] = q[k][i];
      }
      }
      }

      for(i=1;i<=n_same;i++)
      {
        if(i>=1 && i<=5)
        {
         /*Farm[i] = pF_objective_arm_without_ranks(pTh, pkpostmult_T, pq, i);
        }
        if(i==5)
        {*/
          Farm[i] = pF_objective_arm_two(pTh, pkpostmult_T, pq, i, finger1, finger2, finger3, finger4);
        }
        if(i==6)
        {
          Farm[i] = pF_objective_joint_six_two(pTh, pkpostmult_T, pq, i, finger1, finger2, finger3, finger4);
        }
      }

      for(int k=0;k<n_chain;k++)
      {
      if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
      {
        for(i=1;i<=n_same;i++)
        {
          Fprov[k][i] = Farm[i]/n_chain;
          qprov[k][i] = pq[k][i];
        }
      }
      }
      for(int k=0;k<n_chain;k++)
      {
      if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
      {
        for(i=1;i<=n_same;i++)
        {
          pq[k][i] = qprov[k][i];
          F[k][i] = Fprov[k][i];
        }
        imax_arm[k]=maxvector(Fprov[k],n_same);
      }
      }


      if((imax_arm[0]==last_move_arm[0]) || (imax_arm[1]==last_move_arm[1]) || (imax_arm[2]==last_move_arm[2]))
      {
         for(int k=0;k<n_chain;k++) //Bucle fictici per provar si funciona el programa
         {
	 if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
         {
         for(i=1;i<=n_same;i++)
         {
            pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
            pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
            pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
            q[k][i] = pq[k][i];
         }
	 }
         }
         for(int k=0;k<n_chain;k++)
         {
	 if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
         {
         for(i=1;i<=n_same;i++)
         {
            pTh[k][i] = pkpremult_T[k][i-1].inverse() * SA_hand[k];
            pq[k][i] = q[k][i];
         }
	 }
         }

         for(i=1;i<=n_same;i++)
          {
            if(i>=1 && i<=5)
            {
              /*Farm[i] = pF_objective_arm_without_ranks(pTh, pkpostmult_T, pq, i);
            }
            if(i==5)
            {*/
              Farm[i] = pF_objective_arm(pTh, pkpostmult_T, pq, i);
            }
            if(i==6)
            {
              Farm[i] = pF_objective_joint_six(pTh, pkpostmult_T, pq, i);
            }
          }
         for(int k=0;k<n_chain;k++)
          {
	  if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
          {
          for(i=1;i<=n_same;i++)
          {
          Fprov[k][i] = Farm[i]/n_chain;
          qprov[k][i] = pq[k][i];
          }
	  }
        }
        for(int k=0;k<n_chain;k++)
        {
	if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
        {
        for(i=1;i<=n_same;i++)
        {
          pq[k][i] = qprov[k][i];
          F[k][i] = Fprov[k][i];
        }
        imax_arm[k]=maxvector(Fprov[k],n_same);
	}
        }

        for(int k=0;k<n_chain;k++)
        {
	if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
        {
          for(i=1;i<=n_same;i++)
          {
            pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
            pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
            pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
            q[k][i] = pq[k][i];
          }
         last_move_arm[k] = imax_arm[k];
        }
        }
      }
      else
      {
        for(int k=0;k<n_chain;k++)
        {
	if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
        {
         pkT[k][imax_arm[k]].update(int(dh_parameters[k][imax_arm[k]].sigma), pq[k][imax_arm[k]]);
         pupdate_T_premult(pkpremult_T[k], pkT[k], imax_arm[k], k);
         pupdate_T_postmult(pkpostmult_T[k], pkT[k], imax_arm[k]);
         q[k][imax_arm[k]] = pq[k][imax_arm[k]];

         last_move_arm[k] = imax_arm[k];
	 }
        }
      }
      }//If del braç
      //Cinemàtica inversa dels dits

      for(int k=0;k<n_chain;k++)
      {
      if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
      {
      for(i=n_same+1;i<=dof[k];i++)
      {
         pTh[k][i] = pkpremult_T[k][i-1].inverse() * SA_hand[k];
         pq[k][i] = q[k][i];
         if(i>=7 && i<=12)
         {
         F[k][i] = pF_objective(pkpostmult_T[k][i+1], pTh[k][i], pq[k][i], i, k);
         }
         else
         {
         F[k][i] = pF_objective_without_ranks(pkpostmult_T[k][i+1], pTh[k][i], pq[k][i], i, k);
         }

      }
      }
      }

      for(int k=0;k<n_chain;k++)
      {
      if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
      {
        for(i=n_same+1;i<=dof[k];i++)
        {
          Freduit[k][i-n_same] = F[k][i];
        }
        imax_finger[k] = maxvector(Freduit[k],dof[k]-n_same)+n_same;
      }
      }

      for(int k=0;k<n_chain;k++) //Bucle fictici per provar si funciona el programa
      {
      if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
      {
      if((imax_finger[k]==last_move_finger[k]))
      {
         for(i=n_same+1;i<=dof[k];i++)
         {
            pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
            pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
            pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
            q[k][i] = pq[k][i];
         }
         for(i=n_same+1;i<=dof[k];i++)
         {
            pTh[k][i] = pkpremult_T[k][i-1].inverse() * SA_hand[k];
            pq[k][i] = q[k][i];
            if(i>=7 && i<=12)
            {
            F[k][i] = pF_objective(pkpostmult_T[k][i+1], pTh[k][i], pq[k][i], i, k);
            }
            else
            {
            F[k][i] = pF_objective_without_ranks(pkpostmult_T[k][i+1], pTh[k][i], pq[k][i], i, k);
            }
            Freduit[k][i-1] = F[k][i];
            pkT[k][i].update(int(dh_parameters[k][i].sigma),pq[k][i]);
            pupdate_T_premult(pkpremult_T[k], pkT[k], i, k);
            pupdate_T_postmult(pkpostmult_T[k], pkT[k], i);
            q[k][i] = pq[k][i];
         }
         imax_finger[k] = maxvector(Freduit[k],dof[k]-n_same)+n_same;
         last_move_finger[k] = imax_finger[k];

      }
      else
      {
         pkT[k][imax_finger[k]].update(int(dh_parameters[k][imax_finger[k]].sigma), pq[k][imax_finger[k]]);
         pupdate_T_premult(pkpremult_T[k], pkT[k], imax_finger[k], k);
         pupdate_T_postmult(pkpostmult_T[k], pkT[k], imax_finger[k]);
         q[k][imax_finger[k]] = pq[k][imax_finger[k]];
      }
      }
      }

      for(int k=0;k<n_chain;k++)
      {
      if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
      {
      if(F[k][imax_arm[k]]>=F[k][imax_finger[k]])
      {
        Fmax[k] = F[k][imax_arm[k]];
      }
      else
      {
        Fmax[k] = F[k][imax_finger[k]];
      }
      }
      }
      Ftotal = 0;
      for(int k=0;k<n_chain;k++)
      {
      if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
      {
        Ftotal = Ftotal + Fmax[k];
      }
      }
      Ftotal = Ftotal/n_chain;

      iter++;
      }
      pcout << "F màxima: " << Ftotal << endl;
      send_stream(pcout);
     /* cout << Ftotal << endl;
      for(int k=0;k<n_chain;k++)
      {
        for(i=1;i<=dof[k];i++)
        {
          cout << q[k][i];
        }
      }
      cout << endl;*/

   for(int k=0;k<n_chain;k++)
   {
   delete[] F[k];
   delete[] pq[k];
   delete[] Fprov[k];
   delete[] qprov[k];
   delete[] pTh[k];
   delete[] Freduit[k];
   }
   delete[] last_move_arm;
   delete[] last_move_finger;
   delete[] imax_arm;
   delete[] imax_finger;
   delete[] Fmax;
   delete[] Farm;
   return iter;

}

double Rchain_hand::pF_objective_arm_two (Rhmatrix *pTh[], Rhmatrix *pkpostmult_T[], double **qi, int i, bool finger1, bool finger2, bool finger3, bool finger4)
{

   double *t_NUM, *t_DEN, *t_indep, *t_dm, *qi_prov;
   double *Flow, *Fupp, *F;
   double pqi, qi_p=0.0;
   double Flow_tot = 0, Fupp_tot =0, Ftot = 0;
   double C;
   int l;
   double Numerador=0, Denominador=0;

   t_NUM = new double[n_chain];
   t_DEN = new double[n_chain];
   t_indep = new double[n_chain];
   t_dm = new double[n_chain];
   Flow = new double[n_chain];
   Fupp = new double[n_chain];
   F = new double[n_chain];
   qi_prov = new double[n_chain];

   C = (dh_parameters[0][i].upper_value-dh_parameters[0][i].lower_value)/3.0;

   for(int k=0;k<n_chain;k++)
   {
   if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
   {
   t_indep[k] = ptindep (pkpostmult_T[k][i+1], pTh[k][i], dh_parameters[k][i].salpha,
                  dh_parameters[k][i].calpha, dh_parameters[k][i].ai);
   t_dm[k] = -dh_parameters[k][i].salpha * pkpostmult_T[k][i+1].mat[1][3]
            - dh_parameters[k][i].calpha * pkpostmult_T[k][i+1].mat[2][3]
            + pTh[k][i].mat[2][3];

   if (dh_parameters[k][i].sigma == 0)
   {
      F[k] = qi[k][i] * qi[k][i] - 2 * t_dm[k] * qi[k][i] + t_indep[k];

      if (!(dh_parameters[k][i].lower_value <= t_dm[k] && dh_parameters[k][i].upper_value >= t_dm[k]))

      qi[k][i] = t_dm[k];
   }
   else
   {
      t_DEN[k] = pDEN_theta (pkpostmult_T[k][i+1], pTh[k][i], dh_parameters[k][i].salpha, dh_parameters[k][i].calpha,
		    dh_parameters[k][i].ai);
      t_NUM[k] = pNUM_theta (pkpostmult_T[k][i+1], pTh[k][i], dh_parameters[k][i].salpha, dh_parameters[k][i].calpha,
		    dh_parameters[k][i].ai);

      F[k] = 2 * (t_DEN[k] * cos (qi[k][i]) + t_NUM[k] * sin (qi[k][i])) + t_indep[k] +
            	dh_parameters[k][i].di * dh_parameters[k][i].di - 2 * dh_parameters[k][i].di * t_dm[k];

    }
    }
    }

      for(int k=0;k<n_chain;k++)
      {
       if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
       {
        Ftot = Ftot + F[k];
       }
      }

      if(i!=6)
      {
      for(l=0;l<n_chain;l++)
      {
       if ((l==0 && finger1) || (l==1 && finger2) || (l==2 && finger3) || (l==3 && finger4))
       {
        Numerador = Numerador + t_NUM[l];
        Denominador = Denominador + t_DEN[l];
       }
      }

      qi_p = atan2 (Numerador, Denominador);

      if ((-Denominador*cos(qi_p)-Numerador*sin(qi_p))<0)
      {
        qi_p = qi_p + M_PI;
      }
      }

      if (dh_parameters[0][i].lower_value <= qi_p && dh_parameters[0][i].upper_value >= qi_p)
      {
      }
      else
      {
         if (qi_p >= 0)
         {
            pqi = -(M2PI - qi_p);
         }
         else
         {
            pqi = M2PI + qi_p;
         }
 	      if (dh_parameters[0][i].lower_value <= pqi && dh_parameters[0][i].upper_value >= pqi)
	      {
	         qi_p = pqi;
	      }
	      else
	      {
          for(int k=0;k<n_chain;k++)
          {
	  if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
          {
           Flow[k] = 2 * (t_DEN[k] * cos (dh_parameters[k][i].lower_value+C) + t_NUM[k] * sin (dh_parameters[k][i].lower_value+C)) + t_indep[k] +
            	dh_parameters[k][i].di * dh_parameters[k][i].di - 2 * dh_parameters[k][i].di * t_dm[k];
           Fupp[k] = 2 * (t_DEN[k] * cos (dh_parameters[k][i].upper_value-C) + t_NUM[k] * sin (dh_parameters[k][i].upper_value-C)) + t_indep[k] +
            	dh_parameters[k][i].di * dh_parameters[k][i].di - 2 * dh_parameters[k][i].di * t_dm[k];
           }
	   }
           for(int k=0;k<n_chain;k++)
           {
	   if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
           {
             Flow_tot = Flow_tot + Flow[k];
             Fupp_tot = Fupp_tot + Fupp[k];
           }
	   }

           if (Flow_tot < Fupp_tot) {
             qi_p = dh_parameters[0][i].lower_value + C;
             Ftot = Flow_tot;
           }
           else
           {
             qi_p = dh_parameters[0][i].upper_value - C;
             Ftot = Fupp_tot;
           }
           }
       }
       if(i!=6)
       {
       for(int k=0;k<n_chain;k++)
       {
       if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
       {
         qi[k][i] = qi_p;
       }
       }
       }


      delete[] t_NUM;
      delete[] t_DEN;
      delete[] t_indep;
      delete[] t_dm;
      delete[] Flow;
      delete[] Fupp;
      delete[] F;
      delete[] qi_prov;

   return (fabs (Ftot));

}

double Rchain_hand::pF_objective_joint_six_two (Rhmatrix *pTh[], Rhmatrix *pkpostmult_T[], double **qi, int i, bool finger1, bool finger2, bool finger3, bool finger4)
{

   double *t_NUM, *t_DEN, *t_indep, *t_dm;
   double *Flow, *Fupp, *F;
   double qi_p;
   double Ftot = 0;
   int l;
   double Numerador=0, Denominador=0;

   t_NUM = new double[n_chain];
   t_DEN = new double[n_chain];
   t_indep = new double[n_chain];
   t_dm = new double[n_chain];
   Flow = new double[n_chain];
   Fupp = new double[n_chain];
   F = new double[n_chain];


   for(int k=0;k<n_chain;k++)
   {
   if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
   {
   t_indep[k] = ptindep (pkpostmult_T[k][i+1], pTh[k][i], dh_parameters[k][i].salpha,
                  dh_parameters[k][i].calpha, dh_parameters[k][i].ai);
   t_dm[k] = -dh_parameters[k][i].salpha * pkpostmult_T[k][i+1].mat[1][3]
            - dh_parameters[k][i].calpha * pkpostmult_T[k][i+1].mat[2][3]
            + pTh[k][i].mat[2][3];

   if (dh_parameters[k][i].sigma == 0)
   {
      F[k] = qi[k][i] * qi[k][i] - 2 * t_dm[k] * qi[k][i] + t_indep[k];

      if (!(dh_parameters[k][i].lower_value <= t_dm[k] && dh_parameters[k][i].upper_value >= t_dm[k]))

      qi[k][i] = t_dm[k];
   }
   else
   {
      t_DEN[k] = pDEN_theta (pkpostmult_T[k][i+1], pTh[k][i], dh_parameters[k][i].salpha, dh_parameters[k][i].calpha,
		    dh_parameters[k][i].ai);
      t_NUM[k] = pNUM_theta (pkpostmult_T[k][i+1], pTh[k][i], dh_parameters[k][i].salpha, dh_parameters[k][i].calpha,
		    dh_parameters[k][i].ai);

      F[k] = 2 * (t_DEN[k] * cos (qi[k][i]) + t_NUM[k] * sin (qi[k][i])) + t_indep[k] +
            	dh_parameters[k][i].di * dh_parameters[k][i].di - 2 * dh_parameters[k][i].di * t_dm[k];

    }
    }
    }

      for(int k=0;k<n_chain;k++)
      {
      if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
       {
        Ftot = Ftot + F[k];
	}
      }

      /*for(l=0;l<n_chain-1;l++)
      {
        Numerador = Numerador + t_NUM[l];
        Denominador = Denominador + t_DEN[l];
      }

      Numerador = Numerador+(t_NUM[3]*cos(delta))+(t_DEN[3]*sin(delta));
      Denominador = Denominador+(-t_NUM[3]*sin(delta))+(t_DEN[3]*cos(delta));*/

      for(l=0;l<n_chain;l++)
      {
      if ((l==0 && finger1) || (l==1 && finger2) || (l==2 && finger3) || (l==3 && finger4))
       {
        Numerador = Numerador + t_NUM[l]*cos(delta[l]) + t_DEN[l]*sin(delta[l]);
        Denominador = Denominador + t_DEN[l]*cos(delta[l]) - t_NUM[l]*sin(delta[l]);
	}
      }

      qi_p = atan2(Numerador,Denominador);

      if ((-Numerador*cos(qi_p)-Denominador*sin(qi_p))<0)
      {
        qi_p = qi_p + M_PI;
      }

       for(int k=0;k<n_chain;k++)
       {
       if ((k==0 && finger1) || (k==1 && finger2) || (k==2 && finger3) || (k==3 && finger4))
       {
         qi[k][i] = qi_p - delta[k];
	}
       }
       //qi[3][i] = qi_p - delta[3];


      delete[] t_NUM;
      delete[] t_DEN;
      delete[] t_indep;
      delete[] t_dm;
      delete[] Flow;
      delete[] Fupp;
      delete[] F;

   return (fabs (Ftot));

}
