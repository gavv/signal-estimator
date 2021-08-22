#include "MainWindow.hpp"
#include "NotFoundDialog.hpp"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    MainWindow w;
    return a.exec();
}
