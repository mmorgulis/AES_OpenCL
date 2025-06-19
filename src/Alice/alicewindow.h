#pragma once
#include <QMainWindow>
#include "ui_alice.h"  // header generato da uic da alice.ui
#include <botan/auto_rng.h>
#include <botan/ecdh.h>
#include <botan/pubkey.h>
#include <botan/hex.h>
#include <botan/base64.h>
#include <iostream>

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

private:
    Ui::Alice *ui;
    Botan::AutoSeeded_RNG _rng;
    Botan::EC_Group _group;
    Botan::ECDH_PrivateKey _pr_key;
    Botan::ECDH_PublicKey _pb_key;
};
