/***************************************************************************
 *   Copyright (C) 2003 by Xavier Sierra                                   *
 *   xavsierra@yahoo.es                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

  #ifndef NEW_GRASPING_OBJECT_H
#define NEW_GRASPING_OBJECT_H

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

class new_grasping_object : public QDialog
{
    Q_OBJECT

public:
    new_grasping_object( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 , ct_new_grasping_object *pdata = 0);
    ~new_grasping_object();

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
    QLabel* LabelDat_File;
    QLineEdit* valueDat_file;
    QPushButton* buttonFileEngine;
    QLabel* LabelText_O;
    ct_new_grasping_object *data;
    ct_new_grasping_object* query();
    void setdata( ct_new_grasping_object *data );
protected:
    QGridLayout* MainForm;
    QHBoxLayout* ModelFile;
    QHBoxLayout* ModelName;
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

#endif // NEW_GRASPING_OBJECT_H
