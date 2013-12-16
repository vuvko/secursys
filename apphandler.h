#ifndef APPHANDLER_H
#define APPHANDLER_H

#include <QWidget>
#include <QThread>
#include <QList>

#include "fileeditor.h"
#include "fsviewer.h"
#include "logindialog.h"

class AppHandler : public QObject
{
    Q_OBJECT
public:
    explicit AppHandler(QObject *parent = 0);
    void startFS(const QString &path=".");
    void startLogin();

signals:
    void login(bool isCorrect);
    void closeLogin();

public slots:
    void openFile(const QString &fileName);
    void openFS(const QString &path = ".");
    void openLogin();
    void onLoginTry(const QString &user, const QString &pass);
    void onLogin();
};

#endif // APPHANDLER_H
