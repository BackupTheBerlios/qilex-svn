//
// C++ Interface: netthread
//
// Description: 
//
//
// Author: Leopold Palomo,,, <palomo@lira>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NETTHREAD_H
#define NETTHREAD_H

#include <qthread.h>

#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
             
#include "simulator/rchain_rx.h"
#include "panel_control_rx.h"


/**
@author Leopold Palomo,,,
*/
class panelControlRx;

class netthread : public QThread
{
public:
   netthread();
	~netthread();
	panelControlRx *panel;
	virtual void run();
	void init (panelControlRx *panel);

};

#endif
