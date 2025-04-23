#include "signin.h"

signin::signin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

signin::~signin()
{
    delete ui;
}
