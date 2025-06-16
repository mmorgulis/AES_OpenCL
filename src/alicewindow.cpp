#include "alicewindow.h"
#include "ui_alice.h" 

AliceWindow::AliceWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::Alice) 
{
    ui->setupUi(this); 
}

AliceWindow::~AliceWindow()
{
    delete ui;
}