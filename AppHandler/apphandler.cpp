#include "apphandler.h"

AppHandler::AppHandler(QObject *parent) :
    QObject(parent)
{
}

void
AppHandler::startFS(const QString &path)
{
    QThread *thread = new QThread;
    FSViewer *viewer = new FSViewer(path);
    viewer->moveToThread(thread);

    connect(thread, SIGNAL(started()), viewer, SLOT(openDir()));
    connect(viewer, SIGNAL(finished()), thread, SLOT(quit()));
    connect(viewer, SIGNAL(finished()), viewer, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(viewer, SIGNAL(openFile(QString)), this, SLOT(openFile(QString)));

    thread->start();
}

void
AppHandler::openFile(const QString &fileName)
{
    QThread *thread = new QThread;
    FileEditor *editor = new FileEditor(fileName);
    editor->moveToThread(thread);

    connect(thread, SIGNAL(started()), editor, SLOT(openFile()));
    connect(editor, SIGNAL(finished()), thread, SLOT(quit()));
    connect(editor, SIGNAL(finished()), editor, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

void
AppHandler::openFS(const QString &path)
{
    startFS(path);
}
