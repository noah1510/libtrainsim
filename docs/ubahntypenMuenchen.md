Dokumentation der verwendeteten Werte für Züge
========================

**Vorwort**
Die vorliegende Dokumentation beschreibt verwendeteten Werte für die Züge um die Funktionalität des Simulators zu gewährleisten.

**zuggebundene Werte**
Die zuggebundenen Werte beschreiben Fahrzeugeigenschaften des im Simulator verwendeteten Fahrzeugs. Im Rahmen der Weiterentwicklung des Simulators können noch weitere Züge hinzugefügt, und auch verschiedene Züge gefahren werden.

In der vorliegenden Ausarbeitung wurde der Simulator mit den Werten der Baureihe 423/433 getestet.
Es werden jedoch auch noch die Baureihen der Ubahn München implementiert.

**streckengebundene Werte**
Aktuell ist nur der Haftreibungskoeffizient genutzt, da auch nur dieser in der Physik der Aktuellen Version verwendet werden kann. Dieser Wert ist per default auf 0.02 gesetzt.
Diese Zahl kommt durch den in der Literatur nachzulesenen Werte für den Haftreibungskoeffizienten zwischen Stahl und Stahl.

**einzelne Werte der Baureihen**

Besonderheiten sind *kursiv* dargestellt

### Baureihe 423/433

Masse:                    119.4 t
Maximale Geschwindigkeit: 140 kmh^-1
Leistung:                 2350 kW

*(die Werte beziehen sich auf eine Singletraktion, bis zu 3fach Traktion kann gefahren werden)*

### Ubahn München Typ A

Masse:                    53.2 t
Maximale Geschwindigkeit: 80 kmh^-1
Leistung:                 720 kW

*(die Werte beziehen sich auf eine Singletraktion, im Regelbetrieb wird Typ A in 3fach Traktion gefahren)*

### Ubahn München Typ B

Masse:                    57.1 t
Maximale Geschwindigkeit: 80 kmh^-1
Leistung:                 872 kW

*(die Werte beziehen sich auf eine Singletraktion, im Regelbetrieb wird Typ A in 3fach Traktion gefahren)*

### Ubahn München Typ C(1)

Masse:                    164 t
Maximale Geschwindigkeit: 80 kmh^-1
Leistung:                 2400 kW

*(die Werte beziehen sich auf eine 6 teilige Singletraktion, in der Länge vergleichbar mit einer 3fach Traktion Typ A oder B)*

### Ubahn München Typ C2
Masse:                    182 t
Maximale Geschwindigkeit: 90 kmh^-1
Leistung:                 3360 kW

*(die Werte beziehen sich auf eine 6 teilige Singletraktion, in der Länge vergleichbar mit einer 3fach Traktion Typ A oder B, Höchstgeschwindigkeit liegt betriebtlich bei 80 kmh^-1)*


**Quellen**

https://web.archive.org/web/20060821212052/http://www.et423.de/techndaten.html (Baureihe 423/433)
https://www.mvg.de/ueber/das-unternehmen/fahrzeuge.html (Baureihen Münchener Ubahn)
