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
#include "new_kinematic_chain.h"

#include <qvariant.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qfiledialog.h>
#include <qvalidator.h>


/* 
 *  Constructs a new_kinematic_chain as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
new_kinematic_chain::new_kinematic_chain( QWidget* parent, const char* name, bool modal, WFlags fl , ct_new_kinematic_chain *pdata)
    : QDialog( parent, name, modal, fl )

{
    if ( !name )
      setName( "new_kinematic_chain" );

    QDoubleValidator *valid_double = new QDoubleValidator(-999999, 999999, 3, this, "validator");
    data = pdata; 

    data->accepted = false;

    setSizeGripEnabled( TRUE );

    QWidget* privateLayoutWidget = new QWidget( this, "MainForm" );
    privateLayoutWidget->setGeometry( QRect( 10, 10, 290, 287 ) );
    MainForm = new QGridLayout( privateLayoutWidget, 1, 1, 11, 6, "MainForm"); 

    ModelFile = new QHBoxLayout( 0, 0, 6, "ModelFile"); 

    LabelFileModel = new QLabel( privateLayoutWidget, "LabelFileModel" );
    ModelFile->addWidget( LabelFileModel );

    valueModel_file = new QLineEdit( privateLayoutWidget, "valueModel_file" );
   // valueModel_file->installEventFilter(this);
    ModelFile->addWidget( valueModel_file );

    buttonFileModel = new QPushButton( privateLayoutWidget, "buttonFileModel" );
    buttonFileModel->setMinimumSize( QSize( 20, 30 ) );
    buttonFileModel->setMaximumSize( QSize( 20, 30 ) );
    ModelFile->addWidget( buttonFileModel );

    MainForm->addLayout( ModelFile, 1, 0 );

    ModelName = new QHBoxLayout( 0, 0, 6, "ModelName"); 

    LabelName = new QLabel( privateLayoutWidget, "LabelName" );
    ModelName->addWidget( LabelName );

    textName = new QLineEdit( privateLayoutWidget, "textName" );
   // textName->installEventFilter(this);
    ModelName->addWidget( textName );

    MainForm->addLayout( ModelName, 0, 0 );

    KinematicEngine = new QHBoxLayout( 0, 0, 6, "KinematicEngine"); 

    LabelKinematic_Engine = new QLabel( privateLayoutWidget, "LabelKinematic_Engine" );
    KinematicEngine->addWidget( LabelKinematic_Engine );

    type_engine = new QComboBox( FALSE, privateLayoutWidget, "type_engine" );
    KinematicEngine->addWidget( type_engine );

    MainForm->addLayout( KinematicEngine, 5, 0 );

    Position = new QHBoxLayout( 0, 0, 6, "Position"); 

    LabelX = new QLabel( privateLayoutWidget, "LabelX" );
    Position->addWidget( LabelX );

    valueX = new QLineEdit( privateLayoutWidget, "valueX" );
    Position->addWidget( valueX );
    valueX->setValidator(valid_double);
    //valueX->installEventFilter(this);

    LabelY = new QLabel( privateLayoutWidget, "LabelY" );
    Position->addWidget( LabelY );

    valueY = new QLineEdit( privateLayoutWidget, "valueY" );
    Position->addWidget( valueY );
    valueY->setValidator(valid_double);
    //valueY->installEventFilter(this);

    LabelZ = new QLabel( privateLayoutWidget, "LabelZ" );
    Position->addWidget( LabelZ );

    valueZ = new QLineEdit( privateLayoutWidget, "valueZ" );
    Position->addWidget( valueZ );
    valueZ->setValidator(valid_double);
    //valueZ->installEventFilter(this);

    MainForm->addLayout( Position, 3, 0 );

    Orientation = new QHBoxLayout( 0, 0, 6, "Orientation"); 

    LabelAxes = new QLabel( privateLayoutWidget, "LabelAxes" );
    Orientation->addWidget( LabelAxes );

    axeX = new QLineEdit( privateLayoutWidget, "axeX" );
    axeX->setMinimumSize( QSize( 20, 30 ) );
    Orientation->addWidget( axeX );
    axeX->setValidator(valid_double);
    //axeX->installEventFilter(this);

    axeY = new QLineEdit( privateLayoutWidget, "axeY" );
    axeY->setMinimumSize( QSize( 20, 30 ) );
    Orientation->addWidget( axeY );
    axeY->setValidator(valid_double);
    //axeY->installEventFilter(this);

    axeZ = new QLineEdit( privateLayoutWidget, "axeZ" );
    axeZ->setMinimumSize( QSize( 20, 30 ) );
    axeZ->setValidator(valid_double);
    //axeZ->installEventFilter(this);

    Orientation->addWidget( axeZ );
    QSpacerItem* spacer = new QSpacerItem( 16, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Orientation->addItem( spacer );

    LabelAngle = new QLabel( privateLayoutWidget, "LabelAngle" );
    Orientation->addWidget( LabelAngle );

    valueAngle = new QLineEdit( privateLayoutWidget, "valueAngle" );
    valueAngle->setMinimumSize( QSize( 40, 30 ) );
    Orientation->addWidget( valueAngle );
    valueAngle->setValidator(valid_double);
    //valueAngle->installEventFilter(this);

    textDegree = new QLabel( privateLayoutWidget, "textDegree" );
    Orientation->addWidget( textDegree );

    MainForm->addLayout( Orientation, 4, 0 );

    Bottons = new QHBoxLayout( 0, 0, 6, "Bottons"); 

    buttonHelp = new QPushButton( privateLayoutWidget, "buttonHelp" );
    buttonHelp->setAutoDefault( TRUE );
    Bottons->addWidget( buttonHelp );
    QSpacerItem* spacer_2 = new QSpacerItem( 100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Bottons->addItem( spacer_2 );

    buttonOk = new QPushButton( privateLayoutWidget, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Bottons->addWidget( buttonOk );

    buttonCancel = new QPushButton( privateLayoutWidget, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    Bottons->addWidget( buttonCancel );

    MainForm->addLayout( Bottons, 7, 0 );

    DatFile = new QHBoxLayout( 0, 0, 6, "DatFile"); 

    LabelDat_File = new QLabel( privateLayoutWidget, "LabelDat_File" );
    DatFile->addWidget( LabelDat_File );

    valueDat_file = new QLineEdit( privateLayoutWidget, "valueDat_file" );
    DatFile->addWidget( valueDat_file );
    //DatFile->installEventFilter(this);

    buttonFileEngine = new QPushButton( privateLayoutWidget, "buttonFileEngine" );
    buttonFileEngine->setMinimumSize( QSize( 20, 30 ) );
    buttonFileEngine->setMaximumSize( QSize( 20, 30 ) );
    DatFile->addWidget( buttonFileEngine );

    MainForm->addLayout( DatFile, 6, 0 );

    LabelText_O = new QLabel( privateLayoutWidget, "LabelText_O" );

    MainForm->addWidget( LabelText_O, 2, 0 );


    languageChange();
    resize( QSize(311, 311).expandedTo(minimumSizeHint()) );


      // tab order
    setTabOrder( textName, valueModel_file );
    setTabOrder( valueModel_file, buttonFileModel );
    setTabOrder( buttonFileModel, valueX );
    setTabOrder( valueX, valueY );
    setTabOrder( valueY, valueZ );
    setTabOrder( valueZ, axeX );
    setTabOrder( axeX, axeY );
    setTabOrder( axeY, axeZ );
    setTabOrder( axeZ, valueAngle );
    setTabOrder( valueAngle, type_engine );
    setTabOrder( type_engine, valueDat_file );
    setTabOrder( valueDat_file, buttonFileEngine );
    setTabOrder( buttonFileEngine, buttonHelp );
    setTabOrder( buttonHelp, buttonOk );
    setTabOrder( buttonOk, buttonCancel );


         // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( slot_ok() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonFileEngine, SIGNAL( clicked() ), this, SLOT (slotfiledialogdat()));
    connect( buttonFileModel, SIGNAL( clicked() ), this, SLOT (slotfiledialogmodel()));
}

/*
 *  Destroys the object and frees any allocated resources
 */
