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
    qreal x = (now - m_startTime) / 1000.0;

    m_tempSeries->append(x,  sample.temperature);
    m_humSeries->append(x,   sample.humidity);
    m_lightSeries->append(x, sample.light);

    m_current = sample;
    m_history.append(sample);

    emit currentSampleChanged(m_current);
    emit historyChanged();
}

TelemetrySample DataModel::currentSample() const
{
    return m_current;
}

QVector<TelemetrySample> DataModel::history() const
{
    return m_history;
}
