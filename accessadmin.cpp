#include <QDir>
#include "accessadmin.h"
#include "profile.h"

void AccessAdmin::setAccess(QList<AccessObject> *collection, QString cpath,
    int uid, int gid,
    int userMode, int groupMode, int othersMode,
    Role role)
{
    AccessObject newObj(cpath, uid, gid, userMode, groupMode, othersMode, role);

    int idx = collection->indexOf(cpath);

    if (idx < 0) {
        collection->replace(idx, newObj);
    } else {
        collection->append(newObj);
    }
}

void AccessAdmin::setAccessFile(QString cpath, int mode)
{
    //setAccess(&AccessControl::getInstance().allFiles, cpath, ); // TODO
}

void AccessAdmin::setAccessDrive(QString cpath, int mode)
{
    // TODO
}

void AccessAdmin::setAccessDir(QString cpath, int mode)
{
    // TODO
}

void AccessAdmin::setAccessProgramExec(QString cpath)
{
    // TODO
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
