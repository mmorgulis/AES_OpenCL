#include "bobwindow.h"
#include "ui_bob.h"
#include <botan/ec_point.h>
#include <botan/pem.h>


BobWindow::BobWindow(QWidget* parent) : 
    QMainWindow(parent),
    ui(new Ui::Bob),
    _rng(),
    _group(Botan::EC_Group::from_name("secp256r1")),
    _pr_key(_rng, _group),
    _pb_key(_pr_key)
{
    ui->setupUi(this);

    setWindowTitle("Bob");
    resize(500, 500);
    move(1200, 500);
    std::string pubkey_pem = Botan::PEM_Code::encode(_pb_key.public_key_bits(), "PUBLIC KEY");
    QString text = QString::fromStdString(pubkey_pem);
    // Evita caratteri di escape
    text = text.remove('/');
    ui->publickey_text->setPlainText(text);

    // Socket
    connect(&_bob_server, &QTcpServer::newConnection, this, &BobWindow::on_new_socket_connection);
    if (!_bob_server.listen(QHostAddress::Any, 3232)) {
        qFatal("Cannot Listen on port 3232");
    }
    ui->log_line->setText("Server listening on /localhost::3232");
}

BobWindow::~BobWindow() 
{
    delete ui;
}

void BobWindow::on_decrypt_button_clicked()
{

}

void BobWindow::on_showmessage_button_clicked()
{

}

void BobWindow::on_new_socket_connection() 
{
    _client_socket = _bob_server.nextPendingConnection();
    connect(_client_socket, &QTcpSocket::readyRead, this, &BobWindow::on_ready_read);
    _bob_server.close(); // No other connection
    ui->log_line->setText("Connected to Alice on /localhost::3232");
}

void BobWindow::on_ready_read() {
    QByteArray data = _client_socket->readAll();
    QString message = QString::fromUtf8(data);
    // The first message must be the private key
    if (_counter_message == 0) {
        // Recives and save Alice key
        _alice_str_key = message;
        data = QByteArray::fromBase64(message.toUtf8());
        std::vector<uint8_t> pubkey_array(data.begin(), data.end());
        std::optional<Botan::EC_AffinePoint> mb_point = Botan::EC_AffinePoint::deserialize(_group, pubkey_array);
        if (!mb_point) {
            ui->log_line->setText("Failed to deserialize public key");
            return;
        }
        Botan::EC_AffinePoint pub_point = mb_point.value();
        _alice_pb_key = std::make_unique<Botan::ECDH_PublicKey>(_group, pub_point);

        // Send his pb key to Alice
        auto raw_pub = _pb_key.public_value();
        std::string pub_b64 = Botan::base64_encode(raw_pub);
        QByteArray key_array = QByteArray::fromStdString(pub_b64);
        _client_socket->write(key_array);

        // Derive shared secret ecdh
        Botan::PK_Key_Agreement ka(_pr_key, _rng, "Raw");
        _shared_secret = ka.derive_key(32, _alice_pb_key->public_value()).bits_of();

        ui->log_line->setText("Key exchange success");
        ++_counter_message;
        return;     
    }

    // The second message must be the AES_KEY
    if (_counter_message == 1) {
        // Decript AES_Key
        ++_counter_message;
        return;
    }

    // Other messages
    ui->message_text->setPlainText(message);
    ui->log_line->setText("Message delivered from Alice");
    ui->decrypt_button->setEnabled(true);
    ++_counter_message;
}

