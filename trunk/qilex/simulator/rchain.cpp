/***************************************************************************
                          rchain.cpp  -  description
                             -------------------
    begin                : Mon Nov 4 2002
    copyright            : (C) 2002 by Leopold Palomo
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

#include <cmath>
#include <sstream>
#include <iomanip>
#include <cstring>

#include "rchain.h"
#include "../functions.h"


/************************************************************************

Class Rchain
(generic open chain)

************************************************************************/

Rchain::Rchain ()
{
   pS = 1;
   Tworld = Tworld.identity();
   Ttool = Ttool.identity();
   ptimer = new QTimer( this );
   widget = false;
}

Rchain::~Rchain ()
{

   delete[] status_joint;
   delete [] pqf;
   delete [] pqi;
   delete [] pslope_path;
              
   delete[] home;
   delete[] pT;
   delete[] ppremult_T;
   delete[] ppostmult_T;
   
   delete[] ptkT;
   delete[] ptkpremult_T;
   delete[] ptkpostmult_T;

   delete[] dh_parameters;
}

/** Write property of int n.  and init the class with the vars and cubic arrays*/
void Rchain::init (int pnewVal)
{
   dof = pnewVal;

   nsolmax = 16;
  
   status_joint = new double[pnewVal+1];
   pqi = new double[pnewVal+1];
   pqf = new double[pnewVal+1];
   pslope_path = new double[dof+1];
  
   home = new double[pnewVal+1];

   list_plug = new plug[pnewVal];

   for(int i=0;i<dof;i++)
   {
      list_plug[i].setIndex(i);
   }
  /* name = new char[40];
   manufacturer = new char[40];
    */

   dh_parameters = new dh_parameters_joint[pnewVal+1];

   iniciate = true;
   console_mode = false;

   // Why pnewVal + 2?
   // pnewVal is the number of link
   // 1 the world transform
   // and 2 is the tool transform
   // ergo I need to keep n + 2 transforms
   //

 
   pT = new Rhtranslink[pnewVal+2];

   ppremult_T = new Rhmatrix[pnewVal+2];
   ppostmult_T = new Rhmatrix[pnewVal+2];

   // private temporal vars

   ptkT = new Rhtranslink[pnewVal + 2];
   ptkpremult_T = new Rhmatrix[pnewVal + 2];
   ptkpostmult_T = new Rhmatrix[pnewVal + 2];

}

/* You must call this routine in order of the joins. So, first the joint 1, after 2, ... till n
  and the value of the i, the joint */ 

void Rchain::set_dh_pararmeters (unsigned int sigma, double theta, double alphai,
			       double ai, double di, double low_rank,
			       double up_rank, double maxspeed, double maxacc,
			       int i)
{

   double rad_theta = rad (theta);
   double rad_alpha = rad (alphai);
   double temp = 0;
   std::ostringstream pcout;
   
   pcout << endl << "Aquest són els valors del l'articulació: " << i << endl;
   dh_parameters[i].sigma = sigma;
   dh_parameters[i].theta = rad_theta;
   dh_parameters[i].stheta = sin (rad_theta);
   dh_parameters[i].ctheta = cos (rad_theta);
   dh_parameters[i].di = di;

   if (sigma == 1)
      status_joint[i] = rad_theta;
   else
      status_joint[i] = di;

   dh_parameters[i].salpha = sin (rad_alpha);
   dh_parameters[i].calpha = cos (rad_alpha);
   dh_parameters[i].ai = ai;

   // low_rank and up_rank must be in radians if the joint is rotation

   if (sigma == 1)
   {
      dh_parameters[i].lower_value = rad (low_rank);
      dh_parameters[i].upper_value = rad (up_rank);
      dh_parameters[i].m_speed = rad(maxspeed);
      dh_parameters[i].m_acc = rad(maxacc);
   }
   else
   {
      dh_parameters[i].lower_value = low_rank;
      dh_parameters[i].upper_value = up_rank;
      dh_parameters[i].m_speed = maxspeed;
      dh_parameters[i].m_acc = maxacc;

   }

   dh_parameters[i].rank = fabs(dh_parameters[i].upper_value - dh_parameters[i].lower_value);
   
  // create transform matrix T[i]

   pT[i].transformation (dh_parameters[i].stheta, dh_parameters[i].ctheta,
			dh_parameters[i].salpha, dh_parameters[i].calpha,
			dh_parameters[i].ai, dh_parameters[i].di);

  // update and scale
   if (i == dof)
   {
      for (int n = 1; n <= i; n++)
      {
	      temp = temp + dh_parameters[n].di + dh_parameters[n].ai;
      }

      // maximum distante betwend each the links
      pS = pS / (temp * 0.1); // normalized and poderated

      //initial transformation; identity
      pT[0].transformation(0, 1, 0, 1, 0, 0);
      //initial transformation tool; identity
      pT[ dof + 1].transformation(0, 1, 0, 1, 0, 0);
            
      ppremult_T[0] = pT[0].value();
      Tworld = pT[0].value();
      ppostmult_T[dof + 1] = pT[dof + 1].value();
      Ttool = pT[dof + 1].value();
    
      for (int n = 1; n <= i + 1; n++)
      {
	      pT[n].scale_dist (pS);
	      pupdate_T_premult (ppremult_T, pT, n);

         if (n<=dof)
         {
            dh_parameters[n].ai = dh_parameters[n].ai * pS;
   	      dh_parameters[n].di = dh_parameters[n].di * pS;
         }
	   }
      for (int n = i ; n >= 0; n--)
      {
         pupdate_T_postmult (ppostmult_T, pT, n);
      }
      pcout << "Tworld" << endl;
      printmatrix(Tworld);
      pcout << "Ttool" << endl;
      printmatrix(Ttool);

      {
         double *speed = new double[dof+1];
         double *acc = new double[dof+1];
         
         for (int i= 1; i <= dof; i++)
         {
            speed[i] = dh_parameters[i].m_speed;
            acc[i] = dh_parameters[i].m_acc;  
            pmax_speed = dh_parameters[minvector(speed, dof)].m_speed;
            pmax_acceleration = dh_parameters[minvector(acc, dof)].m_acc;
         }
         delete speed;
         delete acc;
      }   
      /********************************************/
    //  pmax_speed = dh_parameters[11][minvector(dh_parameters[11], dof)];
    //  pmax_acceleration = dh_parameters[12][minvector(dh_parameters[12], dof)];
      /********************************************/
      pcurrent_speed = pmax_speed * 0.5;
      pcurrent_acceleration = pmax_acceleration * 0.5;
      
   }
   send_stream(pcout);
}

