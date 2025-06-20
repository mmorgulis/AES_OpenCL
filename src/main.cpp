#include <QApplication>
#include <QProcess>
#include <botan/version.h>
#include <chrono>
#include <thread>

int main(int argc, char* argv[])
{   
    // If Botan is compiled in release also VS need to be on release
    //std::cout << "Botan version: " << Botan::version_string() << std::endl;
    QApplication a(argc, argv);

    QProcess bob;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    QProcess alice;
    

    // Lancia Alice.exe e Bob.exe (attento ai path corretti)
    alice.start("Alice.exe");
    bob.start("Bob.exe");
    

    return a.exec();
}