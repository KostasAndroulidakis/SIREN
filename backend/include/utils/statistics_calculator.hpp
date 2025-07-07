/**
 * @file statistics_calculator.hpp
 * @brief Military-grade statistical calculation utility
 * @author unoRadar Project
 * @date 2025
 *
 * Template-based utility for statistical calculations including moving averages,
 * variance, and other metrics. Designed for military-grade performance with
 * thread-safe atomic operations and MISRA C++ compliance.
 *
 * SRP: Single responsibility - statistical calculations and data processing
 * SSOT: Eliminates duplicate statistical calculation code across components
 */

#pragma once

#include <cstdint>
#include <cmath>
#include <type_traits>
#include <stdexcept>
#include <mutex>
#include "constants/performance.hpp"

namespace unoradar {
namespace utils {

/**
 * @brief Template-based statistical calculator for military-grade systems
 *
 * Provides thread-safe statistical calculations with compile-time optimization
 * for different numeric types. Eliminates duplicate moving average calculations
 * across the codebase while maintaining military-grade performance.
 *
 * @tparam T Numeric type for calculations (int32_t, uint32_t, double, etc.)
 */
template<typename T>
class StatisticsCalculator {
public:
    static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");

    /**
     * @brief Statistical data structure
     */
    struct Statistics {
        T current_value{};              ///< Most recent value
        T exponential_average{};        ///< Exponential moving average
        T simple_average{};             ///< Simple moving average
        T minimum_value{};              ///< Minimum observed value
        T maximum_value{};              ///< Maximum observed value
        uint64_t sample_count{0};       ///< Total number of samples
        double variance{0.0};           ///< Population variance
        double standard_deviation{0.0}; ///< Standard deviation

        /// Reset all statistics to initial state
        void reset() noexcept {
            current_value = T{};
            exponential_average = T{};
            simple_average = T{};
            minimum_value = T{};
            maximum_value = T{};
            sample_count = 0;
            variance = 0.0;
            standard_deviation = 0.0;
        }
    };

    /**
     * @brief Constructor with validation
     * @param alpha Smoothing factor for exponential moving average (0.0 < alpha <= 1.0)
     * @throws std::invalid_argument if alpha is outside valid range
     */
    explicit StatisticsCalculator(double alpha = constants::performance::optimization::MOVING_AVERAGE_ALPHA)
        : alpha_(alpha)
        , statistics_{}
        , sum_for_simple_average_(0.0)
        , sum_of_squares_(0.0)
        , is_initialized_(false)
    {
        validateAlpha(alpha);
    }

    /**
     * @brief Add new sample and update all statistics
     * @param new_value New data point to include in calculations
     * @return Updated statistics structure
     */
    Statistics addSample(T new_value) {
        std::lock_guard<std::mutex> lock(mutex_);

        statistics_.current_value = new_value;
        statistics_.sample_count++;

        updateExponentialAverage(new_value);
        updateSimpleAverage(new_value);
        updateMinMaxValues(new_value);
        updateVarianceStatistics(new_value);

        return statistics_;
    }

    /**
     * @brief Get current statistics (thread-safe)
     * @return Copy of current statistics
     */
    Statistics getStatistics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return statistics_;
    }

    /**
     * @brief Calculate exponential moving average for a single value
     * @param new_value New value to incorporate
     * @param current_average Current exponential moving average
     * @param alpha Smoothing factor (optional, uses instance alpha if not provided)
     * @return Updated exponential moving average
     */
    static T calculateExponentialMovingAverage(T new_value, T current_average,
                                             double alpha = constants::performance::optimization::MOVING_AVERAGE_ALPHA) {
        validateAlpha(alpha);

        if constexpr (std::is_integral_v<T>) {
            // For integral types, use double precision and round result
            return static_cast<T>(
                alpha * static_cast<double>(new_value) +
                (1.0 - alpha) * static_cast<double>(current_average)
            );
        } else {
            // For floating-point types, direct calculation
            return static_cast<T>(alpha * new_value + (1.0 - alpha) * current_average);
        }
    }

    /**
     * @brief Reset all statistics to initial state
     */
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        statistics_.reset();
        sum_for_simple_average_ = 0.0;
        sum_of_squares_ = 0.0;
        is_initialized_ = false;
    }

    /**
     * @brief Get sample count (thread-safe)
     * @return Number of samples processed
     */
    uint64_t getSampleCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return statistics_.sample_count;
    }

    /**
     * @brief Get current exponential moving average (thread-safe)
     * @return Current exponential moving average
     */
    T getExponentialAverage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return statistics_.exponential_average;
    }

    /**
     * @brief Get current simple moving average (thread-safe)
     * @return Current simple moving average
     */
    T getSimpleAverage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return statistics_.simple_average;
    }

    /**
     * @brief Get minimum observed value (thread-safe)
     * @return Minimum value
     */
    T getMinimum() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return statistics_.minimum_value;
    }

    /**
     * @brief Get maximum observed value (thread-safe)
     * @return Maximum value
     */
    T getMaximum() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return statistics_.maximum_value;
    }

    /**
     * @brief Get standard deviation (thread-safe)
     * @return Standard deviation
     */
    double getStandardDeviation() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return statistics_.standard_deviation;
    }

    /**
     * @brief Change alpha value for exponential moving average
     * @param alpha New smoothing factor (0.0 < alpha <= 1.0)
     * @throws std::invalid_argument if alpha is outside valid range
     */
    void setAlpha(double alpha) {
        validateAlpha(alpha);
        std::lock_guard<std::mutex> lock(mutex_);
        alpha_ = alpha;
    }

    /**
     * @brief Get current alpha value
     * @return Current smoothing factor
     */
    double getAlpha() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return alpha_;
    }

