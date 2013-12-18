#ifndef PROFILE_H
#define PROFILE_H

#include <QString>
#include <QList>
#include <QHash>

#include "accesscontrol.h"

class Profile
{
public:
    Profile(AccessControl *accessControl, int aUID);
    int getUID() const;
    int getGID() const;
    QString userName() const;
    QString groupName() const;

    // File canonical path, access mode.
    QHash<QString, int> files() const;
    QHash<QString, int> drives() const;
    QHash<QString, int> dirs() const;
    QHash<QString, int> programs() const;

    bool isRoot() const;

private:
    const User *getUser() const;
    const Group *getGroup() const;
    QHash<QString, int> accessibleObjects(QList<AccessObject> *collection) const;

    AccessControl *ac;

    // My little runtime information :-)
    int uid;
};

#endif // PROFILE_H
