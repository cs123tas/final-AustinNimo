#include <QApplication>
#include "mainwindow.h"
#include "lib/cparse/shunting-yard.h"
#include "lib/cparse/builtin-features.inc"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    bool startFullscreen = false;

    // Start the calculator parser
    cparse_startup();


    w.show();

    if (startFullscreen) {
        // We cannot use w.showFullscreen() here because on Linux that creates the
        // window behind all other windows, so we have to set it to fullscreen after
        // it has been shown.
        w.setWindowState(w.windowState() | Qt::WindowFullScreen);
    }

    return app.exec();
}
