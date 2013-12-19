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

// Some useful macro.
#define LOG  Logger::getInstance()
#define ENDL Logger::LOG_ENDL

class Logger
{
public:
    static Logger &getInstance();

    enum CommandSymbol {
        LOG_ENDL,
        LOG_TIMESTAMP,
        LOG_DATESTAMP
    };

    QString read();

    friend Logger &operator << (Logger &out, const char *text);
    friend Logger &operator << (Logger &out, const QString &text);
    friend Logger &operator << (Logger &out, int val);
    friend Logger &operator << (Logger &out, CommandSymbol sym);

private:
    Logger();

    // Not implement, forbid usage.
    Logger(const Logger &);
    void operator=(const Logger &);

    void unloadBuff();

    QString log;
    QString buff;
};

#endif // LOGGER_H
