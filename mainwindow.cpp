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
 QString archivoMensajesBorrados;

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

    PilaGenerica<QString> pilaMensajesBorrados;
    //const QString archivoMensajesBorrados;

    cargarPacksDisponibles();

    // Conectar señales

    fileWatcher = new QFileSystemWatcher(this);
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &MainWindow::onChatFileChanged);

    watcherContactos = new QFileSystemWatcher(this);
    connect(watcherContactos, &QFileSystemWatcher::fileChanged,
            this, &MainWindow::onContactosFileChanged);

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

    ui->pushButtonAgregarContacto->setEnabled(false);

    stickersLayout = new QGridLayout();
    stickersLayout->setSpacing(5);

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

            if(manejo.getUsuarioPorNombre(username, *usuarioActual)) {
                usuarioActual->setEstado(true);
                manejo.actualizarEstadoUsuario(username, true);
            }

            ui->stackedWidget->setCurrentIndex(3);
            cargarContactos();

            QPixmap avatar(usuarioActual->getPerfil());
            QPixmap circularAvatar = hacerCircular(avatar, ui->perfil->size());
            ui->perfil->setPixmap(circularAvatar);
            ui->userLabel->setText(usuarioActual->getUsername());


             archivoMensajesBorrados = "usuarios/" + usuarioActual->getUsername() + "/mensajes_borrados.txt";

            void cargarPilaMensajesBorrados();
            void guardarPilaMensajesBorrados();

            ui->listaMensajes->clear();
            ui->labelAvatarContacto->clear();
            ui->labelNombreContacto->clear();
        } else {
            QMessageBox::warning(this, "Error", "Usuario o contraseña incorrectos.");
        }
    } else {
        QMessageBox::warning(this, "Campos vacios", "Por favor, complete todos los campos.");
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

        manejo.crearUsuario(t2, t1, correoCompleto, cb, t6int, t4, t7, rutaImagen,0);
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

         }

        QMessageBox::information(this, "Exito", "Cuenta creada correctamente.");
        ui->stackedWidget->setCurrentIndex(1);

        // Limpiar campos
        ui->campo3->clear(); ui->campo4->clear(); ui->campo5->clear();
        ui->campo6->clear(); ui->campo7->clear(); ui->campo8->clear(); ui->campo9->clear();
        rutaImagen = "";
        ui->label_13->clear();

    } else {
        QMessageBox::warning(this, "Campos vacios", "Por favor, complete todos los campos.");
    }
}


//Salir
void MainWindow::on_pushButton_5_clicked()
{
    manejo.actualizarEstadoUsuario(usuarioActual->getUsername(), false);
    delete usuarioActual;
    usuarioActual = nullptr;
    ui->stackedWidget->setCurrentIndex(0);
    ui->labelStickerPreview->setText(" ");
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
    ui->listWidgetResultados->clear();

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
                pixmap = QPixmap("C:/Users/compu/Pictures/archivos/default.jpg");
            }
            avatar->setPixmap(hacerCircular(pixmap, QSize(40, 40)));


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
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
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

    if (!watcherContactos->files().contains(rutaContactos)) {
        watcherContactos->addPath(rutaContactos);
    }

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
                        pixmap = QPixmap("C:/Users/compu/Pictures/archivos/default.jpg");
                        qDebug() << "Usando avatar por defecto";
                    }
                    avatar->setPixmap(hacerCircular(pixmap, QSize(40, 40)));

                    QLabel *nombre = new QLabel(contacto.getUsername());

                    QLabel *estado = new QLabel();
                    estado->setText("●");
                    if(contacto.getEstado()==1){
                        estado->setStyleSheet("QLabel { color: blue; }");
                    } else {
                       estado->setStyleSheet("QLabel { color: gray; }");
                    }



                    layout->addWidget(avatar);
                    layout->addWidget(nombre);
                    layout->addWidget(estado);
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
    currentContactName = nombreContacto;
    if (!fileWatcher->files().isEmpty()) {
        fileWatcher->removePaths(fileWatcher->files());
    }


    ui->stackedWidget_3->setCurrentIndex(0);
    ui->labelNombreContacto->setText(nombreContacto);

    Usuario contacto;
    if (manejo.getUsuarioPorNombre(nombreContacto, contacto)) {
        QPixmap avatar(contacto.getPerfil());
        QPixmap circularAvatar = hacerCircular(avatar, ui->labelAvatarContacto->size());
        ui->labelAvatarContacto->setPixmap(circularAvatar);
    }

    QString archivoChat = "usuarios/" + usuarioActual->getUsername() + "/chat_" + nombreContacto + ".txt";
    fileWatcher->addPath(archivoChat);

    // Cargar mensajes
    actualizarMensajes(nombreContacto);

}

