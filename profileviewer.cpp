#include "profileviewer.h"

ProfileViewer::ProfileViewer(const Profile &profile_, AppHandler *handler_, QWidget *parent) :
    QMainWindow(parent)
{
    handler = handler_;
    profile = profile_;
    centralWidget = new QWidget;
    mainLayout = new QGridLayout;
    setCentralWidget(centralWidget);
    centralWidget->setLayout(mainLayout);

    userBox = new QGroupBox(tr("Информация о пользователе"));
    userLabel = new QLabel(tr("Пользователь: ") + profile.userName());
    groupLabel = new QLabel(tr("Группа: ") + profile.groupName());
    userLayout = new QGridLayout;

    permissionBox = new QGroupBox(tr("Права на доступ"));
    filesLabel = new QLabel(tr("Файлы:"));
    filesList = new QTreeView;
    dirsLabel = new QLabel(tr("Каталоги:"));
    dirsList = new QTreeView;
    drivesLabel = new QLabel(tr("Диски:"));
    drivesList = new QTreeView;
    programsLabel = new QLabel(tr("Программы:"));
    programsList = new QTreeView;
    permissionLayout = new QGridLayout;

    loadFiles();
    loadDrives();
    loadDirs();
    loadPrograms();

    userLayout->addWidget(userLabel);
    userLayout->addWidget(groupLabel);
    userBox->setLayout(userLayout);
    mainLayout->addWidget(userBox);
    permissionLayout->addWidget(filesLabel);
    permissionLayout->addWidget(filesList);
    permissionLayout->addWidget(drivesLabel);
    permissionLayout->addWidget(drivesList);
    permissionLayout->addWidget(dirsLabel);
    permissionLayout->addWidget(dirsList);
    permissionLayout->addWidget(programsLabel);
    permissionLayout->addWidget(programsList);
    permissionBox->setLayout(permissionLayout);
    mainLayout->addWidget(permissionBox);

    mainLayout->setRowStretch(0, 0);
    mainLayout->setRowStretch(1, 1);

    permissionLayout->setRowStretch(0, 0);
    permissionLayout->setRowStretch(1, 1);
    permissionLayout->setRowStretch(2, 0);
    permissionLayout->setRowStretch(3, 1);
    permissionLayout->setRowStretch(4, 0);
    permissionLayout->setRowStretch(5, 1);
    permissionLayout->setRowStretch(6, 0);
    permissionLayout->setRowStretch(7, 1);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    resize(800, 600);
    setWindowTitle(tr("Просмотр профиля пользователя "));
    setWindowIcon(QIcon(":/icons/user_info.png"));
}

void
ProfileViewer::createActions()
{
    updateAct = new QAction(QIcon(":/icons/save.png"), tr("Обновить"), this);
    updateAct->setShortcut(tr("Ctrl+S"));
    connect(updateAct, SIGNAL(triggered()), this, SLOT(onUpdate()));

    exitAct = new QAction(QIcon(":/icons/exit.png"), tr("Выход"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("О программе"), this);
    aboutAct->setShortcut(tr("F1"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(onAbout()));
}

void
ProfileViewer::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("Файл"));
    fileMenu->addAction(updateAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    helpMenu = menuBar()->addMenu(tr("Помощь"));
    helpMenu->addAction(aboutAct);
}

void
ProfileViewer::createToolBars()
{
    fileBar = addToolBar(tr("Файл"));
    fileBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    fileBar->addAction(updateAct);
}

void
ProfileViewer::createStatusBar()
{
    statusBar()->showMessage(tr("Готово."));
}

QString
ProfileViewer::modeToStr(int mode)
{
    QString strMode;
    switch (mode) {
    case Profile::READ:
        strMode = tr("Чтение");
        break;
    case Profile::WRITE:
        strMode = tr("Запись");
        break;
    case Profile::EXEC:
        strMode = tr("Выполнение");
        break;
    case Profile::READ | Profile::WRITE:
        strMode = tr("Полный доступ");
        break;
    default:
        strMode = tr("Нет доступа");
        break;
    }
    return strMode;
}

void
ProfileViewer::loadModel(QStandardItemModel *model, const QHash<QString, int> &data)
{
    model = new QStandardItemModel(0, 2, this);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Путь"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Права"));
    model->setRowCount(data.size());
    int idx = 0;
    for (auto it = data.begin(); it != data.end(); ++it) {
        model->setData(model->index(idx, 0), it.key());
        model->setData(model->index(idx, 1), modeToStr(it.value()));
        ++idx;
    }
}

void
ProfileViewer::loadFiles()
{
    loadModel(filesList, profile.files());
    filesList->setModel(filesModel);
    filesList->setColumnWidth(0, 500);
}

void
ProfileViewer::loadDrives()
{
    loadModel(drivesList, profile.drives());
    drivesList->setModel(drivesModel);
    drivesList->setColumnWidth(0, 500);
}

void
ProfileViewer::loadDirs()
{
    loadModel(dirsList, profile.dirs());
    dirsList->setModel(dirsModel);
    dirsList->setColumnWidth(0, 500);
}

void
ProfileViewer::loadPrograms()
{
    loadModel(programsList, profile.programs());
    programsList->setModel(programsModel);
    programsList->setColumnWidth(0, 500);
}

void
ProfileViewer::onUpdate()
{
    emit update();
}

void
ProfileViewer::onAbout()
{
    QMessageBox::about(this, tr("О программе."),
                       tr("Модуль просмотра профайла пользователя."));
}

void
ProfileViewer::updateResult(bool isUpdated)
{
    if (isUpdated) {
        statusBar()->showMessage(tr("Профиль обновлён"), 2000);
    } else {
        statusBar()->showMessage(tr("Не удалось сохранить профиль"), 2000);
    }
}
