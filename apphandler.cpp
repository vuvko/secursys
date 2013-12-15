#include "apphandler.h"

AppHandler::AppHandler(QObject *parent) :
    QObject(parent)
{
}

void
AppHandler::startInThread(QThread *thread, QWidget *widget)
{
    widget->moveToThread(thread);
    connect(thread, SIGNAL(started()), widget, SLOT(show()));
    connect(widget, SIGNAL(finished()), thread, SLOT(quit()));
    connect(widget, SIGNAL(finished()), widget, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
}

void
AppHandler::startFS(const QString &path)
{
    QThread *thread = new QThread;
    FSViewer *viewer = new FSViewer(path);
    startInThread(thread, viewer);
    connect(viewer, SIGNAL(openFile(QString)), this, SLOT(openFile(QString)));

    thread->start();
}

void
AppHandler::startLogin()
{
    QThread *thread = new QThread;
    LoginDialog *dialog = new LoginDialog;
    startInThread(thread, dialog);

    thread->start();
}

void
AppHandler::openFile(const QString &fileName)
{
    QThread *thread = new QThread;
    FileEditor *editor = new FileEditor(fileName);
    startInThread(thread, editor);

    thread->start();
}

void
AppHandler::openFS(const QString &path)
{
    startFS(path);
}

void
AppHandler::openLogin()
{
    startLogin();
}
