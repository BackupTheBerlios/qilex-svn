//
// C++ Implementation: rchain_rx
//
// Description: 
//
//
// Author: Leopold Palomo,,, <palomo@lira>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "rchain_rx.h"

Rchain_rx::Rchain_rx(): Rchain()
{
	forward = true;
	elbow_up = true;
	flip = false;
	l1 = 0.;
	l2 = 0.;
	l3 = 0.;
	l6 = 0.;
	for (int i = 0; i< 6; i++)
		map[i] = 0.;
}

Rchain_rx::~Rchain_rx()
{
}

int  Rchain_rx::kineinverse (Rhmatrix A_hand, double q[])
{
	int iter = 0;
	/*double bsPee_x = A_hand.mat[0][3];
	double bsPee_y = A_hand.mat[1][3];
	double bsPee_z = A_hand.mat[2][3];
	
	double bsPwr_x = bsPee_x - A_hand.mat[0][2] * l6;
	double bsPwr_y = bsPee_y - A_hand.mat[1][2] * l6;
	double bsPwr_z = bsPee_z - A_hand.mat[2][2] * l6;
	
	double dh = sqrt(bsPwr_x * bsPwr_x + bsPwr_y * bsPwr_y);
	double dv = bsPwr_z - l1;

	double theta[6], c23;		
	Rhmatrix R4bs, R46;
	//theta 1
	if (forward)
	{
		theta[0] = atan2(bsPwr_x, bsPwr_y);
	}
	else{
		theta[0] = atan2(bsPwr_x, -bsPwr_y);
	}
	   
	theta[2] = acos ((dh * dh + dv * dv - l2 * l2 - l3*l3)/2.0 * l2 * l3);	

	//theta 3
	
	if(!elbow_up)
	{
		theta[2] = -theta[2];
	}
	
	//theta 2

	double alpha = atan2(l3*sin(q[2]), l2 * l3*cos(q[2]));
	theta[1] = atan2(dh,dv) - alpha;	
	q[1] = map[1] + theta[1];
	
	//
	c23= cos(theta[1] + theta[2]);
	R4bs.mat[0][0] = cos(theta[0]);
	R4bs.mat[1][0] = sin(theta[0]);
	R4bs.mat[2][0] =	0;
			
	R4bs.mat[0][1] = -R4bs.mat[1][0] * c23;
	R4bs.mat[1][1] = R4bs.mat[0][0] * c23;
	R4bs.mat[2][1] =	sin(theta[1] + theta[2]);
	
	R4bs.mat[0][2] = R4bs.mat[1][0] * R4bs.mat[2][1];
	R4bs.mat[1][2] = -R4bs.mat[0][0] * R4bs.mat[2][1];
	R4bs.mat[2][2] =	c23;
	
	R46 = R4bs.inverse() * A_hand;
	
	theta[3] = atan2(R46.mat[0][2], -R46.mat[1][2]);
	theta[4] = atan2(R46.mat[1][2] * cos(theta[3]) + R46.mat[0][2]* sin(theta[3]), -R46.mat[2][2]);
	theta[5] = atan2(R46.mat[2][0], R46.mat[2][1]);	
	if(flip)
	{
		theta[3] = theta[3] + M_PI;
		theta[4] = -theta[4];
		theta[5] = theta[5] + M_PI;
	}
	*/
   for(int i = 0; i < 6; i++)
		//q[i] = map[i] + theta[i];	
		q[i] = map[i];
		
	return iter;
}



void Rchain_rx::update_conf()
{
	for(int i = 0; i<6;i++)
		map[i]= map[i] - home[i];
	
	l1 = 0;
	l2 = 450.0;
	l3 = 450.0;
	l6 = 85.0;	
}
