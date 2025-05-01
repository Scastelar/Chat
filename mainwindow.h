#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "Cuentas.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    //void on_toolButton_triggered(QAction *arg1);

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButtonBuscar_clicked();

    void on_listWidgetResultados_itemClicked(QListWidgetItem *item);

    void on_pushButtonAgregarContacto_clicked();

    void on_pushButton_11_clicked();

   // void on_pushButton_10_clicked();

private:
    Ui::MainWindow *ui;
    Usuario* usuarioActual = nullptr;
     Usuario usuarioSeleccionado;
};
#endif // MAINWINDOW_H
