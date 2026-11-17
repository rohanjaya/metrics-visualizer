#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <utility>

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
        std::function<void(NamedMetric)> recordCallback,
        std::string label,
        std::chrono::milliseconds collectRate,
        std::chrono::milliseconds collectTimeout
    ) :
        m_metric{std::nullopt, std::move(label)},
        m_recordCallback(std::move(recordCallback)),
        m_collectRate(collectRate),
        m_collectTimeout(collectTimeout)
    {
    }

    virtual ~IMetricCollector() = default;

    IMetricCollector(const IMetricCollector& other) = delete;
    IMetricCollector& operator=(const IMetricCollector& other) = delete;
    IMetricCollector(IMetricCollector&& other) = delete;
    IMetricCollector& operator=(IMetricCollector&& other) = delete;

    NamedMetric takeLatest()
    {
        std::lock_guard lock(m_metricMutex);
        NamedMetric latest = m_metric;
        m_metric.metric = std::nullopt;
        return latest;
    }

protected:
    void start()
    {
        if (m_running || m_thread.joinable())
        {
            return;
        }

        m_running = true;
        m_thread = std::thread(&IMetricCollector::run, this);
    }

    void stop()
    {
        m_running = false;

        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

private:
    virtual std::optional<Metric> collect(std::chrono::milliseconds timeout) noexcept = 0;

    void recordMetric(const NamedMetric& metric)
    {
        if (m_recordCallback && metric.metric)
        {
            m_recordCallback(metric);
        }
    }

    void run()
    {
        while (m_running)
        {
            auto metric = collect(m_collectTimeout);
            std::unique_lock lock(m_metricMutex);
            m_metric.metric = std::move(metric);
            NamedMetric recordCpy = m_metric;
            lock.unlock();
            recordMetric(recordCpy);
            std::this_thread::sleep_for(m_collectRate);
        }
    }

    std::mutex m_metricMutex;
    NamedMetric m_metric;
    std::function<void(NamedMetric)> m_recordCallback;

    std::chrono::milliseconds m_collectRate;
    std::chrono::milliseconds m_collectTimeout;

    std::atomic<bool> m_running{false};
    std::thread m_thread;
};
