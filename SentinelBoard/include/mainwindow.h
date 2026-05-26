#pragma once

#include <QMainWindow>
#include <QString>
#include <QLabel>
#include <QtCharts/QChartView>
#include "telemetrysample.h"
#include "datamodel.h"
#include "SimulationDataSource.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
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
    void exportCsv();
    void onPauseResumeClicked();
    void autoSaveCsv();
    void onAboutClicked();
    void onBackFromAboutClicked();
    void updateIndicators(const TelemetrySample& s);

private:
    QLabel* m_indicatorTemp   = nullptr;
    QLabel* m_indicatorHum    = nullptr;
    QLabel* m_indicatorLight  = nullptr;
    QLabel* m_indicatorStatus = nullptr;

    void setupCharts();

    Ui::MainWindow *ui;
    SerialManager  *m_serialManager;
    DataModel      *m_dataModel;

    QChartView* m_tempChartView  = nullptr;
    QChartView* m_humChartView   = nullptr;
    QChartView* m_lightChartView = nullptr;

    bool m_waitingStartAck = false;
    bool m_waitingStopAck  = false;

    int m_sampleCount = 0;

    SimulationDataSource* m_simulation = nullptr;
    bool m_simulationMode = false;

    double m_thresholdTemp = 30.0;
    double m_thresholdHum  = 70.0;

    bool m_paused = false;
};
