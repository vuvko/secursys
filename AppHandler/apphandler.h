#ifndef APPHANDLER_H
#define APPHANDLER_H

#include <QObject>
#include <QThread>

#include "fileeditor.h"
#include "fsviewer.h"
#include "logindialog.h"

class AppHandler : public QObject
{
    Q_OBJECT
public:
    explicit AppHandler(QObject *parent = 0);
    void startFS(const QString &path=".");
    void startLogin();

signals:

public slots:
    void openFile(const QString &fileName);
    void openFS(const QString &path = ".");
    void openLogin();

private:
    void startInThread(QThread *thread, QWidget *widget);
};

#endif // APPHANDLER_H
