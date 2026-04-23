# SentinelBoard Qt Dashboard - Branch Main -> versione stabile

## Nome progetto
SentinelBoard — Smart Monitoring Dashboard

## Obiettivo
Applicazione desktop sviluppata con Qt Creator e C++ per il monitoraggio ambientale in tempo reale.
La dashboard riceve dati via seriale da Arduino, mostra valori numerici, grafici live, allarmi a soglia e consente l’esportazione dello storico in CSV.

## Stack tecnologico
- C++
- Qt Creator
- Qt Widgets
- Qt SerialPort
- Qt Charts
- Ubuntu (ambiente di sviluppo richiesto)
- GitHub

## Dipendenze Qt
Moduli Qt utilizzati:
- Core
- Gui
- Widgets
- SerialPort
- Charts

## Come compilare
### Con Qt Creator
1. Aprire il progetto in Qt Creator
2. Selezionare un kit Desktop Qt compatibile
3. Compilare con Build
4. Avviare con Run

### Requisiti
- Qt Creator installato
- Compilatore C++ configurato
- Moduli Qt SerialPort e Qt Charts disponibili

## Moduli principali
- `MainWindow` — gestisce layout generale e dashboard
- `SerialManager` — connessione seriale e ricezione dati da Arduino
- `TelemetryParser` — parsing del protocollo seriale testuale
- `DataModel` — stato corrente e storico campioni
- `AlarmManager` — gestione soglie e stati di allarme
- `CsvLogger` — esportazione dati in formato CSV

## Formato del pacchetto seriale
Ogni campione viene inviato da Arduino come una riga testuale terminata da newline:

```text
$TEL;TIME=12345;TEMP=24.6;HUM=51.2;DIST=38.4;MOTION=1;LIGHT=420;STATUS=OK