#include "../include/SimulationDataSource.h"
#include <QRandomGenerator>
#include <QString>

SimulationDataSource::SimulationDataSource(QObject* parent)
    : IDataSource(parent)
{
    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, this, &SimulationDataSource::generateSample);
}

void SimulationDataSource::start() {
    m_timeMs = 0;
    m_timer.start();
    emit statusMessage("Simulation mode avviata");
}

void SimulationDataSource::stop() {
    m_timer.stop();
    emit statusMessage("Simulation mode fermata");
}

void SimulationDataSource::generateSample() {
    m_timeMs += 1000;

    m_temp += QRandomGenerator::global()->bounded(-0.3, 0.31);
    m_hum += QRandomGenerator::global()->bounded(-0.8, 0.81);
    m_dist += QRandomGenerator::global()->bounded(-2.0, 2.01);
    m_light += QRandomGenerator::global()->bounded(-20, 21);

    m_temp = qBound(18.0, m_temp, 35.0);
    m_hum = qBound(20.0, m_hum, 80.0);
    m_dist = qBound(5.0, m_dist, 200.0);
    m_light = qBound(0, m_light, 1023);

    if (QRandomGenerator::global()->bounded(100) < 10)
        m_motion = 1;
    else
        m_motion = 0;

    QString status = "OK";
    if (m_temp > 30.0 || m_dist < 15.0 || m_motion == 1)
        status = "WARN";

    QString line = QString("$TEL;TIME=%1;TEMP=%2;HUM=%3;DIST=%4;MOTION=%5;LIGHT=%6;STATUS=%7")
                       .arg(m_timeMs)
                       .arg(m_temp, 0, 'f', 1)
                       .arg(m_hum, 0, 'f', 1)
                       .arg(m_dist, 0, 'f', 1)
                       .arg(m_motion)
                       .arg(m_light)
                       .arg(status);

    emit lineReceived(line);
}
