/***************************************************************************
                          qvalparser.cpp  -  description
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

#include "qvalparser.h"


QvalParser::QvalParser(Rchain *kinechain)
         : valParser (kinechain)
{
}
 
void QvalParser::set_stdin(QString line)
{
	string linia(QString::QString(line.ascii()).ascii());
   istringstream inn(linia);
   bool ok = parse(&inn);

   if( ok ) {
     emit set_stdout(QString::QString("OK!"));
   } else {
     emit set_stdout(QString::QString("Syntax error"));
   } 

}

int QvalParser::yyerror(string s)
{
   emit set_stdout(QString::QString(s.c_str()));
   return 0;
}

void QvalParser::message(string s)
{
   emit set_stdout(QString::QString(s.c_str()));
}
QvalParser::~QvalParser(){
}

