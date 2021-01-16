#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWidget m;
	m.show();
    return a.exec();
}
