#ifndef APPHANDLER_H
#define APPHANDLER_H

#include <QWidget>
#include <QThread>
#include <QList>

#include "fileeditor.h"
#include "fsviewer.h"
#include "logindialog.h"
#include "profileviewer.h"
#include "logger.h"

class AppHandler : public QObject
{
    Q_OBJECT
public:
    explicit AppHandler(QObject *parent = 0);
    ~AppHandler();

    void startFS(const QString &path=".");
    void startLogin();

signals:
    void login(bool isCorrect);
    void closeLogin();
    void updateResult(bool isUpdated);

public slots:
    void openFile(const QString &fileName);
    void openFS(const QString &path = ".");
    void openProfile();
    void openLogin();
    void onLoginTry(const QString &userName, const QString &pass);
    void onLogin();
    void onProfileUpdate();

private:
    QByteArray key;
};

#endif // APPHANDLER_H
