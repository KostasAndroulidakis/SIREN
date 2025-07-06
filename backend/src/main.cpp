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

int main() {
    std::cout << "unoRadar Military-Grade Server v1.0.0" << std::endl;
    std::cout << "Boost version: " << BOOST_VERSION / 100000 << "."
              << BOOST_VERSION / 100 % 1000 << "."
              << BOOST_VERSION % 100 << std::endl;
    std::cout << "Boost.Asio version: " << BOOST_ASIO_VERSION << std::endl;

    // Minimal test - will be expanded in Phase 2
    boost::asio::io_context io_context;
    std::cout << "Event loop initialized successfully" << std::endl;

    return 0;
}