# AC568 circuit notes

Working notes while tracing the schematic. (stage topologies, component 
values, bias points from LTspice, and what to model vs. approximate.)
> Tracing Fender drawing 047779 rev C, "Bassman-Amp AC 568", sheet 1 of 2.


## Tube complement
| Position   | Tube         | Role                               |
| ---------- | ------------ | ---------------------------------- |
| V1, V2, V3 | 7025 (12AX7) | Channel preamps + shared driver    |
| V4         | 12AT7        | Phase inverter                     |
| V5, V6     | 6L6GC        | Push-pull power, fixed bias        |
| Rectifier  | solid-state  | Silicon diodes (no rectifier tube) |
Transformers: TR1 power, TR2 output, TR3 speaker/impedance section.

## Channels

Each channel has a Volume, Treble and Bass (250k pots).

* Bass Instrument: *Deep* switch adds low-end shelf.
* Normal: *Bright* switch adds treble bleed across volume pot.

