#include "serialmanager.h"

SerialManager::SerialManager(QObject *parent)
    : QObject(parent)
{
    connect(&m_serial, &QSerialPort::readyRead,
            this, &SerialManager::onReadyRead);
}

bool SerialManager::openPort(const QString& portName, qint32 baudRate) {
    if (m_serial.isOpen())
        m_serial.close();

    m_serial.setPortName(portName);
    m_serial.setBaudRate(baudRate);
    m_serial.setDataBits(QSerialPort::Data8);
    m_serial.setParity(QSerialPort::NoParity);
    m_serial.setStopBits(QSerialPort::OneStop);
    m_serial.setFlowControl(QSerialPort::NoFlowControl);

    if (m_serial.open(QIODevice::ReadWrite)) {
        emit statusMessage("Porta seriale aperta");
        return true;
    }

    emit statusMessage("Errore apertura porta seriale");
    return false;
}

void SerialManager::closePort() {
    if (m_serial.isOpen())
        m_serial.close();

    emit statusMessage("Porta seriale chiusa");
}

bool SerialManager::isOpen() const {
    return m_serial.isOpen();
}

void SerialManager::sendCommand(const QString& command) {
    if (!m_serial.isOpen())
        return;

    m_serial.write(command.toUtf8());
    m_serial.flush();
}

void SerialManager::onReadyRead() {
    m_buffer.append(m_serial.readAll());

    while (m_buffer.contains('\n')) {
        int idx = m_buffer.indexOf('\n');
        QByteArray rawLine = m_buffer.left(idx);
        m_buffer.remove(0, idx + 1);

        QString line = QString::fromUtf8(rawLine).trimmed();
        if (!line.isEmpty())
            emit lineReceived(line);
    }
}
