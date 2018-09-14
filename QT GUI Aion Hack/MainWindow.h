#pragma once

#include <QtWidgets/QMainWindow>

#include "ui_QtGuiApplication1.h"

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow(QWidget* parent = Q_NULLPTR);
	~MainWindow();
	void MainWindow::HackLoop();
	void MainWindow::InjectLoop();
	void AnnounceText(QString message);
	void SetStatusText(QString stringToSend);
	void SetCellText();
	void InitialToastNotif();

private:
	Ui::MainWindow* ui;
	void LoadHack(bool turnOn);

private slots:
	void CloseProgram();
	void StackedWidgetController(int CC);
	void on_hack_PushButton_clicked();
	void on_refresh_PushButton_clicked();
};