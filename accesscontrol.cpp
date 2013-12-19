#include <QApplication>
#include <QFile>
#include <QDir>
#include "accesscontrol.h"
#include "crypto.h"
#include "profile.h"
#include "accessadmin.h"

#define DB_FILE "admin.db"

AccessObject::AccessObject(QString path)
{
    this->path = path;

    uid = Profile::getInstance().getUID();
    gid = Profile::getInstance().getGID();
    userMode = ACCESS_READ | ACCESS_WRITE;
    groupMode = ACCESS_READ | ACCESS_WRITE;
    othersMode = ACCESS_READ;
    role = ROLE_NOTHING;
}

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

bool AccessControl::readFile(QString path, QString &to)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString cpath = info.canonicalFilePath();

    bool ok = true;
    ok = ok && checkAccessDrive(getDrive(cpath), ACCESS_READ);
    ok = ok && checkAccessFile(cpath, ACCESS_READ);

    if (!ok) {
        // AccessDenied message
        return false;
    }

    return readFileInt(cpath, to);
}

bool AccessControl::writeFile(QString path, QString data)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString cdir = info.canonicalPath();
    QString cpath = info.canonicalFilePath();

    bool ok = true;
    ok = ok && checkAccessDrive(getDrive(cpath), ACCESS_WRITE);

    bool newFile = info.exists();

    if (newFile)
        ok = ok && checkAccessDir(cdir, ACCESS_WRITE);
    else
        ok = ok && checkAccessFile(cpath, ACCESS_WRITE);

    if (!ok) {
        // AccessDenied message
        return false;
    }

    ok = ok && writeFileInt(cpath, data);

    if (!ok) {
        // SomethingWrong message
        return false;
    }

    if (newFile)
        setDefaultAccessFile(cpath);
    return true;
}

bool AccessControl::cd(QString path)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString cpath = info.canonicalFilePath();

    bool ok = true;
    ok = ok && checkAccessDrive(getDrive(cpath), ACCESS_READ);
    ok = ok && checkAccessDir(cpath, ACCESS_READ);

    if (!ok) {
        // AccessDenied message
        return false;
    }

    if (!info.isDir()) {
        // SomethingWrong message
        return false;
    }

    Profile::getInstance().pwd.cd(cpath);
    return true;
}

bool AccessControl::mkdir(QString path)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString cdir = info.canonicalPath();
    QString cpath = info.canonicalFilePath();
    QString name = info.fileName();

    bool ok = true;
    ok = ok && checkAccessDrive(getDrive(cpath), ACCESS_WRITE);
    ok = ok && checkAccessDir(cdir, ACCESS_WRITE);

    if (!ok) {
        // AccessDenied message
        return false;
    }

    if (!QDir(cdir).mkdir(name)) {
        // SomethingWrong message
        return false;
    }

    setDefaultAccessDir(cpath);
    return true;
}

bool AccessControl::rmdir(QString path)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString cdir = info.canonicalPath();
    QString cpath = info.canonicalFilePath();
    QString name = info.fileName();

    bool ok = true;
    ok = ok && checkAccessDrive(getDrive(cpath), ACCESS_WRITE);
    ok = ok && checkAccessDir(cdir, ACCESS_WRITE);

    if (!ok) {
        // AccessDenied message
        return false;
    }

    ok = ok && info.isDir();
    ok = ok && QDir(cpath).entryList().isEmpty();

    if (!ok || !QDir(cdir).rmdir(name)) {
        // SomethingWrong message
        return false;
    }

    return true;
}

bool AccessControl::rm(QString path)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString cdir = info.canonicalPath();
    QString cpath = info.canonicalFilePath();
    QString name = info.fileName();

    bool ok = true;
    ok = ok && checkAccessDrive(getDrive(cpath), ACCESS_WRITE);
    ok = ok && checkAccessDir(cdir, ACCESS_WRITE);

    if (!ok) {
        // AccessDenied message
        return false;
    }

    ok = ok && info.isFile();

    if (!ok || !QDir(cdir).remove(name)) {
        // SomethingWrong message
        return false;
    }

    return true;
}

bool AccessControl::exec(QString path)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString cpath = info.canonicalFilePath();

    bool ok = true;
    ok = ok && checkAccessDrive(getDrive(cpath), ACCESS_READ);
    ok = ok && checkAccessProgramExec(cpath);

    if (!ok) {
        // AccessDenied message
        return false;
    }

    ok = ok && info.isFile();
    ok = ok && info.isExecutable();

    if (!ok) {
        // SomethingWrong message
        return false;
    }

    return system(cpath.toStdString().c_str()) != -1;
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

bool AccessControl::check(QString cpath) const
{
    QByteArray hash = calcHashFile(cpath);
    return allHashes.value(cpath) == hash;
}

void AccessControl::setDefaultAccessDir(QString cpath)
{
    AccessObject defObj(cpath);
    AccessAdmin::getInstance().setAccessDir(defObj);
}

void AccessControl::setDefaultAccessFile(QString cpath)
{
    AccessObject defObj(cpath);
    AccessAdmin::getInstance().setAccessFile(defObj);
}

QString AccessControl::getDrive(QString cpath)
{
    return QString(cpath.at(0));
}

QByteArray AccessControl::getUserKey() const
{
    return Profile::getInstance().getUser()->passHash;
}

bool AccessControl::readFileInt(QString cpath, QString &to)
{
    QFile file(cpath);
    if (!file.open(QFile::ReadOnly)) {
        return false;
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
    to = QString::fromUtf8(data, data.size());
    return true;
}

bool AccessControl::writeFileInt(QString cpath, QString data)
{
    QFile file(cpath);
    if (!file.open(QFile::WriteOnly)) {
        return false;
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
    return true;
}

QByteArray AccessControl::calcHashFile(QString cpath)
{
    QFile file(cpath);
    file.open(QFile::ReadOnly);
    QByteArray hash = Crypto::getInstance().hash_256(file.readAll());
    file.close();
    return hash;
}
