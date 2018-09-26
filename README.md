# WILSON
> Wonderfully Inefficient Lad and Sort Of a Nuisance :)

> He protec, but he also attac

#  Project
Breadboard computer, from scratch, of course!


# To Doot
```
registers  - ist goed denk ik
MEEM  - eerste test live
ALU
PC  - dan
IR  - ist evan me bezig
```



# Ideads

South bridge

krijgt een control signal hij wordt gecall'd door een van de twee commando's
EIL (External Interface Load) of 
EIS (External Interface Store)
Er wordt een adres overgestuurd totdat de control unit een signaal terugkrijgt dat het volledige adres is verzonden. Daarna wordt er een byte ingelezen of uitgeschreven. Zo kan er worden geinterfaced met een onbeperkt aantal adressen. Dit is het deel wat de computer kan verbinden met misc. IO zoals een terminal of mass-storage device

Recording programs on taep

Programma's op tape opnemen kan simpelweg gedaan worden door een unit te verbinden aan de S-bus, die vervolgens een kleine buffer heeft om de data te verzekeren. De data wordt in een 8>1 mux gevoerd en de output wordt in een transistor gestopt. Die transistor koppelt een extra resistor aan een 555-timercircuit, wat de toon verandert.

Reading programs on taep

Programma's lezen van tape wordt een heel stuk moeilijker. Dit zou gedaan kunnen worden met twee peak-pass filters die staan getuned op de juiste frequenties. Die twee kanalen worden in een opamp gevoerd die triggert vanaf een bepaald voltage, om ruis te filteren.
De tonen worden somehow gesynct met een clock die in een shift register gaat en de data van de inputs worden in de shift register gestoken. Zodra hij vol zit wordt het register geleegd naar het geheugen.





