#pragma once

#include <QtWidgets/QMainWindow>
#include <qtcpsocket.h>

#include <QAbstractSocket>
#include <QtNetwork>
#include <QTcpServer>
#include <QTcpSocket>

class TCPClient : public QMainWindow {
	Q_OBJECT

public:
	//explicit TCPClient(QWidget *parent = Q_NULLPTR);
	void sendMessage(QString msgToSend);
	bool connectToHost(QString hostname, int port);

signals:

public slots:
	void onConnected();
	void onDisconnected();
	void onError(QAbstractSocket::SocketError er);
	void readSocketData();

private:
	QDataStream in;
};
