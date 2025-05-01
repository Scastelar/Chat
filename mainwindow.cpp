#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QFileDialog>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QMessageBox>
#include <QHBoxLayout>


QString rutaImagen="";
Cuentas manejo("cuentas.txt");

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFile file("cuentas.txt");
    if (!file.exists()) {
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "";
            file.close();
        }
    }

    // Conectar señales si no usas auto-connect
    connect(ui->pushButtonBuscar, &QPushButton::clicked,
            this, &MainWindow::on_pushButtonBuscar_clicked);
    connect(ui->listWidgetResultados, &QListWidget::itemClicked,
            this, &MainWindow::on_listWidgetResultados_itemClicked);
    connect(ui->pushButtonAgregarContacto, &QPushButton::clicked,
            this, &MainWindow::on_pushButtonAgregarContacto_clicked);

    // Deshabilitar el botón de agregar al inicio
    ui->pushButtonAgregarContacto->setEnabled(false);
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

//Iniciar Sesion
void MainWindow::on_pushButton_3_clicked()
{
    QString username = ui->campo1->text().trimmed().toLower();
    QString password = ui->campo2->text().trimmed().toLower();

    if (!username.isEmpty() && !password.isEmpty()) {
        Usuario tempUser("", "");
        if (manejo.iniciarSesion(username, password, tempUser)) {
            usuarioActual = new Usuario(tempUser);
            ui->stackedWidget->setCurrentIndex(3);
            QPixmap avatar(usuarioActual->getPerfil());
            QPixmap circularAvatar = hacerCircular(avatar, ui->perfil->size());
            ui->perfil->setPixmap(circularAvatar);
            ui->userLabel->setText(usuarioActual->getUsername());
        } else {
            QMessageBox::warning(this, "Error", "Usuario o contraseña incorrectos.");
        }
    } else {
        QMessageBox::warning(this, "Campos vacíos", "Por favor, complete todos los campos.");
    }
}

//Crear cuenta
void MainWindow::on_pushButton_4_clicked()
{
    QString t1 = ui->campo3->text().trimmed(); // username
    QString t2 = ui->campo4->text().trimmed(); // nombre
    QString t3 = ui->campo5->text().trimmed(); // correo
    QString t4 = ui->campo6->text().trimmed(); // contraseña
    QString t5 = ui->campo7->text().trimmed(); // confirmar contraseña
    QString t6 = ui->campo8->text().trimmed(); // edad
    QString t7 = ui->campo9->text().trimmed(); // respuesta
    QString cb = ui->comboBox->currentText().trimmed();  // pregunta
    QString correoCompleto = t3 + ui->mailsCB->currentText();
    int t6int = t6.toInt();

    if (!t1.isEmpty() && !t2.isEmpty() && !t3.isEmpty() && !t4.isEmpty()
        && !t5.isEmpty() && !t6.isEmpty() && !t7.isEmpty() && !rutaImagen.isEmpty()) {

        if (!manejo.isUsuarioUnico(t1)) {
            QMessageBox::warning(this, "Usuario ya existe", "El nombre de usuario ya está en uso.");
            return;
        }

        if (t6int<=12) {
            QMessageBox::warning(this, "Edad invalida", "Solo se aceptan mayores de 12");
            return;
        }

        if (t4.size()<6) {
            QMessageBox::warning(this, "Contraseña invalida", "Contraseña debe tener minimo 6 caracteres");
            return;
        }

        if (t4 != t5) {
            QMessageBox::warning(this, "Contraseña incorrecta", "La confirmación de contraseña no coincide.");
            return;
        }

        manejo.crearUsuario(t2, t1, correoCompleto, cb, t6int, t4, t7, rutaImagen);
        QDir dir;
        QString userDir = "usuarios/" + t1;
        if (!dir.exists("usuarios")) {
            dir.mkdir("usuarios");
        }
        if (!dir.exists(userDir)) {
            dir.mkdir(userDir);

            QFile contactos(userDir + "/contactos.txt");
            contactos.open(QIODevice::WriteOnly); contactos.close();

            QFile borrados(userDir + "/mensajes_borrados.txt");
            borrados.open(QIODevice::WriteOnly); borrados.close();

            QFile stickers(userDir + "/stickers.txt");
            stickers.open(QIODevice::WriteOnly); stickers.close();
        }

        QMessageBox::information(this, "Éxito", "Cuenta creada correctamente.");
        ui->stackedWidget->setCurrentIndex(1);

        // Limpiar campos
        ui->campo3->clear(); ui->campo4->clear(); ui->campo5->clear();
        ui->campo6->clear(); ui->campo7->clear(); ui->campo8->clear(); ui->campo9->clear();
        rutaImagen = "";
        ui->label_13->clear();

    } else {
        QMessageBox::warning(this, "Campos vacíos", "Por favor, complete todos los campos.");
    }
}


//Salir
void MainWindow::on_pushButton_5_clicked()
{
    delete usuarioActual;
    usuarioActual = nullptr;
    ui->stackedWidget->setCurrentIndex(0);
}

//Seleccionar avatar
void MainWindow::on_pushButton_8_clicked()
{
    rutaImagen = QFileDialog::getOpenFileName(this,"Seleccionar avatar","","Imagenes (*.jpg *.jpeg *.png)");

    if (!rutaImagen.isEmpty()){
        QPixmap avatar(rutaImagen);
        QPixmap circularAvatar = hacerCircular(avatar, ui->label_13->size());
        ui->label_13->setPixmap(circularAvatar);
    }
}

