#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <vector>

class Widget;

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(Widget*);
    virtual ~Server();

public:
    bool startServer(const QString&, int);
    void closeServer();
    void sendData(const QString &);

protected:
    void incomingConnection(qintptr) override;

private:
    Widget* parent;
    std::vector<QTcpSocket*> clients;
    void closeClients();

signals:
    void addClient(const QString&, const QString&);
    void delClient(const QString&, const QString&);
    void recvClient(const QString&);

private slots:
    void onReceive(QTcpSocket*);
    void onDisconnect(QTcpSocket*);
};

#endif // SERVER_H
