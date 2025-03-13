#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setButtonState();
    QObject::connect(&socket_, &QAbstractSocket::connected, this, &Widget::doConnected);
    QObject::connect(&socket_, &QAbstractSocket::disconnected, this, &Widget::doDisconnected);
    QObject::connect(&socket_, &QIODevice::readyRead, this, &Widget::doReadyRead);
    QObject::connect(&socket_, &QAbstractSocket::stateChanged, this, &Widget::setButtonState);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setButtonState()
{
    if(socket_.state() == QSslSocket::ConnectedState) {
        ui->pbConnect->setEnabled(false);
        ui->pbDisconnect->setEnabled(true);
        ui->pbSend->setEnabled(true);
        ui->cbSSL->setEnabled(false);
    }
    else {
        ui->pbConnect->setEnabled(true);
        ui->pbDisconnect->setEnabled(false);
        ui->pbSend->setEnabled(false);
        ui->cbSSL->setEnabled(true);
    }

}

void Widget::on_pbConnect_clicked()
{
    if(ui->cbSSL->checkState() != 0) {
        socket_.connectToHostEncrypted(ui->leHost->text(), ui->lePort->text().toUShort());
    }
    else {
        socket_.connectToHost(ui->leHost->text(), ui->lePort->text().toUShort());
    }
}
void Widget::doConnected()
{
    ui->pteMessage->insertPlainText("Connected");
}

void Widget::on_pbDisconnect_clicked()
{
    socket_.disconnectFromHost();
}
void Widget::doDisconnected() {
    ui->pbConnect->setEnabled(true);
    ui->pbDisconnect->setEnabled(false);
    ui->pbSend->setEnabled(false);
    ui->pteMessage->insertPlainText("Disconnected");
}

void Widget::on_pbClear_clicked()
{
    ui->pteMessage->clear();
}

void Widget::on_pbSend_clicked()
{
    socket_.write(ui->pteSend->toPlainText().toUtf8());
}
void Widget::doReadyRead() {
    ui->pteMessage->insertPlainText(socket_.readAll());
}
