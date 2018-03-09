#include "MainWindow.h"
#include "Bypass\MainBypass.h"
#include "FindMainWindowHWND.h"
#include "Tools\CommandLine.h"

#include <qdebug.h>
#include <qsignalmapper.h>
#include <qmovie.h>
#include <qthread.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <qfile.h>

#define HOME_INDEX 0
#define HACK_INDEX 1

QMovie* movie;
int globalCurrentIndex;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	// Custom flags we can't set inside QT Designer
	//setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint); // Disable window resizing
	//QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	// Connect UI elements to actual functions
	QSignalMapper* signalMapper = new QSignalMapper(this);

	connect(ui->exit_MenuAction, SIGNAL(triggered()), this, SLOT(CloseProgram()));

	/* Menu Items */
	connect(ui->home_MenuAction, SIGNAL(triggered()), signalMapper, SLOT(map()));
	connect(ui->hack_MenuAction, SIGNAL(triggered()), signalMapper, SLOT(map()));
	signalMapper->setMapping(ui->home_MenuAction, 0);
	signalMapper->setMapping(ui->hack_MenuAction, 1);
	connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(StackedWidgetController(int)));

	/* Create Loading Gif */
	movie = new QMovie(":/images/Resources/loading.gif");
	ui->aion_LoadingGif->setGeometry(QRect(0, 0, 636, 400));
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::CloseProgram() {
	QCoreApplication::quit();
}

void MainWindow::StackedWidgetController(int CC) {
	if (ui->MainWindowStackedWidget->currentIndex() != CC) {
		globalCurrentIndex = CC;
		ui->MainWindowStackedWidget->setCurrentIndex(CC);
		switch (CC) {
			case 0: // mainPage_StackedWidget
				LoadHack(FALSE);
				break;
			case 1: // aion_StackedWidget
				LoadHack(TRUE);
				break;
		}
	}
}

void MainWindow::LoadHack(bool turnOn) {
	if (turnOn) {
		if (!movie->isValid())
			QCoreApplication::quit();

		ui->aion_LoadingGif->setMovie(movie);
		movie->start();

		std::thread mThread(&MainWindow::HackLoop, this);
		mThread.detach();
	} else {
		if (movie->isValid())
			movie->stop();
	}
}

void MainWindow::SetStatusText(QString stringToSend) {
	QMetaObject::invokeMethod(ui->statusLabel, "setText", Qt::QueuedConnection, Q_ARG(QString, stringToSend));
}

/*
 * ----------------------------------------------------------------------------------------------------------
 * --                                             Threads                                                  --
 * ----------------------------------------------------------------------------------------------------------
 */

void MainWindow::HackLoop() {
	MainBypass mBypass;
	CommandLine cmdLine;
	int failSafe = 0;

	while (globalCurrentIndex == HACK_INDEX) {
		DWORD xCoronaHostProcessID, NCLauncherID, aionProcessID;
		SetStatusText("Ready for Aion");
		//HANDLE aHandle = OpenProcess(NULL, FALSE, mBypass.GetProcID(L"Aion.bin"));

		cmdLine.GetCommandLines();

		xCoronaHostProcessID = mBypass.GetProcID(L"xcoronahost.xem");
		if (xCoronaHostProcessID != 0) {
			SetStatusText("Bypassing"); 
			NCLauncherID = mBypass.GetProcID(L"NCLauncherR.exe");

			if (NCLauncherID != 0)
				mBypass.KillProcessID(NCLauncherID);

			std::this_thread::sleep_for(std::chrono::seconds(2)); // If we have issues put this back to 2
			if (mBypass.SuspendProcess(xCoronaHostProcessID, true)) {

				aionProcessID = mBypass.GetParentProcessID(xCoronaHostProcessID);

				MODULEENTRY32 xCoronaModule;

				xCoronaModule = mBypass.GetModule(aionProcessID, L"xcorona_64.xem");

				if (xCoronaModule.hModule > 0) {
					if (xCoronaModule.modBaseAddr != 0) {
						qDebug() << "xcorona_64.xem base addr hex is " << std::hex << xCoronaModule.modBaseAddr;

						if (mBypass.SuspendX3Threads(aionProcessID) && mBypass.SuspendProcess(xCoronaHostProcessID, false)) {
							DWORD xddID;
							mBypass.WaitForProcess(L"xxd-0.xem");
							xddID = mBypass.GetProcID(L"xxd-0.xem");

							while (FindWindowEx(NULL, NULL, TEXT("SplashWindowClass"), NULL) != 0 && failSafe <= 200) {
								std::this_thread::sleep_for(std::chrono::milliseconds(100));
								failSafe++;
							}
							mBypass.KillProcessID(xddID);
							mBypass.KillProcessID(xCoronaHostProcessID);
						} else {
							// Couldn't suspend XIGNCODE3, admin rights issues?
						}
					} else {
						// xCoronaModule base address could not be found
					}
				} else {
					// xCoronaModule handle not found
				}
			} else {
				// Couldn't suspend xCoronaHostProcessID
			}
		} else {
			// No xcoronahost.xem exists
		}
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}