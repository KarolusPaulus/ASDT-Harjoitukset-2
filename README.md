# ASDT-Harjoitukset-2
Toinen osa Advanced Software Development Techniques -kurssin tehtävistä

## Tehdyt Tehtävät
### PERUSASIAT (8p) 1-4

#### Tehtävä 1
Tavoite:
- Toteuta rinnakkainen ohjelma, jossa rotat (lapsiprosessit) liikkuvat itsenäisesti labyrintissa.
- Pääprosessi (parent) odottaa kaikkien rottien saavuttavan suuaukon.
- Kun kaikki ovat perillä, ohjelma poistuu.

Testattavat asiat:
- Useiden prosessien rinnakkainen käsittely.
- Yksinkertainen jaetun muistin käyttö prosessien välisessä tiedonvälityksessä.
![1-1](kuvat/1-1.png)
![2-1](kuvat/2-1.png)
![3-1](kuvat/3-1.png)
![4-1](kuvat/4-1.png)

#### Tehtävä 2
Tavoite:
- Toteuta ohjelma, jossa rotat (säikeet) liikkuvat itsenäisesti labyrintissa.
- Main-säie odottaa kaikkien säikeiden valmistumista (rottien pääsy suuaukolle).
- Kun kaikki ovat valmiit, ohjelma poistuu.

Testattavat asiat:
- Useiden säikeiden rinnakkainen käsittely samassa ohjelmassa.

#### Tehtävä 3
Tavoite:
- Toteuta prosessien välinen jaettu muisti, joka sisältää labyrintin kokoisen sijaintikartan.
- Rotat (prosessit) merkitsevät oman sijaintinsa tähän karttaan.
- Kartta pysyy hallituissa olosuhteissa ajantasaisena.
- Kirjoitukseen käytetään eksplisiittistä eksklusiivisuutta.

Testattavat asiat:
- Prosessien välinen writer-synkronointi semaforin avulla.

#### Tehtävä 4
Tavoite:
- Toteuta säikeiden välinen versio labyrintin kokoiselle sijaintikartalle.
- Kartta pysyy koko ajan ajantasaisena, kun rotat liikkuvat.
- Kirjoitukseen toteutetaan säikeiden välinen eksklusiivisuus.

Testattavat asiat:
- Säikeiden välinen eksklusiivinen pääsy.

### SEURAAVA TASO (6p) 5-8