void MainWindow::onChatFileChanged(const QString &path) {
    Q_UNUSED(path);  // Opcional: para evitar advertencias si no usas el parámetro

    if (!currentContactName.isEmpty()) {
        actualizarMensajes(currentContactName);

        QString archivoChat = "usuarios/" + usuarioActual->getUsername() + "/chat_" + currentContactName + ".txt";
        if (!fileWatcher->files().contains(archivoChat)) {
            fileWatcher->addPath(archivoChat);

        }
    }
}

void MainWindow::onContactosFileChanged(const QString &path) {
    Q_UNUSED(path);
    qDebug() << "Archivo de contactos modificado. Actualizando lista...";

    // Forzar recarga del archivo (Qt a veces elimina el watch después de notificar)
    QString contactosPath = "usuarios/" + usuarioActual->getUsername() + "/contactos.txt";
    if (!watcherContactos->files().contains(contactosPath)) {
        watcherContactos->addPath(contactosPath);
    }

    cargarContactos();  // Vuelve a cargar la lista desde el archivo
}

void MainWindow::actualizarMensajes(const QString &nombreContacto)
{
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
                    QString rutaSticker = mensaje.mid(9);
                    mostrarStickerEnChat(remitente, rutaSticker, fecha);
                } else {
                    QListWidgetItem *item = new QListWidgetItem();
                    QWidget *widget = new QWidget();
                    QVBoxLayout *layout = new QVBoxLayout(widget);

                    QLabel *labelMensaje = new QLabel(mensaje);
                    QLabel *labelRemitente = new QLabel(remitente == usuarioActual->getUsername() ? "Tu" : nombreContacto);
                    labelRemitente->setStyleSheet("font-size: 9px; font-weight: bold; margin: 0;");

                    QLabel *labelFecha = new QLabel(fecha);
                    labelFecha->setStyleSheet("font-size: 9px; margin: 0;");

                    layout->setAlignment(remitente == usuarioActual->getUsername() ? Qt::AlignRight : Qt::AlignLeft);
                    layout->addWidget(labelRemitente);
                    layout->addWidget(labelMensaje);
                    layout->addWidget(labelFecha);

                    QString estilo = remitente == usuarioActual->getUsername() ?
                                         "background-color: #FFEAEE; border-radius: 8px; padding: 3px; margin: 2;" :
                                         "background-color: #DCF8C6; border-radius: 8px; padding: 3px; margin: 2;";
                    widget->setStyleSheet(estilo);

                    widget->setLayout(layout);
                    item->setSizeHint(QSize(0, 110));

                    item->setData(Qt::UserRole, linea);
                    ui->listaMensajes->addItem(item);
                    ui->listaMensajes->setItemWidget(item, widget);
                }
            }
        }
        file.close();
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

        // Limpiar
        stickerSeleccionado.clear();
        ui->labelStickerPreview->clear();
    } else {
        // Enviar mensaje texto
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
    item->setSizeHint(QSize(140, 140));

    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(5, 2, 5, 2);
    layout->setSpacing(2);


    // Cargar sticker
    QLabel *labelSticker = new QLabel();
    QPixmap pixmap(stickerPath);
    if (pixmap.isNull()) {
        labelSticker->setText("[Sticker no encontrado]\n" + stickerPath);
    } else {
        labelSticker->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio));
    }

    QLabel *labelRemitente = new QLabel(remitente == usuarioActual->getUsername() ? "Tu" : remitente);
    labelRemitente->setStyleSheet("font-size: 9px; font-weight: bold; margin: 0;");

    QLabel *labelFecha = new QLabel(fecha);
    labelFecha->setStyleSheet("font-size: 9px; margin: 0;");

    layout->setAlignment(remitente == usuarioActual->getUsername() ? Qt::AlignRight : Qt::AlignLeft);
    layout->addWidget(labelRemitente);
    layout->addWidget(labelSticker);
    layout->addWidget(labelFecha);

    QString estilo = remitente == usuarioActual->getUsername() ?
                         "background-color: #FFEAEE; border-radius: 8px; padding: 3px; margin: 2;" :
                         "background-color: #DCF8C6; border-radius: 8px; padding: 3px; margin: 2;";
    widget->setStyleSheet(estilo);

    ui->listaMensajes->setItemWidget(item, widget);
}

void MainWindow::on_pushButton_13_clicked()
{
    ui->stackedWidget_3->setCurrentIndex(3);
    ui->chatLabel->setText(" ");
}


