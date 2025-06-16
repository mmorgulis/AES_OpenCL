#include <QApplication>
#include "alicewindow.h"
#include "bobwindow.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    AliceWindow alice;
    BobWindow bob;

    alice.setWindowTitle("Alice");
    bob.setWindowTitle("Bob");

    alice.resize(400, 300);
    bob.resize(400, 300);
    bob.move(450, 0);

    alice.show();
    bob.show();

    return a.exec();
}