#ifndef FSViewer_H
#define FSViewer_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QToolBar>
#include <QGridLayout>
#include <QMessageBox>
#include <QTreeView>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QDateTime>
#include <QMouseEvent>
#include <QComboBox>
#include <QStandardItem>
#include <QLabel>
#include <QPushButton>
#include <QInputDialog>
#include <QStatusBar>

#include <QDir>
#include <QFile>

#include <QDebug>

#include "apphandler.h"
#include "crypto.h"

class AccessControl;

class AppHandler;

class FSViewer;

class FileView : public QTreeView
{
    Q_OBJECT

public:
    constexpr const static char *secret_suffix = "sf"; // *.sf --- секретные файлы

    explicit FileView(FSViewer *parent_,
                      AppHandler *handler_);
    ~FileView();

    bool cd(const QString &path);
    bool cd(const QDir &dir);
    bool mkdir();
    bool rmdir();
    bool createFile();
    bool rm();
    bool check();

public slots:
    void onUp();
    void update();

signals:
    void openFile(const QString &fileName);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    AppHandler *handler;
    FSViewer *parent;
    QStandardItemModel *dirModel;
    QDir currentDir;

    AccessControl *ac;
};

class FSViewer : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit FSViewer(const QString &path = ".",
                      AppHandler *handler_ = 0,
                      QWidget *parent = 0);

    void changePath(const QString &path);

private slots:
    void onAbout();
    void onDriveChange(const QString &drive);
    void onDirCreate();
    void onDirDelete();
    void onFileCreate();
    void onFileDelete();
    void onCheckHash();
    void onOpenFile(const QString &fileName);
    void onProfile();

signals:
    void openFile(const QString &fileName);
    void openProfile();

private:
    AppHandler *handler;

    QWidget *centralWidget;
    QGridLayout *mainLayout;
    QComboBox *driveBox;
    QLabel *driveLabel;
    QLabel *pathNameLabel;
    QLabel *pathLabel;

    QMenu *fileMenu;
    QAction *mnuDirCreate;
    QAction *mnuDirDelete;
    QAction *mnuFileCreate;
    QAction *mnuFileDelete;
    QAction *mnuCheckHash;
    QAction *mnuUp;
    QAction *mnuExit;

    QMenu *userMenu;
    QAction *mnuUser;

    QMenu *helpMenu;
    QAction *mnuAbout;

    QToolBar *fileBar;

    FileView *dirView;

    void createMenu();
    void createActions();
    void createToolBar();
};

#endif // FSViewer_H
