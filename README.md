# Metrics Visualizer Application

## Project Objective

The objective of this project is to gain exposure to Solid 2.0 + Vue.js, while also reinforcing C++ design skills.

## Overview

The Metrics Visualizer application will allow 1 or more metric categories to be continuously collected and recorded on the backend. The frontend will then display a live trend chart visualization of the metrics, as well as be able to display static trend charts from pre-recorded time ranges. The backend design should be open to extension for future metrics, as long as they are numeric and change over time. CPU usage will be only the initial metric.

## Tech Stack

@startuml
component "Solid 2.0 Frontend" as Frontend
component "C++ Backend" as Backend
database "SQLite" as DB

Frontend <--> Backend : HTTP
Backend <--> DB : Data recording
@enduml


## Backend Classes

@startuml

class Metric {
    + ts: std::chrono::system_clock::time_point
    + value: double
}

class NamedMetric {
    + metric: std::optional<Metric>
    + name: std::string
}

abstract class IMetricCollector {
    + takeLatest(): NamedMetric
    # start(): void
    # stop(): void
    - {abstract} collect(timeout: std::chrono::milliseconds): std::optional<Metric> noexcept
    - recordMetric(metric: const NamedMetric&): void
    - run(): void
}

class CpuUsageCollector {
    - collect(timeout: std::chrono::milliseconds): std::optional<Metric> noexcept
}

class MetricsService {
    + start(): void
    + stop(): void
    - m_server: MetricsHttpServer
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
    - m_webServer: httplib::Server
}

IMetricCollector <|-- CpuUsageCollector
IMetricCollector *-- NamedMetric
NamedMetric *-- Metric
IMetricRepository <|.. SqliteMetricRepository

MetricsService *-- IMetricCollector
MetricsService *-- MetricRecorder
MetricsService *-- MetricsHttpServer
MetricsService *-- SqliteMetricRepository

MetricRecorder --> IMetricRepository : Writes
MetricsHttpServer --> IMetricRepository : Reads

@enduml
