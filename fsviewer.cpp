#include "fsviewer.h"
#include "profile.h"
#include "accesscontrol.h"

FileView::FileView(FSViewer *parent_)
    : QTreeView(parent_)
{
    parent = parent_;
    dirModel = new QStandardItemModel(0, 5, this);
    dirModel->setHeaderData(0, Qt::Horizontal, QObject::tr(""));
    dirModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Название"));
    dirModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Дата последней модификации"));
    dirModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Гриф"));
    dirModel->setHeaderData(4, Qt::Horizontal, QObject::tr("Размер"));
    setModel(dirModel);
    setColumnWidth(0, 50);
    setColumnWidth(1, 250);
    setColumnWidth(2, 200);
    setColumnWidth(3, 100);
}

FileView::~FileView()
{}

//////// /from /////
#if 0

bool
FileView::rmdir()
{
    qDebug() << "Здесь надо проверить право на удаление каталога."; // TODO: log

    int row = selectedIndexes()[0].row();
    QFileInfo dirInfo = currentDir.entryInfoList()[row];
    if (!dirInfo.isDir()) {
        return false;
    }

    QDir dir = dirInfo.dir();
    QString dirCPath = dir.canonicalPath();

    if (!tryAccessDir(ACCESS_WRITE) || !tryAccessDir(dir, ACCESS_READ | ACCESS_WRITE))
        return false;

    if (!dir.entryList().isEmpty())
        return false;

    if (!currentDir.rmdir(dirCPath))
        return false;

    update();
    return true;
}

bool
FileView::editFile(const QString &nameArg)
{
    qDebug() << "Здесь надо проверить на доступ к созданию файлов."; // TODO: log

    if (!tryAccessDir(ACCESS_WRITE))
        return false;

    QString name = nameArg;

    if (name.isEmpty()) {
        name = QInputDialog::getText(this, tr("Создание секретного файла"),
            tr("Название нового файла:"));
    } /*else if (info.suffix() != secret_suffix) {
        return false;
    }*/

    if (name.isEmpty())
        return false;
    
    QString newFilePath = currentDir.canonicalPath() + QDir::separator() + name;
    ac->setDefaultModeFile(newFilePath);
    emit openFile(newFilePath);
    update();
    return true;
}

bool
FileView::rm()
{
    qDebug() << "Здесь надо проверить право на удаление файлов."; // TODO: log

    if (!tryAccessDir(ACCESS_WRITE))
        return false;

    int row = selectedIndexes()[0].row();
    QFileInfo fileInfo = currentDir.entryInfoList()[row];
    if (!fileInfo.isFile() || fileInfo.suffix() != secret_suffix) {
        return false;
    }

    QString fileCPath = fileInfo.canonicalPath();

    if (!QFile::remove(fileCPath))
        return false;

    update();
    return true;
}

bool
FileView::check()
{
    int row = selectedIndexes()[0].row();
    QFileInfo info = currentDir.entryInfoList()[row];
    qDebug() << "Проверка целостности файла" << info.filePath(); // TODO: log
//    qDebug() << "Hash of file" << info.filePath() << curHash.toHex().toUpper();
    return ac->checkHashFile(info.canonicalPath());
}

bool
FileView::exec(const QString &cpath)
{
    qDebug() << "Здесь нужно проверить на право запуска файла."; // TODO: log
    if (!ac->checkAccessProgramExec(cpath))
        return false;

    return system(cpath.toStdString().c_str()) != -1;
}

#endif
////// to //////

void
FileView::onUp()
{
    AccessControl::getInstance().cd("..");
    update();
}

