#ifndef PROFILE_H
#define PROFILE_H

#include <QString>
#include <QList>
#include <QHash>
#include <QDir>

#include "accesscontrol.h"

class Profile
{
    friend class AccessControl;

public:
    static Profile &getInstance();
    void initialize(int uid, QString pwd = QString("."));

    // Canonical path.
    QString getPWD();

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
    Profile();

    // Not implement, forbid usage.
    Profile(const Profile &);
    void operator=(const Profile &);

    const User *getUser() const;
    const Group *getGroup() const;
    QHash<QString, int> accessibleObjects(QList<AccessObject> *collection) const;

    // My little runtime information :-)
    int uid;
    QDir pwd;
};

#endif // PROFILE_H
