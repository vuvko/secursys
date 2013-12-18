#include "logger.h"

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
{
    log = "[" + QDateTime::currentDateTime().toString(Qt::TextDate) + "] Новая сессия.\n";
}

QString
Logger::unload()
{
    return log;
}

void
Logger::unloadBuff()
{
    log.append("[" + QDateTime::currentDateTime().toString(Qt::TextDate) + "] " + buff);
    buff.clear();
}

Logger &
operator << (Logger &out, const char *text)
{
    out.buff.append(text).append(" ");
    return out;
}

Logger &
operator << (Logger &out, const QString &text)
{
    out.buff.append(text).append(" ");
    return out;
}

Logger &
operator << (Logger &out, int val)
{
    out.buff.append(QString::number(val)).append(" ");
    return out;
}

Logger &
operator << (Logger &out, Logger::CommandSymbol sym)
{
    switch (sym) {
    case Logger::ENDL:
        out.buff.append('\n');
        out.unloadBuff();
        break;
    case Logger::DATESTAMP:
        out.buff.append(QDateTime::currentDateTime().toString(Qt::TextDate));
        break;
    case Logger::TIMESTAMP:
        out.buff.append(QTime::currentTime().toString(Qt::TextDate));
        break;
    default:
        break;
    }
    return out;
}
