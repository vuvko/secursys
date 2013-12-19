#ifndef AccessAdmin_H
#define AccessAdmin_H

#include <QString>
#include <QList>
#include "accesscontrol.h"

class AccessAdmin
{
public:
    static AccessAdmin &getInstance();

    // Most of fuctions for root usage only.

    // For root or for (via setDefault*) newly created files/dirs.
    void setAccessFile(const AccessObject &obj);
    void setAccessDrive(const AccessObject &obj);
    void setAccessDir(const AccessObject &obj);
    void setAccessProgramExec(const AccessObject &obj);

    // path can be relative with respect Profile::getPWD().
    bool setHashFile(QString path);

    void setUser(const User &u);
    void setGroup(const Group &g);
    void setUser(int uid, int gid, QString name, QString pass);
    void setGroup(int gid, QString name);

    const User *getUserByUID(int uid);

    // Return -1 on failure.
    int getNewUID();

private:
    AccessAdmin();

    // Not implement, forbid usage.
    AccessAdmin(const AccessAdmin &);
    void operator=(const AccessAdmin &);

    void setAccess(QList<AccessObject> *collection, const AccessObject &obj);
    static QByteArray calcHashFile(QString apath);
};

#endif // AccessAdmin_H
