#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override = default;

private:
	void handleStartButtonClicked();
	void handlePauseButtonClicked();
	void handleEmergencyStopButtonClicked();

	Ui::MainWindowClass ui;
};

