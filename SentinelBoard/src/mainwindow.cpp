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
    ui->labelStartupStatus->setText("Connessione in corso...");

    QString portName = ui->comboPorts->currentText();
    if (portName.isEmpty()) {
        ui->labelStartupStatus->setText("Nessuna porta selezionata");
        return;
    }

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
    if (!m_serialManager->isOpen())
        return;

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
    ui->labelHum->setText(QString::number(s.humidity,    'f', 1) + " %");
    ui->labelLight->setText(QString::number(s.light));
    ui->labelStatus->setText(s.status);

    // Aggiorna gli assi X dei grafici per scorrere con il tempo
    auto updateAxis = [](QChartView* view) {
        if (!view) return;
        auto* chart = view->chart();
        auto axes = chart->axes(Qt::Horizontal);
        if (axes.isEmpty()) return;
        auto* axisX = qobject_cast<QValueAxis*>(axes.first());
        if (!axisX) return;

        auto series = chart->series();
        if (series.isEmpty()) return;
        auto* line = qobject_cast<QLineSeries*>(series.first());
        if (!line || line->count() == 0) return;

        qreal lastX = line->at(line->count() - 1).x();
        qreal minX  = qMax(0.0, lastX - 60.0); // finestra scorrevole di 60 secondi
        axisX->setRange(minX, lastX + 2.0);
    };

    updateAxis(m_tempChartView);
    updateAxis(m_humChartView);
    updateAxis(m_lightChartView);
}
