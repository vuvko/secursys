#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QMainWindow>
#include <QLabel>
#include <QTabWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>

#include "logger.h"
#include "accesscontrol.h"

class ControlPanel : public QMainWindow
{
    Q_OBJECT
public:
    explicit ControlPanel(QWidget *parent = 0);
    
signals:
    
public slots:

private:
    QTabWidget *tabs;
    QWidget *permWidget;
    QGridLayout *permLayout;

    QWidget *regWidget;
    QGridLayout *regLayout;
    QLabel *regLabel;
    QLabel *userLabel;
    QLineEdit *userEdit;
    QLabel *passLabel;
    QLineEdit *passEdit;
    QPushButton *regButton;

    QPlainTextEdit *logEdit;
};

#endif // CONTROLPANEL_H
