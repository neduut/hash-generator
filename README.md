**UŽDUOTIS**
Sukurt savo originalu hasho generatoriu, atlikt testus ir tobulint

**HASHO GENERAVIMO EIGA**

1. Paverciu stringa i bitus

2. Bitus padalinu i blokus

3. Sumaisau blokus (ne random tvarka)

4. Sumaisau elementus bloku viduje (ne random tvarka)

5. ? idedu salta (pries generuojant hasha ar po?)

6. Sukuriu savo 64bitu seeda ir su juo maisau pridedant bitus (ar gal koki originalu kita sugalvot?)


**TYRIMAS**

1. Įvedimas – bet kokio ilgio eilutė (string).
2. Rezultatas – visada vienodo dydžio (pvz., 256 bitai, arba 64 simboliai hex formatu).
3. Deterministiškumas – tas pats įvedimas = tas pats rezultatas.
4. Efektyvumas – turi veikti pakankamai greitai.
5. Atsparumas kolizijoms – neturi būti lengva (praktiškai labai sudėtinga) rasti du skirtingus įvedimus, kurie duotų tą patį hash’ą.
6. Lavinos efektas (angl. Avalanche effect) – pakeitus vieną simbolį, rezultatas pasikeičia iš esmės. 
