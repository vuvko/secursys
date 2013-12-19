#ifndef FILEEDITOR_H
#define FILEEDITOR_H

#include <QApplication>
#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QStatusBar>
#include <QFileInfo>

#include <QDebug>

class FileEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit FileEditor(const QString &file,
                        QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    bool save();
    void about();
    void documentWasModified();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    bool open();

    bool maybeSave();
    bool saveFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QPlainTextEdit *textEdit;
    QString fileName;

    QMenu *fileMenu;
    QAction *saveAct;
    //QAction *saveAsAct;
    QAction *exitAct;

    QMenu *helpMenu;
    QAction *aboutAct;

    QToolBar *fileToolBar;
};

#endif // FILEEDITOR_H
