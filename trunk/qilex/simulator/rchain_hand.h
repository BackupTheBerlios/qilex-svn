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

 #ifndef RCHAIN_HAND_H
#define RCHAIN_HAND_H

#include "rhmatrix.h"
#include "rhtranslink.h"
#include "rhrteuler.h"
#include "plug.h"
#include <widgets/consolewidget.h>

#include <qdom.h>
#include <qstring.h>
#include <qtimer.h>
#include <qfile.h>


class Rhmatrix;
class Rhtranslink;
class QTimer;
class cOutputWidget;
struct dh_parameters_joint2
      {
        int sigma;
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
    
class Rchain_hand: public QObject
{
    Q_OBJECT

   private:
   
      Rhtranslink **pT;
      Rhmatrix **ppremult_T;
      Rhmatrix **ppostmult_T;

      // private temporal vars
      Rhtranslink **ptkT;
      Rhmatrix **ptkpremult_T;
      Rhmatrix **ptkpostmult_T;

      //bool iniciate;
      bool widget;
      int nsolmax;
      QTimer *ptimer;

      double **pqf; //final position in a movement
      double **pqi; // initial position in a movement
      double **pslope_path; //slope | distance

      double p_speed;
      double p_acceleration;

      double ptime_f;
      double ptime;
      

      void pupdate_T_premult (Rhmatrix ppremult_Tt[], Rhtranslink Tt[],
                  int n, int k);
      void pupdate_T_postmult (Rhmatrix ppostmult_Tt[],
                  Rhtranslink pTt[], int n);
     
      double pF_objective (Rhmatrix Ti, Rhmatrix Th_i, double &qi, int i, int k);
      double pF_objective_without_ranks (Rhmatrix Ti, Rhmatrix Th_i, double &qi, int i, int k);
      double pF_calculate (Rhmatrix Ti, Rhmatrix Th_i, double &qi, int i, int k);
      double pF_objective_arm (Rhmatrix *pTh[], Rhmatrix *pkpostmult_T[], double **qi, int i);
      double pF_objective_arm_two (Rhmatrix *pTh[], Rhmatrix *pkpostmult_T[], double **qi, int i, bool finger1, bool finger2, bool finger3, bool finger4);
      double pF_objective_arm_without_ranks (Rhmatrix *pTh[], Rhmatrix *pkpostmult_T[], double **qi, int i);
      double pF_objective_joint_six (Rhmatrix *pTh[], Rhmatrix *pkpostmult_T[], double **qi, int i);
      double pF_objective_joint_six_two (Rhmatrix *pTh[], Rhmatrix *pkpostmult_T[], double **qi, int i, bool finger1, bool finger2, bool finger3, bool finger4);
      
      double pNUM_theta (Rhmatrix Ti, Rhmatrix Th_i, double salphai,
                        double calphai, double ai);
      double pDEN_theta (Rhmatrix Ti, Rhmatrix Th_i, double salphai,
                        double calphai, double ai);
      double ptindep (Rhmatrix Ti, Rhmatrix Th_i, double salphai,
                        double calphai, double ai);
      double pdm (Rhmatrix Ti, Rhmatrix Th_i, double salphai,
                        double calphai);
      double scale (int k);

      //void psequence_generator(int combinatio, int *combi[], int parts);
      //void pupdate_inits(int *comb[], double *pq[], int parts);
      int pkineinverse_iter(double *q[], Rhmatrix *pkppremult_T[], Rhtranslink *pkT[],
                  Rhmatrix *pkpostmult_T[], Rhmatrix SA_hand[], int iterations, double pepsilon);
      int pkineinverse_iter_without_ranks(double *q[], Rhmatrix *pkppremult_T[], Rhtranslink *pkT[],
                  Rhmatrix *pkpostmult_T[], Rhmatrix SA_hand[], int iterations, double pepsilon);
      int pkineinverse_iter_hand_1(double **q, Rhmatrix *pkppremult_T[], Rhtranslink *pkT[],
                  Rhmatrix *pkpostmult_T[], Rhmatrix SA_hand[]);
      int pkineinverse_iter_hand_2(double **q, Rhmatrix *pkppremult_T[], Rhtranslink *pkT[],
                  Rhmatrix *pkpostmult_T[], Rhmatrix SA_hand[], int iterations, double pepsilon);
      int pkineinverse_iter_one_chain(double *q, Rhmatrix *pkpremult_T, Rhtranslink *pkT,
                  Rhmatrix  *pkpostmult_T, Rhmatrix A_hand, int piterations, double pepsilon, int k);
      int pkineinverse_iter_two_chains(double **q, Rhmatrix *pkpremult_T[], Rhtranslink *pkT[],
                  Rhmatrix *pkpostmult_T[], Rhmatrix SA_hand[], int iterations, double pepsilon, bool finger1, bool finger2, bool finger3, bool finger4);
      int pkineinverse_iter_three_chains(double **q, Rhmatrix *pkpremult_T[], Rhtranslink *pkT[],
                  Rhmatrix *pkpostmult_T[], Rhmatrix SA_hand[], bool finger1, bool finger2, bool finger3, bool finger4);
      double pdistance (Rhmatrix Ttcp, Rhmatrix Th);

      void setMessage(QString message);
      void printmatrix(Rhmatrix &matrix);
      void printmatrix(Rhmatrix &matrix, double scale);


   protected:
      void pupdate_join (int join, double value, int k);
      void pupdate_join_arm (int join, double value);
 
   public:

      //QString name;
      //QString manufacturer;
      double *pS;
     /** Values of the joins in the home position and parametrics values */
	  /** Vars of each join[n]
      [0][]=sigma: 0 join prismatic, 1 join revolute
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
      [11][]=max speed of the join (rad/s; mm/s)
      [12][]=max acc of the join (rad/s²; mm/s²)  */

      dh_parameters_joint2 **dh_parameters;

     // Value of the degree of freedom. If dof=0 the class haven't been declared
      int *dof;

     //Number of the kinematic chains
      int n_chain;

      //Number of articulations that belong to different chains
      int n_same;

      //Number of all joints
      int n_joint;

     // Home values of the home positions
      double **home;
      //double temp;

     // Transform Matrix of the world reference
      Rhmatrix Tworld;

     // Transform Matrix of the tool references
      Rhmatrix *Ttool;

     // Values of the current status of the robot's joins. The revolute joins in rads and the prismatic in mm
      double **status_join;
      int error;
      plug *list_plug;
      double *delta;
      // mode de funcionament
      // si fals, mode de botonera
      // per tant l'actualització de les articulacions
      // venen del panell

      // si cert, mode consola
      // l'actualització de les articulacions, parteix del motor de la simulaci

      bool console_mode;
      bool stop_kinematics;
      QString message_out;
      consoleWidget *console;
      int init_dat (const char *argv);
      int read_element_xml (QDomElement kine_element);
      void send_stream(std::ostringstream &oss);
      
      void move_join (int join, double value, int k);
      void move_inc_join (int join, double value, int k);
      void mhome (int k);
      void init (int pnewVal,int k);
      void print_config (int n, int k);
      void tcp (int k);
      void tcp_arm (int k);
      void show_tcp(int k);
      
      Rhmatrix mat_tcp_arm(int k);
      Rhmatrix mat_tcp(int k);
      
      void show_status (int k);
      void rank (int n, int k);
      int  kineinverse (Rhmatrix A_hand, double *q, int k);
      int  kineinverse_hand (Rhmatrix A_hand[], double **q);
      int  kineinverse_hand_retry (Rhmatrix A_hand[], double **q);
      int  kineinverse_two_chains(Rhmatrix A_hand[], double **q, bool finger1, bool finger2, bool finger, bool finger4, int chains_selected);
      
      //void evaluate_initialpoint(Rhmatrix A_hand[]);

      //void kineinverse_all (Rhmatrix A_hand[]);
      void settranform_Tworld (Rhmatrix T_world);
      void settranform_Ttool (Rhmatrix T_tool, int k);  


      void move_path_join (double **qf, int movement);
      void do_ready();
                    
      Rchain_hand ();
      ~Rchain_hand ();

     // Write the home parameters and the kinematic, Denavit-Hatenberg paraperters of the join i
      void set_dh_pararmeters (int sigma, double theta, double alphai,
                     double ai, double di, double low_rank, double up_rank,
                     double maxspeed, double maxacc, int i, int k);

      void num_chains (int cadenes);
      /*double solver_equation_sum (double thetai0, double R, double C, double t_NUM, double t_DEN);
      double solver_equation_mul (double thetai0, double R, double C, double t_NUM, double t_DEN, double dis,
              double t_indep, double t_dm);*/

     /** Write property of int n and dimension the class. */

      // init_dat //

      // envies un nom d'un fitxer xml amb la configuració del robot i
      // inicialitza la classe
      // retorna un 0 si tot ha estat bé.
      // retorna un 1 si el fitxer no ha pogut ser validat
      // retorna un 2 si el fitxer tenia algun tipus d'error
      
      
      //int init_dat (const char *argv);

      void setconsole_mode(bool mode);

      void limits(int join, double &min, double &max, int k);

      QString name;

      void setconsole (  consoleWidget *console);
      
      public slots:
   
         //remember i goes from 1 to dof
      void setValue(int i, double val);
      void slot_genpas_linear();
      void slot_stop_kinematics(bool stop);
    
      signals:
      void messageChanged(QString );
     
                   

/*      signals:
         //remember i goes from 1 to dof
         void valuedChanged(int i, double val);
  */
};

#endif
