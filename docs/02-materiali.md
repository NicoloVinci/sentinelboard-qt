• Arduino -> 5V (USB)
• 1 DHT22 -> 3.3-5.5V -> 1 pin digitale (DATA), 1 VCC e 1 GND -> Librerie: DHT sensor library, Adafruit Unified Sensor -> Legge temperatura e umidità in, rispettivamente, °C e %RH
• 4 LDR -> 5V (partitore su A0-A3) -> 1 pin analogico per LDR (A0-A3), 1 VCC e 1 GND via partitore -> legge tensione proporzionale alla luce (valore ADC 0-1023) in un valore proporzionale a lux
• 2 Mini pannelli fotovoltaici -> 2-6V a vuoto -> vanno su batteria -> leggono tensione/corrente generata in V e mA
• 1 Condensatore 25v 2200uF -> fino a 25V max, usato a 7-12V in questo sistema -> in serie/parallelo nel circuito (niente pin)
• 2 batterie 7.4V -> 7.4V nominali (8.4V a piena carica) -> passano tramite BMS
• Tp5100 -> ingresso 9-12V e uscita fino a 8.4V -> Coellgato ad un alimentatore e pacco batteria
• 2S 20A BMS -> 7.4V nominali per pacco e max 20A -> collegato tra batterie e carico
• 2 servo motori -> tipicamente 4.8-6V (meglio separata da 5V stabile) -> 1 pin PWM per servo (es. D9, D10), +5V, GND in comune -> libreria Servo.h -> legge la posizione angolare del comando (0-180°) -> °(gradi)
• breadboard -> fino a 5-12V
• Jumper
• resistenze -> tipicamente 5-12V -> in serie/parallelo a sensori/LED/partitori
• cavo USB -> 5V -> collegato a PC e Arduino
• eventuali moduli aggiuntivi