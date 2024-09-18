#include <modern/lib.hpp>

#include <tuple>
#include <vector>
#include <algorithm>
#include <string>
#include <print>

#include <ArduinoJson.h>

struct DummyConfig {
    int         dummy_int;
    double      dummy_double;
    std::string dummy_string;
};

std::string save_config(const DummyConfig& config) {
    JsonDocument doc;

    doc["dummy_int"]    = config.dummy_int;
    doc["dummy_double"] = config.dummy_double;
    doc["dummy_string"] = config.dummy_string;

    std::string json;
    if (serializeJsonPretty(doc, json) == 0) {
        std::print("Failed to write to file");
    }

    return json;
}

DummyConfig load_config(const std::string& json) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);

    if (error) {
        std::print("Failed to read from file");
    }

    DummyConfig config;
    config.dummy_int    = doc["dummy_int"];
    config.dummy_double = doc["dummy_double"];
    config.dummy_string = doc["dummy_string"].as<std::string>();

    return config;
}

std::tuple<double, double> accumulate_vector(const std::vector<double>& values) {
    double mean = 0.0;
    double moment = 0.0;

    for (const auto& value : values) {
        mean += value;
        moment += value * value;
    }

    mean /= values.size();
    moment /= values.size();

    return {mean, moment};
}
