#include <QApplication>
#include <QFile>
#include <QDir>
#include "accesscontrol.h"
#include "crypto.h"
#include "profile.h"

#define DB_FILE "admin.db"

AccessControl &AccessControl::getInstance()
{
    static AccessControl instance;
    return instance;
}

AccessControl::AccessControl()
{
    dbRead();
}

int AccessControl::checkLogin(QString userName, QString userPass) const
{
    QByteArray passHash = Crypto::getInstance().hash_256(userPass.toUtf8());

    for (auto u : allUsers) {
        if (u.name == userName && u.passHash == passHash)
            return u.uid;
    }

    return -1;
}

QString AccessControl::readFile(QString path)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString cpath = info.canonicalFilePath();

    bool ok = true;
    //ok = ok && checkAccessDrive(..., ACCESS_READ); // TODO
    ok = ok && checkAccessFile(cpath, ACCESS_READ);

    if (!ok) {
        // TODO AccessDenied message
        return QString();
    }

    return readFileInt(cpath);
}

void AccessControl::writeFile(QString path, QString data)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString cpath = info.canonicalFilePath();

    bool ok = true;
    //ok = ok && checkAccessDrive(..., ACCESS_WRITE); // TODO

    bool newFile = info.exists();

    if (!newFile)
        ok = ok && checkAccessFile(cpath, ACCESS_WRITE);
    else
        ok = ok && checkAccessDir(info.canonicalPath(), ACCESS_WRITE);

    if (!ok) {
        // TODO AccessDenied message
        return;
    }

    writeFileInt(cpath, data);

    if (newFile)
        setDefaultModeFile(cpath);
}

void AccessControl::cd(QString path)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString cpath = info.canonicalPath();

    bool ok = true;
    //ok = ok && checkAccessDrive(..., ACCESS_READ); // TODO
    ok = ok && checkAccessDir(cpath, ACCESS_READ);

    if (!ok) {
        // TODO AccessDenied message
        return;
    }

    if (!info.isDir())
        return;

    Profile::getInstance().pwd.cd(cpath);
}

void AccessControl::mkdir(QString path)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString cpath = info.canonicalPath();

    bool ok = true;
    //ok = ok && checkAccessDrive(..., ACCESS_WRITE); // TODO
    ok = ok && checkAccessDir(cpath, ACCESS_READ);


    /////// TODO!!!
}

void AccessControl::rmdir(QString path)
{
}

void AccessControl::rm(QString path)
{
}

void AccessControl::exec(QString path)
{
}

// =================

void AccessControl::dbRead()
{
    // TODO
}


void AccessControl::dbWrite() const
{
    // TODO
    return;
}
 
void AccessControl::setAccess(QList<AccessObject> *collection, QString cpath,
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

bool AccessControl::checkAccess(const QList<AccessObject> *collection,
    QString cpath, int mode) const
{
    int idx = collection->indexOf(cpath);

    if (idx < 0)
        return false;

    AccessObject obj(collection->at(idx));

    bool ok = true;

    if (obj.uid == Profile::getInstance().getUID()) {
        ok = ok && (obj.userMode & mode) != 0;
    } else if (obj.gid == Profile::getInstance().getGID()) {
        ok = ok && (obj.groupMode & mode) != 0;
    } else {
        ok = ok && (obj.othersMode & mode) != 0;
    }

    ok = ok && obj.role == ROLE_NOTHING; // TODO

    return ok;
}

void AccessControl::setAccessFile(QString cpath, int mode)
{
    //setAccess(&allFiles, cpath, ); // TODO
}

void AccessControl::setAccessDrive(QString cpath, int mode)
{
    // TODO
}

void AccessControl::setAccessDir(QString cpath, int mode)
{
    // TODO
}

void AccessControl::setAccessProgramExec(QString cpath)
{
    // TODO
}

bool AccessControl::checkAccessFile(QString cpath, int mode) const
{
    return checkAccess(&allFiles, cpath, mode);
}

bool AccessControl::checkAccessDrive(QString cpath, int mode) const
{
    return checkAccess(&allDrives, cpath, mode);
}

bool AccessControl::checkAccessDir(QString cpath, int mode) const
{
    return checkAccess(&allDirs, cpath, mode);
}

bool AccessControl::checkAccessProgramExec(QString cpath) const
{
    return checkAccess(&allPrograms, cpath, ACCESS_EXEC);
}

void AccessControl::setHashFile(QString cpath)
{
    QByteArray hash = get_hash_file(cpath);
    allHashes.insert(cpath, hash);
}

bool AccessControl::check(QString cpath) const
{
    QByteArray hash = get_hash_file(cpath);
    return allHashes.value(cpath) == hash;
}

void AccessControl::setDefaultModeDir(QString cpath)
{
    //setAccessDir(...); // TODO
}

void AccessControl::setDefaultModeFile(QString cpath)
{
    //setAccessFile(...); // TODO
}

QByteArray AccessControl::getUserKey()
{
    return Profile::getInstance().getUser()->passHash;
}

QString AccessControl::readFileInt(QString cpath)
{
    QFile file(cpath);
    if (!file.open(QFile::ReadOnly)) {
        return QString();
    }

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
//    QByteArray encr = QByteArray::fromHex(file.readAll());
    QByteArray encr(file.readAll());
    QByteArray data = Crypto::getInstance().decrypt(encr, getUserKey());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    file.close();
    return QString::fromUtf8(data, data.size());
}

void AccessControl::writeFileInt(QString cpath, QString data)
{
    QFile file(cpath);
    if (!file.open(QFile::WriteOnly)) {
        return;
    }

    QDataStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    QByteArray outData(data.toUtf8());
    out << Crypto::getInstance().encrypt(outData, getUserKey());//.toHex().toUpper();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    file.close();
}

QByteArray AccessControl::get_hash_file(QString cpath)
{
    QFile file(cpath);
    file.open(QFile::ReadOnly);
    QByteArray hash = Crypto::getInstance().hash_256(file.readAll());
    file.close();
    return hash;
}
