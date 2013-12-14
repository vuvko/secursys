#ifndef FILEVIEWER_H
#define FILEVIEWER_H

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

#include <QDir>
#include <QFile>

#include <QDebug>

class FileView : public QTreeView
{
    Q_OBJECT

public:
    constexpr static char *secret_suffix = "sf"; // *.sf --- секретные файлы

    explicit FileView(QWidget *parent = 0);
    ~FileView();

    bool cd(const QString &path);
    bool cd(const QDir &dir);
    void update();
    bool mkdir();
    bool rmdir();
    bool createFile();
    bool rm();

public slots:
    void onUp();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    QStandardItemModel *dirModel;
    QDir currentDir;
};

class FileViewer : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit FileViewer(QWidget *parent = 0);
    ~FileViewer();

private slots:
    void onAbout();
    void onDriveChange(const QString &drive);
    void onDirCreate();
    void onDirDelete();
    void onFileCreate();
    void onFileDelete();

private:
    QWidget *centralWidget;
    QGridLayout *mainLayout;
    QComboBox *driveBox;
    QLabel *driveLabel;

    QMenu *fileMenu;
    QAction *mnuDirCreate;
    QAction *mnuDirDelete;
    QAction *mnuFileCreate;
    QAction *mnuFileDelete;
    QAction *mnuUp;
    QAction *mnuExit;

    QMenu *helpMenu;
    QAction *mnuAbout;

    QToolBar *fileBar;
    QAction *toolDirCreate;
    QAction *toolDirDelete;
    QAction *toolFileCreate;
    QAction *toolFileDelete;

    FileView *dirView;

    void createMenu();
    void createActions();
    void createToolBar();
};

#endif // FILEVIEWER_H
