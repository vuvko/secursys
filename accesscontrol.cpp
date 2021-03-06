#include <QApplication>
#include <QFile>
#include <QDir>
#include "accesscontrol.h"
#include "crypto.h"
#include "profile.h"
#include "accessadmin.h"
#include "logger.h"

#ifdef DEBUG
#define DB_FILE "admin-debug.db"
#else
#define DB_FILE "admin.db"
#endif

#define DB_PASS "Qozmv49F0kFejf4Ffj9sdfF4fFJEk493rSfkksfkr49FEJ"

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

bool AccessControl::readFile(QString path, QByteArray &to)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString apath = info.absoluteFilePath();

    bool ok = true;
    ok = ok && checkAccessDrive(getDrive(apath), ACCESS_READ);
    ok = ok && checkAccessFile(apath, ACCESS_READ);

    if (!ok) {
        QMessageBox::warning(NULL, "Попытка чтения файла",
                             QString("Недостаточно прав для чтения файла \"%1\".").arg(apath));
        LOG << tr("Access denied at reading file \"%1\".").arg(apath) << ENDL;
        return false;
    }

    ok = ok && readFileInt(apath, to);

    if (!ok) {
        QMessageBox::warning(NULL, "Попытка чтения файла",
                             QString("Не удалось прочитать файл \"%1\"").arg(apath));
        LOG << tr("Something went wrong at reading file \"%1\".").arg(apath) << ENDL;
        return false;
    }

    return true;
}

bool AccessControl::writeFile(QString path, QByteArray &data)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString adir = info.absolutePath();
    QString apath = info.absoluteFilePath();
    QString name = info.fileName();

    bool ok = true;
    ok = ok && checkAccessDrive(getDrive(apath), ACCESS_WRITE);

    bool newFile = !info.exists();

    if (newFile)
        ok = ok && checkAccessDir(adir, ACCESS_WRITE);
    else
        ok = ok && checkAccessFile(apath, ACCESS_WRITE);

    if (!ok) {
        QMessageBox::warning(NULL, "Попытка записи файла",
                             QString("Недостаточно прав для записи файла \"%1\".").arg(apath));
        LOG << tr("Access denied at writing file \"%1\".").arg(apath) << ENDL;
        return false;
    }

    ok = ok && writeFileInt(apath, data);

    if (!ok) {
        QMessageBox::warning(NULL, "Попытка записи файла",
                             QString("Не удалось записать файл \"%1\"").arg(apath));
        LOG << tr("Something went wrong at writing file \"%1\".").arg(apath) << ENDL;
        return false;
    }

    if (newFile)
        setDefaultAccessFile(apath);
    return true;
}

bool AccessControl::cd(QString path)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString apath = info.absoluteFilePath();

    bool ok = true;
    ok = ok && checkAccessDrive(getDrive(apath), ACCESS_READ);
    ok = ok && checkAccessDir(apath, ACCESS_READ);

    if (!ok) {
        QMessageBox::warning(NULL, "Попытка сменить каталог",
                             QString("Недостаточно прав для перехода в каталог \"%1\".").arg(apath));
        LOG << tr("Access denied at cd \"%1\".").arg(apath) << ENDL;
        return false;
    }

    if (!info.isDir()) {
        QMessageBox::warning(NULL, "Попытка сменить каталог",
                             QString("Не удалось перейти в каталог \"%1\".").arg(apath));
        LOG << tr("Something went wrong at cd \"%1\".").arg(apath) << ENDL;
        return false;
    }

    Profile::getInstance().pwd.cd(apath);
    return true;
}

bool AccessControl::mkdir(QString path)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString adir = info.absolutePath();
    QString apath = info.absoluteFilePath();
    QString name = info.fileName();

    bool ok = true;
    ok = ok && checkAccessDrive(getDrive(apath), ACCESS_WRITE);
    ok = ok && checkAccessDir(adir, ACCESS_WRITE);

    if (!ok) {
        QMessageBox::warning(NULL, "Попытка создания каталога",
                             QString("Недостаточно прав для создания каталога \"%1\".").arg(apath));
        LOG << tr("Access denied at mkdir \"%1\".").arg(apath) << ENDL;
        return false;
    }

    if (!QDir(adir).mkdir(name)) {
        QMessageBox::warning(NULL, "Попытка создания каталога",
                             QString("Не удалось создать каталог \"%1\".").arg(apath));
        LOG << tr("Something went wrong at mkdir \"%1\".").arg(apath) << ENDL;
        return false;
    }

    setDefaultAccessDir(apath);
    return true;
}

