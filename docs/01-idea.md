Nome progetto
SentinelBoard — Smart Monitoring Dashboard

Problema che risolve
SentinelBoard nasce per risolvere un problema semplice ma concreto: monitorare in tempo reale alcuni parametri ambientali e mostrarli in modo chiaro su PC. In molti contesti scolastici, domestici o di laboratorio, i sensori raccolgono dati ma spesso questi restano grezzi, difficili da leggere o non organizzati in una dashboard unica. Il progetto vuole quindi trasformare dati provenienti da Arduino in una telemetria leggibile, aggiornata in tempo reale e utile per prendere decisioni immediate.

Perché è utile
Il sistema permette di visualizzare temperatura, umidità e luminosità in una sola interfaccia. Questo rende più facile controllare lo stato di un ambiente, rilevare anomalie e registrare uno storico esportabile in CSV. Il progetto è utile anche dal punto di vista didattico perché unisce acquisizione dati, comunicazione seriale, visualizzazione grafica e organizzazione software modulare.

Perché è coerente con TPSIT
SentinelBoard è coerente con TPSIT perché integra hardware e software in un sistema completo: Arduino acquisisce i dati, il PC li riceve via seriale e una dashboard sviluppata in Qt Creator li elabora e visualizza. Il project work richiede infatti Arduino, Qt Creator con C++, comunicazione seriale tra Arduino e PC, Ubuntu in macchina virtuale e codice organizzato su GitHub in due repository separati.

Perché è fattibile
Il progetto è fattibile perché usa sensori semplici e diffusi, una comunicazione seriale testuale facile da testare e una dashboard sviluppabile anche senza Arduino fisico grazie a una modalità simulata. Inoltre il docente richiede esplicitamente che il progetto sia fattibile nei tempi, nei costi e nelle capacità del gruppo, e SentinelBoard rispetta questo vincolo perché può essere costruito in blocchi progressivi: prima simulazione e dashboard, poi parsing, poi integrazione con l’hardware reale.