#pragma once
#include <QMainWindow>
#include "ui_bob.h"

namespace Ui {
    class Bob;
}

class BobWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BobWindow(QWidget* parent = nullptr);
    ~BobWindow();

private:
    Ui::Bob *ui;
};