/****************************************************************************
** Form interface generated from reading ui file 'provawidget.ui'
**
** Created: dl abr 7 15:40:34 2003
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.2   edited Dec 19 11:45 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <qvariant.h>
#include <qwidget.h>
#include <qlabel.h>


class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QTextEdit;
class QLineEdit;
class QLabel;

class consoleWidget : public QWidget
{
    Q_OBJECT

public:
    consoleWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~consoleWidget();

   QTextEdit* console;

   QLabel* prompt;
   
   QLineEdit* input;

protected:
   QVBoxLayout* consoleWidgetLayout;
   QHBoxLayout* inputLayout;
               
   void keyPressEvent(QKeyEvent *ekey);
public slots:
   void insertLineAtEnd(QString line);
signals:
   void keyboard_input(QString instruc);

};

#endif // CONSOLEWIDGET_H
