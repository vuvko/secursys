#ifndef AccessControl_H
#define AccessControl_H

#include <QString>
#include <QList>
#include <QHash>
#include <QMessageBox>

class Profile;

#define ROOT_UID 0

typedef enum {
    ACCESS_NOTHING = 0x0,
    ACCESS_READ    = 0x1,
    ACCESS_WRITE   = 0x2,
    ACCESS_EXEC    = 0x4
} ModeComponent;

typedef enum {
    ROLE_NOTHING             = 0x0,
    ROLE_CONFIDENTIAL        = 0x1,
    ROLE_STRICT_CONFIDENTIAL = 0x2
} Role;

struct AccessObject
{
    // default value
    AccessObject(QString path = QString());

    QString path;

    int uid;
    int gid;

    // Bitwise or'ed ModeComponents.
    int userMode;
    int groupMode;
    int othersMode;

    Role role;

    bool operator==(const AccessObject &obj) const
    {
        if (this == &obj)
            return true;

        return path == obj.path;
    }
};

struct User
{
    int uid;
    int gid;
    QString name;
    QByteArray passHash;
    Role maxRole;
};

struct Group
{
    int gid;
    QString name;
};

QDataStream &operator<<(QDataStream &out, const AccessObject &obj);
QDataStream &operator>>(QDataStream &in, AccessObject &obj);
QDataStream &operator<<(QDataStream &out, const User &obj);
QDataStream &operator>>(QDataStream &in, User &obj);
QDataStream &operator<<(QDataStream &out, const Group &obj);
QDataStream &operator>>(QDataStream &in, Group &obj);

class AppHandler;

class AccessControl
{
    friend class Profile;
    friend class AccessAdmin;
    friend class ControlPanel;

public:
    static AccessControl &getInstance();

    // Return uid on success, -1 on failure.
    int checkLogin(QString userName, QString userPass) const;

    // path can be relative with respect Profile::getPWD().
    bool readFile(QString path, QByteArray &to);
    bool writeFile(QString path, QByteArray &data);

    // path can be relative with respect Profile::getPWD().
    bool cd(QString path);
    bool mkdir(QString path);
    bool rmdir(QString path);
    bool rm(QString path);
    bool exec(QString path);

    // Root only.
    // path can be relative with respect Profile::getPWD().
    bool check(QString path) const;

    // path can be relative with respect Profile::getPWD().
    Role getRoleFile(QString path);
    Role getRoleDrive(QString path);
    Role getRoleDir(QString path);
    Role getRoleProgram(QString path);

    void dbInit();
    void dbRead();
    void dbWrite();

private:
    AccessControl();

    // Not implement, forbid usage.
    AccessControl(const AccessControl &);
    void operator=(const AccessControl &);

    // apath -- absolute path to file.
    QString getDrive(QString apath);

    QByteArray getDBKey() const;
    QByteArray getUserKey() const;
    QByteArray getRootKey() const;

    bool checkAccess(const QList<AccessObject> *collection,
        QString apath, int mode) const;

    // Check access for current user.
    // apath -- absolute path to file.
    bool checkAccessFile(QString apath, int mode) const;
    bool checkAccessDrive(QString apath, int mode) const;
    bool checkAccessDir(QString apath, int mode) const;
    bool checkAccessProgramExec(QString apath) const;

    // path can be relative with respect Profile::getPWD().
    Role getRole(const QList<AccessObject> *collection, QString path);

    // For admin or for newly created directories.
    // apath -- absolute path to file.
    void setDefaultAccessFile(QString apath);
    void setDefaultAccessDir(QString apath);

    bool readFileInt(QString apath, QByteArray &to, QByteArray userKey);
    bool readFileInt(QString apath, QByteArray &to);   // use getUserKey().
    bool writeFileInt(QString apath, QByteArray &data, QByteArray userKey);
    bool writeFileInt(QString apath, QByteArray &data); // use getUserKey().

    static QByteArray calcHashFile(QString apath);

    bool isDirEmpty(QString apath) const;

    // Fields
    // ======

    // Access objects.
    QList<AccessObject> allFiles;
    QList<AccessObject> allDrives;
    QList<AccessObject> allDirs;
    QList<AccessObject> allPrograms;

    // File hashes.
    QHash<QString, QByteArray> allHashes;

    // Users and groups.
    QList<User> allUsers;
    QList<Group> allGroups;
};

#endif // AccessControl_H
