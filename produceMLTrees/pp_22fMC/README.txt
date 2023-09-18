>>> path dove sono salvati i file a cui questo README.txt si riferisce: from_2023Jul13 <<<

I PV duplicati sono quelli che sono associati alla stessa collisione MC. Il workflow ha la process che gira per DF, quindi la collisione MC ha indice univoco dentro al DF. Siccome l'output è anch'esso organizzato per DF, allora quando io giro su una directory (== DF) e chiedo la collIDMC essa è univoca all'interno del DF stesso (ossia: non rischio di guardare PV con stessa collIDMC dove la MC collision è da un altro DF. Questo mischierebbe le cose e non avrebbe alcun senso).

Ora, il workflow prende, per ogni PV, la rispettiva MC collision e per questa vengono contati i vertici doppi. Quindi, quando poi creo il tree per il MC io mi aspetto che tutti i PV tali per cui la MC collision associata ha > 2 PV ricostruiti dovranno essere flaggati come "duplicate". Questo però non avviene se richiedo che questi PV abbiano lo stesso BC in comune con un altro PV. Ossia, a quanto pare i duplicati vengono assegnati a due BC diversi.
===> questo lo vedo chiaramente guardando il TTree in treeLHC23d1e_1file.root plottando fGlobalBcInRun richiedendo
     fRecoPVsPerMcColl > 1 && ((fGlobalBcInRun < 41193000000))
        ---> la seconda condizione in (()) è fondamentale perchè altrimenti quando plotti dal TTree non ti accorgi, perchè ho long e non interi!!!

>>> Sarebbe interessante controllare quanti BC di differenza ci sono!!

Allo stesso tempo, la cosa strana è che ci sono collisioni MC associate allo stesso BC, tali per cui dunque:
  - due PV condividono lo stesso BC
  - ma allo stesso tempo non sono duplicati, perchè appartenenti a collisioni MC diverse
Stando a questa indicazione, dunque, non si può dire che i PV duplicati sono individuabili a quelli presenti in BC con >2 PV ricostruiti!
===> questo lo vedo chiaramente quando faccio il cout delle varie entry del TTree (vedi trial_1file) e noto che ci sono casi in cui il PV condividono lo stesso BC (countGlobBcInRun=2) ma appartengono a 2 MC collision diverse (countCollIdMc=1)

>>> vediamo che cosa ci dice il classificatore!!
>>> vedi trial_1file

NB: fDPGCounterDf non funziona come vorrei! A quanto pare, il counter è risettato a 0 all'inizio di ogni cycle (evidentemente la funzione init viene chiamata più volte? Oppure è una feature del merging?)
  --> basta rifare il counter a mano quando preparo i TTree!
  --> Attento! Ricordati che il tree è riempito per collisione, quindi il counter del DF è lo stesso per tutte le collisioni in quel DF!