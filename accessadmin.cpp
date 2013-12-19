#include <QDir>
#include "accessadmin.h"
#include "profile.h"

AccessAdmin &AccessAdmin::getInstance()
{
    static AccessAdmin instance;
    return instance;
}

AccessAdmin::AccessAdmin()
{}

void AccessAdmin::setAccess(QList<AccessObject> *collection, const AccessObject &obj)
{
    int idx = collection->indexOf(obj);

    if (idx < 0) {
        collection->replace(idx, obj);
    } else {
        collection->append(obj);
    }
}

void AccessAdmin::setAccessFile(const AccessObject &obj)
{
    setAccess(&AccessControl::getInstance().allFiles, obj);
}

void AccessAdmin::setAccessDrive(const AccessObject &obj)
{
    setAccess(&AccessControl::getInstance().allDrives, obj);
}

void AccessAdmin::setAccessDir(const AccessObject &obj)
{
    setAccess(&AccessControl::getInstance().allDirs, obj);
}

void AccessAdmin::setAccessProgramExec(const AccessObject &obj)
{
    setAccess(&AccessControl::getInstance().allPrograms, obj);
}

bool AccessAdmin::setHashFile(QString path)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString cpath = info.canonicalFilePath();

    QByteArray hash = AccessControl::calcHashFile(cpath);
    bool ok = !hash.isEmpty();
    if (ok)
        AccessControl::getInstance().allHashes.insert(cpath, hash);
    return ok;
}
