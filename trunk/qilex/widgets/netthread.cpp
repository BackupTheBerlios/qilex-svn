//
// C++ Implementation: netthread
//
// Description: 
//
//
// Author: Leopold Palomo,,, <palomo@lira>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "netthread.h"

netthread::netthread()
{
}

void netthread::init (panelControlRx *ppanel)
{
	panel = ppanel;	
}		 
netthread::~netthread()
{
}

void netthread::run()
{
	//  printf("Proceso:  %i\n\n",*((int*)data));
  	int port=8000;
  	struct sockaddr_in sin;
  	struct sockaddr_in pin;
  	char *interface=NULL;
  	int sock_descriptor;
  	int temp_sock_descriptor;
  	int address_size;
  	char buf[250];
  	int i, len;
	QString text;  
	
	  
  	sock_descriptor=socket(AF_INET, SOCK_STREAM, 0);
   
  	if (sock_descriptor == -1) {
  		perror("Some errors have been occurred calling creating socket");
		text  = "Some error have been occured calling creating socket";
		panel->TextBrowser->insertLineAtEnd(text);
		pthread_exit(NULL);
	}
	
  	bzero(&sin, sizeof(sin));
  	sin.sin_family = AF_INET;
  	sin.sin_addr.s_addr = INADDR_ANY;
  	sin.sin_port = htons(port);
  
  	if (bind(sock_descriptor, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
  		perror("Some errors have been occurred calling joining socket");
		panel->TextBrowser->insertLineAtEnd(QString::QString("Some error have been occured calling joining socket"));
	}
  
  	if(listen(sock_descriptor, 5) == -1) {
  		perror("Some errors have been occurred when listening");
		panel->TextBrowser->insertLineAtEnd(QString::QString("Some errors have been occurred when listening"));
		
	}
	text = "'Listening' on port ";
	text.append(port);
	text.append(" protocol TCP");
  	panel->TextBrowser->insertLineAtEnd(text);
	
	//printf ("'Listening' on port %i, protocol TCP\n\n",port );
  
  	int a=1;
   
  	while(a != 0) {
  		temp_sock_descriptor = accept(sock_descriptor, (struct sockaddr *)&pin, (socklen_t *) &address_size);
		if (temp_sock_descriptor == -1) {
			perror("llamada para aceptar");
			a=1;
		}
	
		if (recv(temp_sock_descriptor, buf, sizeof(buf), 0) == -1){
			perror("llamada para recibir");
			a=1;
		}		
	
	//printf("recibido del cliente: %s\n",buf);
		
		if (send(temp_sock_descriptor, buf, sizeof(buf), 0) == -1){
			perror("llamada para enviar");
			a=1;
		}
		
		double pt[5];
	 	int i=0;
	 	char * valor;
         
	  	valor = strtok (buf," ");
	 
   	panel->TextBrowser->insertLineAtEnd(QString::QString("Values:    "));
					 
					 
		while (valor != NULL)
  	{
	  pt[i]=atof(valor);  
	  text = QString( "" ).number ( pt[i], 'g', 16 );
	  panel->TextBrowser->insertLineAtEnd(text);	                                                                    	
	  //std::cout << std::endl << setw (16) << setprecision(16) << pt[i]<< std::endl;
	  i++;	
   	  valor = strtok (NULL, " ");	 
	  }	
	a=1;	
	panel->close(temp_sock_descriptor);
	}
	
	
}

