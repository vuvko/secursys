#include "fsviewer.h"
#include "fileeditor.h"
#include "apphandler.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //FSViewer w;
    //w.show();
    AppHandler *ah = new AppHandler();
    ah->startFS();
    
    return a.exec();
}
