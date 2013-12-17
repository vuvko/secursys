#ifndef APPHANDLER_H
#define APPHANDLER_H

#include <QWidget>
#include <QThread>
#include <QList>

#include "fileeditor.h"
#include "fsviewer.h"
#include "logindialog.h"
#include "profileviewer.h"
#include "crypto.h"
#include "profile.h"

class AppHandler : public QObject
{
    Q_OBJECT
public:
    explicit AppHandler(QObject *parent = 0);
    void startFS(const QString &path=".");
    void startLogin();

    QByteArray get_hash(const QByteArray &msg);
    QByteArray get_key(const QString &path);
    QString decode(const QByteArray &msg, const QByteArray &key);
    QByteArray encode(const QString &msg, const QByteArray &key);

    QString userName();
    QString groupName();
    int userId();
    int groupId();

signals:
    void login(bool isCorrect);
    void closeLogin();
    void updateResult(bool isUpdated);

public slots:
    void openFile(const QString &fileName);
    void openFS(const QString &path = ".");
    void openProfile();
    void openLogin();
    void onLoginTry(const QString &user, const QString &pass);
    void onLogin();
    void onProfileUpdate();

private:
    Crypto _crypto;
    Profile _profile;
    QByteArray key;
};

#endif // APPHANDLER_H
