#pragma once
#include <QDateTime>
#include <QString>

struct TelemetrySample {
    qint64   timeMs       = 0;
    double   temperature  = 0.0;
    double   humidity     = 0.0;
    int      light        = 0;
    QString  status       = "ERR";
    QDateTime receivedAt;
};
