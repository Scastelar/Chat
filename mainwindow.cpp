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
    ui->stackedWidget->setCurrentIndex(0);

    QFile file("cuentas.txt");
    if (!file.exists()) {
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "";
            file.close();
        }
    }

    // Conectar señales
    connect(ui->pushButtonBuscar, &QPushButton::clicked,
            this, &MainWindow::on_pushButtonBuscar_clicked);

    connect(ui->listWidgetResultados, &QListWidget::itemClicked,
            this, &MainWindow::on_listWidgetResultados_itemClicked);

    connect(ui->pushButtonAgregarContacto, &QPushButton::clicked,
            this, &MainWindow::on_pushButtonAgregarContacto_clicked);

    connect(ui->listaContactos, &QListWidget::itemClicked, [this](QListWidgetItem *item) {
        QString nombreContacto = item->data(Qt::UserRole).toString();
        mostrarChatConContacto(nombreContacto);
    });

    ui->labelStickerPreview->setAlignment(Qt::AlignCenter);



    // Deshabilitar el botón de agregar al inicio
    ui->pushButtonAgregarContacto->setEnabled(false);

    // Crear el layout de stickers (no acceder a través de ui)
    stickersLayout = new QGridLayout();
    stickersLayout->setSpacing(5);

    // Asignar el layout al widget contenedor (usando el nombre correcto de tu QScrollArea)
    if (ui->scrollAreaStickers->widget()) {
        ui->scrollAreaStickers->widget()->setLayout(stickersLayout);
    }
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
            cargarContactos();
            QPixmap avatar(usuarioActual->getPerfil());
            QPixmap circularAvatar = hacerCircular(avatar, ui->perfil->size());
            ui->perfil->setPixmap(circularAvatar);
            ui->userLabel->setText(usuarioActual->getUsername());
            ui->listaMensajes->clear();
            ui->labelAvatarContacto->clear();
            ui->labelNombreContacto->clear();
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

            QFile stickersFile(userDir + "/stickers.txt");
            if (stickersFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&stickersFile);

                // Agregar 3 stickers por defecto (asegúrate de que estas rutas existan)
                out << "C:/Users/compu/Documents/Qt Projects/Chat/Chat/STKmonkey/angry.png\n";
                out << "C:/Users/compu/Documents/Qt Projects/Chat/Chat/STKmonkey/hello.png\n";
                out << "C:/Users/compu/Documents/Qt Projects/Chat/Chat/STKmonkey/laughing.png\n";
                out << "C:/Users/compu/Documents/Qt Projects/Chat/Chat/STKmonkey/sad.png\n";
                out << "C:/Users/compu/Documents/Qt Projects/Chat/Chat/STKmonkey/surprised.png\n";

                stickersFile.close();
            }
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
                pixmap = QPixmap(":/images/default_avatar.png");
            }
            avatar->setPixmap(hacerCircular(pixmap, QSize(40, 40)));
           // avatar->setPixmap(pixmap.scaled(40, 40, Qt::KeepAspectRatio));


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

        QFile file1("usuarios/" + miUsuario + "/contactos.txt");
        QFile file2("usuarios/" + nombreContacto + "/contactos.txt");

        // Agregar a contactos
        if(file1.open(QIODevice::Append | QIODevice::Text) &&
            file2.open(QIODevice::Append | QIODevice::Text)) {

            QTextStream out1(&file1);
            QTextStream out2(&file2);

            out1 << nombreContacto << "\n";
            out2 << miUsuario << "\n";

            file1.close();
            file2.close();

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
            cargarContactos();
        } else {
            QMessageBox::critical(this, "Error",
                                  "No se pudo abrir el archivo de contactos");
        }
    }
}

void MainWindow::cargarContactos()
{
    qDebug() << "=== Cargando contactos para:" << usuarioActual->getUsername() << "===";

    ui->listaContactos->clear();

    QString rutaContactos = "usuarios/" + usuarioActual->getUsername() + "/contactos.txt";
    qDebug() << "Buscando contactos en:" << rutaContactos;

    QFile archivoContactos(rutaContactos);

    if (!archivoContactos.exists()) {
        qDebug() << "El archivo de contactos no existe. Creando uno nuevo...";
        if (archivoContactos.open(QIODevice::WriteOnly | QIODevice::Text)) {
            archivoContactos.close();
        }
        return;
    }

    if (archivoContactos.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Archivo de contactos abierto correctamente";

        QTextStream in(&archivoContactos);
        while (!in.atEnd()) {
            QString nombreContacto = in.readLine().trimmed();
            qDebug() << "Procesando contacto:" << nombreContacto;

            if (!nombreContacto.isEmpty()) {
                Usuario contacto;
                if (manejo.getUsuarioPorNombre(nombreContacto, contacto)) {
                    qDebug() << "Mostrando contacto:" << contacto.getUsername();

                    // Crear item personalizado
                    QListWidgetItem *item = new QListWidgetItem();
                    QWidget *widget = new QWidget();
                    QHBoxLayout *layout = new QHBoxLayout(widget);

                    // Avatar circular
                    QLabel *avatar = new QLabel();
                    QPixmap pixmap(contacto.getPerfil());
                    if (pixmap.isNull()) {
                        pixmap = QPixmap(":/images/default_avatar.png");
                        qDebug() << "Usando avatar por defecto";
                    }
                    avatar->setPixmap(hacerCircular(pixmap, QSize(40, 40)));

                    QLabel *nombre = new QLabel(contacto.getUsername());

                    layout->addWidget(avatar);
                    layout->addWidget(nombre);
                    layout->addStretch();
                    widget->setLayout(layout);

                    item->setSizeHint(widget->sizeHint());
                    ui->listaContactos->addItem(item);
                    ui->listaContactos->setItemWidget(item, widget);

                    // Guardar nombre para usar al hacer clic
                    item->setData(Qt::UserRole, nombreContacto);
                } else {
                    qDebug() << "Contacto no encontrado en cuentas.txt:" << nombreContacto;
                }
            }
        }
        archivoContactos.close();
        qDebug() << "Total contactos cargados:" << ui->listaContactos->count();
    } else {
        qDebug() << "Error al abrir archivo:" << archivoContactos.errorString();
    }
}

