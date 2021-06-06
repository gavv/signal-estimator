#include "mainwindow.h"
#include <QApplication>
#include "sigestnotfound.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    if (FILE* check = fopen("signal-estimator","r")){
        fclose(check);
        MainWindow w;
        return a.exec();
    }
    else {
        SigEstNotFound w;
        w.exec();
    }
    //w.show();

}
