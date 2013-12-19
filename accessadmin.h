#ifndef AccessAdmin_H
#define AccessAdmin_H

#include <QString>
#include <QList>
#include "accesscontrol.h"

class AccessAdmin
{
public:
    static AccessAdmin &getInstance();

    void setAccess(QList<AccessObject> *collection, QString cpath,
        int uid, int gid,
        int userMode, int groupMode, int othersMode,
        Role role);

    // For root or for (via setDefault*) newly created files/dirs.
    // path can be relative with respect Profile::getPWD().
    void setAccessFile(QString path, int mode);
    void setAccessDrive(QString path, int mode);
    void setAccessDir(QString path, int mode);
    void setAccessProgramExec(QString path);

    // For root only.
    // path can be relative with respect Profile::getPWD().
    bool setHashFile(QString path);

private:
    AccessAdmin();

    // Not implement, forbid usage.
    AccessAdmin(const AccessAdmin &);
    void operator=(const AccessAdmin &);

    static QByteArray calcHashFile(QString cpath);
};

#endif // AccessAdmin_H
