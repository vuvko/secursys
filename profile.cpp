#include "profile.h"

Profile::Profile() {}

QList<QString>
Profile::filesRead()
{
    return _files.keys(READ) + _files.keys(READ | WRITE);
}

QList<QString>
Profile::filesWrite()
{
    return _files.keys(WRITE) + _files.keys(READ | WRITE);
}

QList<QString>
Profile::dirsRead()
{
    return _dirs.keys(READ) + _dirs.keys(READ | WRITE);
}

QList<QString>
Profile::dirsWrite()
{
    return _dirs.keys(WRITE) + _dirs.keys(READ | WRITE);
}

QList<QString>
Profile::drivesRead()
{
    return _drives.keys(READ) + _drives.keys(READ | WRITE);
}

QList<QString>
Profile::drivesWrite()
{
    return _drives.keys(WRITE) + _drives.keys(READ | WRITE);
}

QList<QString>
Profile::programmsExec()
{
    return _programms.keys(EXEC);
}

void
Profile::setDirsRead(const QList<QString> &dirsRead)
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
Profile::setDirsWrite(const QList<QString> &dirsWrite)
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
Profile::setDrivesRead(const QList<QString> &drivesRead)
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
Profile::setDrivesWrite(const QList<QString> &drivesWrite)
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
Profile::setFilesRead(const QList<QString> &filesRead)
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
Profile::setFilesWrite(const QList<QString> &filesWrite)
{
    for (auto file : filesWrite) {
        if (_files.contains(file)) {
            _files[file] |= WRITE;
        } else {
            _files[file] = WRITE;
        }
    }
}

void
Profile::setProgrammsExec(const QList<QString> &programmsExec)
{
    for (auto programm : programmsExec) {
        if (_programms.contains(programm)) {
            _programms[programm] |= EXEC;
        } else {
            _programms[programm] = EXEC;
        }
    }
}

bool
Profile::canReadDir(const QString &path)
{
    if (!_dirs.contains(path)) {
        return false;
    }
    return _dirs[path] & ~READ;
}