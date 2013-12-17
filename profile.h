#ifndef ROFILE_H
#define ROFILE_H

#include <QString>
#include <QList>
#include <QHash>

class Profile
{
public:
    Profile();
    QList<QString> filesRead();
    QList<QString> filesWrite();
    QList<QString> drivesRead();
    QList<QString> drivesWrite();
    QList<QString> dirsRead();
    QList<QString> dirsWrite();
    QList<QString> programsExec();
    QHash<QString, int> files();
    QHash<QString, int> drives();
    QHash<QString, int> dirs();
    QHash<QString, int> programs();

    void setUser(int uid, const QString &user);
    void setGroup(int gid, const QString &group);
    void setFilesRead(const QList<QString> &filesRead);
    void setFilesWrite(const QList<QString> &filesWrite);
    void setDrivesRead(const QList<QString> &drivesRead);
    void setDrivesWrite(const QList<QString> &drivesWrite);
    void setDirsRead(const QList<QString> &dirsRead);
    void setDirsWrite(const QList<QString> &dirsWrite);
    void setProgramsExec(const QList<QString> &programsExec);
    void setFiles(const QHash<QString, int> &files);
    void setDrives(const QHash<QString, int> &drives);
    void setDirs(const QHash<QString, int> &dirs);
    void setPrograms(const QHash<QString, int> &programs);

    bool canReadFile(const QString &path);
    bool canWriteFile(const QString &path);
    bool canReadDir(const QString &path);
    bool canWriteDir(const QString &path);
    bool canReadDrive(const QString &path);
    bool canWriteDrive(const QString &path);

    enum {
        NOTHING = 0x0,
        READ = 0x1,
        WRITE = 0x2,
        EXEC = 0x4
    };

private:
    int _uid;
    int _gid;
    QString _user;
    QString _group;
    QHash<QString, int> _files; // хэш-таблица с путями до файлов и правами доступа к ним
    QHash<QString, int> _drives;
    QHash<QString, int> _dirs;
    QHash<QString, int> _programs;
};

#endif // ROFILE_H
