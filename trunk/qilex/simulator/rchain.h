/***************************************************************************
                          rchain.h  -  description
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

#ifndef RCHAIN_H
#define RCHAIN_H

#include "rhmatrix.h"
#include "rhtranslink.h"
#include "rhrteuler.h"
#include "plug.h"

#include <widgets/consolewidget.h>

#include <qdom.h>
#include <qstring.h>
#include <qtimer.h>
#include <qfile.h>


/**
  *@author Leopold Palomo
  */
class Rhmatrix;
class Rhtranslink;
class QTimer;

class cOutputWidget;

struct dh_parameters_joint
{
   unsigned int sigma;
   double theta;
   double stheta;
   double ctheta;
   double salpha;
   double calpha;
   double ai;
   double di;
   double lower_value;
   double upper_value;
   double rank;
   double m_speed;
   double m_acc;
};
        
class Rchain: public QObject
{
   Q_OBJECT
   
   private:
   
      Rhtranslink *pT;
      Rhmatrix *ppremult_T;
      Rhmatrix *ppostmult_T;

      // private temporal vars
      Rhtranslink *ptkT;
      Rhmatrix *ptkpremult_T;
      Rhmatrix *ptkpostmult_T;
              
      bool iniciate;
      bool widget;
      double pS;
      int nsolmax;
   
      void pupdate_T_premult (Rhmatrix ppremult_Tt[], Rhtranslink Tt[],
                  int n);
      void pupdate_T_postmult (Rhmatrix ppostmult_Tt[],
                  Rhtranslink pTt[], int n);
     

      double pF_objective (Rhmatrix Ti, Rhmatrix Th_i, double &qi, int i);

      double pNUM_theta (Rhmatrix Ti, Rhmatrix Th_i, double salphai,
                        double calphai, double ai);
      double pDEN_theta (Rhmatrix Ti, Rhmatrix Th_i, double salphai,
                        double calphai, double ai);
      double ptindep (Rhmatrix Ti, Rhmatrix Th_i, double salphai,
                        double calphai, double ai);
      double pdm (Rhmatrix Ti, Rhmatrix Th_i, double salphai,
                        double calphai);

      void psequence_generator(int combinatio, int comb[], int parts, int combTotal);
      void pupdate_inits(int comb[], double pq[], int parts);
      int pkineinverse_iter(double q[], Rhmatrix pkppremult_T[], Rhtranslink pkT[],
                  Rhmatrix pkpostmult_T[], Rhmatrix SA_hand, int iterations, double pepsilon);

      QTimer *ptimer;
      
      double *pqf; //final position in a movement
      double *pqi; // initial position in a movement
      double *pslope_path; //slope | distance

      double pcurrent_speed;
      double pcurrent_acceleration;
      double pmax_speed;
      double pmax_acceleration;
   
        
      double ptime_f;
      double ptime;

      void setMessage(QString message);

      void printmatrix(Rhmatrix &matrix);
      void printmatrix(Rhmatrix &matrix, double scale);
      void send_stream(std::ostringstream &oss);
      
      Rhrteuler ptrans;
      int pkineinverse(Rhmatrix A_hand, double q[], int piterations, double pepsilon);
      void value_mat(Rhmatrix A_hand);
		
   protected:
      void pupdate_join (int joint, double value);
 
   public:

      void init (int pnewVal);
      void print_config (int n);
      void tcp ();
      void show_tcp();
      double scale ();
      
      Rhmatrix mat_tcp();
      
      void show_status ();
     
      virtual int  kineinverse (Rhmatrix A_hand, double q[]);
      int  kineinverse_fast (Rhmatrix A_hand, double q[]);

      void move (Rhmatrix A_hand);
      void move (double q[]);
      void moves (Rhmatrix A_hand);
      void moves (double q[]);
      
      void setspeed (double sp);

      void kineinverse_all (Rhmatrix A_hand);
      void settranform_Tworld (Rhmatrix T_world);
      void settranform_Ttool (Rhmatrix T_tool);  


      void move_path_join (double qf[], int movement);
      void do_ready();

  
      QString name;
      QString manufacturer;

      
     /** Values of the joins in the home position and parametrics values */
	  /** Vars of each join[n]
      [0][]=sigma: 0 join prismatic, 1 joint revolute
      [1][]=theta in degree
      [2][]=sin(theta)
      [3][]=cos(theta)
      [4][]=sin(alpha)
      [5][]=cos(alpha)
      [6][]=ai
      [7][]=di
      [8][]=lower value of the free var (theta or di) (rad; mm)
      [9][]=upper value of the free var (theta or di) (rad; mm)
      [10][]=rank -> abs(upper - lower)
      [11][]=max speed of the joint (rad/s; mm/s)
      [12][]=max acc of the joint (rad/s²; mm/s²)  */
      
      dh_parameters_joint *dh_parameters;

     // Value of the degree of freedom. If dof=0 the class haven't been declared
      int dof;

     // Home values of the home positions
      double *home;

     // Transform Matrix to the world reference
      Rhmatrix Tworld;

     // Transform Matrix to the tool reference
      Rhmatrix Ttool;

     // Values of the current status of the robot's joins. The revolute joins in rads and the prismatic in mm
      double *status_joint;
      int error;
      plug *list_plug;

      // mode de funcionament
      // si fals, mode de botonera
      // per tant l'actualització de les articulacions
      // venen del panell

      // si cert, mode consola
      // l'actualització de les articulacions, parteix del motor de la simulació

      bool console_mode;
      Rchain ();
      ~Rchain ();

     // Write the home parameters and the kinematic, Denavit-Hatenberg paraperters of the joint i
      void set_dh_pararmeters (unsigned int sigma, double theta, double alphai,
                     double ai, double di, double low_rank, double up_rank,
                     double maxspeed, double maxacc, int i);

     /** Write property of int n and dimension the class. */

      // init_dat //

      // envies un nom d'un fitxer xml amb la configuració del robot i
      // inicialitza la classe
      // retorna un 0 si tot ha estat bé.
      // retorna un 1 si el fitxer no ha pogut ser validat
      // retorna un 2 si el fitxer tenia algun tipus d'error
      
      
      int init_dat (const char *argv);
      int read_element_xml (QDomElement kine_element);

      void setconsole_mode(bool mode);

      void limits(int joint, double &min, double &max);

      QString message_out;
      consoleWidget *console;
      void setconsole (  consoleWidget *console);
      
      public slots:
   
         //remember i goes from 1 to dof
      void setValue(int i, double val);
      void slot_genpas_linear();
      void slot_genpas_linear_parabolic_blend();


      void slot_world_X (double epsilon);
      void slot_world_Y (double epsilon);
      void slot_world_Z (double epsilon);
      void slot_world_RX (double epsilon);
      void slot_world_RY (double epsilon);
      void slot_world_RZ (double epsilon);

      void slot_tool_X (double epsilon);
      void slot_tool_Y (double epsilon);
      void slot_tool_Z (double epsilon);
      void slot_tool_RX (double epsilon);
      void slot_tool_RY (double epsilon);
      void slot_tool_RZ (double epsilon);
           
      signals:
      void messageChanged(QString );
     
                   

/*      signals:
         //remember i goes from 1 to dof
         void valuedChanged(int i, double val);
  */
};

#endif
