#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QPixmap hacerCircular(const QPixmap& original, QSize size) {
    QPixmap circular(size);
    circular.fill(Qt::transparent);

    QPainter painter(&circular);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addEllipse(0, 0, size.width(), size.height());
    painter.setClipPath(path);

    painter.drawPixmap(0, 0, original.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    return circular;
}


void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


void MainWindow::on_pushButton_3_clicked()
{
    QString texto1 = ui->campo1->text().trimmed();
    QString texto2 = ui->campo2->text().trimmed();

    if (!texto1.isEmpty() || !texto2.isEmpty()) {
        ui->stackedWidget->setCurrentIndex(3);
    } else {
        QMessageBox::warning(this, "Campos vacios", "Por favor, complete todos los campos.");
    }
}


void MainWindow::on_pushButton_4_clicked()
{
    QString t1 = ui->campo3->text().trimmed();
    QString t2 = ui->campo4->text().trimmed();
    QString t3 = ui->campo5->text().trimmed();
    QString t4 = ui->campo6->text().trimmed();
    QString t5 = ui->campo7->text().trimmed();
    QString t6 = ui->campo8->text().trimmed();
    QString t7 = ui->campo9->text().trimmed();

    if (!t1.isEmpty() && !t2.isEmpty() && !t3.isEmpty() && !t4.isEmpty() && !t5.isEmpty() && !t6.isEmpty() && !t7.isEmpty() ) {
        QMessageBox::warning(this, "Exito", "Cuenta creada!.");
        ui->campo8->clear();ui->campo9->clear();ui->campo3->clear();ui->campo4->clear();
        ui->campo5->clear();ui->campo6->clear();ui->campo7->clear();

        ui->stackedWidget->setCurrentIndex(1);
    } else {
        QMessageBox::warning(this, "Campos vacios", "Por favor, complete todos los campos.");
    }
}


void MainWindow::on_pushButton_5_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_8_clicked()
{
    QString rutaImagen = QFileDialog::getOpenFileName(this,"Seleccionar avatar","","Imagenes (*.jpg *.jpeg *.png)");

    if (!rutaImagen.isEmpty()){
        QPixmap avatar(rutaImagen);
        QPixmap circularAvatar = hacerCircular(avatar, ui->label_13->size());
        ui->label_13->setPixmap(circularAvatar);
        //ui->label_13->setPixmap(avatar.scaled(ui->label_16->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