void
FileView::update()
{
    QDir pwd(Profile::getInstance().getPWD());
    pwd.setSorting(QDir::DirsFirst | QDir::Name);
    pwd.refresh();
    qDebug() << "Текущий каталог:" << pwd.canonicalPath();
    QFileInfoList infoList = pwd.entryInfoList();
    int idx = 0;
    dirModel->setRowCount(infoList.size());
    for (auto info : infoList) {
        QIcon type(":/icons/file.png");
        if (info.fileName() == ".") {
            type = QIcon(":/icons/home.png");
        } else if (info.fileName() == "..") {
            type = QIcon(":/icons/up.png");
        } else if (info.isDir()) {
            type = QIcon(":/icons/dir.png");
        } else if (info.isExecutable()) {
            type = QIcon(":/icons/exec.png");
        } else if (info.suffix() == secret_suffix) {
            type = QIcon(":/icons/secret.png");
        }
        dirModel->setData(dirModel->index(idx, 0), type, Qt::DecorationRole);
        dirModel->setData(dirModel->index(idx, 1), info.fileName());
        dirModel->setData(dirModel->index(idx, 2), info.lastModified());
        dirModel->setData(dirModel->index(idx, 3), "Не секретно"); // TODO: заглушка
        dirModel->setData(dirModel->index(idx, 4), info.size());
        ++idx;
    }
    setModel(dirModel);
    parent->changePath(pwd.canonicalPath());
    QTreeView::update();
}

void
FileView::mouseDoubleClickEvent(QMouseEvent *)
{
    QDir pwd(Profile::getInstance().getPWD());
    pwd.setSorting(QDir::DirsFirst | QDir::Name);
    pwd.refresh();

    int row = selectedIndexes()[0].row();
    QFileInfo info = pwd.entryInfoList()[row];
    if (info.isDir()) {
        AccessControl::getInstance().cd(info.fileName());
    } else if (info.isExecutable()) {
        AccessControl::getInstance().exec(info.canonicalFilePath());
    } else {
        editFile(info.canonicalFilePath());
    }
}

//-----------
// FSViewer
//-----------

FSViewer::FSViewer(QWidget *parent)
    : QMainWindow(parent)
{
    // Creating UI
    qDebug() << "Инициализация пользовательского интерфейса.";
    // main Layout
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);
    mainLayout = new QGridLayout;
    centralWidget->setLayout(mainLayout);

    dirView = new FileView(this);
    dirView->setAlternatingRowColors(true);

    driveBox = new QComboBox;
    for(auto info : QDir::drives()) {
        driveBox->addItem(info.canonicalPath());
    }
    connect(driveBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onDriveChange(QString)));

    driveLabel = new QLabel(tr("Сменить диск:"));
    pathNameLabel = new QLabel(tr("Путь:"));
    pathEdit = new QLineEdit;
    connect(pathEdit, SIGNAL(returnPressed()), this, SLOT(onPathChange()));

    mainLayout->addWidget(driveLabel, 0, 0);
    mainLayout->addWidget(driveBox, 0, 1);
    mainLayout->addWidget(pathNameLabel, 1, 0);
    mainLayout->addWidget(pathEdit, 1, 1);
    mainLayout->addWidget(dirView, 2, 0, 1, 2);
    mainLayout->setColumnStretch(0, 0);
    mainLayout->setColumnStretch(1, 1);

    createActions();
    createMenu();
    createToolBar();

    resize(800, 500);
    setWindowIcon(QIcon(":/icons/dir.png"));
    setWindowTitle(tr("Просмотр файловой системы"));
    // UI created.
    qDebug() << "Пользовательский интерфейс создан.";
    AccessControl::getInstance().cd(Profile::getInstance().getPWD());
    connect(dirView, SIGNAL(openFile(QString)), this, SLOT(onOpenFile(QString)));
}

void
FSViewer::createActions()
{
    mnuDirCreate = new QAction(QIcon(":/icons/dir_empty.png"), tr("Создать каталог"), this);
    connect(mnuDirCreate, SIGNAL(triggered()), this, SLOT(onDirCreate()));

    mnuDirDelete = new QAction(QIcon(":/icons/dir_delete.png"), tr("Удалить каталог"), this);
    connect(mnuDirDelete, SIGNAL(triggered()), this, SLOT(onDirDelete()));

    mnuFileCreate = new QAction(QIcon(":/icons/secret.png"), tr("Создать секретный файл"), this);
    connect(mnuFileCreate, SIGNAL(triggered()), this, SLOT(onFileCreate()));

    mnuFileDelete = new QAction(QIcon(":/icons/secret_delete.png"), tr("Удалить секретный файл"), this);
    connect(mnuFileDelete, SIGNAL(triggered()), this, SLOT(onFileDelete()));

    mnuCheckHash = new QAction(tr("Проверить целостность"), this);
    connect(mnuCheckHash, SIGNAL(triggered()), this, SLOT(onCheckHash()));

    mnuUp = new QAction(QIcon(":/icons/up.png"), tr("Вверх"), this);
    mnuUp->setShortcut(tr("Alt+Up"));
    connect(mnuUp, SIGNAL(triggered()), dirView, SLOT(onUp()));

    mnuExit = new QAction(QIcon(":/icons/exit.png"), tr("Выход"), this);
    mnuExit->setShortcut(tr("Ctrl+Q"));
    connect(mnuExit, SIGNAL(triggered()), this, SLOT(close()));

    mnuAbout = new QAction(tr("О программе"), this);
    mnuAbout->setShortcut(tr("F1"));;
    connect(mnuAbout, SIGNAL(triggered()), this, SLOT(onAbout()));

    mnuUser = new QAction(QIcon(":/icons/user.png"), tr("Профиль"), this);
    connect(mnuUser, SIGNAL(triggered()), this, SLOT(onProfile()));
}

