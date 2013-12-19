#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QMainWindow>
#include <QLabel>
#include <QTabWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QTreeView>
#include <QStandardItemModel>
#include <QComboBox>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>

#include "logger.h"
#include "accesscontrol.h"
#include "accessadmin.h"

class ControlPanel : public QMainWindow
{
    Q_OBJECT
public:
    explicit ControlPanel(QWidget *parent = 0);
    
signals:
    
public slots:

private slots:
    void onRegister();
    void onAddGroup();
    void updateData();
    void onAbout();

private:
    QMenu *fileMenu;
    QAction *updateAct;
    QAction *exitAct;

    QMenu *helpMenu;
    QAction *aboutAct;

    QToolBar *fileBar;

    QTabWidget *tabs;
    QWidget *permWidget;
    QGridLayout *permLayout;
    QLabel *userLLabel;
    QTreeView *userLList;
    QLabel *groupLLabel;
    QTreeView *groupLList;
    QLabel *permLabel;
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
    QStandardItemModel *usersModel;
    QStandardItemModel *groupsModel;

    QWidget *regWidget;
    QGridLayout *regLayout;
    QLabel *regLabel;
    QLabel *userLabel;
    QLineEdit *userEdit;
    QLabel *passLabel;
    QLineEdit *passEdit;
    QLabel *groupLabel;
    QComboBox *groupBox;
    QPushButton *regButton;
    QLabel *group2Label;
    QLineEdit *groupEdit;
    QPushButton *addGButton;

    QPlainTextEdit *logEdit;

    void createActions();
    void createMenus();
    void createToolbars();

    QString modeToStr(int mode);
    int strToMode(const QString &str);
    QString roleToStr(int role);
    Role strToRole(const QString &str);

    QList<AccessObject> unloadModel(QAbstractItemModel *model);
    void loadModel(QStandardItemModel *model, const QList<AccessObject> &data);
    void loadFiles();
    void loadDrives();
    void loadDirs();
    void loadPrograms();
    void loadUsers();
    void loadGroups();
};

#endif // CONTROLPANEL_H
