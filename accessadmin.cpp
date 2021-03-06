#include <QDir>
#include "accessadmin.h"
#include "profile.h"
#include "crypto.h"

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
        collection->append(obj);
    } else {
        collection->replace(idx, obj);
    }

    AccessControl::getInstance().dbWrite();
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
    QString apath = info.absoluteFilePath();

    QByteArray hash = AccessControl::calcHashFile(apath);
    bool ok = !hash.isEmpty();
    if (ok) {
        AccessControl::getInstance().allHashes.insert(apath, hash);
        AccessControl::getInstance().dbWrite();
    }
    return ok;
}

void AccessAdmin::setUser(const User &u)
{
    bool found = false;

    for (auto au : AccessControl::getInstance().allUsers) {
        if (au.uid == u.uid) {
            au.gid = u.gid;
            au.name = u.name;
            au.passHash = u.passHash;
            au.maxRole = u.maxRole;
            found = true;
            break;
        }
    }

    if (!found)
        AccessControl::getInstance().allUsers.append(u);
    AccessControl::getInstance().dbWrite();
}

QString AccessAdmin::getUserName(int uid)
{
    for (auto au : AccessControl::getInstance().allUsers) {
        if (au.uid == uid) {
            return au.name;
        }
    }

    return QString();
}

QString AccessAdmin::getGroupName(int gid)
{
    for (auto ag : AccessControl::getInstance().allGroups) {
        if (ag.gid == gid) {
            return ag.name;
        }
    }

    return QString();
}

void AccessAdmin::setGroup(const Group &g)
{
    bool found = false;

    for (auto ag : AccessControl::getInstance().allGroups) {
        if (ag.gid == g.gid) {
            ag.name = g.name;
            found = true;
            break;
        }
    }

    if (!found)
        AccessControl::getInstance().allGroups.append(g);
    AccessControl::getInstance().dbWrite();
}

void AccessAdmin::setUser(int uid, int gid, QString name, QString pass,
    Role maxRole)
{
    User u;
    u.uid = uid;
    u.gid = gid;
    u.name = name;
    u.passHash = Crypto::getInstance().hash_256(pass.toUtf8());
    u.maxRole = maxRole;
    setUser(u);
}

void AccessAdmin::setGroup(int gid, QString name)
{
    Group g;
    g.gid = gid;
    g.name = name;
    setGroup(g);
}

const User *AccessAdmin::getUserByUID(int uid)
{
    QListIterator<User> i(AccessControl::getInstance().allUsers);
    while (i.hasNext()) {
        const User &u = i.next();
        if (u.uid == uid)
            return &u;
    }

    return 0;
}

const Group *AccessAdmin::getGroupByGID(int gid)
{
    QListIterator<Group> i(AccessControl::getInstance().allGroups);
    while (i.hasNext()) {
        const Group &g = i.next();
        if (g.gid == gid)
            return &g;
    }

    return 0;
}

int AccessAdmin::getNewUID()
{
    for (int i = 1; i <= INT_MAX; ++i) {
        if (getUserByUID(i) == 0)
            return i;
    }

    return -1;
}

int AccessAdmin::getNewGID()
{
    for (int i = 1; i <= INT_MAX; ++i) {
        if (getGroupByGID(i) == 0)
            return i;
    }

    return -1;
}

int AccessAdmin::getUID(const QString &name)
{
    int uid = -1;
    for (auto user : AccessControl::getInstance().allUsers) {
        if (user.name == name) {
            return user.uid;
        }
    }

    return uid;
}

int AccessAdmin::getGID(const QString &name)
{
    int gid = -1;
    for (auto group : AccessControl::getInstance().allGroups) {
        if (group.name == name) {
            return group.gid;
        }
    }

    return gid;
}
