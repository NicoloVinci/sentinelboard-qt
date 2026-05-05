#pragma once

#include <QObject>
#include <QString>

class IDataSource : public QObject {
    Q_OBJECT
public:
    explicit IDataSource(QObject* parent = nullptr);
    virtual ~IDataSource() = default;

    virtual void start() = 0;
    virtual void stop() = 0;

signals:
    void lineReceived(const QString& line);
    void statusMessage(const QString& message);
};
