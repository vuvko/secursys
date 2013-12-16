#include "userprofile.h"

UserProfile::UserProfile(int uid, const QString &user)
    : _uid(uid), _user(user), _files(), _drives(), _dirs() {}

QString
UserProfile::user()
{
    return _user;
}

int
UserProfile::uid()
{
    return _uid;
}

QList<QString>
UserProfile::filesRead()
{
    return _files.keys(READ) + _files.keys(READ | WRITE);
}

QList<QString>
UserProfile::filesWrite()
{
    return _files.keys(WRITE) + _files.keys(READ | WRITE);
}

QList<QString>
UserProfile::dirsRead()
{
    return _dirs.keys(READ) + _dirs.keys(READ | WRITE);
}

QList<QString>
UserProfile::dirsWrite()
{
    return _dirs.keys(WRITE) + _dirs.keys(READ | WRITE);
}

QList<QString>
UserProfile::drivesRead()
{
    return _drives.keys(READ) + _drives.keys(READ | WRITE);
}

QList<QString>
UserProfile::drivesWrite()
{
    return _drives.keys(WRITE) + _drives.keys(READ | WRITE);
}

void
UserProfile::setDirsRead(const QList<QString> &dirsRead)
{
    for (auto dir : dirsRead) {
        if (_dirs.contains(dir)) {
            _dirs[dir] |= READ;
        } else {
            _dirs[dir] = READ;
        }
    }
}

void
UserProfile::setDirsWrite(const QList<QString> &dirsWrite)
{
    for (auto dir : dirsWrite) {
        if (_dirs.contains(dir)) {
            _dirs[dir] |= WRITE;
        } else {
            _dirs[dir] = WRITE;
        }
    }
}

void
UserProfile::setDrivesRead(const QList<QString> &drivesRead)
{
    for (auto drive : drivesRead) {
        if (_drives.contains(drive)) {
            _drives[drive] |= READ;
        } else {
            _drives[drive] = READ;
        }
    }
}

void
UserProfile::setDrivesWrite(const QList<QString> &drivesWrite)
{
    for (auto drive : drivesWrite) {
        if (_drives.contains(drive)) {
            _drives[drive] |= WRITE;
        } else {
            _drives[drive] = WRITE;
        }
    }
}

void
UserProfile::setFilesRead(const QList<QString> &filesRead)
{
    for (auto file : filesRead) {
        if (_files.contains(file)) {
            _files[file] |= READ;
        } else {
            _files[file] = READ;
        }
    }
}

void
UserProfile::setFilesWrite(const QList<QString> &filesWrite)
{
    for (auto file : filesWrite) {
        if (_files.contains(file)) {
            _files[file] |= WRITE;
        } else {
            _files[file] = WRITE;
        }
    }
}
