#include <httplib.h>

#include "IMetricCollector.h"

class MetricsService
{

private:
    httplib::Server m_webServer;
};