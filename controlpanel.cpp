#include "controlpanel.h"
#include <QDebug>

ControlPanel::ControlPanel(QWidget *parent) :
    QMainWindow(parent)
{
    tabs = new QTabWidget;
    setCentralWidget(tabs);
    permWidget = new QWidget;
    permLayout = new QGridLayout;
    regWidget = new QWidget;
    regLayout = new QGridLayout;
    logEdit = new QPlainTextEdit;
    logEdit->setReadOnly(true);
    tabs->addTab(permWidget, "Редактирование прав доступа");
    tabs->addTab(regWidget, "Регистрация пользователей");
    tabs->addTab(logEdit, "Журнал");

    permWidget->setLayout(permLayout);
    userLLabel = new QLabel("Список пользователей системы");
    groupLLabel = new QLabel("Список групп");
    permLabel = new QLabel("Списки контролируемых объектов");
    filesLabel = new QLabel("Файлы");
    drivesLabel = new QLabel("Диски");
    dirsLabel = new QLabel("Каталоги");
    programsLabel = new QLabel("Программы");
    userLList = new QTreeView;
    groupLList = new QTreeView;
    filesList = new QTreeView;
    drivesList = new QTreeView;
    dirsList = new QTreeView;
    programsList = new QTreeView;
    permLayout->addWidget(userLLabel);
    permLayout->addWidget(userLList);
    permLayout->addWidget(groupLLabel);
    permLayout->addWidget(groupLList);
    permLayout->addWidget(permLabel);
    permLayout->addWidget(filesLabel);
    permLayout->addWidget(filesList);
    permLayout->addWidget(drivesLabel);
    permLayout->addWidget(drivesList);
    permLayout->addWidget(dirsLabel);
    permLayout->addWidget(dirsList);
    permLayout->addWidget(programsLabel);
    permLayout->addWidget(programsList);
    permLayout->setRowStretch(0, 0);
    permLayout->setRowStretch(1, 1);
    permLayout->setRowStretch(2, 0);
    permLayout->setRowStretch(3, 1);
    permLayout->setRowStretch(4, 0);
    permLayout->setRowStretch(5, 0);
    permLayout->setRowStretch(6, 1);
    permLayout->setRowStretch(7, 0);
    permLayout->setRowStretch(8, 1);
    permLayout->setRowStretch(9, 0);
    permLayout->setRowStretch(10, 1);
    permLayout->setRowStretch(11, 0);
    permLayout->setRowStretch(12, 1);

    regWidget->setLayout(regLayout);
    regLabel = new QLabel("Регистрация новых пользователей в системе");
    userLabel = new QLabel("Имя пользователя:");
    passLabel = new QLabel("Пароль:");
    groupLabel = new QLabel("Группа:");
    group2Label = new QLabel("Название новой группы:");
    userEdit = new QLineEdit("Иван Сидоров");
    passEdit = new QLineEdit("");
    passEdit->setEchoMode(QLineEdit::Password);
    groupBox = new QComboBox();
    groupEdit = new QLineEdit("Пользователи");
    regButton = new QPushButton("Зарегистрировать");
    addGButton = new QPushButton("Добавить группу");
    connect(regButton, SIGNAL(clicked()), this, SLOT(onRegister()));
    connect(addGButton, SIGNAL(clicked()), this, SLOT(onAddGroup()));
    regLayout->addWidget(regLabel, 0, 0, 1, 2);
    regLayout->addWidget(userLabel, 1, 0);
    regLayout->addWidget(userEdit, 1, 1);
    regLayout->addWidget(passLabel, 2, 0);
    regLayout->addWidget(passEdit, 2, 1);
    regLayout->addWidget(groupLabel, 3, 0);
    regLayout->addWidget(groupBox, 3, 1);
    regLayout->addWidget(regButton, 4, 0);
    regLayout->addWidget(group2Label, 5, 0);
    regLayout->addWidget(groupEdit, 5, 1);
    regLayout->addWidget(addGButton, 6, 0);
    regLayout->setRowStretch(0, 0);
    regLayout->setRowStretch(1, 0);
    regLayout->setRowStretch(2, 0);
    regLayout->setRowStretch(3, 0);
    regLayout->setRowStretch(4, 0);
    regLayout->setRowStretch(5, 0);
    regLayout->setRowStretch(6, 0);
    regLayout->setRowStretch(7, 1);
    regLayout->setColumnStretch(0, 0);
    regLayout->setColumnStretch(1, 1);

    createActions();
    createMenus();
    createToolbars();

    resize(640, 480);
    setWindowIcon(QIcon(":/icons/user_info.png"));
    setWindowTitle("Панель администрирования");

    for (auto group : AccessControl::getInstance().allGroups) {
        groupBox->addItem(group.name);
    }
    loadFiles();
    loadDrives();
    loadDirs();
    loadPrograms();
    loadUsers();
    loadGroups();
    logEdit->setPlainText(Logger::getInstance().read());
    qDebug() << "Here";
}

