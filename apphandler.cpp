#include "apphandler.h"
#include "accesscontrol.h"

#define CRYPTO_PATH "AES256.dll"

AppHandler::AppHandler(QObject *parent)
    : QObject(parent),
    _crypto(CRYPTO_PATH),
    _profile(0)
{
    ac = new AccessControl(this);
}

void
AppHandler::startFS(const QString &path)
{
    FSViewer *viewer = new FSViewer(ac, path, this);
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
    ProfileViewer *viewer = new ProfileViewer(_profile, this);
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
AppHandler::onLoginTry(const QString &userName, const QString &pass)
{
    QByteArray passHash = _crypto.hash_256(pass.toLocal8Bit());
    int uid = ac->checkLogin(userName, passHash);

    if (uid == -1) {
        emit login(false);
    } else {
        _profile = new Profile(ac, uid);
        emit login(false);
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
    return hash;
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

const Profile *AppHandler::getProfile() const
{
    return _profile;
}
