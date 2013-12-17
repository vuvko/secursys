#ifndef PROFILEVIEWER_H
#define PROFILEVIEWER_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QGridLayout>
#include <QGroupBox>
#include <QTreeView>
#include <QMessageBox>
#include <QStandardItem>

#include "apphandler.h"

class AppHandler;

class ProfileViewer : public QMainWindow
{
    Q_OBJECT
public:
    explicit ProfileViewer(AppHandler *handler_ = 0, QWidget *parent = 0);
    
signals:
    void update();
    
public slots:
    void updateResult(bool isUpdated);

private slots:
    void onUpdate();
    void onAbout();

private:
    AppHandler *handler;

    QMenu *fileMenu;
    QAction *updateAct;
    QAction *exitAct;

    QMenu *helpMenu;
    QAction *aboutAct;

    QToolBar *fileBar;
    
    QWidget *centralWidget;
    QGridLayout *mainLayout;
    QGroupBox *userBox;
    QGridLayout *userLayout;
    QLabel *userLabel;
    QLabel *groupLabel;
    QGroupBox *permissionBox;
    QGridLayout *permissionLayout;
    QLabel *filesLabel;
    QTreeView *filesList;
    QLabel *drivesLabel;
    QTreeView *drivesList;
    QLabel *dirsLabel;
    QTreeView *dirsList;
    QLabel *programsLabel;
    QTreeView *programsList;

    QStandardItemModel *drivesModel;
    QStandardItemModel *filesModel;
    QStandardItemModel *dirsModel;
    QStandardItemModel *programsModel;

    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    void loadFiles();
    void loadDrives();
    void loadDirs();
    void loadPrograms();
};

#endif // PROFILEVIEWER_H
