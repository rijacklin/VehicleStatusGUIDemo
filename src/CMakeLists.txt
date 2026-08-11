include(qt.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
	COMPONENTS
		Core
		Gui
		Widgets
)
qt_standard_project_setup()

set(PROJECT_SOURCES
	main.cpp
	MainWindow.ui
	MainWindow.h
	MainWindow.cpp
	infrastructure/SimulatedTelemetrySource.h
	infrastructure/SimulatedTelemetrySource.cpp
	infrastructure/SimulatedCommandGateway.h
	infrastructure/SimulatedCommandGateway.cpp
	domain/VehicleState.h
	domain/ObstacleSafety.h
	domain/ObstacleSafety.cpp
	domain/MissionCommand.h
	application/VehicleController.h
	application/VehicleController.cpp
)

qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(${PROJECT_NAME}
	PROPERTIES
		WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		Qt::Core
		Qt::Gui
		Qt::Widgets
)

