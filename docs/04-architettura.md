## MainWindow

Ruolo
 MainWindow  è la finestra principale della dashboard.

Responsabilità
	•	mostra i valori attuali dei sensori
	•	aggiorna grafici e indicatori visivi
	•	visualizza lo stato della connessione seriale
	•	permette di avviare o fermare la simulazione
	•	consente l’esportazione dei dati in CSV

Cosa non deve fare
 MainWindow  non deve contenere direttamente tutta la logica del progetto. In particolare:
	•	non deve fare parsing della stringa seriale
	•	non deve gestire direttamente la logica degli allarmi
	•	non deve salvare i file CSV da sola
Il suo compito principale è mostrare i dati e reagire agli eventi generati dagli altri moduli.


## SerialManager

Ruolo
 SerialManager  gestisce la comunicazione seriale tra ESP32 e applicazione Qt.

Responsabilità
	•	rilevare le porte seriali disponibili
	•	aprire e chiudere la porta selezionata
	•	impostare il baud rate corretto
	•	leggere i dati in arrivo
	•	ricostruire le righe complete terminate da newline
	•	emettere un segnale quando arriva un nuovo pacchetto

Vantaggio
Separando questa parte in una classe dedicata, l’applicazione può gestire meglio sia la seriale reale sia una futura modalità simulata.


## TelemetryParser

Ruolo
 TelemetryParser  ha il compito di trasformare una riga di testo ricevuta via seriale in un oggetto dati strutturato.

Responsabilità
	•	verificare che il pacchetto inizi con  $TEL 
	•	separare correttamente i campi
	•	controllare la presenza di tutti i campi obbligatori
	•	convertire i valori da stringa a tipo numerico o booleano
	•	segnalare errori in caso di pacchetti non validi

Output
Il parser restituisce un campione strutturato, ad esempio un oggetto  TelemetrySample , che contiene tutti i valori letti dai sensori.


## DataModel

Ruolo
 DataModel  rappresenta il cuore logico dell’applicazione.

Responsabilità
	•	conservare l’ultimo campione valido ricevuto
	•	mantenere uno storico degli ultimi N campioni
	•	rendere i dati disponibili alla UI
	•	notificare gli aggiornamenti agli altri moduli tramite segnali Qt

Dati gestiti
	•	tempo del campione
	•	temperatura
	•	umidità
	•	luminosità
	•	stato generale

Vantaggio
In questo modo la dashboard non dipende direttamente dalla seriale, ma da un modello dati centrale più pulito e riutilizzabile.


## AlarmManager

Ruolo
 AlarmManager  gestisce la logica degli allarmi e delle soglie.

Responsabilità
	•	confrontare i valori ricevuti con soglie predefinite
	•	stabilire se lo stato è  OK ,  WARNING  o  CRITICAL 
	•	fornire alla UI colori, badge o messaggi di stato
	•	segnalare eventi anomali

Esempi di condizioni di allarme
	•	temperatura troppo alta
	•	umidità troppo bassa o troppo alta
	•	luce ambiente sotto soglia

Vantaggio
Separare la logica degli allarmi dalla UI rende il codice più chiaro e più facile da modificare.


## CsvLogger

Ruolo
 CsvLogger  si occupa dell’esportazione dei dati in formato CSV.

Responsabilità
	•	creare file CSV validi
	•	scrivere intestazioni e righe dati
	•	esportare lo storico acquisito
	•	mantenere separata la logica di salvataggio dal resto dell’applicazione

Utilità
Questa parte permette di conservare i campioni acquisiti e di analizzarli successivamente con Excel, LibreOffice Calc o strumenti simili.