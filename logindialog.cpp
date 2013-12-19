#include "logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QMainWindow(parent)
{
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);
    mainLayout = new QGridLayout;
    centralWidget->setLayout(mainLayout);

    userLabel = new QLabel(tr("Имя пользователя:"));
    passLabel = new QLabel(tr("Пароль:"));
    userEdit = new QLineEdit(tr("root"));
    passEdit = new QLineEdit(tr("password"));
    passEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    tryButton = new QPushButton(QIcon(":/icons/enter.png"), tr("Войти"), this);
    cancelButton = new QPushButton(QIcon(":/icons/cancel.png"), tr("Выйти"), this);

    connect(tryButton, SIGNAL(clicked()), this, SLOT(onTryButton()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(onCancelButton()));

    mainLayout->addWidget(userLabel, 1, 0);
    mainLayout->addWidget(userEdit, 1, 1, 1, 2);
    mainLayout->addWidget(passLabel, 2, 0);
    mainLayout->addWidget(passEdit, 2, 1, 1, 2);
    mainLayout->addWidget(tryButton, 3, 1);
    mainLayout->addWidget(cancelButton, 3, 2);
    mainLayout->setColumnStretch(0, 0);
    mainLayout->setColumnStretch(1, 1);
    mainLayout->setColumnStretch(2, 1);

    setWindowTitle(tr("Вход в систему защиты информации."));
    setWindowIcon(QIcon(":/icons/user.png"));
    resize(350, 100);
}

void
LoginDialog::loginResult(bool isCorrect)
{
    if (isCorrect) {
        emit login();
    } else {
        QMessageBox::information(this, tr("Вход в систему защиты информации"),
                                 tr("Вход не удался, проверьте "
                                    "корректность введённых данных"));
    }
}

void
LoginDialog::onTryButton()
{
    emit loginTry(userEdit->text(), passEdit->text());
}

void
LoginDialog::onCancelButton()
{
    close();
}
