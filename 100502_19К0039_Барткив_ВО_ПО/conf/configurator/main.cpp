#include "configurator.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Configurator configurator;
    configurator.show();
    return app.exec();
}
