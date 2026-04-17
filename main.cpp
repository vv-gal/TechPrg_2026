#include <QApplication>
#include "controller/clientcontroller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("Math Equation Solver");
    a.setApplicationVersion("1.0");

    ClientController controller;
    controller.start();

    return a.exec();
}
