/**
 * @file main.cpp
 * @brief Main entry point for unoRadar military-grade C++ server
 * @author unoRadar Project
 * @date 2025
 */

#include <iostream>
#include <boost/version.hpp>
#include <boost/asio/version.hpp>
#include <boost/asio.hpp>

// Military-grade configuration headers
#include "core/config.hpp"
#include "data/radar_types.hpp"
#include "utils/constants.hpp"

int main() {
    namespace cfg = unoradar::config;
    namespace cnst = unoradar::constants;

    std::cout << "unoRadar Military-Grade Server v" << cfg::system::VERSION << std::endl;
    std::cout << "Boost version: " << BOOST_VERSION / 100000 << "."
              << BOOST_VERSION / 100 % 1000 << "."
              << BOOST_VERSION % 100 << std::endl;
    std::cout << "Boost.Asio version: " << BOOST_ASIO_VERSION << std::endl;
    std::cout << "Build type: " << cnst::version::BUILD_TYPE << std::endl;
    std::cout << "WebSocket port: " << cfg::websocket::DEFAULT_PORT << std::endl;
    std::cout << "Serial baud rate: " << cfg::serial::BAUD_RATE << std::endl;

    // Test configuration system - all values from constants, no magic numbers
    std::cout << "\n=== Military-Grade Configuration Test ===" << std::endl;
    std::cout << "Servo range: " << cnst::servo::MIN_ANGLE_DEGREES << "° to "
              << cnst::servo::MAX_ANGLE_DEGREES << "°" << std::endl;
    std::cout << "Sensor range: " << cnst::sensor::MIN_DISTANCE_CM << "cm to "
              << cnst::sensor::MAX_DISTANCE_CM << "cm" << std::endl;
    std::cout << "Target latency: " << cnst::performance::TARGET_LOOP_TIME_US << "μs" << std::endl;

    // Test data types
    unoradar::data::RadarDataPoint test_point(90, 150);
    std::cout << "Test data point: angle=" << test_point.angle
              << "°, distance=" << test_point.distance << "cm" << std::endl;

    // Minimal test - will be expanded in Phase 2
    boost::asio::io_context io_context;
    std::cout << "\n✅ Event loop initialized successfully" << std::endl;
    std::cout << "✅ All configuration headers loaded" << std::endl;
    std::cout << "✅ Phase 1 Complete - Ready for Phase 2" << std::endl;

    return 0;
}