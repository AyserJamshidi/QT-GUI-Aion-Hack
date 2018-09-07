/********************************************************************************
** Form generated from reading UI file 'QtGuiApplication1.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTGUIAPPLICATION1_H
#define UI_QTGUIAPPLICATION1_H

#include <QtCore/QVariant>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *exit_MenuAction;
    QAction *home_MenuAction;
    QAction *hack_MenuAction;
    QAction *attach_MenuAction;
    QWidget *centralWidget;
    QStackedWidget *MainWindowStackedWidget;
    QWidget *mainPage_StackedWidget;
    QLabel *mainPage_LoadingGif;
    QWebEngineView *webEngineView;
    QWidget *aion_StackedWidget;
    QLabel *aion_LoadingGif;
    QLabel *statusLabel;
    QWidget *attach_StackedWidget;
    QTableWidget *attach_TableWidget;
    QPushButton *hack_PushButton;
    QPushButton *refresh_PushButton;
    QTextEdit *processId_TextEdit;
    QMenuBar *menuBar;
    QMenu *menuNew;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->setWindowModality(Qt::ApplicationModal);
        MainWindow->setEnabled(true);
        MainWindow->resize(631, 413);
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(631, 413));
        MainWindow->setMaximumSize(QSize(631, 413));
        MainWindow->setAutoFillBackground(false);
        MainWindow->setStyleSheet(QLatin1String("QMenuBar, QMenu {\n"
"	color: rgb(176, 30, 58);\n"
"	background-color: rgb(38, 38, 38);\n"
"}\n"
"\n"
"QMenuBar::item:selected, QMenu::item::selected {\n"
"	background-color: rgb(50, 50, 50);\n"
"}\n"
"\n"
"QMenu::item:selected {\n"
"	color: rgb(255, 255, 255);\n"
"	background-color: rgb(50, 50, 50);\n"
"}"));
        exit_MenuAction = new QAction(MainWindow);
        exit_MenuAction->setObjectName(QStringLiteral("exit_MenuAction"));
        exit_MenuAction->setCheckable(false);
        home_MenuAction = new QAction(MainWindow);
        home_MenuAction->setObjectName(QStringLiteral("home_MenuAction"));
        hack_MenuAction = new QAction(MainWindow);
        hack_MenuAction->setObjectName(QStringLiteral("hack_MenuAction"));
        hack_MenuAction->setEnabled(false);
        attach_MenuAction = new QAction(MainWindow);
        attach_MenuAction->setObjectName(QStringLiteral("attach_MenuAction"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        MainWindowStackedWidget = new QStackedWidget(centralWidget);
        MainWindowStackedWidget->setObjectName(QStringLiteral("MainWindowStackedWidget"));
        MainWindowStackedWidget->setEnabled(true);
        MainWindowStackedWidget->setGeometry(QRect(0, 0, 636, 400));
        MainWindowStackedWidget->setFrameShape(QFrame::NoFrame);
        MainWindowStackedWidget->setFrameShadow(QFrame::Plain);
        MainWindowStackedWidget->setMidLineWidth(0);
        mainPage_StackedWidget = new QWidget();
        mainPage_StackedWidget->setObjectName(QStringLiteral("mainPage_StackedWidget"));
        mainPage_LoadingGif = new QLabel(mainPage_StackedWidget);
        mainPage_LoadingGif->setObjectName(QStringLiteral("mainPage_LoadingGif"));
        mainPage_LoadingGif->setGeometry(QRect(0, 0, 61, 21));
        webEngineView = new QWebEngineView(mainPage_StackedWidget);
        webEngineView->setObjectName(QStringLiteral("webEngineView"));
        webEngineView->setGeometry(QRect(0, 0, 631, 391));
        webEngineView->setUrl(QUrl(QStringLiteral("https://www.lmfaoown.com/aion_hack/launcher/")));
        MainWindowStackedWidget->addWidget(mainPage_StackedWidget);
        aion_StackedWidget = new QWidget();
        aion_StackedWidget->setObjectName(QStringLiteral("aion_StackedWidget"));
        aion_LoadingGif = new QLabel(aion_StackedWidget);
        aion_LoadingGif->setObjectName(QStringLiteral("aion_LoadingGif"));
        aion_LoadingGif->setGeometry(QRect(0, 0, 61, 21));
        statusLabel = new QLabel(aion_StackedWidget);
        statusLabel->setObjectName(QStringLiteral("statusLabel"));
        statusLabel->setGeometry(QRect(3, 353, 624, 31));
        QFont font;
        font.setPointSize(12);
        statusLabel->setFont(font);
        statusLabel->setStyleSheet(QLatin1String("QLabel {	\n"
"	color: rgb(176, 30, 58);\n"
"}"));
        statusLabel->setAlignment(Qt::AlignCenter);
        MainWindowStackedWidget->addWidget(aion_StackedWidget);
        attach_StackedWidget = new QWidget();
        attach_StackedWidget->setObjectName(QStringLiteral("attach_StackedWidget"));
        attach_TableWidget = new QTableWidget(attach_StackedWidget);
        if (attach_TableWidget->columnCount() < 2)
            attach_TableWidget->setColumnCount(2);
        if (attach_TableWidget->rowCount() < 10)
            attach_TableWidget->setRowCount(10);
        attach_TableWidget->setObjectName(QStringLiteral("attach_TableWidget"));
        attach_TableWidget->setEnabled(false);
        attach_TableWidget->setGeometry(QRect(10, 10, 254, 326));
        attach_TableWidget->setContextMenuPolicy(Qt::DefaultContextMenu);
        attach_TableWidget->setAcceptDrops(false);
        attach_TableWidget->setAutoFillBackground(false);
        attach_TableWidget->setFrameShape(QFrame::StyledPanel);
        attach_TableWidget->setFrameShadow(QFrame::Sunken);
        attach_TableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        attach_TableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        attach_TableWidget->setTabKeyNavigation(false);
        attach_TableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        attach_TableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        attach_TableWidget->setTextElideMode(Qt::ElideRight);
        attach_TableWidget->setShowGrid(true);
        attach_TableWidget->setGridStyle(Qt::SolidLine);
        attach_TableWidget->setSortingEnabled(false);
        attach_TableWidget->setWordWrap(false);
        attach_TableWidget->setCornerButtonEnabled(false);
        attach_TableWidget->setRowCount(10);
        attach_TableWidget->setColumnCount(2);
        attach_TableWidget->horizontalHeader()->setVisible(true);
        attach_TableWidget->horizontalHeader()->setCascadingSectionResizes(false);
        attach_TableWidget->horizontalHeader()->setDefaultSectionSize(126);
        attach_TableWidget->horizontalHeader()->setMinimumSectionSize(32);
        attach_TableWidget->verticalHeader()->setVisible(false);
        hack_PushButton = new QPushButton(attach_StackedWidget);
        hack_PushButton->setObjectName(QStringLiteral("hack_PushButton"));
        hack_PushButton->setGeometry(QRect(190, 340, 75, 23));
        refresh_PushButton = new QPushButton(attach_StackedWidget);
        refresh_PushButton->setObjectName(QStringLiteral("refresh_PushButton"));
        refresh_PushButton->setEnabled(false);
        refresh_PushButton->setGeometry(QRect(100, 340, 75, 23));
        processId_TextEdit = new QTextEdit(attach_StackedWidget);
        processId_TextEdit->setObjectName(QStringLiteral("processId_TextEdit"));
        processId_TextEdit->setGeometry(QRect(280, 340, 94, 23));
        processId_TextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        processId_TextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        processId_TextEdit->setLineWrapMode(QTextEdit::NoWrap);
        processId_TextEdit->setLineWrapColumnOrWidth(0);
        MainWindowStackedWidget->addWidget(attach_StackedWidget);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 631, 22));
        menuNew = new QMenu(menuBar);
        menuNew->setObjectName(QStringLiteral("menuNew"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuNew->menuAction());
        menuNew->addAction(home_MenuAction);
        menuNew->addAction(hack_MenuAction);
        menuNew->addAction(attach_MenuAction);
        menuNew->addSeparator();
        menuNew->addAction(exit_MenuAction);

        retranslateUi(MainWindow);

        MainWindowStackedWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Lmfaoown's Aion Hack", nullptr));
        exit_MenuAction->setText(QApplication::translate("MainWindow", "Exit", nullptr));
        home_MenuAction->setText(QApplication::translate("MainWindow", "Home", nullptr));
        hack_MenuAction->setText(QApplication::translate("MainWindow", "Hack (disabled)", nullptr));
        attach_MenuAction->setText(QApplication::translate("MainWindow", "Attach", nullptr));
        mainPage_LoadingGif->setText(QString());
        aion_LoadingGif->setText(QString());
        statusLabel->setText(QApplication::translate("MainWindow", "Receiving data from server...", nullptr));
#ifndef QT_NO_TOOLTIP
        attach_TableWidget->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        hack_PushButton->setText(QApplication::translate("MainWindow", "Hack", nullptr));
        refresh_PushButton->setText(QApplication::translate("MainWindow", "Refresh", nullptr));
        processId_TextEdit->setPlaceholderText(QApplication::translate("MainWindow", "Insert process ID", nullptr));
        menuNew->setTitle(QApplication::translate("MainWindow", "Menu", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTGUIAPPLICATION1_H
