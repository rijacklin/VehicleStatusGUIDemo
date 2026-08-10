#include "MainWindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.startButton, &QPushButton::clicked, this, &MainWindow::handleStartButtonClicked);
	connect(ui.pauseButton, &QPushButton::clicked, this, &MainWindow::handlePauseButtonClicked);
	connect(ui.emergencyStopButton, &QPushButton::clicked, this, &MainWindow::handleEmergencyStopButtonClicked);
	connect(&m_telemetrySource, &SimulatedTelemetrySource::telemetryUpdated, this, &MainWindow::handleTelemetryUpdated);
}

void MainWindow::renderMissionUi()
{
	switch (m_missionStatus)
	{
		case MissionStatus::Paused:
			ui.missionFrameStatus->setText(tr("Paused"));

			if (m_connectionStatus == ConnectionStatus::Connected)
			{
				ui.startButton->setEnabled(true);
				ui.pauseButton->setEnabled(false);
			}
			else
			{
				ui.startButton->setEnabled(false);
				ui.pauseButton->setEnabled(false);
			}
			break;
		case MissionStatus::Idle:
			ui.missionFrameStatus->setText(tr("Idle"));

			if (m_connectionStatus == ConnectionStatus::Connected)
			{
				ui.startButton->setEnabled(true);
				ui.pauseButton->setEnabled(false);
			}
			else
			{
				ui.startButton->setEnabled(false);
				ui.pauseButton->setEnabled(false);
			}
			break;
		case MissionStatus::Running:
			ui.missionFrameStatus->setText(tr("Running"));

			if (m_connectionStatus == ConnectionStatus::Connected)
			{
				ui.startButton->setEnabled(false);
				ui.pauseButton->setEnabled(true);
			}
			else
			{
				ui.startButton->setEnabled(false);
				ui.pauseButton->setEnabled(false);
			}

			break;
		case MissionStatus::EmergencyStopped:
			ui.missionFrameStatus->setText(tr("Emergency Stopped"));
			ui.startButton->setEnabled(false);
			ui.pauseButton->setEnabled(false);
			break;
		case MissionStatus::Unavailable:
			ui.missionFrameStatus->setText(tr("Unavailable"));
			ui.startButton->setEnabled(false);
			ui.pauseButton->setEnabled(false);
			break;
	}
}

void MainWindow::handleStartButtonClicked()
{
	if (m_connectionStatus != ConnectionStatus::Connected || (m_missionStatus != MissionStatus::Idle && m_missionStatus != MissionStatus::Paused))
	{
		return;
	}

	m_missionStatus = MissionStatus::Running;
	renderMissionUi();
}

void MainWindow::handlePauseButtonClicked()
{
	if (m_connectionStatus != ConnectionStatus::Connected || m_missionStatus != MissionStatus::Running)
	{
		return;
	}

	m_missionStatus = MissionStatus::Paused;
	renderMissionUi();
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

	m_missionStatus = MissionStatus::EmergencyStopped;
	renderMissionUi();
	ui.safetyFrameValue->setText(tr("Emergency stop activated"));
	ui.emergencyStopButton->setEnabled(false);
}

void MainWindow::handleTelemetryUpdated(const VehicleState &state)
{
	m_connectionStatus = state.connectionStatus;
	renderMissionUi();

	const auto speedKph = state.speedMps * 3.6F;

	switch (m_connectionStatus)
	{
		case ConnectionStatus::Unavailable:
			ui.connectionFrameValue->setText(tr("Unavailable"));
			ui.speedFrameValue->setText(tr("Unavailable"));
			ui.batteryValue->setText(tr("Unavailable"));
			ui.obstacleValue->setText(tr("Unavailable"));
			break;
		case ConnectionStatus::Disconnected:
			ui.connectionFrameValue->setText(tr("Disconnected"));
			ui.speedFrameValue->setText(tr("Unavailable"));
			ui.batteryValue->setText(tr("Unavailable"));
			ui.obstacleValue->setText(tr("Unavailable"));
			break;
		case ConnectionStatus::Connected:
			ui.connectionFrameValue->setText(tr("Connected"));
			ui.speedFrameValue->setText(tr("%1 km/h").arg(speedKph, 0, 'f', 1));

			if (state.batteryPercentage)
			{
				const float batteryPercentage = state.batteryPercentage.value();
				ui.batteryValue->setText(tr("%1%").arg(batteryPercentage, 0, 'f', 1));
			}
			else
			{
				ui.batteryValue->setText(tr("Unavailable"));
			}

			switch (state.obstacleObservation.status)
			{
				case ObstacleStatus::Detected:
					if (state.obstacleObservation.distanceMeters)
					{
						const auto distance = state.obstacleObservation.distanceMeters.value_or(0.0F);
						ui.obstacleValue->setText(tr("%1 m").arg(distance, 0, 'f', 1));
					}
					else
					{
						ui.obstacleValue->setText(tr("Unavailable"));
					}
					break;
				case ObstacleStatus::Clear:
					ui.obstacleValue->setText(tr("Clear"));
					break;
				case ObstacleStatus::Unavailable:
					ui.obstacleValue->setText(tr("Unavailable"));
					break;
			}
			break;
	}
}
