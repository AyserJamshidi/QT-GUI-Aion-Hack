#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include "Tools/WMI/WMI_Information.hpp"
#include "Tools/MD5Hash/md5.hpp"
#include "TCPClient/TCPClient.h"

QString GetMachineGuid() {
	WCHAR vcVER[256];
	DWORD dwSize = _countof(vcVER);
	RegGetValueW(HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Microsoft\\Cryptography",
		L"MachineGuid", RRF_RT_REG_SZ, NULL, &vcVER, &dwSize);

	return QString::fromStdWString(vcVER);
}
std::string GetUniqueHWID() {
	WMI_Information wmiInfo;
	QString moboInfo, hddInfo, cpuInfo, ramInfo, gpuInfo;

	moboInfo.append(wmiInfo.Query(L"SELECT Product FROM Win32_BaseBoard", L"Product")).chop(1);
	moboInfo.append("||");

	hddInfo.append(wmiInfo.Query(L"SELECT SerialNumber FROM Win32_PhysicalMedia", L"SerialNumber")).chop(1);
	hddInfo.append("||");

	cpuInfo.append(wmiInfo.Query(L"SELECT ProcessorId FROM Win32_Processor", L"ProcessorId")).chop(1);
	cpuInfo.append("||");

	ramInfo.append(wmiInfo.Query(L"SELECT PartNumber FROM Win32_PhysicalMemory", L"PartNumber"));
	ramInfo.append(wmiInfo.Query(L"SELECT BankLabel FROM Win32_PhysicalMemory", L"BankLabel")).chop(1);
	ramInfo.append("||");

	gpuInfo.append(wmiInfo.Query(L"SELECT Name FROM Win32_VideoController", L"Name")).chop(1);
	gpuInfo.append("||");

	qDebug() << "All ==" << moboInfo + hddInfo + cpuInfo + ramInfo + gpuInfo + GetMachineGuid();
	std::string md5Str = (moboInfo + hddInfo + cpuInfo + ramInfo + gpuInfo + GetMachineGuid()).toStdString();
	return md5(md5Str);
}

int main(int argc, char *argv[]) {
	// Pre-check to make sure they're authorized
	//GetUniqueHWID();
	//exit(1);
	
	// Pre-creation settings
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	// The rest...
	QApplication a(argc, argv);
	QApplication::setStyle("Fusion");

	TCPClient tcpClient;
	if (!tcpClient.connectToHost("127.0.0.1", 1234))
		return -1;

	MainWindow w;
	w.show();

	return a.exec();
}