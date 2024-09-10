#include <QDebug>
#include "server.h"
#include "widget.h"

Server::Server(Widget *p) : QTcpServer(), parent(p)
{
    connect(this, &Server::addClient, p, &Widget::onAddClient);
    connect(this, &Server::delClient, p, &Widget::onDelClient);
    connect(this, &Server::recvClient, p, &Widget::onRecvData);
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
        close();            
    closeClients();
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

        QHostAddress ip = p->peerAddress();
        quint16 port = p->peerPort();

        emit addClient(ip.toString(), QString::number(port));
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

    emit recvClient(QString::fromUtf8(data));

    // broadcast
    for(auto p : clients) {
        p->write(data);
    }
}

void Server::onDisconnect(QTcpSocket* p)
{
    // 연결이 끊어진 클라이언트만 삭제
    for(std::vector<QTcpSocket*>::iterator itr = clients.begin(); itr!= clients.end(); ++itr)
    {
        if(*itr == p)
        {
            QHostAddress ip = p->peerAddress();
            quint16 port = p->peerPort();
            emit delClient(ip.toString(), QString::number(port));

            //clients.erase(itr);
            break;
        }
    }
}