bool AccessControl::rmdir(QString path)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString adir = info.absolutePath();
    QString apath = info.absoluteFilePath();
    QString name = info.fileName();

    bool ok = true;
    ok = ok && checkAccessDrive(getDrive(apath), ACCESS_WRITE);
    ok = ok && checkAccessDir(adir, ACCESS_WRITE);
    ok = ok && checkAccessDir(apath, ACCESS_WRITE);

    if (!ok) {
        QMessageBox::warning(NULL, "Попытка удаления каталога",
                             QString("Недостаточно прав для удаления каталога \"%1\".").arg(apath));
        LOG << tr("Access denied at rmdir \"%1\".").arg(apath) << ENDL;
        return false;
    }

    ok = ok && info.isDir();
    ok = ok && isDirEmpty(apath);
    ok = ok && QDir(adir).rmdir(name);

    if (!ok) {
        QMessageBox::warning(NULL, "Попытка удаления каталога",
                             QString("Не удалось удалить каталог \"%1\".").arg(apath));
        LOG << tr("Something went wrong at rmdir \"%1\".").arg(apath) << ENDL;
        return false;
    }

    return true;
}

bool AccessControl::rm(QString path)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString adir = info.absolutePath();
    QString apath = info.absoluteFilePath();
    QString name = info.fileName();

    bool ok = true;
    ok = ok && checkAccessDrive(getDrive(apath), ACCESS_WRITE);
    ok = ok && checkAccessDir(adir, ACCESS_WRITE);
    ok = ok && checkAccessFile(apath, ACCESS_WRITE);

    if (!ok) {
        QMessageBox::warning(NULL, "Попытка удаления файла",
                             QString("Недостаточно прав для удаления файла \"%1\".").arg(apath));
        LOG << tr("Access denied at rm \"%1\".").arg(apath) << ENDL;
        return false;
    }

    ok = ok && info.isFile();
    ok = ok && QDir(adir).remove(name);

    if (!ok) {
        QMessageBox::warning(NULL, "Попытка удаления файла",
                             QString("Не удалось удалить файл \"%1\".").arg(apath));
        LOG << tr("Something went wrong at rm \"%1\".").arg(apath) << ENDL;
        return false;
    }

    return true;
}

bool AccessControl::exec(QString path)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString apath = info.absoluteFilePath();

    bool ok = true;
    ok = ok && checkAccessDrive(getDrive(apath), ACCESS_READ);
    ok = ok && checkAccessProgramExec(apath);

    if (!ok) {
        QMessageBox::warning(NULL, "Попытка запуска программы",
                             QString("Недостаточно прав для запуска программы \"%1\".").arg(apath));
        LOG << tr("Access denied at exec \"%1\".").arg(apath) << ENDL;
        return false;
    }

    ok = ok && info.isFile();
    ok = ok && info.isExecutable();
    ok = ok && system(apath.toStdString().c_str()) != -1;

    if (!ok) {
        QMessageBox::warning(NULL, "Попытка запуска программы",
                             QString("Не удалось запустить программу \"%1\".").arg(apath));
        LOG << tr("Something went wrong at exec \"%1\".").arg(apath) << ENDL;
        return false;
    }

    return true;
}

// =================

void AccessControl::dbInit()
{
    LOG << tr("Initialize new database at \"%1\".").arg(DB_FILE) << ENDL;

    User root;
    root.uid = 0;
    root.gid = 0;
    root.name = "root";
    root.passHash = Crypto::getInstance().hash_256(QString("password").toUtf8());
    root.maxRole = ROLE_NOTHING;

    allUsers.append(root);

    Group gRoot;
    gRoot.gid = 0;
    gRoot.name = "root";
    allGroups.append(gRoot);

    QDir pwd(Profile::getInstance().getPWD());
    QString cpwd = pwd.absolutePath();

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

    dbWrite();
}

void AccessControl::dbRead()
{
    QByteArray data;
    bool ok = readFileInt(DB_FILE, data, getDBKey());

    if (!ok) {
        LOG << tr("Something went wrong at db read from \"%1\".").arg(DB_FILE) << ENDL;
        dbInit();
        return;
    }

    QDataStream out(&data, QIODevice::ReadOnly);
    out.setVersion(QDataStream::Qt_5_0);

    out >> allFiles
        >> allDrives
        >> allDirs
        >> allPrograms
        >> allHashes
        >> allUsers
        >> allGroups;
    // TODO: read logs
}

void AccessControl::dbWrite()
{
    QByteArray data;
    QDataStream in(&data, QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_0);

    in << allFiles
       << allDrives
       << allDirs
       << allPrograms
       << allHashes
       << allUsers
       << allGroups;

    bool ok = writeFileInt(DB_FILE, data, getRootKey());

    if (!ok) {
        QMessageBox::warning(NULL, "Попытка сохранения информации",
                             QString("Не удалось сохранить в \"%1\".").arg(DB_FILE));
        LOG << tr("Something went wrong at db write to \"%1\".").arg(DB_FILE) << ENDL;
        return;
    }

    // TODO: write logs
}
 
