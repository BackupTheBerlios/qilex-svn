//
// C++ Interface: rchain_rx
//
// Description: 
//
//
// Author: Leopold Palomo,,, <palomo@lira>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RCHAIN_RX_H
#define RCHAIN_RX_H

#include "rchain.h"
                   

/**
@author Leopold Palomo,,,
*/


class Rchain_rx : public Rchain
{

public:
    Rchain_rx();

    ~Rchain_rx();

//   int  kineinverse (Rhmatrix A_hand, double q[]);
	void update_conf();
	int  kineinverse (Rhmatrix A_hand, double q[]);
	friend class Rchain;	
		 
private:
	bool forward, elbow_up, flip;
	double l1, l2, l3, l6;
	double map[6];

	
};

#endif
