# 05 - Task List

## Obiettivo del documento
Questo documento serve a organizzare il lavoro del gruppo per il progetto **SentinelBoard**.

L’obiettivo è dividere chiaramente i compiti tra i tre membri del gruppo, definire scadenze intermedie e controllare in ogni momento quali blocchi del progetto sono stati completati e quali no. Dal momento che il project work è da svolgere in gruppi da 3 studenti e ha una scadenza fissata al 16 maggio, una pianificazione ordinata è fondamentale.

---

## Divisione ruoli del gruppo

| Vinci | Dashboard Qt, UI, grafici, parser, architettura software, integrazione lato PC |
| Minut - Piazza - Vinci | Documentazione, protocollo seriale, README, gestione repository, supporto organizzativo |
| Minut e Piazza | Parte Arduino, sensori, cablaggio, letture reali, integrazione hardware |

---

## Task per Vinci
Vinci si occupa della parte desktop del progetto.

### Compiti principali
- creare il progetto Qt base
- configurare i moduli Qt necessari
- progettare la `MainWindow`
- implementare `SerialManager`
- implementare `TelemetryParser`
- implementare `DataModel`
- collegare i grafici live
- implementare `AlarmManager`
- implementare `CsvLogger`
- testare la modalità simulazione
- integrare la dashboard con Arduino reale

---

## Task per Minut - Piazza - Vinci
Tutti e tre si occupano della documentazione e dell’organizzazione del progetto.

### Compiti principali
- scrivere `01-idea.md`
- scrivere `02-materiali.md`
- scrivere `03-protocollo-seriale.md`
- scrivere `04-architettura.md`
- scrivere e aggiornare i README dei due repository
- mantenere aggiornato questo file `05-task-list.md`
- documentare scelte tecniche e modifiche del gruppo
- raccogliere screenshot, immagini e materiali per la consegna finale
- supportare i test e la preparazione della presentazione

---

## Task per Minut - Piazza
Minut e Piazza si occupa della parte embedded e dei sensori.

### Compiti principali
- creare la struttura iniziale del repository Arduino
- definire i pin dei sensori
- scrivere lo sketch base con inizializzazione seriale
- leggere i sensori
- costruire il pacchetto seriale nel formato definito
- testare l’output con monitor seriale
- collegare Arduino alla dashboard Qt per i test reali

---

## Scadenze intermedie


| Entro 30 aprile | setup ambiente, repository, struttura documentazione |
| Entro 3 maggio | progetto Qt base compilabile + simulazione dati |
| Entro 6 maggio | protocollo seriale definitivo + parser + data model |
| Entro 9 maggio | dashboard con valori live e grafici |
| Entro 11 maggio | sketch Arduino con output seriale base |
| Entro 13 maggio | integrazione Qt + Arduino reale |
| Entro 15 maggio | test finali, correzione bug, documentazione finale |
| 16 maggio | consegna progetto |

---

## Blocchi principali da completare

### Blocco 1 - Setup iniziale
- [ ] Ubuntu pronto
- [ ] Qt Creator funzionante
- [ ] Git configurato
- [ ] due repository GitHub creati

### Blocco 2 - Documentazione
- [ ] idea progetto scritta
- [ ] lista materiali pronta
- [ ] protocollo seriale definito
- [ ] architettura software documentata
- [ ] README dei repository completati

### Blocco 3 - Sviluppo Qt
- [ ] finestra principale creata
- [ ] simulazione dati funzionante
- [ ] parser funzionante
- [ ] data model funzionante
- [ ] grafici live funzionanti
- [ ] allarmi funzionanti
- [ ] export CSV funzionante

### Blocco 4 - Sviluppo Arduino
- [ ] sketch base creato
- [ ] sensori collegati correttamente
- [ ] lettura sensori funzionante
- [ ] pacchetto seriale conforme al protocollo
- [ ] test seriale superato

### Blocco 5 - Integrazione finale
- [ ] connessione seriale reale da Qt
- [ ] ricezione dati corretta
- [ ] dashboard aggiornata con dati reali
- [ ] demo stabile
- [ ] materiali finali pronti per la consegna

---

## Regole operative del gruppo
Per lavorare in modo ordinato, il gruppo segue queste regole:

- ogni modifica importante deve essere salvata e pushata su GitHub
- i commit devono essere chiari e frequenti
- i membri del gruppo devono coordinarsi prima di modificare gli stessi file
- ogni blocco completato deve essere segnato in questa task list
- i test devono essere fatti durante tutto lo sviluppo, non solo alla fine

---

## Obiettivo finale del task planning
Questo documento serve a mantenere il progetto sotto controllo fino alla consegna. Avere task chiari, scadenze intermedie e blocchi ben definiti aiuta il gruppo a non perdere tempo, a distribuire meglio il lavoro e a rispettare la scadenza del project work.