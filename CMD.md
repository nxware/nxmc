# Befehle

Alle Befehle laufen über `processCommand(cmd)`. Sie kommen z.B. von der seriellen
Schnittstelle (Sketch), vom Webserver (`GET /cmd?cmd=...`, siehe [HTTP.md](HTTP.md)), aus einem Script
(`processScript`, gespeichert über `POST /script`, beim Start per `nx_script()`)
oder per `nxpull` von einer URL.

## Syntax

```
befehl arg1 arg2 "argument mit leerzeichen"
gpio init_out 5 "Lampe im Flur"
```

- Argumente werden an Leerzeichen getrennt, mehrere Leerzeichen zählen als eins.
- Text in `"..."` ist ein Argument; darin steht `\"` für `"` und `\\` für `\`.
- Das erste Wort (`args[0]`) wird in Kleinbuchstaben umgewandelt; Item-Namen werden
  ohne Beachtung der Groß-/Kleinschreibung verglichen (`NxWifi` = `nxwifi`).
- Alternativ (nur ESP32) als JSON-Array: `["befehl", "arg1", 2]`.
- In Scripts steht ein Befehl pro Zeile, leere Zeilen werden übersprungen.

Der Befehl wird der Reihe nach allen Items angeboten, bis eines ihn annimmt.
Nimmt ihn keines an, wird `response Error: Unknown CMD.<cmd>` ausgelöst.

## System (`NxCmds`, immer vorhanden)

| Befehl | Beschreibung |
|---|---|
| `settime <unix>` | Setzt die Uhrzeit (Unix-Zeitstempel in Sekunden). |
| `set_name <name>` | Speichert den Gerätenamen im Flash (nur ESP32, wirkt nach Neustart mit `nx_init(..., true)`). |
| `n <name> <befehl...>` | Führt `<befehl...>` nur aus, wenn `<name>` der eigene Gerätename ist; sonst wird der Befehl still ignoriert. |
| `ack <id> <befehl...>` | Sendet `ack <id>` seriell zurück und führt `<befehl...>` aus. |
| `nack <name> <id> <befehl...>` | Wie `ack`, aber nur wenn `<name>` der eigene Gerätename ist. |
| `rs485_init <pin>` | Legt den Richtungs-Pin für RS485 fest. |
| `rs485_start` | Setzt den RS485-Pin auf HIGH (Senden). |
| `rs485_end` | Wartet 100 ms und setzt den RS485-Pin auf LOW (Empfangen). |
| `print <item> <wert>` | Gibt `val(<wert>)` eines Items seriell aus. |
| `print_page <item>` | Gibt die Seite (`page()`) eines Items seriell aus. |
| `print_val <item> <wert> <ms>` | Legt ein Item an, das `val(<wert>)` alle `<ms>` Millisekunden seriell ausgibt (`?system=..&item=..&name=..&val=..`). |
| `alias <name> <ziel>` | Legt einen Alias an: Befehle und `val()` an `<name>` gehen an das Item `<ziel>`. |

## GPIO

| Befehl | Beschreibung |
|---|---|
| `gpio init_out <pin> ["beschreibung"]` | Legt einen Ausgangs-Pin `pin<pin>` an, optional mit Beschreibung (wird auf der Webseite angezeigt). |
| `gpio write <pin> <wert>` | Kurzform für `pin<pin> <wert>`. |
| `hotpin <pin>` | Legt einen HotPin `pin<pin>` an (Ausgang, der nach einem Trigger automatisch wieder ausgeht). |

### Pin (`pin<nr>`)

| Befehl | Beschreibung |
|---|---|
| `pin<nr> 1` / `pin<nr> 0` | Ausgang ein/aus (beendet auch `blink`). |
| `pin<nr> blink [ms]` | Blinkt, Umschalten alle `ms` (Standard 500 ms, `NX_BLINK_INTERVAL_MS`; ohne Angabe gilt das zuletzt gesetzte Intervall). |
| `pin<nr> pulse` | Einmal 1 s HIGH, dann LOW. |
| `pin<nr> tone` | Einmal 1 s Ton mit 440 Hz. |
| `pin<nr> pwm <wert>` | PWM-Ausgabe mit `<wert>` (`analogWrite`). |

Werte (`val`): `mode`, `value`, `pin`, `comment`, `blink_interval`.

In den Modi `out` und `pwm` wird nur bei einer Änderung von `value` geschrieben
(`analogWrite` in jedem Loop stört auf dem ESP32 das WLAN).

### HotPin (`pin<nr>`)

| Befehl | Beschreibung |
|---|---|
| `pin<nr> trigger` | Setzt den Pin auf HIGH; nach `threshold` ms (Standard 500) geht er wieder auf LOW. |

Werte (`val`): `mode`, `value`.

### AnalogPin (`pin<nr>`)

| Befehl | Beschreibung |
|---|---|
| `pin<nr> read` | (ohne Wirkung, der Wert wird ohnehin regelmäßig gelesen) |
| `pin<nr> print` | Gibt den Wert ab jetzt bei jedem Lesen seriell aus. |
| `pin<nr> interval <ms>` | Leseintervall setzen (Standard 100 ms, `NX_ANALOG_INTERVAL_MS`). |

Werte (`val`): `value`, `interval`.

Gelesen wird nur alle `interval` ms, `analogRead` in jedem Loop stört auf dem ESP32 das WLAN.
AnalogPins werden nur aus dem Sketch angelegt (`add_item(new AnalogPin(nr))` bzw. `new AnalogPin(nr, intervalMs)`).

## Alle Items

| Befehl | Beschreibung |
|---|---|
| `<item> activate` | Aktiviert das Item (`loopActive()` läuft). |
| `<item> deactivate` | Deaktiviert das Item. |

Gilt für Items, deren `cmd()` an `Item::cmd()` weitergibt (z.B. `udp`, `print_val`-Items, WifiConnect).

## WLAN (`NxWifi`, nach `wifi_commands()` bzw. `wlanConnect()`)

| Befehl | Beschreibung |
|---|---|
| `wifi off` | Schaltet WLAN aus. |
| `wifi connect <ssid> <passwort>` | Verbindet mit einem WLAN. |
| `wifi reconnect` | Verbindet erneut mit dem zuletzt konfigurierten WLAN. |
| `wifi ap <ssid> <passwort>` | Startet einen Access Point. |
| `wifi udp` | Legt das UDP-Item an (siehe unten). |
| `wificonnect <ssid> <passwort> <1\|0>` | Legt ein Item an, das die Verbindung hält und alle 30 s neu versucht; mit `1` wird ein laufender AP nach erfolgreicher Verbindung abgeschaltet. |
| `nxpull <url> <modus> <sekunden>` | Lädt alle `<sekunden>` ein Script von `<url>` und führt es aus. Modus `wsleep`: WLAN nur dafür ein- und danach wieder ausschalten. |

Werte (`val` von `NxWifi`): `status`, `localIP`, `rssi`.

### NxPull (`nxpull`)

| Befehl | Beschreibung |
|---|---|
| `nxpull execute` | Lädt und führt das Script sofort aus. |

### UDP (`udp`)

Sendet periodisch `nxudp <name> <ip>` als Broadcast an Port 4242.

| Befehl | Beschreibung |
|---|---|
| `udp` | Periodisches Senden einschalten. |
| `udp_stop` | Periodisches Senden ausschalten. |
| `udp_send <text>` | Sendet `<text>` als Broadcast. |
| `udp_m <n>` | Sendet nur jeden `<n>`-ten Loop (Standard 128). |
| `response <text>` | Leitet Antworten als `response <name> <text>` per UDP weiter. |

## Display (`nxdisplay`, nach `display_start()`)

| Befehl | Beschreibung |
|---|---|
| `nxdisplay alert <text>` | Zeigt `<text>` für 10 Loops an. |
| `nxdisplay add <größe> <x> <y> <item> <wert>` | Zeigt `val(<wert>)` von `<item>` dauerhaft an. |
| `nxdisplay rotate <0-3>` | Dreht die Anzeige. |

## Bekannte Probleme

- `print_val` mit einem unbekannten Item meldet keinen Fehler, es wird einfach nichts
  ausgegeben.
