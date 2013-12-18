#ifndef AccessControl_H
#define AccessControl_H

#include <QString>
#include <QList>
#include <QHash>

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

class AppHandler;

class AccessControl
{
public:
    AccessControl(AppHandler *appHandler);
    ~AccessControl();

    // For admin or for newly created files.
    // path -- canonical path to file.
    void setAccessFile(QString path, int mode);
    void setAccessDrive(QString path, int mode);
    void setAccessDir(QString path, int mode);
    void setAccessProgramExec(QString path);

    // Check access for current user.
    // path -- canonical path to file.
    bool checkAccessFile(QString path, int mode) const;
    bool checkAccessDrive(QString path, int mode) const;
    bool checkAccessDir(QString path, int mode) const;
    bool checkAccessProgramExec(QString path) const;

    // For admin only.
    // path -- canonical path to file.
    void setHashFile(QString path);

    // Check hash for file.
    // path -- canonical path to file.
    bool checkHashFile(QString path) const;

    // For admin or for newly created directories.
    // path -- canonical path to file.
    void setDefaultModeDir(QString path);
    void setDefaultModeFile(QString path);

private:
    bool dbRead();
    bool dbWrite();

    void setAccess(QList<AccessObject> *collection, QString path,
        int uid, int gid,
        int userMode, int groupMode, int othersMode,
        Role role);

    bool checkAccess(const QList<AccessObject> *collection,
        QString path, int mode) const;

    AppHandler *handler;

    // Access objects
    QList<AccessObject> allFiles;
    QList<AccessObject> allDrives;
    QList<AccessObject> allDirs;
    QList<AccessObject> allPrograms;

    // File hashes.
    QHash<QString, QByteArray> allHashes;
};

#endif // AccessControl_H
