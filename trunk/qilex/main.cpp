/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Thu Feb  6 11:57:28 CET 2003
    copyright            : (C) 2003 by Leopold Palomo
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

#include <qapplication.h>
#include <qfont.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qtranslator.h>

#include "qilex.h"

using namespace std;

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
//  a.setFont(QFont("helvetica", 10));
  QTranslator tor( 0 );
  // set the location where your .qm files are in load() below as the last parameter instead of "."
  // for development, use "/" to use the english original as
  // .qm files are stored in the base project directory.
  tor.load( QString("Qilex") + QTextCodec::locale(), "." );
  a.installTranslator( &tor );
  /* uncomment the following line, if you want a Windows 95 look*/
  //a.setStyle(WindowsStyle);

  QilexApp *qilex=new QilexApp();
  a.setMainWidget(qilex);
  if(argc>1)
    qilex->openAcell(QString::QString(argv[1]));
    
  qilex->show();

  return a.exec();
}