void MainWindow::guardarMensaje(const QString &archivoPath, const QString &remitente, const QString &mensaje, const QString &fecha)
{
    QFile archivoChat(archivoPath);
    if (archivoChat.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&archivoChat);
        out << remitente << "|" << fecha << "|" << mensaje << "\n";
        archivoChat.close();
    } else {
        qDebug() << "Error al abrir archivo de chat:" << archivoPath;
        qDebug() << "Error:" << archivoChat.errorString();
    }
}

void MainWindow::mostrarChatConContacto(const QString &nombreContacto)
{
    ui->stackedWidget_3->setCurrentIndex(0);
    ui->labelNombreContacto->setText(nombreContacto);

    Usuario contacto;
    if (manejo.getUsuarioPorNombre(nombreContacto, contacto)) {
        QPixmap avatar(contacto.getPerfil());
        QPixmap circularAvatar = hacerCircular(avatar, ui->labelAvatarContacto->size());
        ui->labelAvatarContacto->setPixmap(circularAvatar);
    }

    // Cargar mensajes
    QString archivoChat = "usuarios/" + usuarioActual->getUsername() + "/chat_" + nombreContacto + ".txt";
    QFile file(archivoChat);

    ui->listaMensajes->clear();

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString linea = in.readLine();
            QStringList partes = linea.split("|");

            if (partes.size() >= 3) {
                QString remitente = partes[0];
                QString fecha = partes[1];
                QString mensaje = partes[2];

                if (mensaje.startsWith("[STICKER]")) {
                    QString rutaSticker = mensaje.mid(9); // Eliminar "[STICKER]"
                    mostrarStickerEnChat(remitente, rutaSticker, fecha);
                } else {

                QListWidgetItem *item = new QListWidgetItem();

                // Crear widget personalizado para el mensaje
                QWidget *widget = new QWidget();
                QVBoxLayout *layout = new QVBoxLayout(widget);

                QLabel *labelRemitente = new QLabel(remitente == usuarioActual->getUsername() ? "Tu" : nombreContacto);
                QLabel *labelMensaje = new QLabel(mensaje);
                QLabel *labelFecha = new QLabel(fecha);

                // Estilo diferente para mensajes propios y ajenos
                if (remitente == usuarioActual->getUsername()) {
                    widget->setStyleSheet("background-color: #FFEAEE; border-radius: 8px; padding: 3px;margin: 2px 0;");
                    layout->setAlignment(Qt::AlignRight);
                } else {
                    widget->setStyleSheet("background-color: #E0F5ED; border-radius: 8px; padding: 3px;margin: 2px 0;");
                    layout->setAlignment(Qt::AlignLeft);
                }

                layout->addWidget(labelRemitente);
                layout->addWidget(labelMensaje);
                layout->addWidget(labelFecha);

                widget->setLayout(layout);
                //item->setSizeHint(widget->sizeHint());
                item->setSizeHint(QSize(0, 110));

                ui->listaMensajes->addItem(item);
                ui->listaMensajes->setItemWidget(item, widget);
            }
        }
        file.close();
    }
    }
    cargarStickers();
    qDebug() << "Stickers cargados exitosamente";
    ui->listaMensajes->scrollToBottom();
}


