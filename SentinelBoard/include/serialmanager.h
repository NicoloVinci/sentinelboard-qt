#pragma once

#include <QObject>
#include <QSerialPort>

class SerialManager : public QObject {
    Q_OBJECT

public:
    explicit SerialManager(QObject *parent = nullptr);

    bool openPort(const QString& portName, qint32 baudRate = QSerialPort::Baud115200);
    void closePort();
    bool isOpen() const;
    void sendCommand(const QString& command);

signals:
    void lineReceived(const QString& line);
    void statusMessage(const QString& message);

private slots:
    void onReadyRead();

private:
    QSerialPort m_serial;
    QByteArray m_buffer;
};
