#include "logger.h"

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
{}

QString
Logger::read()
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

#ifdef DEBUG
#include <QtDebug>
#endif

Logger &
operator << (Logger &out, Logger::CommandSymbol sym)
{
    switch (sym) {
    case Logger::LOG_ENDL:
#ifdef DEBUG
        qDebug() << out.buff.toStdString().c_str();
#endif
        out.buff.append('\n');
        out.unloadBuff();
        break;
    case Logger::LOG_DATESTAMP:
        out.buff.append(QDateTime::currentDateTime().toString(Qt::TextDate));
        break;
    case Logger::LOG_TIMESTAMP:
        out.buff.append(QTime::currentTime().toString(Qt::TextDate));
        break;
    default:
        break;
    }
    return out;
}
