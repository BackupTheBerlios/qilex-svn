/***************************************************************************
                          new_kinematic_chain.h  -  description
                             -------------------
    begin                : Mon Feb 10 2003
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

#ifndef NEW_KINEMATIC_CHAIN_H
#define NEW_KINEMATIC_CHAIN_H

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

class new_kinematic_chain : public QDialog
{
    Q_OBJECT

public:
    new_kinematic_chain( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 , ct_new_kinematic_chain *pdata = 0);
    ~new_kinematic_chain();

    QLabel* LabelFileModel;
    QLineEdit* valueModel_file;
    QPushButton* buttonFileModel;
    QLabel* LabelName;
    QLineEdit* textName;
    QLabel* LabelKinematic_Engine;
    QComboBox* type_engine;
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
    QLabel* LabelDat_File;
    QLineEdit* valueDat_file;
    QPushButton* buttonFileEngine;
    QLabel* LabelText_O;
    ct_new_kinematic_chain *data;
    ct_new_kinematic_chain* query();
    void setdata( ct_new_kinematic_chain *data );
protected:
    QGridLayout* MainForm;
    QHBoxLayout* ModelFile;
    QHBoxLayout* ModelName;
    QHBoxLayout* KinematicEngine;
    QHBoxLayout* Position;
    QHBoxLayout* Orientation;
    QHBoxLayout* Bottons;
    QHBoxLayout* DatFile;
    void keyPressEvent( QKeyEvent *ekey);

protected slots:
    virtual void languageChange();
    void slotfiledialogmodel();
    void slotfiledialogdat();
    void slotupdateValues();
    void slot_ok();
};

#endif // NEW_KINEMATIC_CHAIN_H
