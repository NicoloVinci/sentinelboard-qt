#pragma once

#include <QObject>
#include <QVector>
#include "telemetrysample.h"

class DataModel : public QObject {
    Q_OBJECT

public:
    explicit DataModel(QObject* parent = nullptr);

    void addSample(const TelemetrySample& sample);
    TelemetrySample currentSample() const;
    QVector<TelemetrySample> history() const;

signals:
    void currentSampleChanged(const TelemetrySample& sample);
    void historyChanged();

private:
    TelemetrySample m_current;
    QVector<TelemetrySample> m_history;
};
