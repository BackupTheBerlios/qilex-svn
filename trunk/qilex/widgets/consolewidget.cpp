/****************************************************************************
** Form implementation generated from reading ui file 'provawidget.ui'
**
** Created: dl abr 7 15:41:04 2003
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.2   edited Dec 19 11:45 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "consolewidget.h"

#include <qvariant.h>
#include <qtextedit.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a consoleWidget as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
consoleWidget::consoleWidget( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
   if ( !name )
      setName( "consoleWidget" );
   consoleWidgetLayout = new QVBoxLayout( this, -2, -1, "consoleWidgetLayout"); 

   console = new QTextEdit( this, "console" );
   consoleWidgetLayout->addWidget( console );

  
   input = new QLineEdit( this, "input" );
   input->setFrame(false);   

   prompt =   new QLabel( this, "prompt" );
   prompt->setText("kinechain>");
   prompt->setIndent(5);
   prompt->setPaletteBackgroundColor (input->paletteBackgroundColor ());

   inputLayout = new QHBoxLayout( consoleWidgetLayout, 0,  "inputLayout");
 //  inputLayout = new QHBoxLayout( this, -1,  -1, "inputLayout");
       
   inputLayout->addWidget (prompt);
   inputLayout->addWidget (input);   
                                                                                                                    
 //  consoleWidgetLayout->addLayout (inputLayout) ;
      
   console->setReadOnly(true);
   
   //console->setTextFormat(Qt::LogText);
  
   resize( QSize(255, 230).expandedTo(minimumSizeHint()) );
   
}

/*
 *  Destroys the object and frees any allocated resources
 */
consoleWidget::~consoleWidget()
{
    // no need to delete child widgets, Qt does it all for us
}

void consoleWidget::insertLineAtEnd(QString line)
{
   console->append(line);
}

void consoleWidget::keyPressEvent(QKeyEvent *ekey)
{

   console->moveCursor(QTextEdit::MoveEnd, false);
   if ( ekey->key() == Key_Enter || ekey->key() == Key_Return )
   {
      console->append(prompt->text() + " " + input->text());
      emit keyboard_input( input->text());
      input->clear();
   }
}
