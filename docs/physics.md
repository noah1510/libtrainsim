Dokumentation der neuen Physik
========================

Die Physik berechnet das genaue Fahrverhalten eines Zuges
Dazu besteht diese aus verschiedenen Funktionen und Variablen, die alle nötig sind um eine im ersten Ansatz realistische Physiksimulation zu erhalten.

**Bestandteile der Physiksimulation**

* die get-Funktionen
* die Funktion setSpeedlevel
* die Calc-Funktionen
* die tick-Funktion

**Detailliertere Beschreibung der Bestandteile**

### get-Funktionen

Zu den sogenannten get-Funktionen zählen die Funktionen getVelocity, getLocation, getAcceleration, getTraction und getCurrPower. Alle diese Funktionen sind in ihrer Funktionsweise sehr ähnlich,
weshalb diese zusammengefasst wurden.
Alle diese Funktionen geben eine bestimmte Größe als Rückgabewert zurück. Zusätzlich dazu rufen diese Funktionen noch die Funktion "Tick" auf, sollte die Variable "autoTick" wahr sein.
Die zurückgegebende physikalische Größe lässt sich am Namen der Funktion ablesen.
So gibt als Beispiel die Funktion getVelocity die aktuelle Geschwindigkeit zurück.

### setSpeedlevel Funktion

Diese Funktion setzt den Wert der Variable speedlevel. Dazu benötigt diese Funktion die Variable slvl aus der Klasse input_axis. Das ist notwendig, da in dieser Klasse die Variable slvl durch eingaben des Spielers gesetzt wird.
slvl kann dabei Fließkommazahlen im Wert von -1.0 bis 1.0 annehmen.
Da slvl in erster Näherung die Implementation eines Fahrstufenschalters darstellt, wurde dieser Wertebereich gewählt um eine bedienung mit einem Fahrbremshebel einfacher zu machen.
Zur Differnzierung ist darzustellen, dass die Variable slvl den Zustand der Spielereingabe darstellt, und die Variable speedlevel die Physik berechnet. Die genaue Berechnung der Physik ist im Abschnitt "tick-Funktion" nachzulesen

### Calc-Funktionen

Zu den sogenannten Calc-Funktionen zählen die Funktionen calcMaxForce und calcDrag. Die calcMaxForce Funktion berechnet die Maximal übertragbare Kraft zwischen einem Zug und den Schienen.
Dazu wird die Masse des Zuges, die Erdbeschleunigung und der Reibungskoeffizient zwischen Rad und Schiene bei stehendem Zug.
Die calcDrag Funktion ist noch nicht genauer beschrieben, diese soll jedoch in Zukunft alle Widerstandskräfte die bei einem fahrenden Zug auftreten können berechnen.

### tick-Funktion

Das Herzstück der Physiksimulation ist die sogenannte tick-Funktion. Diese Funktion berechnet die verschiedenen Größen der Physiksimulation für jeden Tick einzeln um eine flüssige Simulation zu erhalten. Die genaue Arbeitsweise wird nun im Folgenden näher beschrieben:
Zunächst werden alle relevanten größen initialisiert und wenn nötig berechnet. In einem zweiten schritt wird nun die aktuelle Leistung in abhängigkeit der stellung des Fahstufenschalters (speedlevel) bestimmt.
Nun werden einige Fallunterscheidungen für den Beschleunigungsvorgang getroffen. Da bei Stand des Zuges die aktuelle Geschwindigkeit null ist kann die Zugkraft nicht per F=P/v berechnet werden da sonst eine divison durch Null erfolgen würde.
Somit wird beim Starten des Zuges mit maximaler Zugkraft angefahren und bei geschwindigkeiten größer Null die Zugkraft über die Leistung berechnet.
Sollte die berechnete Zugkraft aus der Leistung jedoch größer sein als die maximal übertragbare Zugkraft so wird ebenfalls mit maximaler Zugkraft beschleunigt.
Beim Bremsen wird schlicht die Bremskraft direkt aus der stellung des Fahrbremshebels berechnet.
Um ein sofortiges Anfahren des Zuges nach Starten des Simulators zu verhindern ist der Fall speedlevel = 0 gesondert zu betrachten, da in diesem Fall keine veränderung der Zugkraft zu erfolgen hat. Bei stillstand des Zuges kann somit die currTraction auf 0N gesetzt werden, bei Fahrt wird die currTraction weiter über die Leistung berechnet.

Aus der Berechnung der Zugkraft kann nun über die Newtonschen Gesetze die Beschleunigung berechnet werden. Die Geschwindigkeit und der Ort werden nun durch numerisches Ableiten der Beschleunigung berechnet.
