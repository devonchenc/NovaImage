#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.setWindowTitle("NovaImage");
    window.setWindowIcon(QIcon(":/icon/svg/view.svg"));
    window.show();
    window.resize(QSize(900, 600));
    window.showMaximized();
    return app.exec();
}
