#include "bobwindow.h"
#include "ui_bob.h"

BobWindow::BobWindow(QWidget* parent) : 
    QMainWindow(parent),
    ui(new Ui::Bob),
    _rng(),
    _group("secp256r1"),
    _pr_key(_rng, _group),
    _pb_key(_pr_key)
{
    ui->setupUi(this);

    setWindowTitle("Bob");
    resize(500, 500);
    move(1200, 500);
    auto raw_pub = _pb_key.public_value();
    std::string pub_b64 = Botan::base64_encode(raw_pub);
    QString text = QString::fromStdString(pub_b64);
    // Evita caratteri di escape
    text = text.remove('/');
    ui->publickey_text->setPlainText(text);
}

BobWindow::~BobWindow() 
{
    delete ui;
}

void BobWindow::on_exchange_button_clicked()
{

}

void BobWindow::on_decrypt_button_clicked()
{

}

void BobWindow::on_showmessage_button_clicked()
{

}

