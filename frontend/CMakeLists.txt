cmake_minimum_required(VERSION 3.16)
project(unoRadar_frontend VERSION 1.0.0 LANGUAGES CXX)

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

# Find Qt6 components including WebSockets for radar data
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core WebSockets)

# Qt6 standard project setup
qt_standard_project_setup()

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Create executable
qt_add_executable(unoRadar_frontend
    src/main.cpp
)

# Link Qt6 libraries including WebSockets
target_link_libraries(unoRadar_frontend PRIVATE
    Qt6::Widgets
    Qt6::Gui
    Qt6::Core
    Qt6::WebSockets
)