new_kinematic_chain::~new_kinematic_chain()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void new_kinematic_chain::languageChange()
{
    setCaption( tr( "New Kinematic Chain" ) );
    LabelFileModel->setText( tr( "Model file:" ) );
    buttonFileModel->setText( tr( "..." ) );
    LabelName->setText( tr( "Name:" ) );
    textName->setText( tr( "newChain" ) );
    LabelKinematic_Engine->setText( tr( "Kinematic Engine:" ) );
    type_engine->clear();
    type_engine->insertItem( tr( "Rchain" ) );
    type_engine->insertItem( tr( "Rchain_hand" ) );
	 type_engine->insertItem( tr( "Rchain_rx" ) );
    LabelX->setText( tr( "X:" ) );
    LabelY->setText( tr( "Y:" ) );
    LabelZ->setText( tr( "Z:" ) );
    LabelAxes->setText( tr( "Axes:" ) );
    valueX->setText( tr( "0" ) );
    valueY->setText( tr( "0" ) );
    valueZ->setText( tr( "0" ) );
    axeX->setText( tr( "0" ) );
    axeY->setText( tr( "0" ) );
    axeZ->setText( tr( "1" ) );
    LabelAngle->setText( tr( "Angle:" ) );
    valueAngle->setText( tr( "0" ) );
    textDegree->setText( tr( "degree" ) );
    buttonHelp->setText( tr( "&Help" ) );
    buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
    LabelDat_File->setText( tr( "Dat File:" ) );
    buttonFileEngine->setText( tr( "..." ) );
    LabelText_O->setText( tr( "Position and Orientation" ) );
}
void new_kinematic_chain::slotfiledialogmodel()
{
   data->QsModelFile = QFileDialog::getOpenFileName("","Models (*.iv; *.wrl)",this,"Open file dialog", "Choose a file" );
   valueModel_file->insert(data->QsModelFile);
}
void new_kinematic_chain::slotfiledialogdat()
{
   data->QsDatFile = QFileDialog::getOpenFileName("","Data model (*.xml)",this,"Open file dialog", "Choose a file" );
   valueDat_file->insert(data->QsDatFile);
}
void new_kinematic_chain::slotupdateValues()
{
   bool ok=true;

   data->QsModelFile = valueModel_file->text();
   data->QsDatFile = valueDat_file->text();
   data->QsName = textName->text();

   data->simulator_engine = type_engine->currentItem();

   data->x = valueX->text().toFloat(&ok);
   data->y = valueY->text().toFloat(&ok);
   data->z = valueZ->text().toFloat(&ok);
   data->axeX = axeX->text().toFloat(&ok);
   data->axeY = axeY->text().toFloat(&ok);
   data->axeZ = axeZ->text().toFloat(&ok);
   data->angle= valueAngle->text().toFloat(&ok);

   data->error_number = 0;

   data->accepted = true;

}