void MainWindow::on_pushButton_15_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_16_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::cargarPacksDisponibles() {
    QString ruta = "C:/Users/compu/Documents/Qt Projects/Chat/Chat/";
    QDir dir(ruta);

    if (!dir.exists()) {
        qDebug() << "Error: El directorio no existe:" << ruta;
        return;
    }

    qDebug() << "Contenido del directorio:" << dir.entryList();

    QStringList packs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable);

    if (packs.isEmpty()) {
        qDebug() << "No se encontraron carpetas en el directorio";
        return;
    }

    for (const QString &pack : packs) {
        qDebug() << "Carpeta encontrada:" << pack;  // Debug
        if (pack.startsWith("♡")) {
            ui->listWidget_2->addItem(pack);
            qDebug() << "Pack añadido:" << pack;  // Debug
        }
    }

    qDebug() << "Total packs añadidos:" << ui->listWidget_2->count();
}

void MainWindow::on_btnDescargar_clicked()
{
    QListWidgetItem *item = ui->listWidget_2->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Error", "No hay ningún pack seleccionado");
        return;
    }

    QString packNombre = item->text();
    QString userPath = "usuarios/" + usuarioActual->getUsername() + "/stickers.txt";

    QFile archivo(userPath);

    // Verificar si el pack ya está en stickers.txt
    if (archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&archivo);
        while (!in.atEnd()) {
            QString linea = in.readLine().trimmed();
            if (linea.contains(packNombre)) { // Cambiado a contains para mayor flexibilidad
                QMessageBox::information(this, "Pack existente",
                                         "El pack '" + packNombre + "' ya está descargado");
                archivo.close();
                return;
            }
        }
        archivo.close();
    }

    // Usar rutas relativas en lugar de absolutas
    QDir packDir("C:/Users/compu/Documents/Qt Projects/Chat/Chat/" + packNombre);
    if (!packDir.exists()) {
        QMessageBox::warning(this, "Error", "No se encontró el directorio del pack");
        return;
    }

    if (archivo.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&archivo);
        QStringList images = packDir.entryList(QStringList() << "*.png" << "*.jpg", QDir::Files);

        if (images.isEmpty()) {
            QMessageBox::warning(this, "Error", "El pack no contiene imágenes válidas");
            archivo.close();
            return;
        }

        for (const QString &image : images) {
            out << "C:/Users/compu/Documents/Qt Projects/Chat/Chat/" + packNombre + "/" + image << "\n"; // Guardar ruta relativa
        }
        archivo.close();

        QMessageBox::information(this, "Éxito", "Pack '" + packNombre + "' descargado correctamente");
         cargarStickers();
    } else {
        QMessageBox::warning(this, "Error", "No se pudo abrir el archivo para guardar los stickers");
    }
}



