#include "controlpanel.h"

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
    // TODO: добавить редактирование прав

    regWidget->setLayout(regLayout);
    regLabel = new QLabel("Регистрация новых пользователей в системе");
    userLabel = new QLabel("Имя пользователя:");
    passLabel = new QLabel("Пароль:");
    userEdit = new QLineEdit("Иван Сидоров");
    passEdit = new QLineEdit("");
    passEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    regButton = new QPushButton("Зарегистрировать");
    regLayout->addWidget(regLabel, 0, 0, 1, 2);
    regLayout->addWidget(userLabel, 1, 0);
    regLayout->addWidget(userEdit, 1, 1);
    regLayout->addWidget(passLabel, 2, 0);
    regLayout->addWidget(passEdit, 2, 1);
    regLayout->addWidget(regButton, 3, 0);
    regLayout->setRowStretch(0, 0);
    regLayout->setRowStretch(1, 0);
    regLayout->setRowStretch(2, 0);
    regLayout->setRowStretch(3, 0);
    regLayout->setRowStretch(4, 1);
    regLayout->setColumnStretch(0, 0);
    regLayout->setColumnStretch(1, 1);

    logEdit->setPlainText(Logger::getInstance().read());

    resize(640, 480);
    setWindowIcon(QIcon(":/icons/user_info.png"));
    setWindowTitle("Панель администрирования");
}
