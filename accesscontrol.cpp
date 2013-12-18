#include <QFile>
#include "accesscontrol.h"
#include "apphandler.h"

#define DB_FILE "admin.db"

AccessControl::AccessControl(AppHandler * appHandler)
    : handler(appHandler)
{
    // TODO: read DB_FILE
}

AccessControl::~AccessControl()
{
}

void AccessControl::setAccess(QList<AccessObject> *collection, QString path,
    int uid, int gid,
    int userMode, int groupMode, int othersMode,
    Role role)
{
    AccessObject newObj(path, uid, gid, userMode, groupMode, othersMode, role);

    int idx = collection->indexOf(path);

    if (idx < 0) {
        collection->replace(idx, newObj);
    } else {
        collection->append(newObj);
    }
}

bool AccessControl::checkAccess(const QList<AccessObject> *collection,
    QString path, int mode) const
{
    int idx = collection->indexOf(path);

    if (idx < 0)
        return false;

    AccessObject obj(collection->at(idx));

    bool ok = true;

    if (obj.uid == handler->userId()) {
        ok = ok && (obj.userMode & mode) != 0;
    } else if (obj.gid == handler->groupId()) {
        ok = ok && (obj.groupMode & mode) != 0;
    } else {
        ok = ok && (obj.othersMode & mode) != 0;
    }

    ok = ok && obj.role == handler->roleId();

    return ok;
}

void AccessControl::setAccessFile(QString path, int mode)
{
    //setAccess(&allFiles, path, ); // TODO
}

void AccessControl::setAccessDrive(QString path, int mode)
{
    // TODO
}

void AccessControl::setAccessDir(QString path, int mode)
{
    // TODO
}

void AccessControl::setAccessProgramExec(QString path)
{
    // TODO
}

bool AccessControl::checkAccessFile(QString path, int mode) const
{
    return checkAccess(&allFiles, path, mode);
}

bool AccessControl::checkAccessDrive(QString path, int mode) const
{
    return checkAccess(&allDrives, path, mode);
}

bool AccessControl::checkAccessDir(QString path, int mode) const
{
    return checkAccess(&allDirs, path, mode);
}

bool AccessControl::checkAccessProgramExec(QString path) const
{
    return checkAccess(&allPrograms, path, ACCESS_EXEC);
}

void AccessControl::setHashFile(QString path)
{
    QByteArray hash = handler->get_hash_file(path);
    allHashes.insert(path, hash);
}

bool AccessControl::checkHashFile(QString path) const
{
    QByteArray hash = handler->get_hash_file(path);
    return allHashes.value(path) == hash;
}

void AccessControl::setDefaultModeDir(QString path)
{
    //setAccessDir(...); // TODO
}

void AccessControl::setDefaultModeFile(QString path)
{
    //setAccessFile(...); // TODO
}