//Enviar mensaje
void MainWindow::on_pushButton_10_clicked()
{
    if (!stickerSeleccionado.isEmpty()) {
        QString nombreContacto = ui->labelNombreContacto->text();
        QString fecha = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm");

        // Guardar en ambos archivos
        guardarMensaje("usuarios/" + usuarioActual->getUsername() + "/chat_" + nombreContacto + ".txt",
                       usuarioActual->getUsername(), "[STICKER]" + stickerSeleccionado, fecha);
        guardarMensaje("usuarios/" + nombreContacto + "/chat_" + usuarioActual->getUsername() + ".txt",
                       usuarioActual->getUsername(), "[STICKER]" + stickerSeleccionado, fecha);

        // Mostrar en el chat
        mostrarStickerEnChat(usuarioActual->getUsername(), stickerSeleccionado, fecha);

        // Limpiar selección
        stickerSeleccionado.clear();
        ui->labelStickerPreview->clear();
    } else {
        // Enviar mensaje de texto normal
        QString mensaje = ui->textEditMensaje->toPlainText().trimmed();
        if (!mensaje.isEmpty()) {
            QString nombreContacto = ui->labelNombreContacto->text();
            QString fecha = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm");

            // Guardar en ambos archivos
            guardarMensaje("usuarios/" + usuarioActual->getUsername() + "/chat_" + nombreContacto + ".txt",
                           usuarioActual->getUsername(), mensaje, fecha);
            guardarMensaje("usuarios/" + nombreContacto + "/chat_" + usuarioActual->getUsername() + ".txt",
                           usuarioActual->getUsername(), mensaje, fecha);

            // Mostrar en el chat
            //mostrarMensajeEnChat(usuarioActual->getUsername(), mensaje, fecha, nombreContacto);
            mostrarChatConContacto(nombreContacto);

            // Limpiar campo de texto
            ui->textEditMensaje->clear();
        }
    }
}

void MainWindow::cargarStickers()
{
    if (!ui || !ui->widgetStickersContainer) {
        qDebug() << "Error: UI no inicializada";
        return;
    }

    qDebug() << "Intentando cargar stickers...";

    // Limpiar layout existente de forma segura
    if (auto layout = ui->widgetStickersContainer->layout()) {
        QLayoutItem* item;
        while ((item = layout->takeAt(0))) {
            if (item->widget()) delete item->widget();
            delete item;
        }
    } else {
        qDebug() << "Creando nuevo layout para stickers";
        ui->widgetStickersContainer->setLayout(new QGridLayout());
    }

    QString rutaStickers = "usuarios/" + usuarioActual->getUsername() + "/stickers.txt";
    qDebug() << "Buscando stickers en:" << rutaStickers;

    QFile archivo(rutaStickers);


    if (archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&archivo);
        int row = 0, col = 0;
        const int maxCols = 1;

        while (!in.atEnd()) {
            QString path = in.readLine().trimmed();
            if (!path.isEmpty()) {
                qDebug() << "Procesando sticker:" << path;

                if (!QFile::exists(path)) {
                    qDebug() << "¡La imagen no existe en:" << path;
                    continue;
                }

                QPushButton *btn = new QPushButton(ui->widgetStickersContainer);
                btn->setIcon(QIcon(path));
                btn->setIconSize(QSize(64, 64));
                btn->setFlat(true);
                btn->setCursor(Qt::PointingHandCursor);

                connect(btn, &QPushButton::clicked, this, [this, path]() {
                    qDebug() << "Sticker seleccionado:" << path;
                    stickerSeleccionado = path;
                    ui->labelStickerPreview->setPixmap(QPixmap(path).scaled(
                        ui->labelStickerPreview->width(),
                        ui->labelStickerPreview->height(),
                        Qt::KeepAspectRatio
                        ));
                });

                qobject_cast<QGridLayout*>(ui->widgetStickersContainer->layout())->addWidget(btn, row, col);

                if (++col >= maxCols) {
                    col = 0;
                    row++;
                }
            }
        }
        archivo.close();
        qDebug() << "Stickers cargados correctamente. Total:" << row * maxCols + col;
    } else {
        qDebug() << "Error al abrir archivo:" << archivo.errorString();
    }
}

void MainWindow::mostrarStickerEnChat(const QString &remitente, const QString &stickerPath, const QString &fecha)
{
    QListWidgetItem *item = new QListWidgetItem(ui->listaMensajes);
    item->setSizeHint(QSize(120, 120)); // Tamaño adecuado para stickers

    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(5, 2, 5, 2);
    layout->setSpacing(2);

    // Etiqueta del remitente
    QLabel *labelRemitente = new QLabel(remitente == usuarioActual->getUsername() ? "Tú" : remitente);
    labelRemitente->setStyleSheet("font-size: 9px; font-weight: bold; margin: 0;");

    // Cargar la imagen del sticker
    QLabel *labelSticker = new QLabel();
    QPixmap pixmap(stickerPath);
    if (pixmap.isNull()) {
        labelSticker->setText("[Sticker no encontrado]\n" + stickerPath);
    } else {
        labelSticker->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio));
    }

    // Etiqueta de fecha
    QLabel *labelFecha = new QLabel(fecha);
    labelFecha->setStyleSheet("font-size: 8px; color: #666; margin: 0;");

    // Alineación según remitente
    layout->setAlignment(remitente == usuarioActual->getUsername() ? Qt::AlignRight : Qt::AlignLeft);
    layout->addWidget(labelRemitente);
    layout->addWidget(labelSticker);
    layout->addWidget(labelFecha);

    // Estilo del widget
    QString estilo = remitente == usuarioActual->getUsername() ?
                         "background-color: #DCF8C6; border-radius: 8px; padding: 3px;" :
                         "background-color: #ECECEC; border-radius: 8px; padding: 3px;";
    widget->setStyleSheet(estilo);

    ui->listaMensajes->setItemWidget(item, widget);
}
