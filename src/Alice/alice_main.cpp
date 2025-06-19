#include "alicewindow.h"
#include <QApplication>

int main(int argc, char* argv[]) {

	QApplication a(argc, argv);

	AliceWindow alice;
	alice.show();

	return a.exec();
}