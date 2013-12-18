#include "profile.h"
#include "accesscontrol.h"

Profile::Profile(AccessControl *accessControl, int aUID)
    : ac(accessControl), uid(aUID)
{}

const User *Profile::getUser() const
{
    QListIterator<User> i(ac->allUsers);
    while (i.hasNext()) {
        const User &u = i.next();
        if (u.uid == uid)
            return &u;
    }

    return 0;
}

const Group *Profile::getGroup() const
{
    const User *u = getUser();

    QListIterator<Group> i(ac->allGroups);
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
    return accessibleObjects(&ac->allFiles);
}

QHash<QString, int> Profile::drives() const
{
    return accessibleObjects(&ac->allDrives);
}

QHash<QString, int> Profile::dirs() const
{
    return accessibleObjects(&ac->allDirs);
}

QHash<QString, int> Profile::programs() const
{
    return accessibleObjects(&ac->allPrograms);
}

bool Profile::isRoot() const
{
    return uid == ROOT_UID;
}
