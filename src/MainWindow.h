#pragma once

#include <QtWidgets/QMainWindow>
#include <QTimer>
#include "ui_MainWindow.h"
#include "application/VehicleController.h"
#include "infrastructure/SimulatedCommandGateway.h"
#include "infrastructure/SimulatedTelemetrySource.h"
#include "domain/VehicleState.h"
#include "domain/ObstacleSafety.h"

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
	void handleTelemetryUpdated(const VehicleState &state);
	void handleTelemetryStale();
	void renderMissionUi();
	void renderSafetyUi();
	void renderConnectionUi();
	[[nodiscard]] bool isTelemetryUsable() const;

	Ui::MainWindowClass ui;
	QTimer m_staleTelemetryTimer;
	SimulatedTelemetrySource m_telemetrySource;
	ConnectionStatus m_connectionStatus{ ConnectionStatus::Unavailable };
	SafetySeverity m_obstacleSafetySeverity{ SafetySeverity::Unavailable };
	TelemetryFreshness m_telemetryFreshness{ TelemetryFreshness::Unavailable };
	VehicleController m_vehicleController;
	SimulatedCommandGateway m_commandGateway;
};

