# Metrics Visualizer Application

## Project Objective

The objective of this project is to gain exposure to Typescript + Vue.js, while also reinforcing C++ design skills.

## Overview

The Metrics Visualizer application will allow 1 or more metric categories to be continuously collected and recorded on the backend. The frontend will then display a live trend chart visualization of the metrics, as well as be able to display static trend charts from pre-recorded time ranges. The backend design should be open to extension for future metrics, as long as they are numeric and change over time. CPU usage will be only the initial metric.

## Tech Stack

@startuml
component "Vue + Typescript Frontend" as Frontend
component "C++ Backend" as Backend
database "SQLite" as DB

Frontend <--> Backend : HTTP
Backend <--> DB : Data recording
@enduml


## Backend Classes

@startuml

class Metric {
    + ts: std::chrono::system_clock::time_point
    + measurement: double
}

interface IMetricCollector {
    + collect(): std::optional<Metric>
}

class CpuUsageCollector {
    + collect(): std::optional<Metric>
}

class MetricsService {
    + start(): void
    + stop(): void
    - m_webServer: httplib::Server
}

class MetricRecorder {
    + record(metric: Metric): bool
}

interface IMetricRepository {
    + insert(metric: Metric, label: std::string): bool
    + query(start, end, label: std::string): vector<Metric>
    + deleteOlderThan(timestamp, label: std::string): void
}

class SqliteMetricRepository {
}

class MetricsHttpServer {
}

IMetricCollector <|.. CpuUsageCollector
IMetricRepository <|.. SqliteMetricRepository

MetricsService *-- IMetricCollector
MetricsService *-- MetricRecorder
MetricsService *-- MetricsHttpServer
MetricsService *-- SqliteMetricRepository

MetricRecorder --> IMetricRepository : Writes
MetricsHttpServer --> IMetricRepository : Reads

@enduml
 