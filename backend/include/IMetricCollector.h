#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <thread>

struct Metric
{
    std::chrono::system_clock::time_point ts;
    double value;
};

struct NamedMetric
{
    std::optional<Metric> metric;
    std::string name;
};

class IMetricCollector
{
public:
    explicit IMetricCollector(
        std::function<void(const NamedMetric&)> recordCallback,
        std::string label,
        std::chrono::milliseconds collectRate,
        std::chrono::milliseconds collectTimeout
    );

    virtual ~IMetricCollector();

    IMetricCollector(const IMetricCollector& other) = delete;
    IMetricCollector& operator=(const IMetricCollector& other) = delete;
    IMetricCollector(IMetricCollector&& other) = delete;
    IMetricCollector& operator=(IMetricCollector&& other) = delete;

    NamedMetric takeLatest();

protected:
    void start();
    void stop();

private:
    virtual std::optional<Metric> collect(std::chrono::milliseconds timeout) noexcept = 0;

    void recordMetric(const NamedMetric& metric);
    void run();

    std::mutex m_metricMutex;
    NamedMetric m_metric;
    std::function<void(const NamedMetric&)> m_recordCallback;

    std::chrono::milliseconds m_collectRate;
    std::chrono::milliseconds m_collectTimeout;

    std::atomic<bool> m_running{false};
    std::thread m_thread;
};
