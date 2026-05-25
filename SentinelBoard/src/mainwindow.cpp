#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "serialmanager.h"
#include "telemetryparser.h"

#include <QPushButton>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QVBoxLayout>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_serialManager(new SerialManager(this))
    , m_dataModel(new DataModel(this))
{
    ui->setupUi(this);

    connect(ui->btnStartSystem,   &QPushButton::clicked, this, &MainWindow::onStartSystemClicked);
    connect(ui->btnStopSystem,    &QPushButton::clicked, this, &MainWindow::onStopSystemClicked);
    connect(ui->btnRefreshPorts,  &QPushButton::clicked, this, &MainWindow::refreshSerialPorts);

    connect(m_serialManager, &SerialManager::lineReceived,
            this, &MainWindow::handleSerialLine);

    connect(m_dataModel, &DataModel::currentSampleChanged,
            this, &MainWindow::updateDashboard);

    ui->stackedWidget->setCurrentWidget(ui->pageStart);
    refreshSerialPorts();
    resetDashboard();
    setupCharts();

    ui->labelStartupStatus->setText("Sistema spento");
    ui->comboPorts->addItem("SIM");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupCharts()
{
    auto makeChartView = [](QLineSeries* series, const QString& title,
                            const QString& yLabel, QColor color) -> QChartView*
    {
        series->setName(title);
        series->setColor(color);

        QChart* chart = new QChart();
        chart->addSeries(series);
        chart->setTitle(title);
        chart->legend()->hide();
        chart->setMargins(QMargins(4, 4, 4, 4));
        chart->setBackgroundRoundness(6);

        chart->createDefaultAxes();

        auto* axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
        auto* axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
        if (axisX) axisX->setTitleText("Tempo (s)");
        if (axisY) axisY->setTitleText(yLabel);

        QChartView* view = new QChartView(chart);
        view->setRenderHint(QPainter::Antialiasing);
        view->setMinimumHeight(160);
        return view;
    };

    m_tempChartView  = makeChartView(m_dataModel->tempSeries(),  "Temperatura", "°C",   QColor("#e74c3c"));
    m_humChartView   = makeChartView(m_dataModel->humSeries(),   "Umidità",     "%",    QColor("#3498db"));
    m_lightChartView = makeChartView(m_dataModel->lightSeries(), "Luce",        "lux",  QColor("#f39c12"));

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);
    layout->addWidget(m_tempChartView);
    layout->addWidget(m_humChartView);
    layout->addWidget(m_lightChartView);

    ui->chartsContainer->setLayout(layout);
}

void MainWindow::refreshSerialPorts()
{
    ui->comboPorts->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const auto& port : ports)
        ui->comboPorts->addItem(port.portName());
}

void MainWindow::resetDashboard()
{
    ui->labelTemp->setText("--.- °C");
    ui->labelHum->setText("--.- %");
    ui->labelLight->setText("---");
    ui->labelStatus->setText("OFF");
}

void MainWindow::onStartSystemClicked()
{
    QString portName = ui->comboPorts->currentText();

    if (portName == "SIM" || portName.isEmpty()) {
        // Modalità simulazione
        m_simulationMode = true;
        if (!m_simulation)
            m_simulation = new SimulationDataSource(this);

        connect(m_simulation, &SimulationDataSource::lineReceived,
                this, &MainWindow::handleSerialLine, Qt::UniqueConnection);
        connect(m_simulation, &SimulationDataSource::statusMessage,
                this, [this](const QString& msg){
                    ui->labelStartupStatus->setText(msg);
                });

        m_simulation->start();
        ui->stackedWidget->setCurrentWidget(ui->pageDashboard);
        return;
    }

    // Modalità reale (codice già esistente)
    m_simulationMode = false;
    ui->labelStartupStatus->setText("Connessione in corso...");

    if (!m_serialManager->openPort(portName, QSerialPort::Baud115200)) {
        ui->labelStartupStatus->setText("Errore apertura seriale");
        return;
    }

    m_waitingStartAck = true;
    m_waitingStopAck  = false;
    ui->labelStartupStatus->setText("Porta aperta, attendo Arduino...");

    QTimer::singleShot(2000, this, [this]() {
        ui->labelStartupStatus->setText("Invio comando LED_ON...");
        m_serialManager->sendCommand("LED_ON\n");
    });
}

