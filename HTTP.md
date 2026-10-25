# HTTP-Schnittstelle

Der Webserver läuft auf Port 80 und wird im Sketch mit `webserver_start()` gestartet
(`nxwebserver.h`). Ohne WLAN-Unterstützung (`NX_NOWIFI`) wird er nicht mitkompiliert.

Die eigentliche Steuerung läuft über Befehle: `/cmd` nimmt jeden Befehl aus
[CMD.md](CMD.md) entgegen.

## Befehle ausführen

### `GET /cmd?cmd=<befehl>`

Führt `<befehl>` wie über die serielle Schnittstelle aus (`processCommand`). Syntax und
alle Befehle: siehe [CMD.md](CMD.md). Der Befehl muss URL-kodiert sein.

```
GET /cmd?cmd=gpio%20init_out%205%20%22Lampe%20im%20Flur%22
GET /cmd?cmd=pin5%201
```

| Parameter | Beschreibung |
|---|---|
| `cmd` | Der Befehl. |
| `redirect` | Optional. Wenn gesetzt, wird nach dem Befehl auf die Startseite umgeleitet (für die Buttons der Webseite). |

Antwort (`text/plain`): `ok`, außer ein Item antwortet selbst über
`pages_response_async()`. Fehler wie `Unknown CMD` werden **nicht** per HTTP
zurückgegeben, `ok` heißt nur, dass der Befehl angenommen wurde.

## Items

| Endpunkt | Antwort | Beschreibung |
|---|---|---|
| `GET /items` | HTML | Alle Items mit Name, Aktiv-Status und `page()`. Ein Parameter mit dem Namen eines Items wird an dessen `page()` übergeben. |
| `GET /item?name=<item>` | HTML | Detailseite (`pageDetail()`) eines Items, sonst `Error: Item not found.` |
| `GET /val?name=<item>&value=<wert>` | Text | `val(<wert>)` eines Items, z.B. `/val?name=pin5&value=comment`. Leer, wenn das Item fehlt. |
| `GET /names` | JSON | Namen aller Items als Array. |

Welche Werte ein Item für `val` liefert, steht in [CMD.md](CMD.md) beim jeweiligen Item.

## Gerät

| Endpunkt | Antwort | Beschreibung |
|---|---|---|
| `GET /` | HTML | Startseite mit Links, Eingabefeld für Befehle und Buttons zum Anlegen von Pins. |
| `GET /generate_204` | HTML | Wie `/`, für Captive-Portal-Erkennung (Android). |
| `GET /info` | JSON | `name`, `mac`, `features`, `chip`, `success`. |
| `GET /name` | Text | Gerätename. |
| `POST /name` | JSON `{}` | Speichert den Gerätenamen (Wert des Formular-Parameters) im Flash, nur ESP32. Wirkt nach Neustart, wie `set_name`. |
| `GET /features` | JSON | Liste der Features, z.B. `["nxesp","nxmc"]`; Sketches können mit `ws_feature_add()` ergänzen. |
| `GET /status` | Text | Zurzeit nur `ok`. |
| `GET /uptime` | Text | Millisekunden seit dem Start. |
| `GET /time` | HTML | Aktuelle Uhrzeit (nur ESP32). |

## WLAN

| Endpunkt | Antwort | Beschreibung |
|---|---|---|
| `GET /mac` | Text | MAC-Adresse. |
| `GET /rssi` | Text | Signalstärke in dBm. |
| `GET /scan` | Text | WLAN-Scan (nur ESP8266). |
| `GET /ips` | Text | Am Access Point angemeldete Clients (nur ESP8266). |

## Script

Das Script wird beim Start mit `nx_script()` ausgeführt, ein Befehl pro Zeile
(siehe [CMD.md](CMD.md)).

| Endpunkt | Antwort | Beschreibung |
|---|---|---|
| `GET /script` | Text | Das gespeicherte Script (nur ESP32, sonst leer). |
| `POST /script` | JSON `{}` | Speichert das Script (Wert des Formular-Parameters) im Flash, nur ESP32. |

## Bekannte Probleme

- `POST /name` und `POST /script` speichern den Wert des *letzten* Parameters,
  der Parametername ist egal.
