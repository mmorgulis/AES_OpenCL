#pragma once
#include <QMainWindow>
#include "ui_alice.h"  // header generato da uic da alice.ui
#include <botan/auto_rng.h>
#include <botan/ecdh.h>
#include <botan/pubkey.h>
#include <botan/hex.h>
#include <botan/base64.h>
#include <iostream>
#include <QTcpSocket>
#include "../utils/safe_allocator.hpp"
#include "../utils/aes_gcm.h"
#include "../utils/aes_core.h"

namespace Ui {
    class Alice;
}

class AliceWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AliceWindow(QWidget* parent = nullptr);
    ~AliceWindow();

private slots:
    void on_exchange_button_clicked();

    void on_sendmessage_button_clicked();

    void on_connection();

    void on_ready_read();

    void on_aes_exchange_button_clicked();

private:
    Ui::Alice *ui;

    Botan::AutoSeeded_RNG _rng;
    Botan::EC_Group _group;
    Botan::ECDH_PrivateKey _pr_key;
    Botan::ECDH_PublicKey _pb_key;

    QString _bob_str_key;
    std::unique_ptr<Botan::ECDH_PublicKey> _bob_pb_key;
    unsigned int _counter_message = 0;
    Botan::secure_vector<uint8_t> _shared_secret;

    crypto::safe_vector<uint8_t> _aes_key;
    AES_GCM _gcm;

    QTcpSocket* _alice_socket = nullptr;

};