void MainWindow::onStopSystemClicked()
{
    if (m_simulationMode && m_simulation) {
        m_simulation->stop();
        m_simulationMode = false;
        resetDashboard();
        ui->labelStartupStatus->setText("Sistema spento");
        ui->stackedWidget->setCurrentWidget(ui->pageStart);
        return;
    }

    if (!m_serialManager->isOpen()) return;

    m_waitingStopAck  = true;
    m_waitingStartAck = false;
    ui->labelStartupStatus->setText("Invio comando LED_OFF...");
    m_serialManager->sendCommand("LED_OFF\n");
}

void MainWindow::handleSerialLine(const QString& line)
{
    if (line == "ACK_LED_ON" && m_waitingStartAck) {
        m_waitingStartAck = false;
        ui->labelStartupStatus->setText("LED acceso, dashboard attiva");
        ui->stackedWidget->setCurrentWidget(ui->pageDashboard);
        return;
    }

    if (line == "ACK_LED_OFF" && m_waitingStopAck) {
        m_waitingStopAck = false;
        m_serialManager->closePort();
        resetDashboard();
        ui->labelStartupStatus->setText("Sistema spento");
        ui->stackedWidget->setCurrentWidget(ui->pageStart);
        return;
    }

    if (line.startsWith("$TEL;")) {
        TelemetrySample sample;
        QString error;
        if (TelemetryParser::parseLine(line, sample, error))
            m_dataModel->addSample(sample);
        return;
    }

    ui->labelStartupStatus->setText("Messaggio: " + line);
}

void MainWindow::updateDashboard(const TelemetrySample& s)
{
    ui->labelTemp->setText(QString::number(s.temperature, 'f', 1) + " °C");
    ui->labelTemp->setStyleSheet(s.temperature > 30.0 ? "color: red; font-weight: bold;" : "");

    ui->labelHum->setText(QString::number(s.humidity, 'f', 1) + " %");
    ui->labelHum->setStyleSheet(s.humidity > 70.0 ? "color: red; font-weight: bold;" : "");

    ui->labelLight->setText(QString::number(s.light));
    ui->labelLight->setStyleSheet("");

    ui->labelStatus->setText(s.status);
    ui->labelStatus->setStyleSheet(s.status == "ERR" ? "color: red; font-weight: bold;" : "color: green;");

    m_sampleCount++;  // <-- qui
    ui->labelSampleCount->setText(QString("Campioni ricevuti: %1").arg(m_sampleCount));

    auto refreshChart = [](QChartView* view) {
        if (!view) return;
        auto* chart = view->chart();

        auto seriesList = chart->series();
        if (seriesList.isEmpty()) return;
        auto* line = qobject_cast<QLineSeries*>(seriesList.first());
        if (!line || line->count() == 0) return;

        // Calcola min/max Y dai punti nella finestra visibile
        qreal lastX = line->at(line->count() - 1).x();
        qreal minX  = qMax(0.0, lastX - 60.0);

        qreal yMin =  1e9;
        qreal yMax = -1e9;
        for (const auto& pt : line->points()) {
            if (pt.x() >= minX) {
                yMin = qMin(yMin, pt.y());
                yMax = qMax(yMax, pt.y());
            }
        }

        // Margine del 10% sopra e sotto
        qreal margin = (yMax - yMin) * 0.1;
        if (margin < 0.5) margin = 0.5; // margine minimo se i dati sono piatti

        // Aggiorna asse X
        auto axesX = chart->axes(Qt::Horizontal);
        if (!axesX.isEmpty()) {
            auto* axisX = qobject_cast<QValueAxis*>(axesX.first());
            if (axisX) axisX->setRange(minX, lastX + 2.0);
        }

        // Aggiorna asse Y
        auto axesY = chart->axes(Qt::Vertical);
        if (!axesY.isEmpty()) {
            auto* axisY = qobject_cast<QValueAxis*>(axesY.first());
            if (axisY) axisY->setRange(yMin - margin, yMax + margin);
        }

        chart->update();
    };

    refreshChart(m_tempChartView);
    refreshChart(m_humChartView);
    refreshChart(m_lightChartView);
}
