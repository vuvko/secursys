#include "profile.h"
#include "accesscontrol.h"
#include "accessadmin.h"

Profile &Profile::getInstance()
{
    static Profile instance;
    return instance;
}

void Profile::initialize(int uid, QString pwd)
{
    this->uid = uid;
    this->pwd = QDir(pwd);
}

Profile::Profile()
    : uid(-1), pwd(".")
{
    pwd.setSorting(QDir::DirsFirst | QDir::Name);
}

QString Profile::getPWD()
{
    return pwd.absolutePath();
}

QDir &Profile::getConstDirPWD()
{
    return pwd;
}

const User *
Profile::getUser() const
{
    return AccessAdmin::getInstance().getUserByUID(uid);
}

const Group *
Profile::getGroup() const
{
    const User *u = getUser();

    if (!u)
        return 0;

    QListIterator<Group> i(AccessControl::getInstance().allGroups);
    while (i.hasNext()) {
        const Group &g = i.next();
        if (g.gid == u->gid)
            return &g;
    }

    return 0;
}

int Profile::getUID() const
{
    return uid;
}

int Profile::getGID() const
{
    const Group *g = getGroup();

    if (g)
        return g->gid;
    else
        return -1;
}

QString Profile::userName() const
{
    const User *u = getUser();

    if (u)
        return u->name;
    else
        return QString();
}

QString Profile::groupName() const
{
    const Group *g = getGroup();

    if (g)
        return g->name;
    else
        return QString();
}

QHash<QString, int> Profile::accessibleObjects(QList<AccessObject> *collection) const
{
    QHash<QString, int> res;

    for (auto obj : *collection) {
        if (obj.uid == uid) {
            if (obj.userMode)
                res.insert(obj.path, obj.userMode);
        } else if (obj.gid == getGroup()->gid) {
            if (obj.groupMode)
                res.insert(obj.path, obj.groupMode);
        } else {
            if (obj.othersMode)
                res.insert(obj.path, obj.othersMode);
        }
    }

    return res;
}

QHash<QString, int> Profile::files() const
{
    return accessibleObjects(&AccessControl::getInstance().allFiles);
}

QHash<QString, int> Profile::drives() const
{
    return accessibleObjects(&AccessControl::getInstance().allDrives);
}

QHash<QString, int> Profile::dirs() const
{
    return accessibleObjects(&AccessControl::getInstance().allDirs);
}

QHash<QString, int> Profile::programs() const
{
    return accessibleObjects(&AccessControl::getInstance().allPrograms);
}

bool Profile::isRoot() const
{
    return uid == ROOT_UID;
}
