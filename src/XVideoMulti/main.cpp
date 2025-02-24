#include "sdl_qt_yuv.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SDL_QT_YUV w;
    w.show();
    return a.exec();
}
