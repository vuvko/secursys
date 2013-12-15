#ifndef LOGGER_H
#define LOGGER_H

#include <QFile>
#include <QTextStream>
#include <QTime>
#include <QDateTime>

class Logger
{
public:
    Logger(const QString &fileName);
    Logger(QTextStream &stream);
    ~Logger();

    QDateTime datestamp(const QDateTime &dateTime = QDateTime::currentDateTime());
    QTime timestamp(const QTime &time = QTime::currentTime());

    friend Logger operator << (Logger &out, const QString &text);
    friend Logger operator << (Logger &out, int val);

private:
    QFile logFile;
    QTextStream log;
    bool closeAfter;
};

#endif // LOGGER_H
