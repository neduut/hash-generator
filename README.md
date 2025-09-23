**UŽDUOTIS**

Sukurt savo originalu hasho generatoriu, atlikt testus ir tobulint

**Vartotojo įvestis**
Gali pasirinkt ar nori is failo ir irasyti ranka
jei nieko neivest programa negeneruos hasho is tuscios vietos ir mes klaida
klaidoms aptikti naudojami try catch testai

**HASHO GENERAVIMO EIGA**
VERSIJA V0.1 
Tikslas (parasyt i README) - sukurt savo originalu hash generatoriu nežinant kaip veikia normalus hash generatoriai

1. Kiekvieną simbolį paverčiu ASCII kodu 
2. Padalinu skaičius į blokus
2.1 Blokų dydis fiksuotas - 4 skaičiai
2.2 Jei paskutinis blokas trumpesnis, papildau modulio 64 ziurint nuo pradzios
3. Apverčiu visų blokų eilę atbuline tvarka
4. Kiekvieno antro bloko elementus sukeičiu pagal formulę:
new_index = (index * 3 + 1) % block_size
5. Sujungiu blokus į vieną masyvą
6. Apverčiu visą masyvą
7. Padalinu masyvą per pusę ir sukeičiu dalis vietomis
8. Susikuriu savo seed 64-bitu dydzio A1b2C3d4E5f6G7h8I9j0K1l2M3n4O5p6
9. Cikliškai pridedu masyvo elementus prie seed simbolių:
naujas_seed_char = (ASCII(seed_symbol) + masyvo_element modulo 62) → naujas simbolis
Jei masyvas ilgesnis nei seed, cikliškai kartoju seed
10. Modulo 62 konvertuoju į simbolius 0-9, a-z, A-Z
11. Spausdinu galutinį hash


**TESTAI**

1. Įvedimas – bet kokio ilgio eilutė (string).
2. Rezultatas – visada vienodo dydžio (pvz., 256 bitai, arba 64 simboliai hex formatu).
3. Deterministiškumas – tas pats įvedimas = tas pats rezultatas.
4. Efektyvumas – turi veikti pakankamai greitai.
5. Atsparumas kolizijoms – neturi būti lengva (praktiškai labai sudėtinga) rasti du skirtingus įvedimus, kurie duotų tą patį hash’ą.
6. Lavinos efektas (angl. Avalanche effect) – pakeitus vieną simbolį, rezultatas pasikeičia iš esmės. 

