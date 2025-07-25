cmake_minimum_required(VERSION 3.16)
project(SIREN_frontend VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard Configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set Qt6 directory explicitly for Homebrew installation
set(CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/qt/6.9.1/lib/cmake")

# Add Homebrew Qt Framework paths to avoid AGL issue on macOS 26
list(APPEND CMAKE_FRAMEWORK_PATH
     "/opt/homebrew/Cellar/qt/6.9.1/Frameworks")

# Force older macOS SDK to avoid AGL framework removal in macOS 26
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0")
    set(CMAKE_OSX_SYSROOT "/Library/Developer/CommandLineTools/SDKs/MacOSX15.5.sdk")
endif()

# Find Qt6 components including WebSockets for sonar data
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core WebSockets)

# Qt6 standard project setup
qt_standard_project_setup()

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Qt Resource file for embedded assets (MISRA C++ Rule 21.2.1)
qt_add_resources(RESOURCES resources/resources.qrc)

# Create executable
qt_add_executable(SIREN_frontend
    src/main.cpp
    src/ui/controls/WindowControlButton.cpp
    src/ui/controls/MinimizeButton.cpp
    src/ui/controls/MaximizeButton.cpp
    src/ui/controls/CloseButton.cpp
    src/ui/controls/WindowControlBar.cpp
    src/ui/controls/WindowControlHandler.cpp
    src/ui/SplashScreen.cpp
    src/ui/MainWindow.cpp
    src/ui/MainLayout.cpp
    src/ui/PanelFactory.cpp
    src/ui/Theme.cpp
    src/ui/ConnectionStatusWidget.cpp
    src/ui/SonarDataWidget.cpp
    src/ui/SonarVisualizationWidget.cpp
    src/network/WebSocketClient.cpp
    src/data/SonarDataParser.cpp
    src/visualization/PolarCoordinateConverter.cpp
    src/visualization/SonarDataBuffer.cpp
    src/visualization/SonarAnimationController.cpp
    include/ui/controls/WindowControlButton.h
    include/ui/controls/MinimizeButton.h
    include/ui/controls/MaximizeButton.h
    include/ui/controls/CloseButton.h
    include/ui/controls/WindowControlBar.h
    include/ui/controls/WindowControlHandler.h
    include/ui/SplashScreen.h
    include/ui/MainWindow.h
    include/ui/MainLayout.h
    include/ui/PanelFactory.h
    include/ui/Theme.h
    include/ui/ConnectionStatusWidget.h
    include/ui/SonarDataWidget.h
    include/ui/SonarVisualizationWidget.h
    include/network/IWebSocketClient.h
    include/network/WebSocketClient.h
    include/data/SonarDataParser.h
    include/visualization/PolarCoordinateConverter.h
    include/visualization/SonarDataBuffer.h
    include/visualization/SonarAnimationController.h
    include/constants/WindowControls.h
    include/constants/LayoutConstants.h
    include/constants/VisualizationConstants.h
    ${RESOURCES}
)

# Link Qt6 libraries including WebSockets
target_link_libraries(SIREN_frontend PRIVATE
    Qt6::Widgets
    Qt6::Gui
    Qt6::Core
    Qt6::WebSockets
)

