#pragma once
#include <QMainWindow>
#include "ui_alice.h"  // header generato da uic da alice.ui

namespace Ui {
    class Alice;
}

class AliceWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AliceWindow(QWidget* parent = nullptr);
    ~AliceWindow();

private:
    Ui::Alice *ui;
};