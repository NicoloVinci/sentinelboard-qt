#pragma once

#include "idatasource.h"
#include <QTimer>

class SimulationDataSource : public IDataSource {
    Q_OBJECT

public:
    explicit SimulationDataSource(QObject* parent = nullptr);

    void start() override;
    void stop() override;

private slots:
    void generateSample();

private:
    QTimer m_timer;
    qint64 m_timeMs = 0;
    double m_temp = 24.0;
    double m_hum = 50.0;
    double m_dist = 40.0;
    int m_motion = 0;
    int m_light = 450;
};
