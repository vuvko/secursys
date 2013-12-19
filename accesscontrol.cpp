#include <QApplication>
#include <QFile>
#include <QDir>
#include "accesscontrol.h"
#include "crypto.h"
#include "profile.h"
#include "accessadmin.h"
#include "logger.h"

#ifdef DEBUG
#include <QtDebug>
#define DB_FILE "admin-debug.db"
#else
#define DB_FILE "admin.db"
#endif

inline QString tr(const char *srcText)
{
    return QObject::tr(srcText);
}

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
{}

int AccessControl::checkLogin(QString userName, QString userPass) const
{
    QByteArray passHash = Crypto::getInstance().hash_256(userPass.toUtf8());

    for (auto u : allUsers) {
        if (u.name == userName && u.passHash == passHash)
            return u.uid;
    }

    LOG << tr("Failure try to login with user \"%1\".").arg(userName) << ENDL;
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
        LOG << tr("Access denied at reading file \"%1\".").arg(cpath) << ENDL;
        return false;
    }

    ok = ok && readFileInt(cpath, to);

    if (!ok) {
        LOG << tr("Something went wrong at reading file \"%1\".").arg(cpath) << ENDL;
        return false;
    }

    return true;
}

bool AccessControl::writeFile(QString path, QString data)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString cdir = info.canonicalPath();
    QString name = info.fileName();
    QString cpath = cdir + QDir::separator() + name;

    bool ok = true;
    ok = ok && checkAccessDrive(getDrive(cpath), ACCESS_WRITE);

    bool newFile = info.exists();

    if (newFile)
        ok = ok && checkAccessDir(cdir, ACCESS_WRITE);
    else
        ok = ok && checkAccessFile(cpath, ACCESS_WRITE);

    if (!ok) {
        LOG << tr("Access denied at writing file \"%1\".").arg(cpath) << ENDL;
        return false;
    }

    ok = ok && writeFileInt(cpath, data);

    if (!ok) {
        LOG << tr("Something went wrong at writing file \"%1\".").arg(cpath) << ENDL;
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
        LOG << tr("Access denied at cd \"%1\".").arg(cpath) << ENDL;
        return false;
    }

    if (!info.isDir()) {
        LOG << tr("Something went wrong at cd \"%1\".").arg(cpath) << ENDL;
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
    QString name = info.fileName();
    QString cpath = cdir + QDir::separator() + name;

    qDebug() << "mkdir cpath" << cpath;
    bool ok = true;
    ok = ok && checkAccessDrive(getDrive(cpath), ACCESS_WRITE);
    ok = ok && checkAccessDir(cdir, ACCESS_WRITE);

    if (!ok) {
        LOG << tr("Access denied at mkdir \"%1\".").arg(cpath) << ENDL;
        return false;
    }

    if (!QDir(cdir).mkdir(name)) {
        LOG << tr("Something went wrong at mkdir \"%1\".").arg(cpath) << ENDL;
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
        LOG << tr("Access denied at rmdir \"%1\".").arg(cpath) << ENDL;
        return false;
    }

    ok = ok && info.isDir();
    ok = ok && QDir(cpath).entryList().isEmpty();
    ok = ok && QDir(cdir).rmdir(name);

    if (!ok) {
        LOG << tr("Something went wrong at rmdir \"%1\".").arg(cpath) << ENDL;
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
        LOG << tr("Access denied at rm \"%1\".").arg(cpath) << ENDL;
        return false;
    }

    ok = ok && info.isFile();
    ok = ok && QDir(cdir).remove(name);

    if (!ok) {
        LOG << tr("Something went wrong at rm \"%1\".").arg(cpath) << ENDL;
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
        LOG << tr("Access denied at exec \"%1\".").arg(cpath) << ENDL;
        return false;
    }

    ok = ok && info.isFile();
    ok = ok && info.isExecutable();
    ok = ok && system(cpath.toStdString().c_str()) != -1;

    if (!ok) {
        LOG << tr("Something went wrong at exec \"%1\".").arg(cpath) << ENDL;
        return false;
    }

    return true;
}

// =================

