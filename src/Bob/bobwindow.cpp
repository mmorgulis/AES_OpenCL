#include "bobwindow.h"
#include "ui_bob.h"
#include <botan/ec_point.h>
#include <botan/pem.h>
#include <botan/kdf.h>
#include <botan/hash.h>


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
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x + width()/2, y);

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
    if (_last_message_received.size() > 0) {
        _gcm.clear();
        _gcm.set_key(_aes_key);
        std::vector<uint8_t> message_encrypted(_last_message_received.begin(), _last_message_received.end() - 12);
        std::vector<uint8_t> iv(_last_message_received.end() - 12, _last_message_received.end());
        std::vector<uint8_t> message_decrypted(message_encrypted.size());
        _gcm.set_iv(iv);
        try
        {
            _gcm.decrypt_tag_appended(message_encrypted, message_decrypted);
            _last_message_decrypted.assign(message_decrypted.begin(), message_decrypted.end());
            ui->showmessage_button->setEnabled(true);
        }
        catch (const std::exception& e) {
            QString err = QString::fromUtf8(e.what());
            ui->log_line->setText(err);
        }
        catch (...)
        {   
            ui->log_line->setText("Error in decrypt");
        }
    }
    ui->decrypt_button->setEnabled(false);
    ui->log_line->setText("Message decrypted correctly");
}

void BobWindow::on_showmessage_button_clicked()
{
    QByteArray dec_data = QByteArray::fromRawData(_last_message_decrypted.data(), _last_message_decrypted.size());
    QString msg_dec = QString::fromUtf8(dec_data);
    ui->message_text->setPlainText(msg_dec);
    ui->showmessage_button->setEnabled(false);
    ui->log_line->setText("Message showed correctly");
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
        _client_socket->flush();

        // Derive shared secret ecdh
        Botan::PK_Key_Agreement ka(_pr_key, _rng, "Raw");
        _shared_secret = ka.derive_key(32, _alice_pb_key->public_value()).bits_of();

        ui->log_line->setText("ECHD Key exchange success");
        ++_counter_message;
        return;     
    }

    // The second message must be the AES_KEY + iv (not ciphered)
    if (_counter_message == 1) {
        std::vector<uint8_t> aes_key_enc(data.begin(), data.end() - 12);
        std::vector<uint8_t> iv(data.end() - 12, data.end());
        /*QByteArray iv_arr(reinterpret_cast<const char*>(iv.data()), static_cast<int>(iv.size()));
        QString iv_str = iv_arr.toHex().toUpper();
        ui->message_text->setPlainText(iv_str);*/

        // Decript AES_Key
        // Find key_for_enc as Alice (determinist function)
        std::unique_ptr<Botan::KDF> hkdf = Botan::KDF::create("HKDF(SHA-256)");
        if (!hkdf) {
            ui->log_line->setText("Hkdf not available");
            return;
        }
        Botan::secure_vector<uint8_t> raw_key = hkdf->derive_key(16, _shared_secret);
        Botan::SymmetricKey key_for_enc(raw_key);
        _gcm.set_key(std::span<const uint8_t>(key_for_enc.begin(), key_for_enc.size()));
        _gcm.set_iv(iv);
        std::vector<uint8_t> aes_key_ciph(aes_key_enc.begin(), aes_key_enc.end());
        std::vector<uint8_t> aes_key_pl(aes_key_ciph.size());
        try {
            _gcm.decrypt_tag_appended(aes_key_ciph, aes_key_pl);
        }
        catch (const std::exception& e) {
            QString err = QString::fromUtf8(e.what());
            ui->log_line->setText(err);
        }
        catch (...) {
            ui->log_line->setText("Error in decrypt");
        }
            
        _aes_key.insert(_aes_key.begin(), aes_key_pl.begin(), aes_key_pl.end());
        /*QByteArray aes_arr(reinterpret_cast<const char*>(_aes_key.data()), static_cast<int>(_aes_key.size()));
        QString aes_str = aes_arr.toHex().toUpper();
        ui->message_text->setPlainText(aes_str);*/

        ui->log_line->setText("AES_GCM Key exchange success");
        ++_counter_message;
        return;
    }

    // Other messages
    _last_message_received.assign(data.begin(), data.end());
    ui->message_text->setPlainText(message);
    ui->log_line->setText("Message delivered from Alice");
    ui->decrypt_button->setEnabled(true);
    ++_counter_message;
}

