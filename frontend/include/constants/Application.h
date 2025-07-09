/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: UNORADAR-2025
 *
 * @file Application.h
 * @brief Application metadata and configuration constants
 * @author unoRadar Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#ifndef UNORADAR_CONSTANTS_APPLICATION_H
#define UNORADAR_CONSTANTS_APPLICATION_H

namespace unoRadar {
namespace Constants {
namespace Application {

// Application Metadata
constexpr char NAME[] = "unoRadar";
constexpr char FULL_NAME[] = "unoRadar Military-Grade Radar System";
constexpr char VERSION[] = "1.0.0";
constexpr char BUILD_TYPE[] = "RELEASE";

// Security Classification
constexpr char CLASSIFICATION[] = "UNCLASSIFIED";
constexpr char DISTRIBUTION[] = "UNLIMITED";
constexpr char EXPORT_CONTROL[] = "NOT SUBJECT TO EAR/ITAR";

// Organization Information
constexpr char ORGANIZATION[] = "unoRadar Defense Systems";
constexpr char CONTRACT_NUMBER[] = "UNORADAR-2025";
constexpr char CDRL[] = "A001";

// Certification Standards
constexpr char STANDARDS[] = "MISRA C++ 2008, DO-178C Level A, MIL-STD-1472";

} // namespace Application
} // namespace Constants
} // namespace unoRadar

#endif // UNORADAR_CONSTANTS_APPLICATION_H