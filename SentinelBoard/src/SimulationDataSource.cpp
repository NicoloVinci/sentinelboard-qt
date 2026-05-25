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

    m_temp  += -0.3 + QRandomGenerator::global()->generateDouble() * 0.61;
    m_hum   += -0.8 + QRandomGenerator::global()->generateDouble() * 1.61;
    m_light += QRandomGenerator::global()->bounded(-20, 21);

    m_temp  = qBound(18.0, m_temp,  35.0);
    m_hum   = qBound(20.0, m_hum,   80.0);
    m_light = qBound(0,    m_light, 1023);

    QString line = QString("$TEL;TIME=%1;TEMP=%2;HUM=%3;LIGHT=%4;STATUS=%5")
                       .arg(m_timeMs)
                       .arg(m_temp,  0, 'f', 1)
                       .arg(m_hum,   0, 'f', 1)
                       .arg(m_light)
                       .arg("OK");

    emit lineReceived(line);
}
