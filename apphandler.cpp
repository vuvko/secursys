#include "apphandler.h"
#include "accesscontrol.h"
#include "profile.h"
#include "crypto.h"

AppHandler::AppHandler(QObject *parent)
    : QObject(parent)
{
    LOG << "Запуск программы." << ENDL;
}

void
AppHandler::startFS(const QString &path)
{
    LOG << "Открытие файлового менеджера. Текущий путь: " << path << ENDL;
    FSViewer *viewer = new FSViewer();
    connect(viewer, SIGNAL(openFile(QString)), this, SLOT(openFile(QString)));
    connect(viewer, SIGNAL(openProfile()), this, SLOT(openProfile()));
    viewer->show();
}

void
AppHandler::startLogin()
{
    LOG << "Запрос авторизации пользователя." << ENDL;
    LoginDialog *dialog = new LoginDialog;

    if (!Crypto::getInstance().isReady()) {
        QMessageBox::critical(dialog, tr("Критическая ошибка"),
            tr("Не удалось найти %1. Завершение работы...").arg(CRYPTO_PATH));
        exit(EXIT_FAILURE);
    }

    // здесь надо перевести сигналы в надлежащий модуль
     // TODO: заглушка
    connect(dialog, SIGNAL(loginTry(QString,QString)), this, SLOT(onLoginTry(QString,QString)));
    connect(this, SIGNAL(login(bool)), dialog, SLOT(loginResult(bool)));
    connect(dialog, SIGNAL(login()), this, SLOT(onLogin()));
    connect(this, SIGNAL(closeLogin()), dialog, SLOT(close()));
    dialog->show();
}

void
AppHandler::openFile(const QString &fileName)
{
    LOG << "Открытие файла" << fileName << ENDL;
    FileEditor *editor = new FileEditor(fileName);
    editor->show();
}

void
AppHandler::openProfile()
{
    ProfileViewer *viewer = new ProfileViewer();
    connect(viewer, SIGNAL(update()), this, SLOT(onProfileUpdate()));
    connect(this, SIGNAL(updateResult(bool)), viewer, SLOT(updateResult(bool)));
    viewer->show();
}

void
AppHandler::openFS(const QString &path)
{
    startFS(path);
}

void
AppHandler::openLogin()
{
    startLogin();
}

void
AppHandler::onLoginTry(const QString &userName, const QString &userPass)
{
    LOG << "Попытка авторизации." <<
                          "Пользователь:" << userName <<
                          "Пароль:" << userPass << ENDL;
    int uid = AccessControl::getInstance().checkLogin(userName, userPass);

    if (uid == -1) {
        LOG << "Авторизация пользователя неудачна." << ENDL;
        emit login(false);
    } else {
        LOG << "Авторизация пользователя прошла успешно." << ENDL;
        Profile::getInstance().initialize(uid);
        emit login(true);
    }
}

void
AppHandler::onLogin()
{
    emit closeLogin();
    startFS();
}

void
AppHandler::onProfileUpdate()
{
    emit updateResult(true);
}