ct_new_kinematic_chain* new_kinematic_chain::query()
{
   ct_new_kinematic_chain *fdata = new ct_new_kinematic_chain;
   bool ok=true;

   fdata->QsModelFile = valueModel_file->text();
   fdata->QsDatFile = valueDat_file->text();
   fdata->QsName = textName->text();

   fdata->simulator_engine = type_engine->currentItem();

   fdata->x = valueX->text().toFloat(&ok);
   fdata->y = valueY->text().toFloat(&ok);
   fdata->z = valueZ->text().toFloat(&ok);
   fdata->axeX = axeX->text().toFloat(&ok);
   fdata->axeY = axeY->text().toFloat(&ok);
   fdata->axeZ = axeZ->text().toFloat(&ok);
   fdata->angle= valueAngle->text().toFloat(&ok);

   fdata->error_number = 0;

   fdata->accepted = true;
   return fdata;
   
}
void new_kinematic_chain::setdata( ct_new_kinematic_chain *fdata )
{
   data = fdata;

}

void new_kinematic_chain::slot_ok()
{
   slotupdateValues();
   accept();
}

void new_kinematic_chain::keyPressEvent(QKeyEvent *ekey)
{
   if(ekey->key()== Qt::Key_Return)
   {
         slotupdateValues();
   }
   
}
