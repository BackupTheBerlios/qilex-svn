/***************************************************************************
                          qvalparser.h  -  description
                             -------------------
    begin                : dg abr 13 2003
    copyright            : (C) 2003 by Leopold Palomo Avellaneda
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

#ifndef QVALPARSER_H
#define QVALPARSER_H

#include "interpret/valplus.h"
#include <simulator/rchain.h>
#include <simulator/rhmatrix.h>
#include <functions.h>

#include <iostream>
#include <sstream>
#include <string>



#include <FlexLexer.h>

/**
  *@author Leopold Palomo Avellaneda
  */
class Rchain;
class RhtransRotTras;
class QString;
  
class QvalParser : public QObject, public valParser
{
   Q_OBJECT
private:
public:
   int yyerror(string m);
   void message(string m);       
  
   QvalParser(Rchain *kinechain);
   ~QvalParser();

public slots:
   void set_stdin(QString line);
signals:
   void set_stdout(QString line);
   
};

#endif

