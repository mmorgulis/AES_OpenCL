#include "bobwindow.h"
#include "ui_bob.h"

BobWindow::BobWindow(QWidget* parent) : 
    QMainWindow(parent),
    ui(new Ui::Bob)
{
    ui->setupUi(this);
}

BobWindow::~BobWindow() 
{
    delete ui;
}