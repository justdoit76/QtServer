#include <QDebug>
#include "server.h"
#include "widget.h"

Server::Server(Widget *p) : QTcpServer(), parent(p)
{
    connect(this, &Server::addSignal, p, &Widget::onAddClient);
    connect(this, &Server::delSignal, p, &Widget::onDelClient);
    connect(this, &Server::recvSignal, p, &Widget::onRecvData);
}

Server::~Server()
{
    closeServer();
}

bool Server::startServer(const QString& ip, int port)
{
    QHostAddress addr(ip);
    if (!listen(addr, port))
        return false;
    return true;
}

void Server::closeServer()
{
    closeClients();
    if(isListening())
        close();            
}

void Server::closeClients()
{
    // delete clients
    for (auto p : clients)
    {
        if (p)
        {
            QHostAddress ip = p->peerAddress();
            quint16 port = p->peerPort();
            emit delSignal(ip.toString(), QString::number(port));

            // disconnect signals
            p->disconnect();

            // close the socket
            p->close();

            // delete the socket safely
            p->deleteLater();
        }
    }
    clients.clear();  // clear the vector after deleting clients
}

void Server::incomingConnection(qintptr socket)
{
    QTcpSocket *p = new QTcpSocket(this);
    if( p->setSocketDescriptor(socket) )
    {
        // 시그널, 슬롯 연결
        connect(p, &QTcpSocket::readyRead, this, [this, p]() {onReceive(p);});
        connect(p, &QTcpSocket::disconnected, this, [this, p] {onDisconnect(p);});
        clients.push_back(p);
        qDebug() << "Connect client";

        QHostAddress ip = p->peerAddress();
        quint16 port = p->peerPort();

        emit addSignal(ip.toString(), QString::number(port));
    }
    else
    {
        delete p;
        qDebug() << "Fail to connect client";
    }
}

void Server::sendData(const QString& msg)
{
    for(auto p : clients)
    {
        p->write(msg.toStdString().c_str());
    }
}

void Server::onReceive(QTcpSocket* p)
{
    QByteArray data = p->readAll();
    qDebug() << "[received data]\n" << data;

    emit recvSignal(QString::fromUtf8(data));

    // broadcast
    for(auto p : clients) {
        p->write(data);
    }
}

void Server::onDisconnect(QTcpSocket* p)
{
    // 연결이 끊어진 클라이언트만 삭제
    // 연결이 끊어진 클라이언트만 삭제
    auto itr = std::find(clients.begin(), clients.end(), p);

    if (itr != clients.end())
    {
        QHostAddress ip = p->peerAddress();
        quint16 port = p->peerPort();
        emit delSignal(ip.toString(), QString::number(port));

        clients.erase(itr);

        p->disconnect();
        p->deleteLater();
    }

    qDebug() << "Number of clients:" << clients.size();
}

