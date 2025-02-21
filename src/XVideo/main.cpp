#include "sdl_qt_rgb.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SDL_QT_RGB w;
    w.show();
    return a.exec();
}
