#include "MainWindow.h"
#include "Bypass\MainBypass.h"

#include <qsignalmapper.h>
#include <qmovie.h>
#include <qmessagebox.h>
#include <qthread.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <WinToast\WinToast.h>

#define HOME_INDEX 0
#define HACK_INDEX 1

QMovie* movie;
int globalCurrentIndex;
WinToastLib::WinToastTemplate templ = WinToastLib::WinToastTemplate(WinToastLib::WinToastTemplate::Text02);

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	WinToastLib::WinToast::instance()->setAppName(L"Lmfaoown's Aion Hack");
	WinToastLib::WinToast::instance()->setAppUserModelId(
		WinToastLib::WinToast::configureAUMI(L"Company Name", L"Product Name", L"Sub Product", L"Version #"));
	if (!WinToastLib::WinToast::instance()->initialize()) {
		qDebug() << "Error, your system in not compatible!";
	}

	templ.setTextField(L"Do you think this feature is cool?", WinToastLib::WinToastTemplate::FirstLine);
	templ.setTextField(L"Ofc,it is!", WinToastLib::WinToastTemplate::SecondLine);

	std::vector<std::wstring> actions;
	actions.push_back(L"Yes");
	actions.push_back(L"Maybe");
	actions.push_back(L"Maybe");
	actions.push_back(L"No");
	templ.setExpiration(2000);
	for (auto const &action : actions)
		templ.addAction(action);
	INT64 toastInt = WinToastLib::WinToast::instance()->showToast(templ, new CustomHandler());






	// Custom flags we can't set inside QT Designer
	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint); // Disable window resizing

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
	movie = new QMovie("Resources/loading.gif");

	//connect(ui->exit_MenuAction, SIGNAL(triggered()), this, SLOT(CloseProgram()));
	//connect(ui->northAmerica_MenuAction, SIGNAL(triggered()), this, SLOT(StackedWidgetController()));
	//connect(ui->northAmerica_MenuAction, SIGNAL(triggered()), this, SLOT(ui->MainWindowStackedWidget->setCurrentIndex(1)));
	//connect(ui->europe_MenuAction, SIGNAL(triggered()), this, SLOT(StackedWidgetController(2)));
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

void MainWindow::HackLoop() {
	MainBypass mBypass;

	mBypass.EnableDebugPriv();
	while (globalCurrentIndex == HACK_INDEX) {
		DWORD xCoronaHostProcessID, NCLauncherID, aionProcessID;

		NCLauncherID = mBypass.GetProcID(L"NCLauncherR.exe");
		if (NCLauncherID != 0) 
			mBypass.KillProcessID(NCLauncherID);

		xCoronaHostProcessID = mBypass.GetProcID(L"xcoronahost.xem");
		if (xCoronaHostProcessID != 0) { 
			std::this_thread::sleep_for(std::chrono::seconds(2)); 
			mBypass.SuspendProcess(xCoronaHostProcessID, TRUE);

			aionProcessID = mBypass.GetParentProcessID(xCoronaHostProcessID);

			//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, true, aionProcessID);
		}
		//WaitForProcess(L"Aion.bin");
		//aionProcessID = GetProcID(L"Aion.bin");

		//if (NCLauncherID != 0)
		//	KillProcessID(NCLauncherID);


		/*Sleep(2000);

		//SuspendProcess(xCoronaHostProcessID, TRUE);

		// Now is the time to acquire xcorona_64.xem module base address

		std::cout << "aionProcessID = " << aionProcessID << std::endl;

		MODULEENTRY32 xCoronaModule;

		xCoronaModule = GetModule(aionProcessID, L"xcorona_64.xem");

		if (xCoronaModule.hModule > 0) {
			std::wcout << "xcorona_64.xem base addr hex is " << std::hex << xCoronaModule.modBaseAddr << std::endl;
		} else {
			std::cout << "No module found...?" << std::endl;
			return 0;
		}

		SuspendX3Threads(aionProcessID);

		SuspendProcess(xCoronaHostProcessID, FALSE);

		DWORD xddID;
		WaitForProcess(L"xxd-0.xem");
		Sleep(2500);
		xddID = GetProcID(L"xxd-0.xem");

		if (xddID != 0)
			KillProcessID(xddID);
		KillProcessID(xCoronaHostProcessID);

		return 1;
		std::cout << "Looping.." << std::endl;

		Sleep(10);*/
	}
	//CloseProgram();
}

void MainWindow::LoadHack(bool turnOn) {
	if (turnOn) {
		if (!movie->isValid())
			QCoreApplication::quit();

		ui->aion_LoadingGif->setMovie(movie);
		ui->aion_LoadingGif->setGeometry(QRect(0, 0, 636, 400));
		movie->start();

		std::thread mThread(&MainWindow::HackLoop, this);
		mThread.detach();
	} else {
		if (movie->isValid())
			movie->stop();
	}
}