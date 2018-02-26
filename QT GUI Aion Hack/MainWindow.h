#pragma once

#include <QtWidgets/QMainWindow>
#include <qthread.h>

#include "ui_QtGuiApplication1.h"

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow(QWidget* parent = Q_NULLPTR);
	~MainWindow();
	void MainWindow::HackLoop();

private:
	Ui::MainWindow* ui;
	void LoadHack(bool turnOn);

	private slots:
	void CloseProgram();
	void StackedWidgetController(int CC);
};