void AccessControl::dbRead()
{
#if 0
    QString str;
    bool ok = readFileInt(DB_FILE, str, getRootKey());

    if (!ok) {
        LOG << tr("Something went wrong at db read from \"%1\".").arg(DB_FILE) << ENDL;
        return;
    }

    QDataStream data(str);
    data.setVersion(QDataStream::Qt_5_0);

    data >> allFiles
         >> allDrives
         >> allDirs
         >> allPrograms
         >> allHashes
         >> allUsers
         >> allGrours;
    // TODO: read logs
#else
    // For initial tests.
    User root;
    root.uid = 0;
    root.gid = 0;
    root.name = "root";
    root.passHash = Crypto::getInstance().hash_256(QString("password").toUtf8());
    allUsers.append(root);

    Group gRoot;
    gRoot.gid = 0;
    gRoot.name = "root";
    allGroups.append(gRoot);

    QDir pwd(Profile::getInstance().getPWD());
    QString cpwd = pwd.canonicalPath();

    AccessObject obj(cpwd);
    obj.uid = 0;
    obj.gid = 0;
    obj.userMode = 7;
    obj.groupMode = 7;
    obj.othersMode = 7;
    obj.role = ROLE_NOTHING;

    AccessAdmin::getInstance().setAccessDir(obj);
    obj.path = getDrive(cpwd);
    AccessAdmin::getInstance().setAccessDrive(obj);
#endif
}

void AccessControl::dbWrite()
{
    QDataStream data;
    data.setVersion(QDataStream::Qt_5_0);

    data << allFiles
         << allDrives
         << allDirs
         << allPrograms
         << allHashes
         << allUsers
         << allGroups;

    QString str;
    data >> str;

    bool ok = writeFileInt(DB_FILE, str, getRootKey());

    if (!ok) {
        LOG << tr("Something went wrong at db write to \"%1\".").arg(DB_FILE) << ENDL;
        return;
    }

    // TODO: write logs
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

QByteArray AccessControl::getRootKey() const
{
    return Profile::getUserByUID(ROOT_UID)->passHash;
}

bool AccessControl::readFileInt(QString cpath, QString &to, QByteArray userKey)
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
    QByteArray data = Crypto::getInstance().decrypt(encr, userKey);
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    file.close();
    to = QString::fromUtf8(data, data.size());
    return true;
}

bool AccessControl::writeFileInt(QString cpath, QString data, QByteArray userKey)
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
    out << Crypto::getInstance().encrypt(outData, userKey);//.toHex().toUpper();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    file.close();
    return true;
}

bool AccessControl::readFileInt(QString cpath, QString &to)
{
    return readFileInt(cpath, to, getUserKey());
}

bool AccessControl::writeFileInt(QString cpath, QString data)
{
    return writeFileInt(cpath, data, getUserKey());
}

QByteArray AccessControl::calcHashFile(QString cpath)
{
    QFile file(cpath);
    file.open(QFile::ReadOnly);
    QByteArray hash = Crypto::getInstance().hash_256(file.readAll());
    file.close();
    return hash;
}

QDataStream &operator<<(QDataStream &out, const AccessObject &obj)
{
    out << obj.path << obj.uid << obj.gid
        << obj.userMode << obj.groupMode << obj.othersMode
        << static_cast<int>(obj.role);
    return out;
}

QDataStream &operator>>(QDataStream &in, AccessObject &obj)
{
    int tmpInt;
    in >> obj.path >> obj.uid >> obj.gid
        >> obj.userMode >> obj.groupMode >> obj.othersMode
        >> tmpInt;
    obj.role = static_cast<Role>(tmpInt);
    return in;
}

QDataStream &operator<<(QDataStream &out, const User &obj)
{
    out << obj.uid << obj.gid << obj.name << obj.passHash;
    return out;
}

QDataStream &operator>>(QDataStream &in, User &obj)
{
    in >> obj.uid >> obj.gid >> obj.name >> obj.passHash;
    return in;
}

QDataStream &operator<<(QDataStream &out, const Group &obj)
{
    out << obj.gid << obj.name;
    return out;
}

QDataStream &operator>>(QDataStream &in, Group &obj)
{
    in >> obj.gid >> obj.name;
    return in;
}
