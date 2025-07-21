/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: SIREN-2025
 *
 * @file Application.h
 * @brief Application metadata and configuration constants
 * @author KostasAndroulidakis
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#ifndef SIREN_CONSTANTS_APPLICATION_H
#define SIREN_CONSTANTS_APPLICATION_H

namespace siren {
namespace Constants {
namespace Application {

// Application Metadata
constexpr char NAME[] = "SIREN";
constexpr char FULL_NAME[] = "SIREN Military-Grade Sonar System";
constexpr char COMMAND_CENTER_TITLE[] = "COMMAND CENTER";
constexpr char VERSION[] = "1.0.0";
constexpr char BUILD_TYPE[] = "RELEASE";

// Security Classification
constexpr char CLASSIFICATION[] = "UNCLASSIFIED";
constexpr char DISTRIBUTION[] = "UNLIMITED";
constexpr char EXPORT_CONTROL[] = "NOT SUBJECT TO EAR/ITAR";

// Organization Information
constexpr char ORGANIZATION[] = "SIREN Defense Systems";
constexpr char CONTRACT_NUMBER[] = "SIREN-2025";
constexpr char CDRL[] = "A001";

// Certification Standards
constexpr char STANDARDS[] = "MISRA C++ 2008, DO-178C Level A, MIL-STD-1472";

} // namespace Application
} // namespace Constants
} // namespace siren

#endif // SIREN_CONSTANTS_APPLICATION_H