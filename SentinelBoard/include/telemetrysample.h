#pragma once

#include <QString>
#include <QDateTime>

struct TelemetrySample {
    qint64 timeMs = 0;
    double temperature = 0.0;
    double humidity = 0.0;
    double distance = 0.0;
    bool motion = false;
    int light = 0;
    QString status;
    QDateTime receivedAt;
};