bool AccessControl::checkAccess(const QList<AccessObject> *collection,
    QString apath, int mode) const
{
    if (Profile::getInstance().isRoot()) {
        return true;
    }

    int idx = collection->indexOf(apath);

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

    ok = ok && obj.role <= Profile::getInstance().curRole();

    if (ok && (mode & ACCESS_WRITE)) {
        obj.role = Profile::getInstance().curRole();
    }

    return ok;
}

bool AccessControl::checkAccessFile(QString apath, int mode) const
{
    return checkAccess(&allFiles, apath, mode);
}

bool AccessControl::checkAccessDrive(QString apath, int mode) const
{
    return checkAccess(&allDrives, apath, mode);
}

bool AccessControl::checkAccessDir(QString apath, int mode) const
{
    return checkAccess(&allDirs, apath, mode);
}

bool AccessControl::checkAccessProgramExec(QString apath) const
{
    return checkAccess(&allPrograms, apath, ACCESS_EXEC);
}

bool AccessControl::check(QString apath) const
{
    QByteArray hash = calcHashFile(apath);
    return allHashes.value(apath) == hash;
}

Role AccessControl::getRole(const QList<AccessObject> *collection,
    QString path)
{
    QDir pwd(Profile::getInstance().getPWD());
    QFileInfo info(pwd, path);
    QString apath = info.absoluteFilePath();

    int idx = collection->indexOf(apath);

    if (idx < 0)
        return ROLE_NOTHING;

    return collection->at(idx).role;
}

Role AccessControl::getRoleFile(QString path)
{
    return getRole(&allFiles, path);
}

Role AccessControl::getRoleDrive(QString path)
{
    return getRole(&allDrives, path);
}

Role AccessControl::getRoleDir(QString path)
{
    return getRole(&allDirs, path);
}

Role AccessControl::getRoleProgram(QString path)
{
    return getRole(&allPrograms, path);
}

void AccessControl::setDefaultAccessDir(QString apath)
{
    AccessObject defObj(apath);
    AccessAdmin::getInstance().setAccessDir(defObj);
}

void AccessControl::setDefaultAccessFile(QString apath)
{
    AccessObject defObj(apath);
    AccessAdmin::getInstance().setAccessFile(defObj);
}

QString AccessControl::getDrive(QString apath)
{
    return QString(apath.at(0)).toUpper();
}

QByteArray AccessControl::getDBKey() const
{
    return Crypto::getInstance().hash_256(QString(DB_PASS).toUtf8());
}

QByteArray AccessControl::getUserKey() const
{
    return Profile::getInstance().getUser()->passHash;
}

QByteArray AccessControl::getRootKey() const
{
    return AccessAdmin::getInstance().getUserByUID(ROOT_UID)->passHash;
}

bool AccessControl::readFileInt(QString apath, QByteArray &to, QByteArray userKey)
{
    QFile file(apath);
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    // TODO: remove fromHex
    QByteArray encr = QByteArray::fromHex(file.readAll());
//    QByteArray encr = file.readAll();
    to = Crypto::getInstance().decrypt(encr, userKey);
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    file.close();
    return true;
}

bool AccessControl::writeFileInt(QString apath, QByteArray &data, QByteArray userKey)
{
    QFile file(apath);
    if (!file.open(QFile::WriteOnly)) {
        return false;
    }

    QDataStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    QByteArray encr = Crypto::getInstance().encrypt(data, userKey);
    // TODO: remove toHex
    out << encr.toHex().toUpper();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    file.close();
    return true;
}

bool AccessControl::readFileInt(QString apath, QByteArray &to)
{
    return readFileInt(apath, to, getUserKey());
}

bool AccessControl::writeFileInt(QString apath, QByteArray &data)
{
    return writeFileInt(apath, data, getUserKey());
}

QByteArray AccessControl::calcHashFile(QString apath)
{
    QFile file(apath);
    file.open(QFile::ReadOnly);
    QByteArray hash = Crypto::getInstance().hash_256(file.readAll());
    file.close();
    return hash;
}

bool AccessControl::isDirEmpty(QString apath) const
{
    int cnt = 0;
    for (auto e : QDir(apath).entryList()) {
        cnt += (e != "." && e != "..");
    }
    return cnt == 0;
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
    out << obj.uid << obj.gid << obj.name << obj.passHash
        << static_cast<int>(obj.maxRole);
    return out;
}

QDataStream &operator>>(QDataStream &in, User &obj)
{
    int tmpInt;
    in >> obj.uid >> obj.gid >> obj.name >> obj.passHash
        >> tmpInt;
    obj.maxRole = static_cast<Role>(tmpInt);
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
