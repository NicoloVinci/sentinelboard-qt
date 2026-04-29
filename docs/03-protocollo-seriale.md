## Formato del pacchetto
Ogni campione viene inviato da Arduino come una singola riga di testo nel seguente formato:

$TEL;TIME=12345;TEMP=24.6;HUM=51.2;LIGHT=420;STATUS=OK

## Significato dei campi
TIME -> intero -> Timestamp del campione -> Es. 12345
TEMP -> decimale -> Temperatura in gradi Celsius -> Es. 24.6
HUM -> decimale -> Umidità relativa in percentuale -> Es. 51.2
LIGHT -> intero -> Valore grezzo di luminosità letto dalla LDR -> Es. 420
STATUS -> stringa -> Stato generale del sistema -> Es. OK

## Esempi validi e non validi

Validi:
$TEL;TIME=1000;TEMP=23.4;HUM=48.9;LIGHT=350;STATUS=OK
$TEL;TIME=2040;TEMP=29.1;HUM=71.0;LIGHT=120;STATUS=WARN
$TEL;TIME=3050;TEMP=21.8;HUM=55.0;LIGHT=540;STATUS=OK

Non validi:
Prefisso errato (non inizia con $TEL) -> $DATA;TIME=1000;TEMP=23.4;HUM=48.9;LIGHT=350;STATUS=OK
Campo mancante (manca HUM) -> $TEL;TIME=1000;TEMP=23.4;LIGHT=350;STATUS=OK
Separatore errato (, al posto di ;) -> $TEL,TIME=1000,TEMP=23.4,HUM=48.9,LIGHT=350,STATUS=OK
Valore non numerico (TEMP non convertibile in un numero) -> $TEL;TIME=1000;TEMP=abc;HUM=48.9;LIGHT=350;STATUS=OK

## Baud rate
115200 (sia nello sketch Arduino sia nella configurazione della porta seriale lato Qt)

## Terminazione con newline
Ogni pacchetto deve terminare con il carattere di newline:
\n
Questo permette al software Qt di leggere i dati riga per riga, semplificando il parsing e riducendo il rischio di leggere campioni incompleti.