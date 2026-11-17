#include "IMetricCollector.h"

#include <utility>

IMetricCollector::IMetricCollector(
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

IMetricCollector::~IMetricCollector() = default;

NamedMetric IMetricCollector::takeLatest()
{
    std::lock_guard lock(m_metricMutex);
    NamedMetric latest = m_metric;
    m_metric.metric = std::nullopt;
    return latest;
}

void IMetricCollector::start()
{
    if (m_running || m_thread.joinable())
    {
        return;
    }

    m_running = true;
    m_thread = std::thread(&IMetricCollector::run, this);
}

void IMetricCollector::stop()
{
    m_running = false;

    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

void IMetricCollector::recordMetric(const NamedMetric& metric)
{
    if (m_recordCallback && metric.metric)
    {
        m_recordCallback(metric);
    }
}

void IMetricCollector::run()
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
