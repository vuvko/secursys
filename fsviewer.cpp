#include "fsviewer.h"
#include "profile.h"
#include "accesscontrol.h"

FileView::FileView(FSViewer *parent_)
    : QTreeView(parent_),
    pwd(Profile::getInstance().getConstDirPWD())
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

bool FileView::cd(QString path)
{
    bool res = AccessControl::getInstance().cd(path);
    update();
    return res;
}

bool FileView::mkdir()
{
    QString name = QInputDialog::getText(this, tr("Создание каталога"),
            tr("Название нового каталога:"));

    if (name.isEmpty())
        return false;

    bool res = AccessControl::getInstance().mkdir(name);
    update();
    return res;
}

bool FileView::rmdir()
{
    int row = selectedIndexes()[0].row();
    QFileInfo info = pwd.entryInfoList()[row];
    bool res = AccessControl::getInstance().rmdir(info.absoluteFilePath());
    update();
    return res;
}

bool FileView::rm()
{
    int row = selectedIndexes()[0].row();
    QFileInfo info = pwd.entryInfoList()[row];
    if (info.suffix() != secret_suffix)
        return false;
    bool res = AccessControl::getInstance().rm(info.absoluteFilePath());
    update();
    return res;
}

bool FileView::exec(QString path)
{
    bool res = AccessControl::getInstance().exec(path);
    update();
    return res;
}

bool FileView::check()
{
    int row = selectedIndexes()[0].row();
    QFileInfo info = pwd.entryInfoList()[row];
    bool res = AccessControl::getInstance().check(info.absoluteFilePath());
    update();
    return res;
}

bool
FileView::editFile(const QString &nameArg)
{
    QString name = nameArg;

    if (name.isEmpty()) {
        name = QInputDialog::getText(this, tr("Создание секретного файла"),
            tr("Название нового файла:"));

        if (name.isNull())
            return false;
    }

    if (name.isEmpty() || QFileInfo(name).suffix() != secret_suffix) {
        QMessageBox::warning(NULL, tr("Недопустимое имя"),
            tr("Имя секретного файла не должно быть пустым и должно оканчиваться на \".sf\"."));
        return false;
    }

    emit openFile(name);
    update();
    return true;
}

void
FileView::onUp()
{
    cd("..");
}

void
FileView::update()
{
    pwd.refresh();
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

        AccessControl &ac = AccessControl::getInstance();
        Role role =
            info.isExecutable() ? ac.getRoleProgram(info.fileName()) :
            info.isDir()        ? ac.getRoleDir(    info.fileName()) :
            info.isFile()       ? ac.getRoleFile(   info.fileName()) :
            ROLE_NOTHING;
        // TODO: drives

        QString roleStr =
            (role == ROLE_NOTHING) ? tr("Не конфиденциально") :
            (role == ROLE_CONFIDENTIAL) ? tr("Конфиденциально") :
            (role == ROLE_STRICT_CONFIDENTIAL) ? tr("Строго конфиденциально") :
            tr("Битая метка конфиденциальности");

        dirModel->setData(dirModel->index(idx, 0), type, Qt::DecorationRole);
        dirModel->setData(dirModel->index(idx, 1), info.fileName());
        dirModel->setData(dirModel->index(idx, 2), info.lastModified());
        dirModel->setData(dirModel->index(idx, 3), roleStr);
        dirModel->setData(dirModel->index(idx, 4), info.size());
        ++idx;
    }
    setModel(dirModel);
    parent->changePath(pwd.absolutePath());
    QTreeView::update();
}

void
FileView::mouseDoubleClickEvent(QMouseEvent *)
{
    int row = selectedIndexes()[0].row();
    QFileInfo info = pwd.entryInfoList()[row];
    if (info.isDir()) {
        cd(info.fileName());
    } else if (info.isExecutable()) {
        exec(info.absoluteFilePath());
    } else {
        editFile(info.fileName());
    }
}

//-----------
// FSViewer
//-----------

FSViewer::FSViewer(QWidget *parent)
    : QMainWindow(parent)
{
    // Creating UI
    // main Layout
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);
    mainLayout = new QGridLayout;
    centralWidget->setLayout(mainLayout);

    dirView = new FileView(this);
    dirView->setAlternatingRowColors(true);

    driveBox = new QComboBox;
    for(auto info : QDir::drives()) {
        driveBox->addItem(info.absolutePath());
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
    dirView->cd(Profile::getInstance().getPWD());
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

    mnuPanel = new QAction(QIcon(":/icons/user_info.png"), "Панель администрирования", this);
    connect(mnuPanel, SIGNAL(triggered()), this, SLOT(onPanel()));
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
    userMenu->addAction(mnuPanel);

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
    dirView->cd(drive);
    update();
}

void
FSViewer::onDirCreate()
{
    dirView->mkdir();
    update();
}

void
FSViewer::onDirDelete()
{
    dirView->rmdir();
    update();
}

void
FSViewer::onFileCreate()
{
    dirView->editFile();
    update();
}

void
FSViewer::onFileDelete()
{
    dirView->rm();
    update();
}

void
FSViewer::onCheckHash()
{
    // TODO
    if (AccessControl::getInstance().check("")) {
    } else {
    }
    update();
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
FSViewer::onPanel()
{
    emit openPanel();
}

void
FSViewer::onPathChange()
{
    QDir pwd(Profile::getInstance().getPWD());
    QString path = pathEdit->text();
    if (QFileInfo(pwd, path).exists()) {
        dirView->cd(path);
    } else {
        QMessageBox::warning(this, tr("Ошибка перехода в каталог"), tr("Не существует каталога ") + path);
    }
}
