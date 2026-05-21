#pragma once

#include <QObject>
#include <QVector>
#include <QDateTime>
#include "telemetrysample.h"

#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>

class DataModel : public QObject {
    Q_OBJECT

public:
    explicit DataModel(QObject* parent = nullptr);

    void addSample(const TelemetrySample& sample);
    TelemetrySample currentSample() const;
    QVector<TelemetrySample> history() const;

    QLineSeries* tempSeries()  const { return m_tempSeries; }
    QLineSeries* humSeries()   const { return m_humSeries; }
    QLineSeries* lightSeries() const { return m_lightSeries; }

signals:
    void currentSampleChanged(const TelemetrySample& sample);
    void historyChanged();

private:
    TelemetrySample m_current;
    QVector<TelemetrySample> m_history;

    QLineSeries* m_tempSeries;
    QLineSeries* m_humSeries;
    QLineSeries* m_lightSeries;
    qint64 m_startTime = 0;
};
