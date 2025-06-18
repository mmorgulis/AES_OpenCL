#include <QApplication>
#include "alicewindow.h"
#include "bobwindow.h"
#include <botan/version.h>

int main(int argc, char* argv[])
{   
    // If Botan is compiled in release also VS need to be on release
    //std::cout << "Botan version: " << Botan::version_string() << std::endl;
    QApplication a(argc, argv);

    AliceWindow alice;
    BobWindow bob;

    alice.show();
    bob.show();

    

    return a.exec();
}