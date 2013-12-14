#include "fileviewer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FileViewer w;
    w.show();
    
    return a.exec();
}
