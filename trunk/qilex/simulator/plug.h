/***************************************************************************
                          plug.h  -  description
                             -------------------
    begin                : Tue Feb 25 2003
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

#ifndef PLUG_H
#define PLUG_H

#include <qobject.h>
/**
  *@author Leopold Palomo
  */


class plug : public QObject  {
     Q_OBJECT
   public:
      plug();
      ~plug();
      void setIndex(int ind);    
      double pval;
   public slots:
      void setValue(double);
   signals:
      void valueChanged (double);
      void valueChangedfull(int , double);
   private:

      int index;
    
};

#endif
