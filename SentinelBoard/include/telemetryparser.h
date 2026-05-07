#pragma once

#include <QString>
#include "telemetrysample.h"

class TelemetryParser {
public:
    static bool parseLine(const QString& line, TelemetrySample& sample, QString& error);
};
