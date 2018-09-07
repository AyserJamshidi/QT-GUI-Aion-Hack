#include "MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
	// Pre-creation settings
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	// The rest...
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}