/* procedure to update the "premult" vector from a "T" vector*/

void Rchain::pupdate_T_premult (Rhmatrix ppremult_Tt[], Rhtranslink pTt[], int n)
{
  int cont = n + 1;

  // if n = 0, it's the world transformation

  //we know that you only update a joint between
  // 1 and dof
  // 0 is the world transform
  //dof + 1 is the tools transform

  ppremult_Tt[n] = ppremult_Tt[n - 1] * pTt[n].value ();

  while (cont <= dof + 1)
  {
      ppremult_Tt[cont] = ppremult_Tt[cont - 1] * pTt[cont].value ();
      cont++;
  }
}

void
Rchain::pupdate_T_postmult (Rhmatrix ppostmult_Tt[], Rhtranslink pTt[],
			       int n)
{
  //we know that you only update a joint between
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

void Rchain::print_config (int n)
{
   std::ostringstream pcout;
   if (n > 0)
   {
      pcout << "-----------------------------------------------------" << endl;
      pcout << "Value of T matrix " << endl;
      send_stream(pcout);
      printmatrix(pT[n], (1 / pS));
      pcout << "Value of premultT matrix " << endl;
      send_stream(pcout);
      printmatrix(ppremult_T[n],(1 / pS));
      send_stream(pcout);
      pcout << "Value of postmultT matrix " << endl;
      send_stream(pcout);
      printmatrix(ppostmult_T[n], (1 / pS));
      pcout << "Value of the articulation: " << degree (status_joint[n]) << endl;
      pcout << "----------------------------------------------------" << endl;
      send_stream(pcout);
   }
}

void Rchain::tcp ()
{
   std::ostringstream pcout;
   pcout << "----------------------------------------------------" << endl;
   pcout << "Value of the TCP: " << endl;
   send_stream(pcout);
   printmatrix(ppremult_T[dof+1], (1 / pS));
   pcout << "----------------------------------------------------" << endl;
   send_stream(pcout);
}
Rhmatrix Rchain::mat_tcp()
{
   return ppremult_T[dof+1].descaled(1 / pS);
}

// this function return the scale how is working the class.
double Rchain::scale ()
{
  return pS;
}

void Rchain::pupdate_join (int joint, double value)
{
   if ((dh_parameters[joint].lower_value <= value) && (value <= dh_parameters[joint].upper_value))
   {
      status_joint[joint] = value;

      list_plug[joint-1].setValue(value);

      pT[joint].update (dh_parameters[joint].sigma, value);
      pupdate_T_premult (ppremult_T, pT, joint);
      pupdate_T_postmult (ppostmult_T, pT, joint);
   }
   else
   {
      if (ptimer->isActive())
      {
         ptimer->stop();
      }
      std::ostringstream pcout;
      pcout << "Joint " << joint << " is out of limits" << endl;
      send_stream(pcout);
   }   
}


void Rchain::show_status ()
{
   std::ostringstream pcout;
   
   pcout << setiosflags (ios::fixed | ios::showpoint);
   pcout << endl;
   pcout << setw (10) << setprecision (5) << endl;


   for (int k = 1; k <= dof; k++)
   {
      if (dh_parameters[k].sigma == 0)
      {
         pcout << endl << "joint: " << k  << "  Current value:"
         << status_joint[k] << endl;
      }
      else
      {
         pcout << endl << "joint: " << k << "  Current value:"
         << degree (status_joint[k]) << endl;
      }
      pcout << "Limit inferior : " << dh_parameters[k].lower_value << endl;
      pcout << "Limit superior : " << dh_parameters[k].upper_value << endl;
   }
   send_stream(pcout);
   tcp ();
}

double Rchain::pF_objective (Rhmatrix Ti, Rhmatrix Th_i, double &qi, int i)
{

   double F = 0;
   double t_NUM, t_DEN, t_indep, t_dm, pqi;
   double Flow = 0, Fupp =0;
   double C;
      
   // C, Xavi's formula to find a good value to the contrain function
   C = (dh_parameters[i].upper_value-dh_parameters[i].lower_value)/3.0;
                 
   t_indep = ptindep (Ti, Th_i, dh_parameters[i].salpha, dh_parameters[i].calpha, dh_parameters[i].ai);
   t_dm = -dh_parameters[i].salpha * Ti.mat[1][3] - dh_parameters[i].calpha * Ti.mat[2][3] + Th_i.mat[2][3];

   if (dh_parameters[i].sigma == 0)
   {
      F = qi * qi - 2 * t_dm * qi + t_indep;

      if (!(dh_parameters[i].lower_value <= t_dm && dh_parameters[i].upper_value >= t_dm))
         qi = t_dm;
   }
   else
   {
      t_DEN = pDEN_theta (Ti, Th_i, dh_parameters[i].salpha, dh_parameters[i].calpha,
		    dh_parameters[i].ai);
      t_NUM = pNUM_theta (Ti, Th_i, dh_parameters[i].salpha, dh_parameters[i].calpha,
		    dh_parameters[i].ai);

      F = 2 * (t_DEN * cos (qi) + t_NUM * sin (qi)) + t_indep +
            	dh_parameters[i].di * dh_parameters[i].di - 2 * dh_parameters[i].di * t_dm;

          
      qi = atan2 (t_NUM, t_DEN);

      if ((-t_DEN*cos(qi)-t_NUM*sin(qi))<0)
      {
        qi = qi + M_PI;
      } 

     
      if (dh_parameters[i].lower_value <= qi && dh_parameters[i].upper_value >= qi)
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
 	      if (dh_parameters[i].lower_value <= pqi && dh_parameters[i].upper_value >= pqi)
	      {
	         qi = pqi;
	      }
	      else
	      {
           Flow = 2 * (t_DEN * cos (dh_parameters[i].lower_value+C) + t_NUM * sin (dh_parameters[i].lower_value+C)) + t_indep +
            	dh_parameters[i].di * dh_parameters[i].di - 2 * dh_parameters[i].di * t_dm;
           Fupp = 2 * (t_DEN * cos (dh_parameters[i].upper_value-C) + t_NUM * sin (dh_parameters[i].upper_value-C)) + t_indep +
            	dh_parameters[i].di * dh_parameters[i].di - 2 * dh_parameters[i].di * t_dm;

           if (Flow < Fupp) {
             qi = dh_parameters[i].lower_value + C;
             F = Flow;
           }
           else
           {
             qi = dh_parameters[i].upper_value - C;
             F = Fupp;
           }
           }
       }
   }
   return (fabs (F));

}


