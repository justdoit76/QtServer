#include "server.h"
#include <QDebug>

Server::Server(QObject *p) : QTcpServer(), parent(p)
{
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
    if(isListening())
    {
        close();
        closeClients();
    }
}

void Server::closeClients()
{
    // delete clients
    for(int i=0; i<clients.size(); i++)
    {
        clients[i]->close();
        delete clients[i];
    }
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
    }
    else
    {
        delete p;
        qDebug() << "Fail to connect client";
    }
}

void Server::onReceive(QTcpSocket* p)
{
    QByteArray data = p->readAll();
    qDebug() << "[received data]\n" << data;

    // broadcast
    for(auto p : clients) {
        p->write(data);
    }
}

void Server::onDisconnect(QTcpSocket* p)
{
}

