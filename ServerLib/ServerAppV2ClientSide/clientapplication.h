#ifndef CLIENTAPPLICATION_H
#define CLIENTAPPLICATION_H

#include <QWidget>

#include "clientconnection.h"

namespace Ui {
class ClientApplicationUi;
}

class ClientApplication : public QWidget
{
    Q_OBJECT

public:
    explicit ClientApplication(QWidget *parent = 0);
    ~ClientApplication();

private:
    Ui::ClientApplicationUi *ui;

    ClientConnection *connection;

private slots:
    void enableButtons();
    void disableButtons();
    void connectToServer();
    void disconnectFromServer();
    void displayMessage(QByteArray bytes);
    void dataToSend();
};

#endif // CLIENTAPPLICATION_H
