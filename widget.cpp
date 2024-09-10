#include "widget.h"
#include "ui_widget.h"
#include <QNetworkInterface>

const int Widget::port = 5614;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget), server(this)
{
    ui->setupUi(this);
    setWindowTitle("Server Side");
    ui->pb1->setCheckable(true);

    QStringList labels = {"IP", "PORT"};
    ui->tw->setColumnCount(labels.size());
    ui->tw->setHorizontalHeaderLabels(labels);

    // ip, port 기본값 설정
    QString ip = getActiveIP();
    ui->le1->setText(ip);
    ui->le2->setText(QString::number(port));

    // signal, slot 연결
    connect(ui->pb1, &QPushButton::clicked, this, &Widget::onPb1);
    connect(ui->pb2, &QPushButton::clicked, this, &Widget::onPb2);
    connect(ui->pb3, &QPushButton::clicked, this, &Widget::onPb3);
}

Widget::~Widget()
{
    delete ui;
}

QString Widget::getActiveIP()
{
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

    for (const QNetworkInterface &interface : interfaces) {
        // 네트워크 인터페이스가 UP 상태인지 확인
        if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
            interface.flags().testFlag(QNetworkInterface::IsRunning) &&
            !interface.flags().testFlag(QNetworkInterface::IsLoopBack) &&
            !interface.flags().testFlag(QNetworkInterface::IsPointToPoint)) {

            QList<QNetworkAddressEntry> entries = interface.addressEntries();
            for (const QNetworkAddressEntry &entry : entries) {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    return entry.ip().toString();
                }
            }
        }
    }

    return QString();
}

void Widget::onPb1()
{
    if(ui->pb1->isChecked())
    {
        ui->pb1->setText("Stop");
        // 현재 pc ip 얻기
        QString ip = getActiveIP();
        qDebug() << "IP:" << ip;
        server.startServer(ip, port);
    }
    else
    {
        ui->pb1->setText("Start");
        server.closeServer();
    }
}

void Widget::onPb2()
{
    ui->lw->clear();
}

void Widget::onPb3()
{
    QString msg = ui->le3->text();
    server.sendData(msg);
    ui->le3->clear();
}

void Widget::onAddClient(const QString&ip, const QString& port)
{
    int row = ui->tw->rowCount();
    ui->tw->setRowCount(row+1);

    ui->tw->setItem(row, 0, new QTableWidgetItem(ip));
    ui->tw->setItem(row, 1, new QTableWidgetItem(port));
}

void Widget::onDelClient(const QString& ip, const QString& port)
{
    int row = ui->tw->rowCount();
    for(int i=0; i<row; i++)
    {
        QString _ip = ui->tw->item(i, 0)->text();
        QString _port = ui->tw->item(i, 1)->text();

        if(ip==_ip && port==_port)
        {
            ui->tw->removeRow(i);
            break;
        }
    }
}

void Widget::onRecvData(const QString& msg)
{
    ui->lw->addItem(msg);
    int n = ui->lw->count();
    ui->lw->setCurrentRow(n-1);
}

