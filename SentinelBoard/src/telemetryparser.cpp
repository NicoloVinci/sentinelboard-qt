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

    QMap<QString, QString> values;
    QStringList parts = line.mid(5).split(';', Qt::SkipEmptyParts);

    for (const QString& part : parts) {
        int eq = part.indexOf('=');
        if (eq < 0) { error = "Campo malformato: " + part; return false; }
        values[part.left(eq).trimmed()] = part.mid(eq + 1).trimmed();
    }

    for (const QString& key : {"TIME", "TEMP", "HUM", "LIGHT", "STATUS"}) {
        if (!values.contains(key)) { error = "Campo mancante: " + key; return false; }
    }

    bool ok;
    sample.timeMs      = values["TIME"].toLongLong(&ok);  if (!ok) { error = "TIME non valido";  return false; }
    sample.temperature = values["TEMP"].toDouble(&ok);    if (!ok) { error = "TEMP non valido";  return false; }
    sample.humidity    = values["HUM"].toDouble(&ok);     if (!ok) { error = "HUM non valido";   return false; }
    sample.light       = values["LIGHT"].toInt(&ok);      if (!ok) { error = "LIGHT non valido"; return false; }
    sample.status      = values["STATUS"];
    sample.receivedAt  = QDateTime::currentDateTime();

    return true;
}
