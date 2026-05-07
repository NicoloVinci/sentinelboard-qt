#include "datamodel.h"

DataModel::DataModel(QObject* parent)
    : QObject(parent)
{
}

void DataModel::addSample(const TelemetrySample& sample)
{
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
