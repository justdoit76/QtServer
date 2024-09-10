#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <vector>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(QObject*);
    virtual ~Server();

public:
    bool startServer(const QString&, int);
    void closeServer();

protected:
    void incomingConnection(qintptr) override;

private:
    QObject* parent;
    std::vector<QTcpSocket*> clients;
    void closeClients();

private slots:
    void onReceive(QTcpSocket*);
    void onDisconnect(QTcpSocket*);
};

#endif // SERVER_H
