#include "MainWindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), m_staleTelemetryTimer(this)
{
	ui.setupUi(this);

	m_staleTelemetryTimer.setInterval(1500);
	m_staleTelemetryTimer.setSingleShot(true);

	connect(ui.startButton, &QPushButton::clicked, this, &MainWindow::handleStartButtonClicked);
	connect(ui.pauseButton, &QPushButton::clicked, this, &MainWindow::handlePauseButtonClicked);
	connect(ui.emergencyStopButton, &QPushButton::clicked, this, &MainWindow::handleEmergencyStopButtonClicked);
	connect(&m_telemetrySource, &SimulatedTelemetrySource::telemetryUpdated, this, &MainWindow::handleTelemetryUpdated);
	connect(&m_staleTelemetryTimer, &QTimer::timeout, this, &MainWindow::handleTelemetryStale);
	connect(&m_vehicleController, &VehicleController::commandIssued, &m_commandGateway, &SimulatedCommandGateway::sendCommand);
	connect(&m_commandGateway, &SimulatedCommandGateway::commandAcknowledged, &m_vehicleController, &VehicleController::handleCommandAcknowledgement);
	connect(&m_vehicleController, &VehicleController::missionStatusChanged, this, &MainWindow::renderMissionUi);
	connect(&m_vehicleController, &VehicleController::missionStatusChanged, this, &MainWindow::renderSafetyUi);
	connect(&m_vehicleController, &VehicleController::pendingCommandChanged, this, &MainWindow::renderMissionUi);
	connect(&m_vehicleController, &VehicleController::pendingCommandChanged, this, &MainWindow::renderSafetyUi);
	connect(&m_vehicleController, &VehicleController::commandIssued, this, &MainWindow::renderMissionUi);
	connect(&m_vehicleController, &VehicleController::commandIssued, this, &MainWindow::renderSafetyUi);
}

void MainWindow::renderMissionUi()
{
	const auto missionStatus = m_vehicleController.missionStatus();

	switch (missionStatus)
	{
		case MissionStatus::Paused:
			ui.missionFrameStatus->setText(tr("Paused"));

			if (isTelemetryUsable())
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

			if (isTelemetryUsable())
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

			if (isTelemetryUsable())
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

	if (m_vehicleController.hasPendingCommand())
	{
		ui.startButton->setEnabled(false);
		ui.pauseButton->setEnabled(false);
	}

	ui.emergencyStopButton->setEnabled(
		missionStatus != MissionStatus::EmergencyStopped && !m_vehicleController.isEmergencyStopPending()
	);
}

void MainWindow::renderSafetyUi()
{
	if (m_vehicleController.missionStatus() == MissionStatus::EmergencyStopped)
	{
		ui.safetyFrameValue->setText(tr("Emergency stop activated"));
	}
	else if (m_vehicleController.isEmergencyStopPending())
	{
		ui.safetyFrameValue->setText(tr("Emergency stop requested"));
	}
	else if (m_telemetryFreshness != TelemetryFreshness::Fresh)
	{
		ui.safetyFrameValue->setText(tr("Unavailable"));
	}
	else if (m_connectionStatus != ConnectionStatus::Connected)
	{
		ui.safetyFrameValue->setText(tr("Unavailable"));
	}
	else if (m_obstacleSafetySeverity == SafetySeverity::Critical)
	{
		ui.safetyFrameValue->setText(tr("Critical obstacle proximity"));
	}
	else if (m_obstacleSafetySeverity == SafetySeverity::Warning)
	{
		ui.safetyFrameValue->setText(tr("Obstacle nearby"));
	}
	else if (m_obstacleSafetySeverity == SafetySeverity::Normal)
	{
		ui.safetyFrameValue->setText(tr("Normal"));
	}
	else
	{
		ui.safetyFrameValue->setText(tr("Unavailable"));
	}
}

void MainWindow::renderConnectionUi()
{
	switch (m_connectionStatus)
	{
		case ConnectionStatus::Connected:
			if (m_telemetryFreshness == TelemetryFreshness::Fresh)
			{
				ui.connectionFrameValue->setText(tr("Connected | Fresh"));
			}
			else if (m_telemetryFreshness == TelemetryFreshness::Stale)
			{
				ui.connectionFrameValue->setText(tr("Connected | Stale"));
			}
			else
			{
				ui.connectionFrameValue->setText(tr("Unavailable"));
			}
			break;
		case ConnectionStatus::Disconnected:
			if (m_telemetryFreshness == TelemetryFreshness::Fresh)
			{
				ui.connectionFrameValue->setText(tr("Disconnected | Fresh"));
			}
			else if (m_telemetryFreshness == TelemetryFreshness::Stale)
			{
				ui.connectionFrameValue->setText(tr("Disconnected | Stale"));
			}
			else
			{
				ui.connectionFrameValue->setText(tr("Unavailable"));
			}
			break;
		case ConnectionStatus::Unavailable:
			ui.connectionFrameValue->setText(tr("Unavailable"));
			break;
	}
}

void MainWindow::handleStartButtonClicked()
{
	const auto missionStatus = m_vehicleController.missionStatus();

	if (!isTelemetryUsable() || (missionStatus != MissionStatus::Idle && missionStatus != MissionStatus::Paused))
	{
		return;
	}

	m_vehicleController.requestStartMission();
}

void MainWindow::handlePauseButtonClicked()
{
	if (!isTelemetryUsable() || m_vehicleController.missionStatus() != MissionStatus::Running)
	{
		return;
	}

	m_vehicleController.requestPauseMission();
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

	m_vehicleController.requestEmergencyStop();
}

void MainWindow::handleTelemetryUpdated(const VehicleState &state)
{
	m_telemetryFreshness = TelemetryFreshness::Fresh;
	m_staleTelemetryTimer.start();

	m_connectionStatus = state.connectionStatus;
	renderConnectionUi();
	renderMissionUi();

	if (m_connectionStatus == ConnectionStatus::Connected)
	{
		m_obstacleSafetySeverity = evaluateSafetySeverity(state.obstacleObservation);
	}
	else
	{
		m_obstacleSafetySeverity = SafetySeverity::Unavailable;
	}

	renderSafetyUi();

	const auto speedKph = state.speedMps * 3.6F;

	switch (m_connectionStatus)
	{
		case ConnectionStatus::Unavailable:
			ui.speedFrameValue->setText(tr("Unavailable"));
			ui.batteryValue->setText(tr("Unavailable"));
			ui.obstacleValue->setText(tr("Unavailable"));
			break;
		case ConnectionStatus::Disconnected:
			ui.speedFrameValue->setText(tr("Unavailable"));
			ui.batteryValue->setText(tr("Unavailable"));
			ui.obstacleValue->setText(tr("Unavailable"));
			break;
		case ConnectionStatus::Connected:
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

void MainWindow::handleTelemetryStale()
{
	m_telemetryFreshness = TelemetryFreshness::Stale;

	renderMissionUi();
	renderSafetyUi();
	renderConnectionUi();

	ui.speedFrameValue->setText(tr("Unavailable"));
	ui.batteryValue->setText(tr("Unavailable"));
	ui.obstacleValue->setText(tr("Unavailable"));
}

bool MainWindow::isTelemetryUsable() const
{
	if (m_connectionStatus == ConnectionStatus::Connected && m_telemetryFreshness == TelemetryFreshness::Fresh)
	{
		return true;
	}

	return false;
}