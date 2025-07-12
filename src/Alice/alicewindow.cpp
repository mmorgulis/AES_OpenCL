#include "alicewindow.h"
#include "ui_alice.h"
#include <botan/pem.h>
#include <botan/kdf.h>
#include <botan/hash.h>

AliceWindow::AliceWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::Alice),
    _group(Botan::EC_Group::from_name("secp256r1")),
    _pr_key(_rng, _group),
    _pb_key(_pr_key),
    _alice_socket(new QTcpSocket(this))
{
    ui->setupUi(this); 
    setWindowTitle("Alice");
    resize(500, 500);
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x - width()/2, y);

    // Public key
    std::string pubkey_pem = Botan::PEM_Code::encode(_pb_key.public_key_bits(), "PUBLIC KEY");
    QString key_text = QString::fromStdString(pubkey_pem);
    // Evita caratteri di escape, cosi non va a capo solo per visualizzazione
    key_text = key_text.remove('/');
    ui->publickey_text->setPlainText(key_text);

    //AES key
    _aes_key = generate_aes_key(128);

    connect(_alice_socket, &QTcpSocket::connected, this, &AliceWindow::on_connection);
    _alice_socket->connectToHost(QHostAddress::LocalHost, 3232);
}

void AliceWindow::on_connection() {
    connect(_alice_socket, &QTcpSocket::readyRead, this, &AliceWindow::on_ready_read);
    ui->log_line->setText("Connected to Bob in /localhost::3232");
}

void AliceWindow::on_ready_read() {
    QByteArray data = _alice_socket->readAll();
    QString message = QString::fromUtf8(data);

    try
    {
        if (_counter_message == 0) {
            // Recives and save Bob key
            _bob_str_key = message;
            data = QByteArray::fromBase64(message.toUtf8());
            std::vector<uint8_t> pubkey_array(data.begin(), data.end());
            std::optional<Botan::EC_AffinePoint> mb_point = Botan::EC_AffinePoint::deserialize(_group, pubkey_array);
            if (!mb_point) {
                ui->log_line->setText("Failed to deserialize public key");
                return;
            }
            Botan::EC_AffinePoint pub_point = mb_point.value();
            _bob_pb_key = std::make_unique<Botan::ECDH_PublicKey>(_group, pub_point);

            // Derives shared secret
            Botan::PK_Key_Agreement ka(_pr_key, _rng, "Raw");
            _shared_secret = ka.derive_key(32, _bob_pb_key->public_value()).bits_of();

            ++_counter_message;
        }

    }
    catch (const std::exception& e)
    {
        QString err = QString::fromUtf8(e.what());
        ui->log_line->setText(err);
    } 
    catch (...) {
        ui->log_line->setText("Error in on_ready_read()");
    }
    // Only counter = 0 because is the only msg expected from Bob
    
}

AliceWindow::~AliceWindow()
{
    delete ui;
}

void AliceWindow::on_exchange_button_clicked()
{   
    // ECHD public key send
    auto raw_pub = _pb_key.public_value();
    std::string pub_b64 = Botan::base64_encode(raw_pub);
    QByteArray key_array = QByteArray::fromStdString(pub_b64);
    _alice_socket->write(key_array);

    ui->log_line->setText("ECHD Key exchange success");
    ui->exchange_button->setEnabled(false);
    ui->aes_exchange_button->setEnabled(true);
}

void AliceWindow::on_aes_exchange_button_clicked()
{
    try
    {
        // AES Key (encrypted with ECHD)
        // Derivo una chiave sicura da shared_secret
        std::unique_ptr<Botan::KDF> hkdf = Botan::KDF::create("HKDF(SHA-256)");
        if (!hkdf) {
            ui->log_line->setText("Hkdf not available");
            return;
        }
        
        // Derive key to encrypt messages
        Botan::secure_vector<uint8_t> raw_key = hkdf->derive_key(16, _shared_secret);
        Botan::SymmetricKey key_for_enc(raw_key);
        // Send aes_key encrypted in AES + key_for_enc
        _gcm.set_key(std::span<const uint8_t>(key_for_enc.begin(), key_for_enc.size()));
        std::vector<uint8_t> iv(12);
        _rng.randomize(iv.data(), iv.size());
        _gcm.set_iv(iv);
        std::vector<uint8_t> pl_aes_key(_aes_key.begin(), _aes_key.end());
        std::vector<uint8_t> cipher_key(pl_aes_key.size());
        _gcm.encrypt_append_tag(pl_aes_key, cipher_key);
        cipher_key.insert(cipher_key.end(), iv.begin(), iv.end());
        QByteArray key_ciph_array(reinterpret_cast<const char*>(cipher_key.data()), static_cast<int>(cipher_key.size()));
        _alice_socket->write(key_ciph_array);
        ui->log_line->setText("AES_GCM Key exchange success");
        ui->aes_exchange_button->setEnabled(false);
        ui->message_text->setEnabled(true);
        ui->sendmessage_button->setEnabled(true);
    }
    catch (const std::exception& e) {
        QString err = QString::fromUtf8(e.what());
        ui->log_line->setText(err);
    }
    catch (...)
    {
        ui->log_line->setText("Error in on_aes_exchange_button_clicked()");
    }
   
}

void AliceWindow::on_sendmessage_button_clicked()
{
    QString message = ui->message_text->toPlainText();
    if (message.size() > 0) {
        QByteArray message_byte = message.toUtf8();
        std::vector<uint8_t> plain_msg(message_byte.begin(), message_byte.end());
        std::vector<uint8_t> cipher_msg(plain_msg.size());
        
        // AES Encryption
        try
        {
            _gcm.clear();
            _gcm.set_key(_aes_key);
            std::vector<uint8_t> iv(12);
            _rng.randomize(iv.data(), iv.size());
            _gcm.set_iv(iv);
            _gcm.encrypt_append_tag(plain_msg, cipher_msg);
            cipher_msg.insert(cipher_msg.end(), iv.begin(), iv.end());
            QByteArray message_byte_enc(reinterpret_cast<const char*>(cipher_msg.data()), static_cast<int>(cipher_msg.size()));
            _alice_socket->write(message_byte_enc);
            ui->log_line->setText("Message sent correctly");
            ui->message_text->clear();
        }
        catch (const std::exception& e) {
            QString err = QString::fromUtf8(e.what());
            ui->log_line->setText(err);
        }
        catch (...)
        {
            ui->log_line->setText("Error in on_sendmessage_button_clicked()");
        }
        
    }
    
}




