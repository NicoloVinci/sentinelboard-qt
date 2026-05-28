#include "mainwindow.h"
#include "forms/ui_mainwindow.h"

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
#include <QtCharts/QDateTimeAxis>
#include <QFileDialog>
#include <QTextStream>
#include <QDir>
#include <QApplication>
#include <QFile>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_serialManager(new SerialManager(this))
    , m_dataModel(new DataModel(this))
{
    ui->setupUi(this);

    // Carica QSS
    QFile styleFile(":/resources/style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        qApp->setStyleSheet(styleFile.readAll());
    } else {
        qDebug() << "QSS non trovato in:" << styleFile.fileName();
    }

    // Margini layout
    ui->centralLayout->setContentsMargins(0, 0, 0, 0);
    ui->pageStartLayout->setContentsMargins(20, 20, 20, 20);
    ui->pageDashboardLayout->setContentsMargins(8, 8, 8, 8);
    ui->pageAboutLayout->setContentsMargins(20, 20, 20, 20);

    // Indicatori
    m_indicatorTemp   = ui->indicatorTemp;
    m_indicatorHum    = ui->indicatorHum;
    m_indicatorLight  = ui->indicatorLight;
    m_indicatorStatus = ui->indicatorStatus;

    connect(ui->btnStartSystem, &QPushButton::clicked, this, &MainWindow::onStartSystemClicked);
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
    connect(ui->btnExportCsv, &QPushButton::clicked, this, &MainWindow::exportCsv);
    connect(ui->btnPauseResume, &QPushButton::clicked, this, &MainWindow::onPauseResumeClicked);
    setWindowTitle("SentinelBoard — Sistema spento");
    connect(ui->btnAbout,          &QPushButton::clicked, this, &MainWindow::onAboutClicked);
    connect(ui->btnBackFromAbout,  &QPushButton::clicked, this, &MainWindow::onBackFromAboutClicked);
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

        // Asse Y
        QValueAxis* axisY = new QValueAxis();
        axisY->setTitleText(yLabel);
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        // Asse X con orario reale
        QDateTimeAxis* axisX = new QDateTimeAxis();
        axisX->setFormat("HH:mm:ss");
        axisX->setTitleText("Orario");
        axisX->setTickCount(5);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

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
    m_indicatorTemp->setStyleSheet("background-color: green; border-radius: 8px;");
    m_indicatorHum->setStyleSheet("background-color: green; border-radius: 8px;");
    m_indicatorLight->setStyleSheet("background-color: green; border-radius: 8px;");
    m_indicatorStatus->setStyleSheet("background-color: green; border-radius: 8px;");

    ui->labelTemp->setText("--.- °C");
    ui->labelHum->setText("--.- %");
    ui->labelLight->setText("---");
    ui->labelStatus->setText("OFF");

    m_sampleCount = 0;
    ui->labelSampleCount->setText("Campioni ricevuti: 0");

    auto resetColor = [](QLabel* lbl) {
        if (lbl) lbl->setStyleSheet("background-color: green; border-radius: 8px;");
    };
    resetColor(m_indicatorTemp);
    resetColor(m_indicatorHum);
    resetColor(m_indicatorLight);
    resetColor(m_indicatorStatus);

    m_dataModel->clearHistory();

    ui->pageDashboard->setStyleSheet("background-color: none;");
}

