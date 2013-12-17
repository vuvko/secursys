#ifndef FILEEDITOR_H
#define FILEEDITOR_H

#include <QApplication>
#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QStatusBar>
#include <QFileInfo>

#include <QDebug>

#include "apphandler.h"

class AppHandler;

class FileEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit FileEditor(const QString &file,
                        const QByteArray key,
                        AppHandler *handler_ = 0,
                        QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void openFile();

private slots:
    bool save();
    void about();
    void documentWasModified();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    bool open();

    bool maybeSave();
    bool saveFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QPlainTextEdit *textEdit;
    QString fileName;

    QMenu *fileMenu;
    QAction *saveAct;
    //QAction *saveAsAct;
    QAction *exitAct;

    QMenu *helpMenu;
    QAction *aboutAct;

    QToolBar *fileToolBar;

    AppHandler *handler;
    QByteArray key;
};

#endif // FILEEDITOR_H
