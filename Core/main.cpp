#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
	window.setWindowTitle("NovaImage");
	window.show();
	window.resize(QSize(900, 600));
	window.showMaximized();
    return app.exec();
}
