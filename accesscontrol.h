#ifndef AccessControl_H
#define AccessControl_H

#include <QString>
#include <QList>
#include <QHash>

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
    AccessObject(QString aPath)
    {
        path = aPath;
    }

    AccessObject(QString path,
        int uid, int gid,
        int userMode, int groupMode, int othersMode,
        Role role)
    {
        this->path = path;
        this->uid = uid;
        this->gid = gid;
        this->userMode = userMode;
        this->groupMode = groupMode;
        this->othersMode = othersMode;
        this->role = role;
    }

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
};

struct Group
{
    int gid;
    QString name;
};

class AppHandler;

class AccessControl
{
    friend class Profile;

public:
    static AccessControl &getInstance();

    // Return uid on success, -1 on failure.
    int checkLogin(QString userName, QString userPass) const;

    // path can be relative with respect Profile::getPWD().
    QString readFile(QString path);
    void writeFile(QString path, QString data);

    // path can be relative with respect Profile::getPWD().
    void cd(QString path);
    void mkdir(QString path);
    void rmdir(QString path);
    void rm(QString path);
    void exec(QString path);

    // Root only.
    // path can be relative with respect Profile::getPWD().
    bool check(QString path) const;

private:
    AccessControl();

    // Not implement, forbid usage.
    AccessControl(const AccessControl &);
    void operator=(const AccessControl &);

    void dbRead();
    void dbWrite() const;

    QByteArray getUserKey();

    void setAccess(QList<AccessObject> *collection, QString cpath,
        int uid, int gid,
        int userMode, int groupMode, int othersMode,
        Role role);

    bool checkAccess(const QList<AccessObject> *collection,
        QString cpath, int mode) const;

    // For admin or for newly created files.
    // path -- canonical path to file.
    void setAccessFile(QString path, int mode);
    void setAccessDrive(QString path, int mode);
    void setAccessDir(QString path, int mode);
    void setAccessProgramExec(QString path);

    // Check access for current user.
    // path -- canonical path to file.
    bool checkAccessFile(QString cpath, int mode) const;
    bool checkAccessDrive(QString cpath, int mode) const;
    bool checkAccessDir(QString cpath, int mode) const;
    bool checkAccessProgramExec(QString cpath) const;

    // For admin only.
    // path -- canonical path to file.
    void setHashFile(QString cpath);

    // For admin or for newly created directories.
    // path -- canonical path to file.
    void setDefaultModeFile(QString cpath);
    void setDefaultModeDir(QString cpath);

    QString readFileInt(QString cpath);
    void writeFileInt(QString cpath, QString data);

    static QByteArray get_hash_file(QString cpath);

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
