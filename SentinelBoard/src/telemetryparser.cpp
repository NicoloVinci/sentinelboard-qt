#include "telemetryparser.h"
#include <QStringList>
#include <QMap>
#include <QDateTime>

bool TelemetryParser::parseLine(const QString& line, TelemetrySample& sample, QString& error)
{
    if (!line.startsWith("$TEL;")) {
        error = "Prefisso non valido";
        return false;
    }

    QString payload = line.mid(5);
    QStringList parts = payload.split(';', Qt::SkipEmptyParts);

    QMap<QString, QString> values;
    for (const QString& part : parts) {
        QStringList kv = part.split('=');
        if (kv.size() != 2) {
            error = "Campo malformato";
            return false;
        }
        values[kv[0].trimmed()] = kv[1].trimmed();
    }

    QStringList required = {"TIME", "TEMP", "HUM", "DIST", "MOTION", "LIGHT", "STATUS"};
    for (const QString& key : required) {
        if (!values.contains(key)) {
            error = "Campo mancante: " + key;
            return false;
        }
    }

    bool ok = false;

    sample.timeMs = values["TIME"].toLongLong(&ok);
    if (!ok) { error = "TIME non valido"; return false; }

    sample.temperature = values["TEMP"].toDouble(&ok);
    if (!ok) { error = "TEMP non valido"; return false; }

    sample.humidity = values["HUM"].toDouble(&ok);
    if (!ok) { error = "HUM non valido"; return false; }

    sample.distance = values["DIST"].toDouble(&ok);
    if (!ok) { error = "DIST non valido"; return false; }

    int motionInt = values["MOTION"].toInt(&ok);
    if (!ok || (motionInt != 0 && motionInt != 1)) {
        error = "MOTION non valido";
        return false;
    }

    sample.motion = (motionInt == 1);

    sample.light = values["LIGHT"].toInt(&ok);
    if (!ok) { error = "LIGHT non valido"; return false; }

    sample.status = values["STATUS"];
    sample.receivedAt = QDateTime::currentDateTime();

    return true;
}
