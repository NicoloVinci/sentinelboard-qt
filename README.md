# SentinelBoard Qt Dashboard - Branch main per versione stabile

## Descrizione
SentinelBoard è una Smart Monitoring Dashboard sviluppata in C++ con Qt Creator.
L'applicazione riceve in tempo reale i dati da una scheda Arduino tramite comunicazione seriale
e visualizza temperatura, umidità, distanza, movimento e luminosità tramite:

- card numeriche
- grafici live
- indicatori di allarme
- storico esportabile in CSV

Il progetto fa parte del Project Work di TPSIT (4ª AII ITIS A. Rossi, Vicenza) e utilizza come
ambiente di sviluppo Ubuntu in macchina virtuale come richiesto dal docente.

---

## Obiettivo
- Monitorare parametri ambientali in tempo reale
- Visualizzare i dati in modo chiaro tramite una dashboard Qt
- Gestire soglie e allarmi (warning / critical)
- Salvare lo storico in file CSV per analisi successive

---

## Stack tecnologico
- Linguaggio: C++
- Framework: Qt 5
- IDE: Qt Creator
- Sistema operativo di sviluppo: Ubuntu (VirtualBox / UTM)
- Controllo versione: Git + GitHub
- Comunicazione: Seriale USB verso Arduino

---

## Dipendenze Qt
Moduli Qt utilizzati dal progetto:

- `Core`
- `Gui`
- `Widgets`
- `SerialPort`
- `Charts`

---

## Struttura del progetto

```text
sentinelboard-qt/
├── README.md
├── SentinelBoard.pro
├── src/
│   ├── main.cpp
│   ├── mainwindow.cpp
│   ├── serialmanager.cpp
│   ├── telemetryparser.cpp
│   ├── datamodel.cpp
│   ├── alarmmanager.cpp
│   └── csvlogger.cpp
├── include/
│   ├── mainwindow.h
│   ├── serialmanager.h
│   ├── telemetryparser.h
│   ├── datamodel.h
│   ├── alarmmanager.h
│   └── csvlogger.h
├── forms/
│   └── mainwindow.ui
├── resources/
│   └── icons.qrc (se usato)
└── docs/
    ├── protocollo-seriale.md
    └── architettura.md

## Come compilare
- Aprire Qt Creator
- Selezionare Open Project e scegliere  SentinelBoard.pro  (o  CMakeLists.txt )
- Selezionare un Kit Desktop valido (es. Desktop Qt 6.x GCC 64-bit)
- Cliccare su Configure Project (al primo avvio)
- Compilare con il comando Build
- Avviare l’app con il pulsante Run

## Moduli principali
- MainWindow
	Layout principale della dashboard
	Visualizzazione card, grafici, log eventi, stato connessione
- SerialManager
	Gestisce la porta seriale (apertura/chiusura)
	Legge le righe ricevute da Arduino
	Emette segnali con le stringhe complete del pacchetto
- TelemetryParser
	Riceve una riga di testo
	Valida la sintassi del pacchetto
	Estrae i campi e li converte in un oggetto  TelemetrySample 
- DataModel
	Mantiene lo stato corrente dell’ultima telemetria
	Memorizza lo storico degli ultimi N campioni
	Emette segnali quando i dati vengono aggiornati
- AlarmManager
	Applica soglie a temperatura, umidità, distanza, luce, movimento
	Calcola lo stato di allarme (OK / WARNING / CRITICAL)
	Espone lo stato per la UI (colori, icone, messaggi)
- CsvLogger
	Esporta lo storico dei campioni in un file CSV
	Gestisce intestazioni e formato dei campi

## Formato pacchetto seriale
$TEL;TIME=12345;TEMP=24.6;HUM=51.2;LIGHT=420;STATUS=OK