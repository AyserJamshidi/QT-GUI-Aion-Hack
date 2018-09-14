#include "TCPClient/TCPClient.h"
#include "MainWindow.h"

#include <QAbstractSocket>
#include <QtNetwork>
#include <QTcpServer>
#include <QTcpSocket>

QTcpSocket* tcpSocket;

/*TCPClient::TCPClient(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);
}*/

bool TCPClient::connectToHost(QString hostname, int port) {
	if (!tcpSocket) {
		tcpSocket = new QTcpSocket(this);
		tcpSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
		in.setDevice(tcpSocket);
		in.setVersion(QDataStream::Qt_5_11);
	}

	//connect(tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SIGNAL(tcpSocketState(QAbstractSocket::SocketState)), Qt::UniqueConnection);
	connect(tcpSocket, SIGNAL(connected()), SLOT(onConnected()), Qt::UniqueConnection);
	connect(tcpSocket, SIGNAL(disconnected()), SLOT(onDisconnected()), Qt::UniqueConnection);
	connect(tcpSocket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error), this, &TCPClient::onError);
	connect(tcpSocket, SIGNAL(readyRead()), SLOT(readSocketData()), Qt::UniqueConnection);

	if (!(QAbstractSocket::ConnectedState == tcpSocket->state())) {
		tcpSocket->connectToHost(hostname, port, QIODevice::ReadWrite);
	}

	if (!tcpSocket->waitForConnected(2000)) {
		qDebug() << "Could not connect to server.  Make sure you have a stable internet connection.";
		//exit(-1);
		QCoreApplication::exit();
		return false;
	}
	qDebug() << "Done.";
	return true;
}

void TCPClient::onConnected() {
	//sendMessage("WELCOME_MSG");
}

void TCPClient::onDisconnected() {
	qDebug() << "bye.";
	//exit(-1);
	QCoreApplication::exit();
}

void TCPClient::onError(QAbstractSocket::SocketError err) {
	MainWindow w;
	QString announcement;
	announcement.append("An error \"")
		.append(err)
		.append("\" ")
		.append("has occured.");

		w.AnnounceText(announcement);
	//exit(-1);
	QCoreApplication::exit();
}

void TCPClient::readSocketData() {
	in.startTransaction();

	QString test;
	in >> test;

	if (!in.commitTransaction()) {
		qDebug() << "Error during transaction.";
	}

	qDebug() << "TCPServer Sent:" << test;
}

void TCPClient::sendMessage(QString msgToSend) {
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_11);
	QString blockMessage;

	blockMessage.append(msgToSend);

	out << tr(blockMessage.toLatin1());

	tcpSocket->write(block);
}