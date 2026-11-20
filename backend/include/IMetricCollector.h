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
        std::function<void(NamedMetric&&)> recordCallback,
        std::string label,
        std::chrono::milliseconds collectRate
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
    virtual std::optional<Metric> collect() noexcept = 0;

    void recordMetric(NamedMetric&& metric);
    void run();

    std::mutex m_metricMutex;
    NamedMetric m_metric;
    std::function<void(NamedMetric&&)> m_recordCallback;

    std::chrono::milliseconds m_collectRate;
    std::chrono::milliseconds m_collectTimeout;

    std::atomic<bool> m_running{false};
    std::thread m_thread;
};
