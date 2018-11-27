# WILSON
> Wonderfully Inefficient Lad and Sort Of a Nuisance :)

> He protec, but he also attac

#  Project
Breadboard computer, from scratch, of course!


# To Doot
```
registers - Done
MEEM - Done
ALU - Lijkt me OK
PC - dan
IR - Done
```



# Ideads

South bridge

Krijgt een control signal en daarna twee bytes doorgestuurd die fungeren als 16-bit adres. 
De commandos zijn SBL (South Bridge Load) en SBS (South Bridge Store).
Na het volgende clocksignaal wordt de byte op de 16-bit bus en meteen op de 8-bit bus gegooid naar het A register. 
Dit is het deel wat de computer kan verbinden met misc. IO zoals een terminal of mass-storage device.

Recording programs on taep

Programma's op tape opnemen kan simpelweg gedaan worden door een device te verbinden aan de Southbridge samen met een 256 byte buffer of de seriële poort. De data wordt in een 8>1 mux gevoerd en de output wordt naar een transistor gestuurt. Die transistor koppelt een extra resistor aan een 555-timercircuit, wat de toon verandert.

Reading programs on taep

Programma's lezen van tape wordt een heel stuk moeilijker. Dit zou gedaan kunnen worden met twee peak-pass filters die staan getuned op de juiste frequenties. Die twee kanalen worden in een opamp gevoerd die triggert vanaf een bepaald voltage, om ruis te filteren.
De tonen worden somehow gesynct met een clock die in een shift register gaat en de data van de inputs worden in de shift register gestoken. Zodra hij vol zit wordt het register geleegd naar het geheugen.