void MainWindow::onStartSystemClicked()
{
    m_thresholdTemp = ui->spinThresholdTemp->value();
    m_thresholdHum  = ui->spinThresholdHum->value();

    QString portName = ui->comboPorts->currentText();

    if (portName == "SIM" || portName.isEmpty()) {
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
        setWindowTitle("SentinelBoard — Sessione attiva");  // <-- aggiungi
        ui->stackedWidget->setCurrentWidget(ui->pageDashboard);
        return;
    }

    // Modalità reale
    m_simulationMode = false;
    setWindowTitle("SentinelBoard — Connessione in corso...");
    ui->labelStartupStatus->setText("Connessione in corso...");

    if (!m_serialManager->openPort(portName, QSerialPort::Baud115200)) {
        ui->labelStartupStatus->setText("Errore apertura seriale");
        setWindowTitle("SentinelBoard — Sistema spento");
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
    setWindowTitle("SentinelBoard — Sistema spento");
    if (m_simulationMode && m_simulation) {
        m_simulation->stop();
        m_simulationMode = false;
        autoSaveCsv();
        resetDashboard();
        ui->labelStartupStatus->setText("Sistema spento — sessione salvata");
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
        setWindowTitle("SentinelBoard — Sessione attiva");
        ui->labelStartupStatus->setText("LED acceso, dashboard attiva");
        ui->stackedWidget->setCurrentWidget(ui->pageDashboard);
        return;
    }

    if (line == "ACK_LED_OFF" && m_waitingStopAck) {
        m_waitingStopAck = false;
        m_serialManager->closePort();
        autoSaveCsv();
        resetDashboard();
        setWindowTitle("SentinelBoard — Sistema spento");
        ui->labelStartupStatus->setText("Sistema spento — sessione salvata");
        ui->stackedWidget->setCurrentWidget(ui->pageStart);
        return;
    }

    if (line.startsWith("$TEL;")) {
        if (!m_paused) {
            TelemetrySample sample;
            QString error;
            if (TelemetryParser::parseLine(line, sample, error))
                m_dataModel->addSample(sample);
        }
        return;
    }

    ui->labelStartupStatus->setText("Messaggio: " + line);
}

void MainWindow::updateDashboard(const TelemetrySample& s)
{
    // Indicatore temperatura
    if (s.temperature > m_thresholdTemp)
        m_indicatorTemp->setStyleSheet("background-color: red; border-radius: 8px;");
    else if (s.temperature > m_thresholdTemp * 0.85)
        m_indicatorTemp->setStyleSheet("background-color: orange; border-radius: 8px;");
    else
        m_indicatorTemp->setStyleSheet("background-color: green; border-radius: 8px;");

    // Indicatore umidità
    if (s.humidity > m_thresholdHum)
        m_indicatorHum->setStyleSheet("background-color: red; border-radius: 8px;");
    else if (s.humidity > m_thresholdHum * 0.85)
        m_indicatorHum->setStyleSheet("background-color: orange; border-radius: 8px;");
    else
        m_indicatorHum->setStyleSheet("background-color: green; border-radius: 8px;");

    // Indicatore luce (soglia fissa: buio sotto 100, ottimale sopra)
    if (s.light < 100)
        m_indicatorLight->setStyleSheet("background-color: orange; border-radius: 8px;");
    else
        m_indicatorLight->setStyleSheet("background-color: green; border-radius: 8px;");

    m_indicatorStatus->setStyleSheet(
        s.status == "ERR" ? "background-color: red; border-radius: 8px;"
                          : "background-color: green; border-radius: 8px;");

    ui->labelTemp->setText(QString::number(s.temperature, 'f', 1) + " °C");
    ui->labelTemp->setStyleSheet(s.temperature > m_thresholdTemp ? "color: red; font-weight: bold;" : "");

    ui->labelHum->setText(QString::number(s.humidity, 'f', 1) + " %");
    ui->labelHum->setStyleSheet(s.humidity    > m_thresholdHum  ? "color: red; font-weight: bold;" : "");

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

        qreal lastX = line->at(line->count() - 1).x();
        qreal minX  = lastX - 60000.0; // 60 secondi in ms

        // Asse X
        auto axesX = chart->axes(Qt::Horizontal);
        if (!axesX.isEmpty()) {
            auto* axisX = qobject_cast<QDateTimeAxis*>(axesX.first());
            if (axisX) {
                axisX->setRange(
                    QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(minX)),
                    QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(lastX) + 2000)
                    );
            }
        }

        // Asse Y dinamico
        qreal yMin =  1e9, yMax = -1e9;
        for (const auto& pt : line->points()) {
            if (pt.x() >= minX) {
                yMin = qMin(yMin, pt.y());
                yMax = qMax(yMax, pt.y());
            }
        }
        qreal margin = qMax(0.5, (yMax - yMin) * 0.1);

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

    // Sfondo allarme
    bool allarme = (s.temperature > m_thresholdTemp) || (s.humidity > m_thresholdHum);
    ui->pageDashboard->setStyleSheet(
        allarme ? "background-color: #ffdddd;" : "background-color: none;");
}

void MainWindow::exportCsv()
{
    QString path = QFileDialog::getSaveFileName(
        this, "Esporta CSV", "telemetria.csv", "CSV (*.csv)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QTextStream out(&file);
    out << "Timestamp,Temperatura,Umidità,Luce,Status\n";

    for (const auto& s : m_dataModel->history()) {
        out << s.receivedAt.toString("yyyy-MM-dd HH:mm:ss") << ","
            << QString::number(s.temperature, 'f', 1) << ","
            << QString::number(s.humidity,    'f', 1) << ","
            << s.light << ","
            << s.status << "\n";
    }

    file.close();
    ui->labelStartupStatus->setText("CSV esportato: " + path);
}

void MainWindow::autoSaveCsv()
{
    if (m_dataModel->history().isEmpty()) return;

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
    QString path = QDir::homePath() + "/sentinelboard_" + timestamp + ".csv";

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QTextStream out(&file);
    out << "Timestamp,Temperatura,Umidità,Luce,Status\n";

    for (const auto& s : m_dataModel->history()) {
        out << s.receivedAt.toString("yyyy-MM-dd HH:mm:ss") << ","
            << QString::number(s.temperature, 'f', 1) << ","
            << QString::number(s.humidity,    'f', 1) << ","
            << s.light << ","
            << s.status << "\n";
    }

    file.close();
    ui->labelStartupStatus->setText("Sessione salvata: " + path);
}

void MainWindow::onPauseResumeClicked()
{
    m_paused = !m_paused;
    ui->btnPauseResume->setText(m_paused ? "Riprendi" : "Pausa");
}

void MainWindow::onAboutClicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageAbout);
}

void MainWindow::onBackFromAboutClicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageStart);
}

void MainWindow::updateIndicators(const TelemetrySample& s)
{
    auto setColor = [](QLabel* lbl, const QString& color) {
        if (lbl) lbl->setStyleSheet(
                QString("background-color: %1; border-radius: 8px;").arg(color));
    };

    setColor(m_indicatorTemp,
             s.temperature > m_thresholdTemp         ? "red" :
                 s.temperature > m_thresholdTemp * 0.85  ? "orange" : "green");

    setColor(m_indicatorHum,
             s.humidity > m_thresholdHum         ? "red" :
                 s.humidity > m_thresholdHum * 0.85  ? "orange" : "green");

    setColor(m_indicatorLight, s.light < 100 ? "orange" : "green");

    setColor(m_indicatorStatus, s.status == "ERR" ? "red" : "green");
}