void MainWindow::on_listWidget_2_itemClicked(QListWidgetItem *item)
{
    if (!item) {
        qDebug() << "Error: Item es nulo";
        return;
    }

    QString selectedPack = item->text();
    QString packPath = "C:/Users/compu/Documents/Qt Projects/Chat/Chat/" + selectedPack;

    qDebug() << "Intentando abrir directorio:" << packPath;

    QDir packDir(packPath);
    if (!packDir.exists()) {
        qDebug() << "Error: Directorio no existe:" << packPath;
        QMessageBox::warning(this, "Error", "Directorio no encontrado: " + packPath);
        return;
    }

    QStringList images = packDir.entryList(QStringList() << "*.png" << "*.jpg", QDir::Files);
    qDebug() << "Imágenes encontradas:" << images;

    if (images.isEmpty()) {
        qDebug() << "Advertencia: No hay imágenes en el directorio";
        QMessageBox::information(this, "Info", "El pack no contiene imágenes");
        return;
    }

    // Limpiar área previa de forma segura
    QLayoutItem *child;
    while ((child = ui->layoutPrevisualizacion->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }

    // Mostrar hasta 5 imágenes
    int imagesToShow = qMin(5, images.size());
    for (int i = 0; i < imagesToShow; ++i) {
        QString imagePath = packPath + "/" + images[i];
        qDebug() << "Cargando imagen:" << imagePath;

        QPixmap pixmap(imagePath);
        if (pixmap.isNull()) {
            qDebug() << "Error: No se pudo cargar la imagen:" << imagePath;
            continue;
        }

        QLabel *label = new QLabel();
        label->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("QLabel { background-color : white;}");

        ui->layoutPrevisualizacion->addWidget(label);
        qDebug() << "Imagen agregada al layout:" << imagePath;
    }

    // Forzar actualización de la interfaz
    ui->layoutPrevisualizacion->update();
    qDebug() << "Actualización del layout completada";

    // Verificar si el layout tiene widgets hijos
    qDebug() << "Widgets en layout después de actualizar:" << ui->layoutPrevisualizacion->count();
}


void MainWindow::on_listaContactos_itemDoubleClicked(QListWidgetItem *item)
{

    QString contactoAEliminar = item->data(Qt::UserRole).toString();
    QString archivoContactos = "usuarios/" + usuarioActual->getUsername() + "/contactos.txt";

    // Asegurar que el watcher esté activo
    if (!watcherContactos->files().contains(archivoContactos)) {
        watcherContactos->addPath(archivoContactos);
    }


    // Mostrar mensaje de confirmación
    QMessageBox::StandardButton respuesta;
    respuesta = QMessageBox::question(this, "Confirmar eliminación",
                                      "¿Estás seguro que deseas eliminar a " + contactoAEliminar + "?",
                                      QMessageBox::Yes | QMessageBox::No);

    if (respuesta == QMessageBox::Yes) {
        QString file("usuarios/" + usuarioActual->getUsername() + "/contactos.txt");
        QString file2("usuarios/" + contactoAEliminar + "/contactos.txt");
        manejo.eliminarContacto(contactoAEliminar,file);
        manejo.eliminarContacto(usuarioActual->getUsername(),file2);
        QFile::remove("usuarios/" + usuarioActual->getUsername() + "/chat_" + contactoAEliminar + ".txt");
        QFile::remove("usuarios/" + contactoAEliminar + "/chat_" + usuarioActual->getUsername() + ".txt");
        QMessageBox::information(this, "Éxito", "Contacto eliminado correctamente");
        cargarContactos();
        ui->listaMensajes->clear();
        ui->labelNombreContacto->setText(" ");
        ui->labelAvatarContacto->setText(" ");
    } else {
        QMessageBox::warning(this, "Error", "No se pudo eliminar el contacto");
    }
}

void MainWindow::guardarMensajeBorrado(const QString &remitente, const QString &mensaje, const QString &fecha) {
    QString archivoBorrados = "usuarios/" + usuarioActual->getUsername() + "/mensajes_borrados.txt";
    guardarMensaje(archivoBorrados, remitente, mensaje, fecha);
}

void MainWindow::eliminarMensajeDelChat(const QString &contacto, const QString &mensajeCompleto) {
    // Archivo del usuario actual
    QString archivoChat1 = "usuarios/" + usuarioActual->getUsername() + "/chat_" + contacto + ".txt";
    // Archivo del contacto
    QString archivoChat2 = "usuarios/" + contacto + "/chat_" + usuarioActual->getUsername() + ".txt";

    // Eliminar de ambos archivos
    eliminarMensajeDeArchivo(archivoChat1, mensajeCompleto);
    eliminarMensajeDeArchivo(archivoChat2, mensajeCompleto);
    }


    void MainWindow::eliminarMensajeDeArchivo(const QString &archivo, const QString &mensajeCompleto) {
        if (!QFile::exists(archivo)) return;

        QString archivoTemp = archivo + ".tmp";

        QFile fileIn(archivo);
        QFile fileOut(archivoTemp);

        if (fileIn.open(QIODevice::ReadOnly | QIODevice::Text) &&
            fileOut.open(QIODevice::WriteOnly | QIODevice::Text)) {

            QTextStream in(&fileIn);
            QTextStream out(&fileOut);

            while (!in.atEnd()) {
                QString line = in.readLine();
                if (line != mensajeCompleto) {
                    out << line << "\n";
                }
            }

            fileIn.close();
            fileOut.close();

            // Reemplazar archivo original
            QFile::remove(archivo);
            QFile::rename(archivoTemp, archivo);
        }
    }

void MainWindow::on_listaMensajes_itemDoubleClicked(QListWidgetItem *item)
{
    QString mensajeCompleto = item->data(Qt::UserRole).toString();
    QStringList parts = mensajeCompleto.split("|");

    if (parts.size() < 3) return;

    QString remitente = parts[0];
    QString fecha = parts[1];
    QString mensaje = parts[2];

    // Verificar si el mensaje es del usuario actual
    if (remitente != usuarioActual->getUsername()) {
        QMessageBox::information(this, "Borrar mensaje",
                                 "Solo puedes borrar tus propios mensajes");
        return;
    }

    // Preguntar confirmación
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Borrar mensaje",
                                  "¿Estás seguro de querer borrar este mensaje?\n"
                                  "Se eliminará de ambas conversaciones",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Guardar en la pila y archivo
        QString datosMensaje = mensajeCompleto + "|" + currentContactName;
        pilaMensajesBorrados.insertar(datosMensaje);
        guardarPilaMensajesBorrados();

        // 1. Guardar en mensajes_borrados.txt
        guardarMensajeBorrado(remitente, mensaje, fecha);

        // 2. Eliminar de ambos chats
        eliminarMensajeDelChat(currentContactName, mensajeCompleto);

        // 3. Actualizar la vista
        actualizarMensajes(currentContactName);

        ui->btnDeshacerBorrado->setEnabled(true);
}
}

