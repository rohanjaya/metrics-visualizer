#include "IMetricCollector.h"

#include <chrono>
#include <future>
#include <unordered_map>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

class TestCollector final : public IMetricCollector
{
public:
    TestCollector(std::function<void(NamedMetric&&)> recordCallback,
        std::string label,
        std::chrono::milliseconds collectRate,
        std::vector<double> recordings
    ) :
        IMetricCollector(std::move(recordCallback), std::move(label), collectRate),
        m_recordings(std::move(recordings))
    {
        start();
    }

    ~TestCollector()
    {
        stop();
    }
private:
    std::optional<Metric> collect() noexcept override
    {
        auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now()
        );

        if (m_recordings.size() == 0)
        {
            return std::nullopt;
        }

        Metric metric{now, m_recordings[currInd++]};
        currInd = currInd % m_recordings.size();
        return metric;
    }

    std::vector<double> m_recordings;
    int currInd = 0;
};

TEST_CASE("Collector executes callback")
{
    std::vector<double> recordings{10, -99.99, 22, 0.54, 6};
    std::unordered_map<std::string, std::vector<Metric>> capture;

    std::string testCollectorName = "TestCollector";

    std::promise<bool> p;
    std::future<bool> f = p.get_future();

    auto recordCb = [&](NamedMetric&& metric)
    {
        capture[metric.name].push_back(*metric.metric);

        if (capture[metric.name].size() == recordings.size())
        {
            p.set_value(true);
        }
    };

    bool res;

    {
        TestCollector t{recordCb, testCollectorName, std::chrono::milliseconds(100), recordings};

        res = f.get();
    }

    REQUIRE(res);

    for (size_t i = 0; i < recordings.size(); ++i)
    {
        REQUIRE(capture[testCollectorName][i].value == recordings[i]);
    }

}
