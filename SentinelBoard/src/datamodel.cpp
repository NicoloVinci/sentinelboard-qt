#include "datamodel.h"

DataModel::DataModel(QObject* parent) : QObject(parent)
{
    m_tempSeries  = new QLineSeries(this);
    m_humSeries   = new QLineSeries(this);
    m_lightSeries = new QLineSeries(this);
}

void DataModel::addSample(const TelemetrySample& sample)
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (m_startTime == 0) m_startTime = now;

    // X = ms epoch (compatibile con QDateTimeAxis)
    m_tempSeries->append(now,  sample.temperature);
    m_humSeries->append(now,   sample.humidity);
    m_lightSeries->append(now, sample.light);

    m_current = sample;
    m_history.append(sample);

    emit currentSampleChanged(m_current);
    emit historyChanged();
}

void DataModel::clearHistory()
{
    m_tempSeries->clear();
    m_humSeries->clear();
    m_lightSeries->clear();
    m_startTime = 0;
    m_history.clear();
    m_current = TelemetrySample{};
}

TelemetrySample DataModel::currentSample() const
{
    return m_current;
}

QVector<TelemetrySample> DataModel::history() const
{
    return m_history;
}
