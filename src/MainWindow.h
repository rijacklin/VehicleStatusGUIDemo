#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "infrastructure/SimulatedTelemetrySource.h"
#include "domain/VehicleState.h"

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
	void renderMissionUi();

	Ui::MainWindowClass ui;
	SimulatedTelemetrySource m_telemetrySource;
	MissionStatus m_missionStatus{ MissionStatus::Idle };
	ConnectionStatus m_connectionStatus{ ConnectionStatus::Unavailable };
};

