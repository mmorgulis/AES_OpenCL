#include "alicewindow.h"
#include "ui_alice.h" 

AliceWindow::AliceWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::Alice),
    _group("secp256r1"),
    _pr_key(_rng, _group),
    _pb_key(_pr_key)
{
    ui->setupUi(this); 
    setWindowTitle("Alice");
    resize(500, 500);
    move(500, 500);
    auto raw_pub = _pb_key.public_value();
    std::string pub_b64 = Botan::base64_encode(raw_pub);
    QString text = QString::fromStdString(pub_b64);
    // Evita caratteri di escape, così non va a capo
    text = text.remove('/');
    ui->publickey_text->setPlainText(text);
}

AliceWindow::~AliceWindow()
{
    delete ui;
}

void AliceWindow::on_exchange_button_clicked()
{

}

void AliceWindow::on_sendmessage_button_clicked()
{

}

