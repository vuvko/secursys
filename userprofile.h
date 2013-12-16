#ifndef USERPROFILE_H
#define USERPROFILE_H

#include <QString>
#include <QList>
#include <QHash>

class UserProfile
{
public:
    UserProfile(int uid = 100, const QString &user = "Гость");
    QString user();
    int uid();
    QList<QString> filesRead();
    QList<QString> filesWrite();
    QList<QString> drivesRead();
    QList<QString> drivesWrite();
    QList<QString> dirsRead();
    QList<QString> dirsWrite();

    void setUser(int uid, const QString &user);
    void setFilesRead(const QList<QString> &filesRead);
    void setFilesWrite(const QList<QString> &filesWrite);
    void setDrivesRead(const QList<QString> &drivesRead);
    void setDrivesWrite(const QList<QString> &drivesWrite);
    void setDirsRead(const QList<QString> &dirsRead);
    void setDirsWrite(const QList<QString> &dirsWrite);

    enum {
        NOTHING = 0x0,
        READ = 0x1,
        WRITE = 0x2
    };

private:
    int _uid; // id пользователя
    QString _user; // имя пользователя
    QHash<QString, int> _files; // хэш-таблица с путями до файлов и правами доступа к ним
    QHash<QString, int> _drives;
    QHash<QString, int> _dirs;
};

#endif // USERPROFILE_H
