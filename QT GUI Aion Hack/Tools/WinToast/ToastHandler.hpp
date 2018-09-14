#pragma once
#include "Tools/WinToast/WinToast.h"

#include <qdebug.h>

class ToastHandler : public WinToastLib::IWinToastHandler {
public:
	void toastActivated() const {
		qDebug() << "The user clicked in this toast";
	}

	void toastActivated(int actionIndex) const {
		qDebug() << "The user clicked on button #" << actionIndex << L" in this toast";
	}

	void toastFailed() const {
		qDebug() << "Error showing current toast";
	}
	void toastDismissed(WinToastDismissalReason state) const {
		switch (state) {
		case UserCanceled:
			qDebug() << "The user dismissed this toast";
			break;
		case ApplicationHidden:
			qDebug() << "The application hid the toast using ToastNotifier.hide()";
			break;
		case TimedOut:
			qDebug() << "The toast has timed out";
			break;
		default:
			qDebug() << "Toast not activated";
			break;
		}
	}
};