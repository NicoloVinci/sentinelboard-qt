#pragma once

#include <QMainWindow>
#include <QString>
#include "telemetrysample.h"
#include "datamodel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class SerialManager;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartSystemClicked();
    void onStopSystemClicked();
    void handleSerialLine(const QString& line);
    void refreshSerialPorts();
    void updateDashboard(const TelemetrySample& sample);
    void resetDashboard();

private:
    Ui::MainWindow *ui;
    SerialManager *m_serialManager;
    DataModel *m_dataModel;

    bool m_waitingStartAck = false;
    bool m_waitingStopAck = false;
};