void
FSViewer::createMenu()
{
    fileMenu = menuBar()->addMenu(tr("Файл"));
    fileMenu->addAction(mnuDirCreate);
    fileMenu->addAction(mnuDirDelete);
    fileMenu->addAction(mnuFileCreate);
    fileMenu->addAction(mnuFileDelete);
    fileMenu->addAction(mnuCheckHash);
    fileMenu->addAction(mnuUp);
    fileMenu->addSeparator();
    fileMenu->addAction(mnuExit);

    userMenu = menuBar()->addMenu(tr("Пользователь"));
    userMenu->addAction(mnuUser);

    helpMenu = menuBar()->addMenu(tr("Помощь"));
    helpMenu->addAction(mnuAbout);
}

void
FSViewer::createToolBar()
{
    fileBar = addToolBar(tr("Файл"));
    fileBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    fileBar->addAction(mnuDirCreate);
    fileBar->addAction(mnuDirDelete);
    fileBar->addAction(mnuFileCreate);
    fileBar->addAction(mnuFileDelete);
}

void
FSViewer::changePath(const QString &path)
{
    pathEdit->setText(path);
}

// slots

void
FSViewer::onAbout()
{
    QMessageBox::about(this, tr("О программе."),
                       tr("Учебная система защиты информации от несанкционированного доступа."));
}

void
FSViewer::onDriveChange(const QString &drive)
{
    AccessControl::getInstance().cd(drive);
    update();
}

void
FSViewer::onDirCreate()
{
    QString name = QInputDialog::getText(this, tr("Создание каталога"), tr("Название нового каталога:"));
    if (name.isEmpty())
        return;

    AccessControl::getInstance().mkdir(name);
    update();
}

void
FSViewer::onDirDelete()
{
    QDir pwd(Profile::getInstance().getPWD());
    pwd.setSorting(QDir::DirsFirst | QDir::Name);
    pwd.refresh();

    int row = dirView->selectedIndexes()[0].row();
    QFileInfo dirInfo = pwd.entryInfoList()[row];
    QString path = dirInfo.canonicalPath();

    AccessControl::getInstance().rmdir(path);
    update();
}

void
FSViewer::onFileCreate()
{
    dirView->editFile();
}

void
FSViewer::onFileDelete()
{
    QDir pwd(Profile::getInstance().getPWD());
    pwd.setSorting(QDir::DirsFirst | QDir::Name);
    pwd.refresh();

    int row = dirView->selectedIndexes()[0].row();
    QFileInfo fileInfo = currentDir.entryInfoList()[row];
    if (fileInfo.suffix() != secret_suffix)
        return;
    QString path = dirInfo.canonicalPath();

    AccessControl::getInstance().rm(path);
}

void
FSViewer::onCheckHash()
{
    if (AccessControl::getInstance().check()) {
        // TODO
    } else {
        // TODO
    }
}

void
FSViewer::onOpenFile(const QString &fileName)
{
    emit openFile(fileName);
}

void
FSViewer::onProfile()
{
    emit openProfile();
}

void
FSViewer::onPathChange()
{
    QDir dir;
    QString path = pathEdit->text();
    if (dir.exists(path)) {
        AccessControl::getInstance().cd(path);
    } else {
        QMessageBox::warning(this, tr("Ошибка перехода в каталог"), tr("Не существует каталога ") + path);
    }
}
