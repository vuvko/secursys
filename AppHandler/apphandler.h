#ifndef APPHANDLER_H
#define APPHANDLER_H

#include <QObject>
#include <QThread>

#include "fileeditor.h"
#include "fsviewer.h"

class AppHandler : public QObject
{
    Q_OBJECT
public:
    explicit AppHandler(QObject *parent = 0);
    void startFS(const QString &path=".");

signals:

public slots:
    void openFile(const QString &fileName);
    void openFS(const QString &path = ".");

};

#endif // APPHANDLER_H
