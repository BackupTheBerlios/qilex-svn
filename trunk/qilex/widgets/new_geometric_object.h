/***************************************************************************
                          new_geometric_object.h  -  description
                             -------------------
    begin                : Fri Mar 21 2003
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

#ifndef NEW_GEOMETRIC_OBJECT_H
#define NEW_GEOMETRIC_OBJECT_H

#include <qvariant.h>
#include <qdialog.h>
#include <qevent.h>
#include <qobject.h>

#include "containers.h"



class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QDoubleValidator;

class new_geometric_object : public QDialog
{
    Q_OBJECT

public:
    new_geometric_object( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 , ct_new_geometric_object *pdata = 0);
    ~new_geometric_object();

    QLabel* LabelFileModel;
    QLineEdit* valueModel_file;
    QPushButton* buttonFileModel;
    QLabel* LabelName;
    QLineEdit* textName;
    QLabel* LabelX;
    QLineEdit* valueX;
    QLabel* LabelY;
    QLineEdit* valueY;
    QLabel* LabelZ;
    QLineEdit* valueZ;
    QLabel* LabelAxes;
    QLineEdit* axeX;
    QLineEdit* axeY;
    QLineEdit* axeZ;
    QLabel* LabelAngle;
    QLineEdit* valueAngle;
    QLabel* textDegree;
    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QLabel* LabelText_O;
    ct_new_geometric_object *data;
    ct_new_geometric_object* query();
    void setdata( ct_new_geometric_object *data );
protected:
    QGridLayout* MainForm;
    QHBoxLayout* ModelFile;
    QHBoxLayout* ModelName;
    QHBoxLayout* Position;
    QHBoxLayout* Orientation;
    QHBoxLayout* Bottons;
    void keyPressEvent( QKeyEvent *ekey);

protected slots:
    virtual void languageChange();
    void slotfiledialogmodel();
    void slotupdateValues();
    void slot_ok();
};

#endif // NEW_GEOMETRIC_OBJECT_H
