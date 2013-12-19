#ifndef AccessAdmin_H
#define AccessAdmin_H

#include <QString>
#include <QList>
#include "accesscontrol.h"

class AccessAdmin
{
public:
    static AccessAdmin &getInstance();

    // For root or for (via setDefault*) newly created files/dirs.
    void setAccessFile(const AccessObject &obj);
    void setAccessDrive(const AccessObject &obj);
    void setAccessDir(const AccessObject &obj);
    void setAccessProgramExec(const AccessObject &obj);

    // For root only.
    // path can be relative with respect Profile::getPWD().
    bool setHashFile(QString path);

private:
    AccessAdmin();

    // Not implement, forbid usage.
    AccessAdmin(const AccessAdmin &);
    void operator=(const AccessAdmin &);

    void setAccess(QList<AccessObject> *collection, const AccessObject &obj);
    static QByteArray calcHashFile(QString cpath);
};

#endif // AccessAdmin_H
