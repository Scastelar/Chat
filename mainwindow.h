#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QGridLayout>
#include <QFileSystemWatcher>
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

    void on_pushButton_10_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_16_clicked();

    void on_btnPrevisualizar_clicked();

    void on_btnDescargar_clicked();

    void on_listWidgetPacks_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidgetPacks_itemClicked(QListWidgetItem *item);


    void on_listWidget_2_itemClicked(QListWidgetItem *item);

    void onChatFileChanged(const QString &path);

private:
    Ui::MainWindow *ui;
    Usuario* usuarioActual = nullptr;
    Usuario usuarioSeleccionado;
    QFileSystemWatcher *fileWatcher;
    QTimer *timerActualizarChat;
    QString currentContactName;
    void cargarContactos();
    void mostrarChatConContacto(const QString &nombreContacto);
    QGridLayout *stickersLayout;
    QString stickerSeleccionado;
    void cargarStickers();
    void mostrarStickerEnChat(const QString &remitente, const QString &stickerPath, const QString &fecha);
    void actualizarMensajes(const QString &nombreContacto);
    void guardarMensaje(const QString &archivo, const QString &remitente, const QString &mensaje, const QString &fecha);
    void cargarPacksDisponibles();
};
#endif // MAINWINDOW_H