void MainWindow::cargarPilaMensajesBorrados() {
    try {
        pilaMensajesBorrados.cargarDesdeArchivo(archivoMensajesBorrados.toStdString());
        ui->btnDeshacerBorrado->setEnabled(!pilaMensajesBorrados.pilaVacia());
    } catch (const std::exception& e) {
        qWarning() << "Error al cargar mensajes borrados:" << e.what();
    }
}

void MainWindow::guardarPilaMensajesBorrados() {
    try {
        pilaMensajesBorrados.guardarEnArchivo(archivoMensajesBorrados.toStdString());
    } catch (const std::exception& e) {
        qWarning() << "Error al guardar mensajes borrados:" << e.what();
    }
}



void MainWindow::on_pushButton_17_clicked()
{
    ui->stackedWidget_3->setCurrentIndex(4);
    ui->chatLabel->setText(" ");
}


void MainWindow::on_btnDeshacerBorrado_clicked() {
    try {
        if (pilaMensajesBorrados.pilaVacia()) return;

        QString datos = pilaMensajesBorrados.quitar();
        QStringList partes = datos.split("|");

        if (partes.size() < 4) return;

        QString remitente = partes[0];
        QString fecha = partes[1];
        QString mensaje = partes[2];
        QString contacto = partes[3];

        QString mensajeCompleto = remitente + "|" + fecha + "|" + mensaje;

        // Cambia esta línea:
        restaurarMensajeEnChats(contacto, mensajeCompleto, fecha);

        if (currentContactName == contacto) {
            actualizarMensajes(currentContactName);
        }

        guardarPilaMensajesBorrados();
        ui->btnDeshacerBorrado->setEnabled(!pilaMensajesBorrados.pilaVacia());
    } catch (...) {
        QMessageBox::warning(this, "Error", "No se pudo restaurar el mensaje");
    }
}
void MainWindow::restaurarMensajeEnChats(const QString &contacto, const QString &mensajeCompleto, const QString &fechaOriginal) {
    QString archivoUsuario = "usuarios/" + usuarioActual->getUsername() + "/chat_" + contacto + ".txt";
    QString archivoContacto = "usuarios/" + contacto + "/chat_" + usuarioActual->getUsername() + ".txt";

    // Restaurar en ambos archivos manteniendo el orden cronológico
    insertarMensajeEnPosicion(archivoUsuario, mensajeCompleto, fechaOriginal);
    insertarMensajeEnPosicion(archivoContacto, mensajeCompleto, fechaOriginal);
}
void MainWindow::agregarMensajeAArchivo(const QString &archivo, const QString &mensajeCompleto) {
    QFile file(archivo);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << mensajeCompleto << "\n";
        file.close();
    }
}

void MainWindow::insertarMensajeEnPosicion(const QString &archivoChat, const QString &mensajeCompleto, const QString &fechaOriginal) {
    if (!QFile::exists(archivoChat)) {
        // Si el archivo no existe, simplemente lo creamos con el mensaje
        QFile file(archivoChat);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << mensajeCompleto << "\n";
            file.close();
        }
        return;
    }

    // Leer todo el contenido del archivo
    QFile file(archivoChat);
    QStringList lineas;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            lineas << in.readLine();
        }
        file.close();
    }

    // Buscar la posición correcta basada en la fecha
    bool insertado = false;
    QString fechaNuevoMensaje = fechaOriginal; // Formato: "yyyy-MM-dd hh:mm:ss"

    for (int i = 0; i < lineas.size(); ++i) {
        QString linea = lineas[i];
        QStringList partes = linea.split("|");
        if (partes.size() >= 2) {
            QString fechaExistente = partes[1].trimmed();

            // Comparar fechas (asegúrate de que estén en formato ordenable)
            if (fechaNuevoMensaje < fechaExistente) {
                lineas.insert(i, mensajeCompleto);
                insertado = true;
                break;
            }
        }
    }

    // Si no se insertó antes, agregar al final
    if (!insertado) {
        lineas << mensajeCompleto;
    }

    // Escribir de vuelta al archivo
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString &linea : lineas) {
            out << linea << "\n";
        }
        file.close();
    }
}
