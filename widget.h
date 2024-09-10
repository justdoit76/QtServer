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

};
#endif // WIDGET_H
