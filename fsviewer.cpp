#include "fsviewer.h"

FileView::FileView(FSViewer *parent_, AppHandler *handler_)
    : QTreeView(parent_)
{
    handler = handler_;
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
    currentDir.setSorting(QDir::DirsFirst | QDir::Name);
}

bool
FileView::cd(const QString &path)
{
    qDebug() << "Здесь надо проверять на доступ к каталогу."; // TODO: заглушка
    bool r = currentDir.cd(path);
    update();
    return r;
}

bool
FileView::cd(const QDir &dir)
{
    qDebug() << "Здесь надо проверять на доступ к каталогу."; // TODO: заглушка
    bool r = currentDir.cd(dir.absolutePath());
    update();
    return r;
}

bool
FileView::mkdir()
{
    qDebug() << "Здесь надо проверить на доступ к созданию каталогов."; // TODO: заглушка
    QString name = QInputDialog::getText(this, tr("Создание каталога"), tr("Название нового каталога:"));
    bool r = false;
    if (!name.isEmpty()) {
        r = currentDir.mkdir(name);
        update();
    }
    return r;
}

bool
FileView::rmdir()
{
    qDebug() << "Здесь надо проверить право на удаление каталога."; // TODO: заглушка
    int row = selectedIndexes()[0].row();
    QFileInfo info = currentDir.entryInfoList()[row];
    if (!info.isDir()) {
        return false;
    }
    bool r = currentDir.rmdir(info.fileName());
    update();
    return r;
}

bool
FileView::createFile()
{
    qDebug() << "Здесь надо проверить на доступ к созданию файлов."; // TODO: заглушка
    QString name = QInputDialog::getText(this, tr("Создание секретного файла"), tr("Название нового файла:"));
    bool r = false;
    if (!name.isEmpty()) {
        emit openFile(currentDir.absolutePath() + QDir::separator() + name);
        update();
    }
    return r;
}

bool
FileView::rm()
{
    qDebug() << "Здесь надо проверить право на удаление файлов."; // TODO: заглушка
    int row = selectedIndexes()[0].row();
    QFileInfo info = currentDir.entryInfoList()[row];
    if (!info.isFile() || info.suffix() != secret_suffix) {
        return false;
    }
    bool r = QFile::remove(info.fileName());
    update();
    return r;
}

void
FileView::check()
{
    int row = selectedIndexes()[0].row();
    QFileInfo info = currentDir.entryInfoList()[row];
    qDebug() << "Проверка целостности файла" << info.filePath();
    QFile file(info.filePath());
    file.open(QFile::ReadOnly);
    QByteArray curHash = handler->get_hash(file.readAll()); // TODO: заглушка
    qDebug() << "Hash of file" << info.filePath() << curHash.toHex().toUpper();
}

void
FileView::onUp()
{
    qDebug() << "Поднятие на директорию выше. Нужно проверить права доступа."; // TODO: заглушка
    currentDir.cdUp();
    update();
}

void
FileView::update()
{
    currentDir.refresh();
    qDebug() << "Текущий каталог:" << currentDir.absolutePath();
    QFileInfoList infoList = currentDir.entryInfoList();
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
    parent->changePath(currentDir.absolutePath());
    QTreeView::update();
}

void
FileView::mouseDoubleClickEvent(QMouseEvent *)
{
    int row = selectedIndexes()[0].row();
    QFileInfo info = currentDir.entryInfoList()[row];
    if (info.isDir()) {
        qDebug() << "Здесь нужно проверять доступ к каталогу."; // TODO: заглушка
        currentDir.cd(info.fileName());
        update();
    } else if (info.isExecutable()) {
        qDebug() << "Здесь нужно проверить на право запуска файла."; // TODO: заглушка
        system(info.absoluteFilePath().toStdString().c_str());
    } else {
        qDebug() << "Здесь нужно проверять на доступ к файлу."; // TODO: заглушка
        if (info.suffix() == secret_suffix) {
            emit openFile(info.absoluteFilePath());
        }
    }
}

//-----------
// FSViewer
//-----------

FSViewer::FSViewer(const QString &path, AppHandler *handler_, QWidget *parent)
    : QMainWindow(parent)
{
    handler = handler_;
    // Creating UI
    qDebug() << "Инициализация пользовательского интерфейса.";
    // main Layout
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);
    mainLayout = new QGridLayout;
    centralWidget->setLayout(mainLayout);

    dirView = new FileView(this, handler_);
    dirView->setAlternatingRowColors(true);

    driveBox = new QComboBox;
    for(auto info : QDir::drives()) {
        driveBox->addItem(info.absolutePath());
    }
    connect(driveBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onDriveChange(QString)));

    driveLabel = new QLabel(tr("Сменить диск:"));
    pathNameLabel = new QLabel(tr("Путь:"));
    pathLabel = new QLabel;

    mainLayout->addWidget(driveLabel, 0, 0);
    mainLayout->addWidget(driveBox, 0, 1);
    mainLayout->addWidget(pathNameLabel, 1, 0);
    mainLayout->addWidget(pathLabel, 1, 1);
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
    dirView->cd(path);
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
    pathLabel->setText(path);
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
}

void
FSViewer::onDirCreate()
{
    dirView->mkdir();
}

void
FSViewer::onDirDelete()
{
    dirView->rmdir();
}

void
FSViewer::onFileCreate()
{
    dirView->createFile();
}

void
FSViewer::onFileDelete()
{
    dirView->rm();
}

void
FSViewer::onCheckHash()
{
    dirView->check();
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