//Buscar contactos (cambio de pagina)
void MainWindow::on_pushButton_6_clicked()
{
    ui->stackedWidget_3->setCurrentIndex(1);
    ui->chatLabel->setText(" ");

}

//Chats (main page)
void MainWindow::on_pushButton_7_clicked()
{
    ui->stackedWidget_3->setCurrentIndex(0);
    ui->chatLabel->setText("Chats");
}

/*
//Recuperar mensaje borrado
void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{

}*/

//Historial page
void MainWindow::on_pushButton_11_clicked()
{
    ui->stackedWidget_3->setCurrentIndex(2);
    ui->chatLabel->setText(" ");
}

//Buscar contactos
void MainWindow::on_pushButtonBuscar_clicked()
{
    QString textoBusqueda = ui->lineEditBusqueda->text().trimmed().toLower();
    ui->listWidgetResultados->clear(); // Limpiar resultados anteriores

    if(textoBusqueda.isEmpty()) {
        QMessageBox::warning(this, "Búsqueda vacía", "Por favor ingresa un nombre de usuario");
        return;
    }

    QVector<Usuario> todosUsuarios;
    manejo.leerUsuarios(todosUsuarios);

    QString miUsuario = usuarioActual->getUsername().toLower();

    bool encontrado = false;
    for(const Usuario &usuario : todosUsuarios) {
        QString username = usuario.getUsername().toLower();
        if(username.contains(textoBusqueda) && username != miUsuario) {
            // Agregar a la lista de resultados
            QListWidgetItem *item = new QListWidgetItem(ui->listWidgetResultados);

            // Crear un widget personalizado para mostrar el usuario
            QWidget *widget = new QWidget();
            QHBoxLayout *layout = new QHBoxLayout(widget);

            // Mostrar avatar
            QLabel *avatar = new QLabel();
            QPixmap pixmap(usuario.getPerfil());
            if(pixmap.isNull()) {
                pixmap = QPixmap(":/images/default_avatar.png"); // Imagen por defecto
            }
            avatar->setPixmap(pixmap.scaled(40, 40, Qt::KeepAspectRatio));

            // Mostrar nombre de usuario
            QLabel *nombre = new QLabel(usuario.getUsername());

            layout->addWidget(avatar);
            layout->addWidget(nombre);
            layout->addStretch();

            widget->setLayout(layout);

            item->setSizeHint(widget->sizeHint());
            ui->listWidgetResultados->setItemWidget(item, widget);

            // Guardar el objeto Usuario como dato del item
            QVariant data;
            data.setValue(usuario);
            item->setData(Qt::UserRole, data);

            encontrado = true;
        }
    }

    if(!encontrado) {
        QListWidgetItem *item = new QListWidgetItem("No se encontraron usuarios");
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable); // Hacerlo no seleccionable
        ui->listWidgetResultados->addItem(item);
    }
}

// Cuando se selecciona un usuario de la lista
void MainWindow::on_listWidgetResultados_itemClicked(QListWidgetItem *item)
{
    //ui->listaChats->currentItem().
    // Obtener el usuario guardado en los datos del item
    QVariant data = item->data(Qt::UserRole);
    if(data.canConvert<Usuario>()) {
        usuarioSeleccionado = data.value<Usuario>();
        ui->pushButtonAgregarContacto->setEnabled(true);
    }
}

// Función para agregar contacto
void MainWindow::on_pushButtonAgregarContacto_clicked()
{
    if(!usuarioSeleccionado.getUsername().isEmpty()) {
        QString nombreContacto = usuarioSeleccionado.getUsername();
        QString miUsuario = usuarioActual->getUsername();

        // Verificar si ya es contacto
        QString rutaContactos = "usuarios/" + miUsuario + "/contactos.txt";
        QFile archivoContactos(rutaContactos);

        if(archivoContactos.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&archivoContactos);
            while(!in.atEnd()) {
                if(in.readLine().trimmed() == nombreContacto) {
                    QMessageBox::information(this, "Contacto existente",
                                             "Este usuario ya está en tu lista de contactos");
                    archivoContactos.close();
                    return;
                }
            }
            archivoContactos.close();
        }

        // Agregar a contactos
        if(archivoContactos.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&archivoContactos);
            out << nombreContacto << "\n";
            archivoContactos.close();

            // Crear archivos de chat
            QString chat1 = "usuarios/" + miUsuario + "/chat_" + nombreContacto + ".txt";
            QString chat2 = "usuarios/" + nombreContacto + "/chat_" + miUsuario + ".txt";

            QFile file1(chat1);
            if(file1.open(QIODevice::WriteOnly | QIODevice::Text)) {
                file1.close();
            }

            QFile file2(chat2);
            if(file2.open(QIODevice::WriteOnly | QIODevice::Text)) {
                file2.close();
            }

            QMessageBox::information(this, "Contacto agregado",
                                     nombreContacto + " ha sido agregado a tus contactos");
        } else {
            QMessageBox::critical(this, "Error",
                                  "No se pudo abrir el archivo de contactos");
        }
    }
}