double Rchain::pNUM_theta (Rhmatrix Ti, Rhmatrix Th_i, double salphai,
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


double Rchain::pDEN_theta (Rhmatrix Ti, Rhmatrix Th_i, double salphai,
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

double Rchain::ptindep (Rhmatrix Ti, Rhmatrix Th_i, double salphai, double calphai,
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

double Rchain::pdm (Rhmatrix Ti, Rhmatrix Th_i, double salphai, double calphai)
{
  return (-salphai * Ti.mat[1][3] - calphai * Ti.mat[2][3] + Th_i.mat[2][3]);
}


/* Aquesta funció calcula totes les solucions d'una configuració i
les imprimeix  */


void Rchain:: kineinverse_all (Rhmatrix A_hand) 
{
   int i, iter;
   int parts, combTotal;
   int nsol, pnsol, piterations;
   double pepsilon;

   bool found;

   int index;
  
   double *pq; //status temporal de les articulacions
   double *q;
   double *solucio[16];
   int *combi;

   A_hand.mat[0][3]=A_hand.mat[0][3]*pS;
   A_hand.mat[1][3]=A_hand.mat[1][3]*pS;
   A_hand.mat[2][3]=A_hand.mat[2][3]*pS;

   //A_hand = Tworld.inverse() * A_hand;
   A_hand = A_hand * Ttool.inverse();
      
   Rhtranslink *pkT;
   Rhmatrix *pkpremult_T;
   Rhmatrix *pkpostmult_T;

   pkT = new Rhtranslink[dof + 2];
   pkpremult_T = new Rhmatrix[dof + 2];
   pkpostmult_T = new Rhmatrix[dof + 2];

   pq = new double[dof+1];
   q = new double[dof+1];   
   combi = new int[dof];


   for (int h=0;h<16;h++)
   {
      solucio[h] = new double[dof];
   }

   Rhmatrix pTh_1;

    // variables temporals, potser aniria més ràpid declarant-les globals a la clase
   for (i=1;i<=dof;i++)
   {
      pq[i] = status_joint[i];
      q[i] = status_joint[i];
      pkT[i]= pT[i];
      pkpremult_T[i] = ppremult_T[i];
      pkpostmult_T[i] = ppostmult_T[i];
   }

   
   pkT[0]= pT[0];
   pkpremult_T[0] = ppremult_T[0];
   pkpostmult_T[0] = ppostmult_T[0];

   pkT[dof+1]= pT[dof+1];
   pkpremult_T[dof+1] = ppremult_T[dof+1];
   pkpostmult_T[dof+1] = ppostmult_T[dof+1];
           
   parts = 4;
   combTotal = (int) pow((float)parts,(float)dof);

   nsol=0;

   piterations = 3000;
   pepsilon = 0.00001;
// Main Bucle to find the invense 
  
   int combinatio=0;
            
   while (combinatio < combTotal && nsol < nsolmax)
   {
      psequence_generator(combinatio, combi, parts, combTotal);
      pupdate_inits(combi, pq, parts);


      for(i=1;i<=dof;i++)
      {
         pkT[i].update(dh_parameters[i].sigma, pq[i]);
         pupdate_T_premult(pkpremult_T, pkT, i);
         pupdate_T_postmult(pkpostmult_T, pkT, i);
         q[i] = pq[i];
      }
      iter = pkineinverse_iter(q, pkpremult_T, pkT, pkpostmult_T, A_hand, piterations, pepsilon);

          
      if (iter!=piterations)
      {
         if(nsol==0)
         {
            for(int h=0;h<dof;h++)
            {
               solucio[0][h]= q[h+1];
            }
            cout << "Solució nº:" << nsol + 1 << ":" << endl;
            cout.precision(4);
            nsol = 1;
            for (int h=0;h<dof;h++)
            {
               if (dh_parameters[h+1].sigma == 1) // rotation
               {
                  cout << degree(q[h+1]) << ";";
               }
               else
               {
                  cout << q[h+1] << ";" ;
               }   
            }
            cout << endl;
         }
         else
         {
            found = false; pnsol = 0;

            while(pnsol< nsol && !found)
            {
               index = 0;
               
               found =  equal_vector( solucio[pnsol], q, 0, 1, dof , 0.5);
               pnsol++;
            }
            if (!found)
            {
               for(int h=0;h<dof;h++)
               {
                  solucio[nsol][h]=q[h+1];
               }
               nsol ++;
               
               cout << "Solució nº:" << nsol << ":" << endl;
               cout.precision(4);

               for (int h=0;h<dof;h++)
               {
                  if (dh_parameters[h+1].sigma == 1) // rotation
                  {
                     cout << degree(q[h+1]) << ";";
                  }
                  else
                  {
                     cout << q[h+1] << ";" ;
                  }   
               }
               cout << endl;
                         
            }
         }
      }
      combinatio ++;
   }

   //this part optimize the solutions
   piterations = 3000;
   pepsilon = 0.0000001;
   
   for(int s=0; s<nsol ; s++)
   {
      for(int h=0;h<dof;h++)
      {
         pq[h+1] = solucio[s][h];
      }
      
      for(i=1;i<=dof;i++)
      {
         pkT[i].update(int(dh_parameters[i].sigma),pq[i]);
         pupdate_T_premult(pkpremult_T, pkT, i);
         pupdate_T_postmult(pkpostmult_T, pkT, i);
         q[i] = pq[i];
      }
      iter = pkineinverse_iter(q, pkpremult_T, pkT, pkpostmult_T, A_hand, piterations, pepsilon);

      if(iter != piterations)
      {
         cout << iter << endl;
         
         for(int h=0;h<dof;h++)
         {
            solucio[s][h]=q[h+1];
         }
      }   
   }
   cout << "########################################" << endl;
   cout << "########################################" << endl;
   cout << "########################################" << endl;

   cout << setiosflags (ios::fixed | ios::showpoint);
   cout << endl;
   cout << setw (10) << setprecision (5) << endl;
   
   cout << "Les solucions trobades són :" << endl;
   for (pnsol=0;pnsol<nsol;pnsol++)
   {
      cout << "Solució nº:" << pnsol+1 << ":" << endl;
      cout << setiosflags (ios::fixed | ios::showpoint);
      cout << setw (8) << setprecision (3);
   
      for (int h=0;h<dof;h++)
      {
         cout << degree(solucio[pnsol][h]) << ";";    
      }
      cout << endl;
   }
                     
   delete[] pkT ;
   delete[] pkpremult_T;
   delete[] pkpostmult_T;
   delete[] pq;
   delete[] combi;
   for(int n=0;n<16;n++)      
      delete[] solucio[n];

  
}

void Rchain::psequence_generator(int combinatio, int combi[], int parts, int combTotal)
{
   int n, pcomb;

   combTotal = (int) pow((float)parts,(float)dof);

   pcomb = combinatio;

   for(n=0;n<dof;n++)
   {
      combi[n] = (int) (pcomb / pow((float)parts,(float)(dof-n-1)));
      pcomb = pcomb % (int) pow((float)parts,(float)(dof-n-1));
   }
}


void Rchain::pupdate_inits(int combi[], double pq[], int parts)
{
   for (int i=0;i<dof;i++)
   {
      pq[i+1]= dh_parameters[i+1].lower_value + (2 * combi[i] +1) * dh_parameters[i+1].rank / (parts*2);
   }   
}

int Rchain::pkineinverse_iter(double q[], Rhmatrix pkpremult_T[], Rhtranslink pkT[],
                  Rhmatrix pkpostmult_T[], Rhmatrix SA_hand, int iterations, double pepsilon)
{
   // q[] must have the current values of the articulations

   int iter=0, i, last_move=1;
   int imax=1;

   double Fmax=1, last_Fmax=0;
   double *F;
   double *pq;  //status temporal de les articulacions

   F = new double[dof+1];
   pq = new double[dof+1];
   
   Rhmatrix pTh_1;

   for (i=1;i<=dof;i++)
   {
      pq[i] = q[i];
   }

// Main Bucle to find the inverse

   while (Fmax > pepsilon && iter < iterations)
   {
      for(i=1;i<=dof;i++)
      {
         pTh_1 = pkpremult_T[i-1].inverse() * SA_hand;
         pq[i] = q[i];
         F[i] = pF_objective(pkpostmult_T[i+1], pTh_1, pq[i], i);
      }
      imax=maxvector(F,dof);

//      if(imax==last_move)
      if(imax==last_move && Fmax == last_Fmax)
      {
         for(i=1;i<=dof;i++)
         {
            pkT[i].update(dh_parameters[i].sigma, pq[i]);
            pupdate_T_premult(pkpremult_T, pkT, i);
            pupdate_T_postmult(pkpostmult_T, pkT, i);
            q[i] = pq[i];
         }
         for(i=1;i<=dof;i++)
         {
            pTh_1 = pkpremult_T[i-1].inverse() * SA_hand;
            pq[i] = q[i];
            F[i] = pF_objective(pkpostmult_T[i+1], pTh_1, pq[i], i);
            pkT[i].update(dh_parameters[i].sigma,pq[i]);
            pupdate_T_premult(pkpremult_T, pkT, i);
            pupdate_T_postmult(pkpostmult_T, pkT, i);
            q[i] = pq[i];
         }
         imax=maxvector(F,dof);
        // last_move = imax;
        // last_Fmax = Fmax;
        // Fmax = F[imax];

      }
      else
      {
         pkT[imax].update(dh_parameters[imax].sigma, pq[imax]);
         pupdate_T_premult(pkpremult_T, pkT, imax);
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

/* The inverse function. Returns 0 if position not allowed
   Returns 1 if position is allowed and q with the values of the joins  */
 /* Versió amb variables temporals
 funciona força bé, ja que només actualitza una articulació a l'hora,
 la que té la funció objectiu pitjor */
int Rchain::kineinverse(Rhmatrix A_hand, double q[])
{
   return(pkineinverse(A_hand, q, 10000, 0.000000001));
}
int Rchain::kineinverse_fast(Rhmatrix A_hand, double q[])
{                                       
   return(pkineinverse(A_hand, q, 5000, 0.000000001));
}
int Rchain::pkineinverse(Rhmatrix A_hand, double q[], int piterations, double pepsilon)
{
   int iter_done = 0;

   A_hand.mat[0][3]=A_hand.mat[0][3]*pS;
   A_hand.mat[1][3]=A_hand.mat[1][3]*pS;
   A_hand.mat[2][3]=A_hand.mat[2][3]*pS;

   //A_hand = Tworld.inverse() * A_hand;
   A_hand = A_hand * Ttool.inverse();
            
   for (int i=1;i<=dof;i++)
   {
      q[i] = status_joint[i];
      ptkT[i]= pT[i];
      ptkpremult_T[i] = ppremult_T[i];
      ptkpostmult_T[i] = ppostmult_T[i];
   }
      
   ptkT[0]= pT[0];
   ptkpremult_T[0] = ppremult_T[0];
   ptkpostmult_T[0] = ppostmult_T[0];

   ptkT[dof+1]= pT[dof+1];
   ptkpremult_T[dof+1] = ppremult_T[dof+1];
   ptkpostmult_T[dof+1] = ppostmult_T[dof+1];
      
   iter_done = pkineinverse_iter(q, ptkpremult_T, ptkT, ptkpostmult_T, A_hand,
                   piterations, pepsilon);
               

   if (iter_done==piterations)
      return -1;
   else
      return iter_done;

}
void Rchain::settranform_Tworld (Rhmatrix T_world)
{
   printmatrix(T_world);
	Tworld = T_world.descaled(pS);

   //update with scaled value
   
   pT[0].mat[0][3] = T_world.mat[0][3]*pS;
   pT[0].mat[1][3] = T_world.mat[1][3]*pS;
   pT[0].mat[2][3] = T_world.mat[2][3]*pS;

   pT[0].mat[0][2] = T_world.mat[0][2];
   pT[0].mat[1][2] = T_world.mat[1][2];
   pT[0].mat[2][2] = T_world.mat[2][2];

   pT[0].mat[0][1] = T_world.mat[0][1];
   pT[0].mat[1][1] = T_world.mat[1][1];
   pT[0].mat[2][1] = T_world.mat[2][1];

   pT[0].mat[0][0] = T_world.mat[0][0];
   pT[0].mat[1][0] = T_world.mat[1][0];
   pT[0].mat[2][0] = T_world.mat[2][0];

   ppremult_T[0] = pT[0].value();
    // update de transpremult vector
   pupdate_T_premult (ppremult_T, pT, 1);

   // update de postmult vector
   pupdate_T_postmult (ppostmult_T, pT, dof);
   
}
void Rchain::settranform_Ttool (Rhmatrix T_tool)
{
   Ttool = T_tool;

   //update with scaled value

   pT[dof+1].mat[0][3] = T_tool.mat[0][3]*pS;
   pT[dof+1].mat[1][3] = T_tool.mat[1][3]*pS;
   pT[dof+1].mat[2][3] = T_tool.mat[2][3]*pS;

   pT[dof+1].mat[0][2] = T_tool.mat[0][2];
   pT[dof+1].mat[1][2] = T_tool.mat[1][2];
   pT[dof+1].mat[2][2] = T_tool.mat[2][2];

   pT[dof+1].mat[0][1] = T_tool.mat[0][1];
   pT[dof+1].mat[1][1] = T_tool.mat[1][1];
   pT[dof+1].mat[2][1] = T_tool.mat[2][1];

   pT[dof+1].mat[0][0] = T_tool.mat[0][0];
   pT[dof+1].mat[1][0] = T_tool.mat[1][0];
   pT[dof+1].mat[2][0] = T_tool.mat[2][0];


   ppostmult_T[dof + 1] = pT[dof + 1].value();

    // update de transpremult vector
   pupdate_T_premult (ppremult_T, pT, dof);

   // update de postmult vector
   pupdate_T_postmult (ppostmult_T, pT, 1);

}


/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/


void Rchain::setValue(int i, double val)
{
   if (val != status_joint[i])
   {
      if ((dh_parameters[i].lower_value <= val) && (val <= dh_parameters[i].upper_value))
      {
            pupdate_join (i, val);
      }
      else
      {
         cout << "joint out of range " << endl;
      }
   }
}


/* this funcion move the chain the the qf values 1...dof */
/* at % of the maximal speed */

void Rchain::move_path_join (double qf[], int movement)
{
   double distance;

   //assign the values of the positions
   for (int i=1; i<=dof;i++)
   {
      pqi[i] = status_joint[i];
      pqf[i] = qf[i];
      pslope_path[i] = fabs(pqi[i] - pqf[i]);
   }
   distance = pslope_path[maxvector(pslope_path, dof)];

   ptime_f = distance / pcurrent_speed;

   ptime = 0;

   for (int i = 1; i<=dof; i++)
   {
      pslope_path[i] = (pqf[i] - pqi[i])/ptime_f;
   }
  
   if (movement == 0) // linear movement
   {
      connect( ptimer, SIGNAL(timeout()), this, SLOT(slot_genpas_linear()) );
   }
   ptimer->start( 40, FALSE );
    
}

void Rchain::slot_genpas_linear()
{

   double value;
   
   if (ptime >= ptime_f) // last movement
   {
      for (int i=1; i<=dof;i++)
      {
         pupdate_join (i, pqf[i]);
      }
      disconnect(ptimer, SIGNAL(timeout()), this, SLOT(slot_genpas_linear()) );
      ptimer->stop();
   }
   else
   {
      for (int i=1; i<=dof;i++)
      {
         value = pslope_path[i] * ptime + pqi[i];
         pupdate_join (i, value);
      }
      ptime = ptime + 0.040; // incrementa el timer
   }   

}

void Rchain::slot_genpas_linear_parabolic_blend()
{
   //
}

void Rchain::setconsole_mode(bool mode)
{
   // mode de funcionament
   // si fals, mode de botonera
   // per tant l'actualització de les articulacions
   // venen del panell

   // si cert, mode consola
   // l'actualització de les articulacions, parteix del motor de la simulació
   console_mode = mode;

   if(mode)
   {
      disconnect();
      
   }
   else
   {
      disconnect();
      for(int i=0; i<dof; i++)
      {
         connect(&list_plug[i], SIGNAL(valueChangedfull(int, double)), this, SLOT(setValue(int, double)));
      }
   }
}

int Rchain::init_dat (const char *robot_file)
{
      // envies un nom d'un fitxer xml amb la configuració del robot i
      // inicialitza la classe
      // retorna un 0 si tot ha estat bé.
      // retorna un 1 si el fitxer no ha pogut ser validat
      // retorna un 2 si el fitxer estava buit
      // retorna un 3 si el fitxer tenia algun tipus d'error a les dades
      // retorna un 4 si el fitxer no s'ha pogut obrir

   int error;
   
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
   return error;
}
int Rchain::read_element_xml (QDomElement kine_element)
{
   QDomNodeList list;
   QDomElement element;
   QDomNode node;
   QString data;
   bool ok;
   double sigma, theta, alpha, ai, di, low_rank, up_rank, maxspeed, maxacc;
   double phome;
   int int_number;
   int error = 0;

   if ( kine_element.tagName () == "kinechain" )
   {
      data = kine_element.attribute ("dof", QString::null);
      if (data.isNull())
      {
         error = 3;
      }
      else
      {
         int_number = data.toInt( &ok, 10 );
         if (ok)
         {
            init(int_number);
         }
         else
         {
            error = 3;
         }
      }
      name = kine_element.attribute ("name", QString::null);
   }
   else
      error = 3;
                
   list = kine_element.elementsByTagName ( "manufacturer");
   if (list.length() == 1)
   {
      node = list.item(0);
      if ( node.isElement())
      {
         element = node.toElement();
         manufacturer = element.text();
      }      
      
   }

   list = kine_element.elementsByTagName ("joint");
   
   if (list.count()==(unsigned int)dof)
   {
      for (int i=0;i<dof;i++)
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
         
            set_dh_pararmeters (sigma, theta, alpha, ai, di, low_rank, up_rank, maxspeed, maxacc, int(i+1));

            if (sigma == 1)
               home[i+1] = rad(phome);
            else
               home[i+1] = phome;
         }
         else
            error = 3;
      }
   }
   else
      error = 3;
   
   return error;       
}
void Rchain::do_ready()
{
   move_path_join (home, 0);
   show_tcp();
}
void Rchain::limits(int joint, double &min, double &max)
{
   if(joint<=dof)
   {
      min = dh_parameters[joint+1].lower_value;
      max = dh_parameters[joint+1].upper_value;
   }
}

void Rchain::slot_world_X (double epsilon)
{

   int sol;
   std::ostringstream pcout;
 
   Rhmatrix m_tcp;

   m_tcp = mat_tcp();

  // m_tcp = m_tcp.descaled(1/pS);
   m_tcp.mat[0][3] = m_tcp.mat[0][3] + epsilon;

   sol = kineinverse_fast(m_tcp, pqf);
   if(sol == -1)
   {
      pcout << "No solution found" << endl;
		printmatrix(m_tcp);
   }
   else
   {
      for(int i=1;i<=dof;i++)
      {
         pupdate_join (i, pqf[i]);
      }
   }
   send_stream(pcout);       
}
void Rchain::slot_world_Y (double epsilon)
{
   int sol;
   Rhmatrix m_tcp;
   std::ostringstream pcout;

   m_tcp = mat_tcp();

   m_tcp.mat[1][3] = m_tcp.mat[1][3] + epsilon;

   sol = kineinverse_fast(m_tcp, pqf);
   if(sol == -1)
   {
      pcout << "No solution found" << endl;
		printmatrix(m_tcp);
   }
   else
   {
      for(int i=1;i<=dof;i++)
      {
         pupdate_join (i, pqf[i]);
      }
   }
   send_stream(pcout);
}
void Rchain::slot_world_Z (double epsilon)
{
   int sol;
   Rhmatrix m_tcp;
   std::ostringstream pcout;
      
   m_tcp = mat_tcp();

   m_tcp.mat[2][3] = m_tcp.mat[2][3] + epsilon;

   sol = kineinverse_fast(m_tcp, pqf);
   if(sol == -1)
   {
      pcout << "No solution found" << endl;
		printmatrix(m_tcp);
   }
   else
   {
      for(int i=1;i<=dof;i++)
      {
         pupdate_join (i, pqf[i]);
      }
   }
   send_stream(pcout);
}
void Rchain::slot_world_RX (double epsilon)
{
   int sol;
   double cosalpha, sinalpha;
   std::ostringstream pcout;

   cosalpha = cos(epsilon);
   sinalpha = sin(epsilon);

   Rhmatrix m_tcp;
   m_tcp = m_tcp.identity();
   m_tcp.mat[1][1] = cosalpha;
   m_tcp.mat[1][2] = -sinalpha;
   m_tcp.mat[2][1] = sinalpha;
   m_tcp.mat[2][2] = cosalpha;
   
   printmatrix(m_tcp);
      
   m_tcp = m_tcp * mat_tcp() ;

   printmatrix(m_tcp);

   sol = kineinverse_fast(m_tcp, pqf);
   if(sol == -1)
   {
      pcout << "No solution found" << endl;
		printmatrix(m_tcp);
   }
   else
   {
      for(int i=1;i<=dof;i++)
      {
         pupdate_join (i, pqf[i]);
      }
   }
   send_stream(pcout);
}
void Rchain::slot_world_RY (double epsilon)
{
   int sol;
   std::ostringstream pcout;
   
   double cosalpha, sinalpha;
   cosalpha = cos(epsilon);
   sinalpha = sin(epsilon);

   Rhmatrix m_tcp;
   m_tcp = m_tcp.identity();
   m_tcp.mat[0][0] = cosalpha;
   m_tcp.mat[0][2] = sinalpha;
   m_tcp.mat[2][1] = -sinalpha;
   m_tcp.mat[2][2] = cosalpha;

   printmatrix(m_tcp);

   m_tcp = m_tcp * mat_tcp() ;

   printmatrix(m_tcp);

   sol = kineinverse_fast(m_tcp, pqf);
   if(sol == -1)
   {
      pcout << "No solution found" << endl;
		printmatrix(m_tcp);
   }
   else
   {
      for(int i=1;i<=dof;i++)
      {
         pupdate_join (i, pqf[i]);
      }
   }
   send_stream(pcout);
}
void Rchain::slot_world_RZ (double epsilon)
{
   int sol;
   std::ostringstream pcout;
   double cosalpha, sinalpha;

   cosalpha = cos(epsilon);
   sinalpha = sin(epsilon);

   Rhmatrix m_tcp;
   m_tcp = m_tcp.identity();
   m_tcp.mat[0][0] = cosalpha;
   m_tcp.mat[0][1] = -sinalpha;
   m_tcp.mat[1][0] = sinalpha;
   m_tcp.mat[1][1] = cosalpha;

   printmatrix(m_tcp);

   m_tcp = m_tcp * mat_tcp() ;

   printmatrix(m_tcp);

   sol = kineinverse_fast(m_tcp, pqf);
   if(sol == -1)
   {
      pcout << "No solution found" << endl;
		printmatrix(m_tcp);
   }
   else
   {
      for(int i=1;i<=dof;i++)
      {
         pupdate_join (i, pqf[i]);
      }
   }
   send_stream(pcout);
}
void Rchain::slot_tool_X (double epsilon)
{
   int sol;
   std::ostringstream pcout;
       
   Rhmatrix m_tcp;
   m_tcp = m_tcp.identity();
   m_tcp.mat[0][3] = epsilon;

   m_tcp = mat_tcp() * m_tcp ;


   sol = kineinverse_fast(m_tcp, pqf);
   if(sol == -1)
   {
      pcout << "No soulution found" << endl;
   }
   else
   {
      for(int i=1;i<=dof;i++)
      {
         pupdate_join (i, pqf[i]);
      }
   }
   send_stream(pcout);
}
void Rchain::slot_tool_Y (double epsilon)
{
   int sol;
   std::ostringstream pcout;
   
   Rhmatrix m_tcp;
   m_tcp = m_tcp.identity();
   m_tcp.mat[1][3] = epsilon;

   m_tcp = mat_tcp() * m_tcp ;


   sol = kineinverse_fast(m_tcp, pqf);
   if(sol == -1)
   {
      pcout << "No solution found" << endl;
		printmatrix(m_tcp);
   }
   else
   {
      for(int i=1;i<=dof;i++)
      {
         pupdate_join (i, pqf[i]);
      }
   }
   send_stream(pcout);
}
void Rchain::slot_tool_Z (double epsilon)
{
   int sol;
   std::ostringstream pcout;
   
   Rhmatrix m_tcp;
   m_tcp = m_tcp.identity();
   m_tcp.mat[2][3] = epsilon;

   m_tcp = mat_tcp() * m_tcp ;


   sol = kineinverse_fast(m_tcp, pqf);
   if(sol == -1)
   {
      pcout << "No solution found" << endl;
		printmatrix(m_tcp);
   }
   else
   {
      for(int i=1;i<=dof;i++)
      {
         pupdate_join (i, pqf[i]);
      }
   }
   send_stream(pcout);
}
void Rchain::slot_tool_RX (double epsilon)
{
   int sol;
   std::ostringstream pcout;
   
   double cosalpha, sinalpha;
   cosalpha = cos(epsilon);
   sinalpha = sin(epsilon);

   Rhmatrix m_tcp;
   m_tcp = m_tcp.identity();
   m_tcp.mat[1][1] = cosalpha;
   m_tcp.mat[1][2] = -sinalpha;
   m_tcp.mat[2][1] = sinalpha;
   m_tcp.mat[2][2] = cosalpha;

   printmatrix(m_tcp);

   m_tcp = mat_tcp() * m_tcp ;

   printmatrix(m_tcp);

   sol = kineinverse_fast(m_tcp, pqf);
   if(sol == -1)
   {
      pcout << "No solution found" << endl;
		printmatrix(m_tcp);
   }
   else
   {
      for(int i=1;i<=dof;i++)
      {
         pupdate_join (i, pqf[i]);
      }
   }
   send_stream(pcout);

}
void Rchain::slot_tool_RY (double epsilon)
{
   int sol;
   std::ostringstream pcout;
   
   double cosalpha, sinalpha;
   cosalpha = cos(epsilon);
   sinalpha = sin(epsilon);

   Rhmatrix m_tcp;
   m_tcp = m_tcp.identity();
   m_tcp.mat[0][0] = cosalpha;
   m_tcp.mat[0][2] = sinalpha;
   m_tcp.mat[2][1] = -sinalpha;
   m_tcp.mat[2][2] = cosalpha;

   printmatrix(m_tcp);

   m_tcp = mat_tcp() * m_tcp ;

   printmatrix(m_tcp);

   sol = kineinverse_fast(m_tcp, pqf);
   if(sol == -1)
   {
      pcout << "No solution found" << endl;
		printmatrix(m_tcp);
   }
   else
   {
      for(int i=1;i<=dof;i++)
      {
         pupdate_join (i, pqf[i]);
      }
   }
   send_stream(pcout);
}
void Rchain::slot_tool_RZ (double epsilon)
{
   int sol;
   std::ostringstream pcout;
   
   double cosalpha, sinalpha;
   cosalpha = cos(epsilon);
   sinalpha = sin(epsilon);

   Rhmatrix m_tcp;
   m_tcp = m_tcp.identity();
   m_tcp.mat[0][0] = cosalpha;
   m_tcp.mat[0][1] = -sinalpha;
   m_tcp.mat[1][0] = sinalpha;
   m_tcp.mat[1][1] = cosalpha;

   printmatrix(m_tcp);

   m_tcp = mat_tcp() * m_tcp;

   printmatrix(m_tcp);

   sol = kineinverse_fast(m_tcp, pqf);
   if(sol == -1)
   {
      pcout << "No solution found" << endl;
		printmatrix(m_tcp);
   }
   else
   {
      for(int i=1;i<=dof;i++)
      {
         pupdate_join (i, pqf[i]);
      }
   }
   send_stream(pcout);
   
}

void Rchain::setMessage(QString message)
{
   message_out = message;
   //emit messageChanged(message);
   console->insertLineAtEnd(message);
   
}       
void Rchain::show_tcp()
{

   tcp();
  
   emit messageChanged("RChain ha anat a home");
}

void Rchain::setconsole (  consoleWidget *pconsole)
{
   console = pconsole;
   widget = true;
}

void Rchain::printmatrix(Rhmatrix &matrix)
{
   std::ostringstream pcout;
   
   pcout << setiosflags (ios::fixed | ios::showpoint);
   pcout << endl;
   for (int i = 0; i < 4; i++)
   {
      for (int j = 0; j < 4; j++)
      {
         pcout << setw (12) << setprecision (3) << matrix.mat[i][j];
      }
      pcout << endl;
   }
   pcout << endl;
   send_stream(pcout);
}

void Rchain::printmatrix(Rhmatrix &matrix, double scale)
{
   std::ostringstream pcout;

   pcout << setiosflags (ios::fixed | ios::showpoint);
   pcout << endl;

   for (int i = 0; i < 3; i++)
   {
      for (int j = 0; j < 3; j++)
      {
         pcout << setw (12) << setprecision (3) << matrix.mat[i][j];
      }
      pcout << setw (12)<< setprecision (3) << scale * matrix.mat[i][3];
      pcout << endl;
   }
   for (int j = 0; j < 4; j++)
   {
      pcout << setw (12) << setprecision (3) << matrix.mat[3][j];
   }
   pcout << endl;
   send_stream(pcout);
   
}

void Rchain::send_stream(std::ostringstream &oss)
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

void Rchain::move (Rhmatrix A_hand)
{
   double *q = new double[dof+1];
   int steps = kineinverse (A_hand, q);
   if (steps != -1)
      move_path_join (q, 0);
   else{
      console->insertLineAtEnd(QString::QString("No inverse found"));
   	printmatrix(A_hand);
	}
}

void Rchain::move (double q[])
{
   double *pfq = new double[dof+1];
   for(int i =0; i<dof ; i++)
      pfq[i+1]=q[i];
      
   move_path_join (pfq, 0);
   delete[] pfq;
}

void Rchain::moves (Rhmatrix A_hand)
{
}

void Rchain::moves (double q[])
{
}

void Rchain::setspeed (double sp)
{
}

void Rchain::value_mat(Rhmatrix A_hand)
{
   std::ostringstream pcout;
   pcout << "----------------------------------------------------" << endl;
   pcout << "Value " << endl;
   send_stream(pcout);
   printmatrix(A_hand, (1/pS));
   pcout << "----------------------------------------------------" << endl;
   send_stream(pcout);
}

