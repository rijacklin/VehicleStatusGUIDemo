#include "MainWindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.startButton, &QPushButton::clicked, this, &MainWindow::handleStartButtonClicked);
	connect(ui.pauseButton, &QPushButton::clicked, this, &MainWindow::handlePauseButtonClicked);
	connect(ui.emergencyStopButton, &QPushButton::clicked, this, &MainWindow::handleEmergencyStopButtonClicked);
}

void MainWindow::handleStartButtonClicked()
{
	ui.missionFrameStatus->setText(tr("Running"));
	ui.startButton->setEnabled(false);
	ui.pauseButton->setEnabled(true);
}

void MainWindow::handlePauseButtonClicked()
{
	ui.missionFrameStatus->setText(tr("Paused"));
	ui.pauseButton->setEnabled(false);
	ui.startButton->setEnabled(true);
}

void MainWindow::handleEmergencyStopButtonClicked()
{
	const auto result = QMessageBox::warning(
		this,
		tr("Confirm emergency stop"),
		tr("Are you sure you want to activate the emergency stop?"),
		QMessageBox::Yes | QMessageBox::Cancel,
		QMessageBox::Cancel
	);

	if (result != QMessageBox::Yes)
	{
		return;
	}

	ui.missionFrameStatus->setText(tr("Paused"));
	ui.safetyFrameValue->setText(tr("Emergency stop activated"));
	ui.startButton->setEnabled(false);
	ui.pauseButton->setEnabled(false);
	ui.emergencyStopButton->setEnabled(false);
}
