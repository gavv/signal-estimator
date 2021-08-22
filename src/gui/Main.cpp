#include "MainWindow.hpp"
#include <QApplication>
#include "NotFoundDialog.hpp"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    return a.exec();
    //w.show();
}
