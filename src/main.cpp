#include <QApplication>
#include <QProcess>
#include <botan/version.h>

int main(int argc, char* argv[])
{   
    // If Botan is compiled in release also VS need to be on release
    //std::cout << "Botan version: " << Botan::version_string() << std::endl;
    QApplication a(argc, argv);

    QProcess alice;
    QProcess bob;

    // Lancia Alice.exe e Bob.exe (attento ai path corretti)
    alice.start("Alice.exe");
    bob.start("Bob.exe");
    

    return a.exec();
}