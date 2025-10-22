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
![1-2](kuvat/1-2.png)
![2-2](kuvat/2-2.png)

#### Tehtävä 3
Tavoite:
- Toteuta prosessien välinen jaettu muisti, joka sisältää labyrintin kokoisen sijaintikartan.
- Rotat (prosessit) merkitsevät oman sijaintinsa tähän karttaan.
- Kartta pysyy hallituissa olosuhteissa ajantasaisena.
- Kirjoitukseen käytetään eksplisiittistä eksklusiivisuutta.

Testattavat asiat:
- Prosessien välinen writer-synkronointi semaforin avulla.
![1-3](kuvat/1-3.png)
![5-3](kuvat/5-3.png)
![2-3](kuvat/2-3.png)
![3-3](kuvat/3-3.png)
![4-3](kuvat/4-3.png)

#### Tehtävä 4
Tavoite:
- Toteuta säikeiden välinen versio labyrintin kokoiselle sijaintikartalle.
- Kartta pysyy koko ajan ajantasaisena, kun rotat liikkuvat.
- Kirjoitukseen toteutetaan säikeiden välinen eksklusiivisuus.

Testattavat asiat:
- Säikeiden välinen eksklusiivinen pääsy.
![1-4](kuvat/1-4.png)
![2-4](kuvat/2-4.png)
![3-4](kuvat/3-4.png)
![4-4](kuvat/4-4.png)
![5-4](kuvat/5-4.png)

### SEURAAVA TASO (6p) 5-8

#### Tehtävä 5
Tavoite:
- Toteuta toimiva prosessien suspend-toiminnallisuus jäädytetyn labyrinttitilanteen talteenottamiseksi.
- Parent-prosessi jäädyttää kaikki lapsiprosessit määräajoin, tallentaa labyrintin sijaintikartan tiedostoon ja jatkaa prosessien suoritusta.
- Tallennus tehdään tekstimuotoon: jokaisen "valokuvan" jälkeen tyhjä rivi ja seuraava kartta sen alle.
- Toteutus hyödyntää prosessien hallintaa ja synkronointia (SIGSTOP, SIGCONT).

Testattavat asiat:
- Edistyneempi prosessien hallinta (pysäytys, jatkaminen, synkronointi).
- Kartan tallentuminen oikein ja loogisessa järjestyksessä.
- Prosessien jatkuminen samasta kohdasta pysäytyksen jälkeen.
![1-5](kuvat/1-5.png)
![2-5](kuvat/2-5.png)

#### Tehtävä 6
Tavoite:
- Toteuta säikeiden suspend-toiminnallisuus ilman signaaleja.
- Pääsäie toimii ohjaavana säikeenä kuten prosessiesimerkissä.
- Säikeiden pysäytys ja jatkaminen tehdään säikeiden omilla synkronointikeinoilla (mutex ja condition_variable).

Testattavat asiat:
- Edistyneempi säikeiden hallinta ja säikeiden välinen synkronointi.
- mutex ja condition_variable käyttö oikein (ei deadlockeja).
- Karttatilanteen oikea tallentuminen pysäytyksen yhteydessä.
![1-6](kuvat/1-6.png)
![2-6](kuvat/2-6.png)
![3-6](kuvat/3-6.png)
![4-6](kuvat/4-6.png)
![5-6](kuvat/5-6.png)
![6-6](kuvat/6-6.png)

#### Tehtävä 7
Tavoite:
- Toteuta toimiva FIFO rajattu rengaspuskuri (bounded buffer) prosessien välille.
- Puskuri toimii jaetussa muistissa, ja sitä käytetään esim. viestien välittämiseen parent-prosessin ja lapsiprosessien välillä (producer–consumer-malli).
- Toteutus varmistaa synkronoidun ja rajatun pääsyn puskuriin.

Testattavat asiat:
- Rajatun puskurin käyttö prosessien välillä.
- Tuottaja–kuluttaja-synkronoinnin toimivuus (ei kilpajuoksua tai puskurin ylivuotoa).
- Jaetun muistin ja semaforien oikea käyttö.
![1-7](kuvat/1-7.png)
![2-7](kuvat/2-7.png)
![3-7](kuvat/3-7.png)
![4-7](kuvat/4-7.png)

#### Tehtävä 8
Tavoite:
- Toteuta toimiva FIFO rajattu rengaspuskuri (bounded buffer) säikeiden välillä.
- Käytä säikeiden välisiä synkronointimekanismeja (mutex, condition_variable).
- Toteutus noudattaa tuottaja–kuluttaja-mallia säikeiden sisällä.

Testattavat asiat:
- Bounded buffer toteutuksen toiminta säikeiden välillä.
- Synkronoinnin toimivuus (mutex, condition_variable).
- Oikea viestinvälitys ilman kilpailutilanteita tai menetettyjä viestejä.
![1-8](kuvat/1-8.png)
![2-8](kuvat/2-8.png)
![3-8](kuvat/3-8.png)
![4-8](kuvat/4-8.png)