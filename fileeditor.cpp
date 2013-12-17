#include "fileeditor.h"

FileEditor::FileEditor(const QString &fileN, const QByteArray key_, AppHandler *handler_, QWidget *parent)
    : QMainWindow(parent)
{
    handler = handler_;
    key = key_;
    fileName = fileN;
    qDebug() << fileName;
    textEdit = new QPlainTextEdit;
    setCentralWidget(textEdit);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    connect(textEdit->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));

    resize(640, 480);
    setWindowTitle(tr("Редактор секретного файла"));
    setWindowIcon(QIcon(":/icons/secret.png"));

    qDebug() << open();
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
    qDebug() << "Здесь надо расшифровать файл";
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    QByteArray encr = QByteArray::fromHex(file.readAll());
    textEdit->setPlainText(handler->decode(encr, key));
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    file.close();

    statusBar()->showMessage(tr("Файл загружен"), 2000);
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
    qDebug() << "Здесь надо проверять на доступ к записи.";
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("Сохранение секретного файла"),
                             tr("Невозможно записать файл %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    qDebug() << "Здесь надо зашифровать информацию";
    QDataStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    QString msg = textEdit->toPlainText();
    out << handler->encode(msg, key).toHex().toUpper();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    file.close();

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

void
FileEditor::openFile()
{
    qDebug() << fileName;
    show();
}
