#include "bobwindow.h"
#include <QApplication>

int main(int argc, char* argv[]) {

	QApplication a(argc, argv);

	BobWindow bob;
	bob.show();

	return a.exec();
}