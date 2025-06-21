#pragma once
#include <QMainWindow>
#include "ui_bob.h"
#include <botan/auto_rng.h>
#include <botan/ecdh.h>
#include <botan/pubkey.h>
#include <botan/hex.h>
#include <botan/base64.h>
#include <iostream>
#include <QTcpServer>
#include <QTcpSocket>
#include <memory>
#include "../utils/safe_allocator.hpp"
#include "../utils/aes_gcm.h"
#include "../utils/aes_core.h"


namespace Ui {
    class Bob;
}

class BobWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BobWindow(QWidget* parent = nullptr);
    ~BobWindow();

private slots:

    void on_decrypt_button_clicked();

    void on_showmessage_button_clicked();

    void on_new_socket_connection();

    void on_ready_read();

private:
    Ui::Bob *ui;
    Botan::AutoSeeded_RNG _rng;
    Botan::EC_Group _group;
    Botan::ECDH_PrivateKey _pr_key;
    Botan::ECDH_PublicKey _pb_key;

    QString _alice_str_key;
    std::unique_ptr<Botan::ECDH_PublicKey> _alice_pb_key;
    unsigned int _counter_message = 0;
    Botan::secure_vector<uint8_t> _shared_secret;

    crypto::safe_vector<uint8_t> _aes_key;
    std::vector<uint8_t> _last_message_received;
    std::string _last_message_decrypted;
    AES_GCM _gcm;

    QTcpServer _bob_server; // automatically initialized (no need for pointer)
    QTcpSocket* _client_socket = nullptr;
};
