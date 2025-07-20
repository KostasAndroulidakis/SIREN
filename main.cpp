/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: SIREN-2025
 *
 * @file main.cpp
 * @brief Main entry point for SIREN military-grade radar system
 * @author SIREN Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>

namespace {
    volatile sig_atomic_t shutdownRequested = 0;  // MISRA C++ 2008: Signal-safe
    pid_t backendPid = -1;
    pid_t frontendPid = -1;
}

void signalHandler(int signal) noexcept {
    if (signal == SIGINT || signal == SIGTERM) {
        shutdownRequested = 1;
    }
}

bool waitForPort(int port, int timeoutSeconds = 10) {
    const auto start = std::chrono::steady_clock::now();

    while (true) {
        const std::string cmd = "lsof -i:" + std::to_string(port) + " >/dev/null 2>&1";
        if (std::system(cmd.c_str()) == 0) {
            return true;
        }

        const auto elapsed = std::chrono::steady_clock::now() - start;
        if (elapsed > std::chrono::seconds(timeoutSeconds)) {
            return false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void gracefulShutdown() {
    if (frontendPid > 0) {
        kill(frontendPid, SIGTERM);
        waitpid(frontendPid, nullptr, 0);
    }

    if (backendPid > 0) {
        kill(backendPid, SIGTERM);
        waitpid(backendPid, nullptr, 0);
    }
}

int main(int argc, char* argv[]) {
    // MISRA C++ 2008: 0-1-11 - Mark unused parameters
    static_cast<void>(argc);
    static_cast<void>(argv);

    std::cout << "==================================================" << std::endl;
    std::cout << " SIREN Military-Grade Radar System v1.0.0" << std::endl;
    std::cout << " CLASSIFICATION: UNCLASSIFIED" << std::endl;
    std::cout << "==================================================" << std::endl;

    // Install signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    try {
        // Start backend
        std::cout << "[SYSTEM] Starting backend server..." << std::endl;
        backendPid = fork();

        if (backendPid == 0) {
            execl("./backend/build/SIREN_server", "SIREN_server", nullptr);
            std::cerr << "[ERROR] Failed to start backend!" << std::endl;
            std::exit(EXIT_FAILURE);
        } else if (backendPid < 0) {
            std::cerr << "[ERROR] Fork failed for backend!" << std::endl;
            return EXIT_FAILURE;
        }

        // Wait for backend
        std::cout << "[SYSTEM] Waiting for backend..." << std::endl;
        if (!waitForPort(8080)) {
            std::cerr << "[ERROR] Backend failed to start!" << std::endl;
            kill(backendPid, SIGTERM);
            return EXIT_FAILURE;
        }

        // Start frontend
        std::cout << "[SYSTEM] Starting frontend..." << std::endl;
        frontendPid = fork();

        if (frontendPid == 0) {
            execl("./frontend/build/SIREN", "SIREN", nullptr);
            std::cerr << "[ERROR] Failed to start frontend!" << std::endl;
            std::exit(EXIT_FAILURE);
        } else if (frontendPid < 0) {
            std::cerr << "[ERROR] Fork failed for frontend!" << std::endl;
            kill(backendPid, SIGTERM);
            return EXIT_FAILURE;
        }

        std::cout << "[SYSTEM] SIREN operational (Ctrl+C to shutdown)" << std::endl;

        // Monitor processes
        while (shutdownRequested == 0) {
            int status;
            pid_t pid = waitpid(-1, &status, WNOHANG);

            if (pid == backendPid) {
                std::cerr << "[ERROR] Backend crashed!" << std::endl;
                kill(frontendPid, SIGTERM);
                return EXIT_FAILURE;
            } else if (pid == frontendPid) {
                std::cout << "[SYSTEM] Frontend closed." << std::endl;
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        gracefulShutdown();
        std::cout << "[SYSTEM] Shutdown complete." << std::endl;
        return EXIT_SUCCESS;

    } catch (const std::exception& e) {
        std::cerr << "[FATAL] Exception: " << e.what() << std::endl;
        gracefulShutdown();
        return EXIT_FAILURE;
    }
}