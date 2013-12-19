#include "fileeditor.h"
#include "accesscontrol.h"

FileEditor::FileEditor(const QString &fileN, QWidget *parent)
    : QMainWindow(parent)
{
    fileName = fileN;
    textEdit = new QPlainTextEdit;
    setCentralWidget(textEdit);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    connect(textEdit->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));

    resize(640, 480);
    setWindowTitle(tr("[*] Редактор секретного файла"));
    setWindowIcon(QIcon(":/icons/secret.png"));

    if (!open())
        close();
}

void
FileEditor::createActions()
{
    saveAct = new QAction(QIcon(":/icons/save.png"), tr("Сохранить"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    exitAct = new QAction(QIcon(":/icons/exit.png"), tr("Выход"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("О программе"), this);
    aboutAct->setShortcut(tr("F1"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void
FileEditor::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("Файл"));
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    helpMenu = menuBar()->addMenu(tr("Помощь"));
    helpMenu->addAction(aboutAct);
}

void
FileEditor::createToolBars()
{
    fileToolBar = addToolBar(tr("Файл"));
    fileToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    fileToolBar->addAction(saveAct);
}

void
FileEditor::createStatusBar()
{
    statusBar()->showMessage(tr("Готово"));
}

void
FileEditor::about()
{
    QMessageBox::about(this, tr("О программе"),
                       tr("Модуль редактирования секретного файла."));
}

void
FileEditor::documentWasModified()
{
    setWindowModified(textEdit->document()->isModified());
}

void
FileEditor::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool
FileEditor::open()
{
    QByteArray data;
    if (QFileInfo(fileName).exists()) {
        if (!AccessControl::getInstance().readFile(fileName, data))
            return false;
    }

    textEdit->setPlainText(QString::fromUtf8(data));
    statusBar()->showMessage(tr("Файл загружен"), 2000);
    return true;
}

bool
FileEditor::save()
{
    if (fileName.isEmpty()) {
        return false;
    } else {
        return saveFile(fileName);
    }
}

bool
FileEditor::maybeSave()
{
    if (textEdit->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Редактирование секретного файла"),
                                   tr("Документ был изменён.\n"
                                      "Желаете сохранить изменения?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

bool
FileEditor::saveFile(const QString &fileName)
{
    QByteArray data = textEdit->toPlainText().toUtf8();
    bool ok = AccessControl::getInstance().writeFile(fileName, data);
    if (!ok)
        return false;

    textEdit->document()->setModified(false);
    setWindowModified(textEdit->document()->isModified());
    statusBar()->showMessage(tr("Файл сохранён"), 2000);
    return true;
}

QString
FileEditor::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
