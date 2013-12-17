#include "apphandler.h"

AppHandler::AppHandler(QObject *parent) :
    QObject(parent) {}

void
AppHandler::startFS(const QString &path)
{
    FSViewer *viewer = new FSViewer(path);

    connect(viewer, SIGNAL(openFile(QString)), this, SLOT(openFile(QString)));
    connect(viewer, SIGNAL(openProfile()), this, SLOT(openProfile()));

    viewer->show();
}

void
AppHandler::startLogin()
{
    LoginDialog *dialog = new LoginDialog;

    // здесь надо перевести сигналы в надлежащий модуль
    connect(dialog, SIGNAL(loginTry(QString,QString)), this, SLOT(onLoginTry(QString,QString)));
    connect(this, SIGNAL(login(bool)), dialog, SLOT(loginResult(bool)));
    connect(dialog, SIGNAL(login()), this, SLOT(onLogin()));
    connect(this, SIGNAL(closeLogin()), dialog, SLOT(close()));

    dialog->show();
}

void
AppHandler::openFile(const QString &fileName)
{
    FileEditor *editor = new FileEditor(fileName);
    editor->show();
}

void
AppHandler::openProfile()
{
    ProfileViewer *viewer = new ProfileViewer;
    connect(viewer, SIGNAL(update()), this, SLOT(onProfileUpdate()));
    connect(this, SIGNAL(updateResult(bool)), viewer, SLOT(updateResult(bool)));
    viewer->show();
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

void
AppHandler::onLoginTry(const QString &user, const QString &pass)
{
    emit login(true);
}

void
AppHandler::onLogin()
{
    emit closeLogin();
    startFS();
}

void
AppHandler::onProfileUpdate()
{
    emit updateResult(true);
}