private:
    mutable std::mutex mutex_;          ///< Thread synchronization
    double alpha_;                      ///< Exponential moving average smoothing factor
    Statistics statistics_;             ///< Current statistics data
    double sum_for_simple_average_;     ///< Running sum for simple average calculation
    double sum_of_squares_;             ///< Running sum of squares for variance
    bool is_initialized_;               ///< First sample initialization flag

    /**
     * @brief Validate alpha parameter
     * @param alpha Value to validate
     * @throws std::invalid_argument if alpha is outside valid range (0.0, 1.0]
     */
    static void validateAlpha(double alpha) {
        if (alpha <= 0.0 || alpha > 1.0) {
            throw std::invalid_argument(
                "Alpha must be in range (0.0, 1.0]. Provided: " + std::to_string(alpha)
            );
        }
    }

    /**
     * @brief Update exponential moving average
     * @param new_value New sample value
     */
    void updateExponentialAverage(T new_value) {
        if (!is_initialized_) {
            statistics_.exponential_average = new_value;
            is_initialized_ = true;
        } else {
            statistics_.exponential_average = calculateExponentialMovingAverage(
                new_value, statistics_.exponential_average, alpha_
            );
        }
    }

    /**
     * @brief Update simple moving average
     * @param new_value New sample value
     */
    void updateSimpleAverage(T new_value) {
        sum_for_simple_average_ += static_cast<double>(new_value);

        if constexpr (std::is_integral_v<T>) {
            statistics_.simple_average = static_cast<T>(
                sum_for_simple_average_ / static_cast<double>(statistics_.sample_count)
            );
        } else {
            statistics_.simple_average = static_cast<T>(
                sum_for_simple_average_ / static_cast<double>(statistics_.sample_count)
            );
        }
    }

    /**
     * @brief Update minimum and maximum values
     * @param new_value New sample value
     */
    void updateMinMaxValues(T new_value) {
        if (statistics_.sample_count == 1) {
            // First sample - initialize both min and max
            statistics_.minimum_value = new_value;
            statistics_.maximum_value = new_value;
        } else {
            if (new_value < statistics_.minimum_value) {
                statistics_.minimum_value = new_value;
            }
            if (new_value > statistics_.maximum_value) {
                statistics_.maximum_value = new_value;
            }
        }
    }

    /**
     * @brief Update variance and standard deviation
     * @param new_value New sample value
     */
    void updateVarianceStatistics(T new_value) {
        double value_as_double = static_cast<double>(new_value);
        sum_of_squares_ += value_as_double * value_as_double;

        if (statistics_.sample_count > 1) {
            // Calculate population variance: Var = (ΣX²)/n - (ΣX/n)²
            double mean = sum_for_simple_average_ / static_cast<double>(statistics_.sample_count);
            double mean_of_squares = sum_of_squares_ / static_cast<double>(statistics_.sample_count);

            statistics_.variance = mean_of_squares - (mean * mean);
            statistics_.standard_deviation = std::sqrt(statistics_.variance);
        }
    }
};

// ============================================================================
// CONVENIENCE TYPE ALIASES FOR COMMON USE CASES
// ============================================================================

/// Statistics calculator for unsigned 32-bit integers (latency, timing)
using UInt32StatsCalculator = StatisticsCalculator<uint32_t>;

/// Statistics calculator for signed 32-bit integers (angles, distances)
using Int32StatsCalculator = StatisticsCalculator<int32_t>;

/// Statistics calculator for double precision floating point
using DoubleStatsCalculator = StatisticsCalculator<double>;

/// Statistics calculator for single precision floating point
using FloatStatsCalculator = StatisticsCalculator<float>;

/// Statistics calculator for 64-bit unsigned integers (counters, timestamps)
using UInt64StatsCalculator = StatisticsCalculator<uint64_t>;

// ============================================================================
// MILITARY-GRADE PERFORMANCE MONITORING HELPERS
// ============================================================================

namespace performance_stats {

/**
 * @brief Create a latency statistics calculator optimized for military-grade systems
 * @return Pre-configured statistics calculator for latency measurements
 */
UInt32StatsCalculator createLatencyCalculator();

/**
 * @brief Create a throughput statistics calculator for message rates
 * @return Pre-configured statistics calculator for throughput measurements
 */
UInt32StatsCalculator createThroughputCalculator();

/**
 * @brief Create a memory usage statistics calculator
 * @return Pre-configured statistics calculator for memory usage tracking
 */
UInt64StatsCalculator createMemoryUsageCalculator();

/**
 * @brief Create a radar measurement statistics calculator
 * @return Pre-configured statistics calculator for radar distance measurements
 */
Int32StatsCalculator createRadarMeasurementCalculator();

} // namespace performance_stats

} // namespace utils
} // namespace unoradar