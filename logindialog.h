#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>

class LoginDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    
signals:
    void loginTry(const QString &user, const QString &password);
    void login();
    
public slots:
    void loginResult(bool isCorrect);

private slots:
    void onTryButton();
    void onCancelButton();

private:
    QWidget *centralWidget;
    QGridLayout *mainLayout;
    QLabel *userLabel;
    QLabel *passLabel;
    QLineEdit *userEdit;
    QLineEdit *passEdit;
    QPushButton *tryButton;
    QPushButton *cancelButton;
};

#endif // LOGINDIALOG_H