void
ControlPanel::createActions()
{
    updateAct = new QAction("Обновить", this);
    updateAct->setShortcut(tr("F5"));
    connect(updateAct, SIGNAL(triggered()), this, SLOT(updateData()));

    exitAct = new QAction(QIcon(":/icons/exit.png"), "Выход", this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction("О программе", this);
    aboutAct->setShortcut(tr("F1"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(onAbout()));
}

void
ControlPanel::createMenus()
{
    fileMenu = menuBar()->addMenu("Файл");
    fileMenu->addAction(updateAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    helpMenu = menuBar()->addMenu("Помощь");
    helpMenu->addAction(aboutAct);
}

void
ControlPanel::createToolbars()
{
    fileBar = addToolBar(tr("Файл"));
    fileBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    fileBar->addAction(updateAct);
}

void
ControlPanel::onRegister()
{
    LOG << "Регистрация нового пользователя" << userEdit->text() << ENDL;
    AccessAdmin &aa = AccessAdmin::getInstance();
    int gid = AccessControl::getInstance().allGroups[groupBox->currentIndex()].gid;
    aa.setUser(aa.getNewUID(), gid, userEdit->text(), passEdit->text(), ROLE_NOTHING); // TODO
    updateData();
}

void
ControlPanel::onAddGroup()
{
    LOG << "Добавление новой группы" << groupEdit->text() << ENDL;
    AccessAdmin &aa = AccessAdmin::getInstance();
    aa.setGroup(aa.getNewGID(), groupEdit->text());
    updateData();
}

void
ControlPanel::onAbout()
{
    QMessageBox::about(this, tr("О программе."),
                       tr("Модуль администрирования."));
}

void
ControlPanel::updateData()
{
    AccessControl &ac = AccessControl::getInstance();
    ac.allFiles = unloadModel(filesList->model());
    ac.allDirs = unloadModel(dirsList->model());
    ac.allDrives = unloadModel(drivesList->model());
    ac.allPrograms = unloadModel(programsList->model());
    groupBox->clear();
    for (auto group : AccessControl::getInstance().allGroups) {
        groupBox->addItem(group.name);
    }
    loadFiles();
    loadDrives();
    loadDirs();
    loadPrograms();
    loadUsers();
    loadGroups();
    logEdit->setPlainText(Logger::getInstance().read());
}

QString
ControlPanel::modeToStr(int mode)
{
    QString str;
    if (mode & ACCESS_READ) {
        str.append("R");
    }
    if (mode & ACCESS_WRITE) {
        str.append("W");
    }
    if (mode & ACCESS_EXEC) {
        str.append("X");
    }
    return str;
}

int
ControlPanel::strToMode(const QString &str)
{
    int mode = 0;

    for (auto c : str) {
        if (c == 'R') {
            mode |= ACCESS_READ;
        }
        if (c == 'W') {
            mode |= ACCESS_WRITE;
        }
        if (c == 'X') {
            mode |= ACCESS_EXEC;
        }
    }

    return mode;
}

QString
ControlPanel::roleToStr(int role)
{
    QString str;
    switch (role) {
    case ROLE_NOTHING:
        str = "Не конфиденциально";
        break;
    case ROLE_CONFIDENTIAL:
        str = "Конфиденциально";
        break;
    case ROLE_STRICT_CONFIDENTIAL:
        str = "Строго конфиденциально";
        break;
    default:
        str = "Битая метка конфиденциальности";
        break;
    }

    return str;
}

Role
ControlPanel::strToRole(const QString &str)
{
    if (str == "Не конфиденциально") {
        return ROLE_NOTHING;
    } else if (str == "Конфиденциально") {
        return ROLE_CONFIDENTIAL;
    } else if (str == "Строго конфиденциально") {
        return ROLE_STRICT_CONFIDENTIAL;
    } else {
        return ROLE_NOTHING;
    }
}

QList<AccessObject>
ControlPanel::unloadModel(QAbstractItemModel *model)
{
    QList<AccessObject> objects;
    if (!model) {
        return objects;
    }
    int size = model->rowCount();
    for (int i = 0; i < size; ++i) {
        AccessObject ao(model->data(model->index(i, 0), Qt::EditRole).toString());
        ao.uid = 0;
        ao.gid = 0;
        ao.userMode = strToMode(model->data(model->index(i, 3), Qt::EditRole).toString());
        ao.groupMode = strToMode(model->data(model->index(i, 4), Qt::EditRole).toString());
        ao.othersMode = strToMode(model->data(model->index(i, 5), Qt::EditRole).toString());
        ao.role = strToRole(model->data(model->index(i, 6), Qt::EditRole).toString());
    }

    return objects;
}

void
ControlPanel::loadModel(QStandardItemModel *model, const QList<AccessObject> &data)
{
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Путь"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Владелец"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Группа"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Права владельца"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Права группы"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Права прочих"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Гриф"));
    model->setRowCount(data.size());
    int idx = 0;
    for (auto obj : data) {
        model->setData(model->index(idx, 0), obj.path);
        model->setData(model->index(idx, 1), AccessAdmin::getInstance().getUserName(obj.uid));
        model->setData(model->index(idx, 2), AccessAdmin::getInstance().getGroupName(obj.gid));
        model->setData(model->index(idx, 3), modeToStr(obj.userMode));
        model->setData(model->index(idx, 4), modeToStr(obj.groupMode));
        model->setData(model->index(idx, 5), modeToStr(obj.othersMode));
        model->setData(model->index(idx, 6), roleToStr(obj.role));
        ++idx;
    }
}

void
ControlPanel::loadFiles()
{
    filesModel = new QStandardItemModel(0, 7, this);
    loadModel(filesModel, AccessControl::getInstance().allFiles);
    filesList->setModel(filesModel);
}

void
ControlPanel::loadDrives()
{
    drivesModel = new QStandardItemModel(0, 7, this);
    loadModel(drivesModel, AccessControl::getInstance().allDrives);
    drivesList->setModel(drivesModel);
}

void
ControlPanel::loadDirs()
{
    dirsModel = new QStandardItemModel(0, 7, this);
    loadModel(dirsModel, AccessControl::getInstance().allDirs);
    dirsList->setModel(dirsModel);
}

void
ControlPanel::loadPrograms()
{
    programsModel = new QStandardItemModel(0, 7, this);
    loadModel(programsModel, AccessControl::getInstance().allPrograms);
    programsList->setModel(programsModel);
}

void
ControlPanel::loadUsers()
{
    usersModel = new QStandardItemModel(0, 3);
    usersModel->setHeaderData(0, Qt::Horizontal, "Имя");
    usersModel->setHeaderData(1, Qt::Horizontal, "Идентификатор");
    usersModel->setHeaderData(2, Qt::Horizontal, "Группы");
    QList<User> data = AccessControl::getInstance().allUsers;
    usersModel->setRowCount(data.size());
    int idx = 0;
    for (auto user : data) {
        usersModel->setData(usersModel->index(idx, 0), user.name);
        usersModel->setData(usersModel->index(idx, 1), user.uid);
        usersModel->setData(usersModel->index(idx, 2), user.gid);
        ++idx;
    }
    userLList->setModel(usersModel);
}

void
ControlPanel::loadGroups()
{
    groupsModel = new QStandardItemModel(0, 2);
    groupsModel->setHeaderData(0, Qt::Horizontal, "Имя");
    groupsModel->setHeaderData(1, Qt::Horizontal, "Идентификатор");
    QList<Group> data = AccessControl::getInstance().allGroups;
    groupsModel->setRowCount(data.size());
    int idx = 0;
    for (auto user : data) {
        groupsModel->setData(groupsModel->index(idx, 0), user.name);
        groupsModel->setData(groupsModel->index(idx, 1), user.gid);
        ++idx;
    }
    groupLList->setModel(groupsModel);
}
