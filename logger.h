#ifndef LOGGER_H
#define LOGGER_H

#include <QFile>
#include <QTextStream>
#include <QTime>
#include <QDateTime>

/*
 * При использовании следует всегда посылать Logger::ENDL,
 * чтобы добавить запись в лог (к ней приписывается время в начале).
 */

class Logger
{
public:
    Logger();
    ~Logger();

    enum ComandSymbol {
        ENDL,
        TIMESTAMP,
        DATESTAMP
    };

    QString unload();

    friend Logger &operator << (Logger &out, const char *text);
    friend Logger &operator << (Logger &out, const QString &text);
    friend Logger &operator << (Logger &out, int val);
    friend Logger &operator << (Logger &out, ComandSymbol sym);

private:
    void unloadBuff();

    QString log;
    QString buff;
};

#endif // LOGGER_H
