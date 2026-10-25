# TODO

## nx_println: Meldung wird zerlegt

`nx_println(str)` ruft `processCommand("response " + str)` auf. Dadurch läuft die
Meldung durch `split()`:

- Sie wird an Leerzeichen zerlegt, ein Item, das auf `response` reagiert, bekommt in
  `args[1]` nur das erste Wort.
- Anführungszeichen (`"`) und Escapes (`\"`, `\\`) in der Meldung werden als
  Quoting ausgewertet und verschwinden bzw. verändern den Text.

Lösung: `nx_println` sollte die Meldung nicht parsen, sondern direkt als ein Argument
weitergeben, z.B. über `processResponse(str)` bzw. ein Array
`{"response", str, ...}` an `items_cmd()`.
