#include "apphandler.h"

#define CRYPTO_PATH "AES256.dll"

AppHandler::AppHandler(QObject *parent) :
    QObject(parent), _crypto(CRYPTO_PATH), _profile()
{
}

void
AppHandler::startFS(const QString &path)
{
    FSViewer *viewer = new FSViewer(path, this);
    connect(viewer, SIGNAL(openFile(QString)), this, SLOT(openFile(QString)));
    connect(viewer, SIGNAL(openProfile()), this, SLOT(openProfile()));
    viewer->show();
}

void
AppHandler::startLogin()
{
    LoginDialog *dialog = new LoginDialog;

    if (!_crypto.isReady()) {
        QMessageBox::critical(dialog, tr("Critical error"),
            tr("Cannot find %1. Quitting...").arg(CRYPTO_PATH));
        delete dialog;
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
    FileEditor *editor = new FileEditor(fileName, key, this); // TODO: заглушка
    editor->show();
}

void
AppHandler::openProfile()
{
    ProfileViewer *viewer = new ProfileViewer(this);
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
AppHandler::onLoginTry(const QString &user, const QString &pass)
{
    _profile.setUser(100, user); // TODO: заглушка
    _profile.setGroup(100, "Гости"); // TODO: заглушка
    key = _crypto.hash_256(pass.toLocal8Bit());
    emit login(true); // TODO: заглушка
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

QByteArray
AppHandler::get_hash(const QByteArray &msg)
{
    return _crypto.hash_256(msg);
}

QByteArray
AppHandler::get_hash_file(const QString path)
{
    QFile file(path);
    file.open(QFile::ReadOnly);
    QByteArray hash = get_hash(file.readAll());
    file.close();
}

QByteArray
AppHandler::get_key(const QString &path)
{
    qDebug() << "Запрос ключа шифрования файла" << path;
    return key; // TODO: заглушка
}

QString
AppHandler::decode(const QByteArray &msg, const QByteArray &key_)
{
    return _crypto.decrypt(msg, key);
}

QByteArray
AppHandler::encode(const QString &msg, const QByteArray &key_)
{
    return _crypto.encrypt(msg.toLocal8Bit(), key);
}

QString
AppHandler::userName()
{
    return _profile.user();
}

QString
AppHandler::groupName()
{
    return _profile.group();
}

int
AppHandler::userId()
{
    return _profile.uid();
}

int
AppHandler::groupId()
{
    return _profile.gid();
}

int
AppHandler::roleId()
{
    // TODO
}
