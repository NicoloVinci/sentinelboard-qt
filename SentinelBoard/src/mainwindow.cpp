#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "serialmanager.h"
#include "telemetryparser.h"

#include <QPushButton>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_serialManager(new SerialManager(this))
    , m_dataModel(new DataModel(this))
{
    ui->setupUi(this);

    connect(ui->btnStartSystem, &QPushButton::clicked,
            this, &MainWindow::onStartSystemClicked);

    connect(ui->btnStopSystem, &QPushButton::clicked,
            this, &MainWindow::onStopSystemClicked);

    connect(ui->btnRefreshPorts, &QPushButton::clicked,
            this, &MainWindow::refreshSerialPorts);

    connect(m_serialManager, &SerialManager::lineReceived,
            this, &MainWindow::handleSerialLine);

    connect(m_dataModel, &DataModel::currentSampleChanged,
            this, &MainWindow::updateDashboard);

    ui->stackedWidget->setCurrentWidget(ui->pageStart);
    refreshSerialPorts();
    resetDashboard();
    ui->labelStartupStatus->setText("Sistema spento");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshSerialPorts() {
    ui->comboPorts->clear();

    const auto ports = QSerialPortInfo::availablePorts();
    for (const auto& port : ports) {
        ui->comboPorts->addItem(port.portName());
    }
}

void MainWindow::resetDashboard() {
    ui->labelTemp->setText("--.- °C");
    ui->labelHum->setText("--.- %");
    ui->labelLight->setText("---");
    ui->labelStatus->setText("OFF");
}

void MainWindow::onStartSystemClicked() {
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
    m_waitingStopAck = false;

    ui->labelStartupStatus->setText("Porta aperta, attendo Arduino...");

    QTimer::singleShot(2000, this, [this]() {
        ui->labelStartupStatus->setText("Invio comando START...");
        m_serialManager->sendCommand("START\n");
    });
}

void MainWindow::onStopSystemClicked() {
    if (!m_serialManager->isOpen())
        return;

    m_waitingStopAck = true;
    m_waitingStartAck = false;

    ui->labelStartupStatus->setText("Invio comando STOP...");
    m_serialManager->sendCommand("STOP\n");
}

void MainWindow::handleSerialLine(const QString& line) {
    if (line == "ACK_START" && m_waitingStartAck) {
        m_waitingStartAck = false;
        ui->labelStartupStatus->setText("Sistema acceso");
        ui->stackedWidget->setCurrentWidget(ui->pageDashboard);
        return;
    }

    if (line == "ACK_STOP" && m_waitingStopAck) {
        resetDashboard();
        ui->stackedWidget->setCurrentWidget(ui->pageStart);
    }

    if (line.startsWith("$TEL;")) {
        TelemetrySample sample;
        QString error;

        if (TelemetryParser::parseLine(line, sample, error)) {
            m_dataModel->addSample(sample);
        }
        return;
    }

    ui->labelStartupStatus->setText("Messaggio: " + line);
}

void MainWindow::updateDashboard(const TelemetrySample& s) {
    ui->labelTemp->setText(QString::number(s.temperature, 'f', 1) + " °C");
    ui->labelHum->setText(QString::number(s.humidity, 'f', 1) + " %");
    ui->labelLight->setText(QString::number(s.light));
    ui->labelStatus->setText(s.status);
}
