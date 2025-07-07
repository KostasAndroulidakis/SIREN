/**
 * @file statistics_calculator.cpp
 * @brief Implementation of military-grade statistical calculation utility
 * @author unoRadar Project
 * @date 2025
 *
 * Template specializations and explicit instantiations for commonly used
 * statistical calculator types in the unoRadar system.
 */

#include "utils/statistics_calculator.hpp"
#include <limits>

namespace unoradar {
namespace utils {

// ============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS FOR COMMON TYPES
// ============================================================================

// Instantiate commonly used template specializations to reduce compile time
// and ensure consistent behavior across the system

/// Instantiation for uint32_t (most common for latency, timing metrics)
template class StatisticsCalculator<uint32_t>;

/// Instantiation for int32_t (angles, distances from radar)
template class StatisticsCalculator<int32_t>;

/// Instantiation for double (high-precision calculations)
template class StatisticsCalculator<double>;

/// Instantiation for float (memory-efficient floating point)
template class StatisticsCalculator<float>;

/// Instantiation for uint64_t (large counters, timestamps)
template class StatisticsCalculator<uint64_t>;

// ============================================================================
// UTILITY FUNCTIONS FOR MILITARY-GRADE STATISTICAL ANALYSIS
// ============================================================================

namespace statistics_utils {

/**
 * @brief Calculate coefficient of variation (CV) as percentage
 * @param standard_deviation Standard deviation of the dataset
 * @param mean Mean value of the dataset
 * @return Coefficient of variation as percentage (0-100)
 * @note Returns 0.0 if mean is zero to avoid division by zero
 */
double calculateCoefficientOfVariation(double standard_deviation, double mean) noexcept {
    if (std::abs(mean) < std::numeric_limits<double>::epsilon()) {
        return 0.0; // Avoid division by zero
    }
    return (standard_deviation / std::abs(mean)) * 100.0;
}

/**
 * @brief Determine if a value is an outlier using the IQR method
 * @param value Value to test
 * @param q1 First quartile (25th percentile)
 * @param q3 Third quartile (75th percentile)
 * @param multiplier IQR multiplier (default 1.5 for standard outlier detection)
 * @return True if value is considered an outlier
 */
bool isOutlier(double value, double q1, double q3, double multiplier = 1.5) noexcept {
    double iqr = q3 - q1;
    double lower_bound = q1 - (multiplier * iqr);
    double upper_bound = q3 + (multiplier * iqr);

    return value < lower_bound || value > upper_bound;
}

/**
 * @brief Calculate Z-score for a value
 * @param value Value to calculate Z-score for
 * @param mean Population mean
 * @param standard_deviation Population standard deviation
 * @return Z-score (number of standard deviations from mean)
 * @note Returns 0.0 if standard deviation is zero
 */
double calculateZScore(double value, double mean, double standard_deviation) noexcept {
    if (std::abs(standard_deviation) < std::numeric_limits<double>::epsilon()) {
        return 0.0; // Avoid division by zero
    }
    return (value - mean) / standard_deviation;
}

/**
 * @brief Determine if a system metric is within acceptable military-grade tolerances
 * @param current_value Current measured value
 * @param target_value Target/expected value
 * @param tolerance_percentage Acceptable tolerance as percentage (e.g., 5.0 for ±5%)
 * @return True if value is within tolerance
 */
bool isWithinTolerance(double current_value, double target_value, double tolerance_percentage) noexcept {
    if (std::abs(target_value) < std::numeric_limits<double>::epsilon()) {
        // If target is zero, check absolute tolerance
        return std::abs(current_value) <= (tolerance_percentage / 100.0);
    }

    double relative_error = std::abs((current_value - target_value) / target_value) * 100.0;
    return relative_error <= tolerance_percentage;
}

/**
 * @brief Calculate Signal-to-Noise Ratio (SNR) in decibels
 * @param signal_power Signal power or mean squared signal
 * @param noise_variance Noise variance
 * @return SNR in decibels
 * @note Returns negative infinity if noise variance is zero
 */
double calculateSNR_dB(double signal_power, double noise_variance) noexcept {
    if (noise_variance <= 0.0) {
        return -std::numeric_limits<double>::infinity();
    }

    if (signal_power <= 0.0) {
        return -std::numeric_limits<double>::infinity();
    }

    return 10.0 * std::log10(signal_power / noise_variance);
}

} // namespace statistics_utils

// ============================================================================
// MILITARY-GRADE PERFORMANCE MONITORING HELPERS
// ============================================================================

namespace performance_stats {

/**
 * @brief Create a latency statistics calculator optimized for military-grade systems
 * @return Pre-configured statistics calculator for latency measurements
 */
UInt32StatsCalculator createLatencyCalculator() {
    // Use a higher alpha for latency to respond quickly to performance changes
    constexpr double LATENCY_ALPHA = 0.2; // More responsive than default 0.1
    return UInt32StatsCalculator(LATENCY_ALPHA);
}

/**
 * @brief Create a throughput statistics calculator for message rates
 * @return Pre-configured statistics calculator for throughput measurements
 */
UInt32StatsCalculator createThroughputCalculator() {
    // Use standard alpha for throughput - we want stability over responsiveness
    return UInt32StatsCalculator(constants::magic_numbers::MOVING_AVERAGE_ALPHA);
}

/**
 * @brief Create a memory usage statistics calculator
 * @return Pre-configured statistics calculator for memory usage tracking
 */
UInt64StatsCalculator createMemoryUsageCalculator() {
    // Use lower alpha for memory - changes should be gradual
    constexpr double MEMORY_ALPHA = 0.05; // More stable than default
    return UInt64StatsCalculator(MEMORY_ALPHA);
}

/**
 * @brief Create a radar measurement statistics calculator
 * @return Pre-configured statistics calculator for radar distance measurements
 */
Int32StatsCalculator createRadarMeasurementCalculator() {
    // Use default alpha for radar measurements
    return Int32StatsCalculator(constants::magic_numbers::MOVING_AVERAGE_ALPHA);
}

} // namespace performance_stats

} // namespace utils
} // namespace unoradar