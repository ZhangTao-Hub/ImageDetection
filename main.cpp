#include "image.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Image w;
    w.show();
    return a.exec();
}
