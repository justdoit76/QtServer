#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "server.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}

QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    Server server;
    static const int port;

    QString getActiveIP();

private slots:
    void onPb1();
    void onPb2();
    void onPb3();

public slots:
    void onAddClient(const QString&, const QString&);
    void onDelClient(const QString&, const QString&);
    void onRecvData(const QString&);
};
#endif // WIDGET_H
