# Podrobná uživatelská dokumentace

Tato dokumentace obsahuje podrobný návod ke všem programům, které obsahuje příloha práce. Stručnou ukázku obsahuje dokument `Uživatelská dokumentace.pdf`.

Všechny cesty uvedené v tomto souboru jsou relativní k cestě složky, která obsahuje PDF soubor této dokumentace.

## Potřebné programy

- Docker
- Překladač C++17
- Python 3.9

## Programátorská dokumentace

C++ kód nového indexu naleznete ve složce `src/index/chm`. Webovou stránku s programátorskou dokumentací nového indexu HNSW zobrazíte pomocí skriptu `docs/openDocs.py`. Spustíte jej pomocí následujícího příkazu.

*OS Windows*
```bash
py .\docs\openDocs.py
```

*OS Linux*
```
python docs/openDocs.py
```

Dokumentaci můžete také zobrazit otevřením souboru `docs/html/index.html` v internetovém prohlížeči.

## Virtuální prostředí

Pokud není uvedeno jinak, skripty uvnitř složky `src/scripts` vždy spouštějte pomocí vygenerovaného virtuální prostředí. Prostředí aktivujete pomocí aktivačního skriptu ve složce `.venv/Scripts`. Výběr skriptu závisí na použitém OS a interpretu.

| OS | Interpret | Cesta k aktivačnímu skriptu |
| :-- | :-- | :-- |
| Linux |  | ./.venv/Scripts/activate |
| Windows | Batch | .\\.venv\Scripts\activate.bat |
| Windows | Powershell | .\\.venv\Scripts\Activate.ps1 |

## Seznam skriptů

Následuje seznam skriptů ve složce `src/scripts`.

| Název skriptu        | Stručný popis skriptu                                        |
| -------------------- | ------------------------------------------------------------ |
| buildProject         | Vytvoří virtuální prostředí, nativní C++ řešení a jeho Python rozhraní. |
| clean                | Odstraní vygenerované soubory a vrátí projekt do původního stavu. |
| datasetGenerator     | Vygeneruje datové soubory pro debugování.                    |
| datasetToText        | Převede datový soubor do textového formátu.                  |
| formatCMakeTemplates | Vygeneruje CMakeLists.txt.                                   |
| generateTables       | Vygeneruje LaTeX tabulky podobné těm, které jsou v bakalářské práci. |
| latexTable           | Vygeneruje LaTeX tabulku na základě výsledků srovnání.       |
| **runBenchmarks**    | Spustí srovnání, vygeneruje a otevře webovou stránku s výsledky. |
| runRecallTable       | Postaví nový index a zobrazí tabulku závislosti přesnosti na parametru vyhledávání ef<sub>search</sub>. |
| SIMDCapability       | Zobrazí SIMD rozšíření instrukční sady procesoru, která jsou k dispozici. |

## Podrobný popis skriptů

U každého skriptu je uveden jeho účel, parametry a příklad spuštění. Pokud skript obsahuje alespoň jeden parametr, pak použitím parametru `--help` nebo `-h` zobrazíte nápovědu v anglickém jazyce.

### buildProject

Tento skript lze spustit bez virtuálního prostředí.

Vytvoří virtuální prostředí interpretu Python, stáhne potřebné softwarové balíčky, vygeneruje nativní C++ řešení pro knihovnu nového indexu, vytvoří rozhraní v jazyce Python pro nový index a otestuje funkčnost tohoto indexu spuštěním skriptu `runRecallTable`.

| Parametr, zkratka                              | Význam                                                       |
| ---------------------------------------------- | ------------------------------------------------------------ |
| &#x2011;&#x2011;clean, &#x2011;c               | Vrátí projekt do původního stavu před jeho opětovným sestavením. |
| &#x2011;&#x2011;cleanResults, &#x2011;r        | Pokud je `--clean` nastaven, odstraní naměřené výsledky.     |
| &#x2011;&#x2011;ignorePythonVersion, &#x2011;i | Umožňuje spustit skript s libovolnou verzí interpretu Python. Skript poté nemusí fungovat správně. |

Příklad spuštění:

```bash
py -3.9 buildProject.py --clean --cleanResults
```

### clean

Tento skript lze spustit bez virtuálního prostředí.

Odstraní datové soubory pro debugování, C++ nativní řešení a Python rozhraní. Pokud je spuštěn mimo virtuální prostředí, pak odstraní toto prostředí. Naměřené výsledky odstraněny nebudou, pokud o to uživatel nepožádá.

| Parametr, zkratka                  | Význam                                                       |
| ---------------------------------- | ------------------------------------------------------------ |
| &#x2011;&#x2011;results, &#x2011;r | Odstraní naměřené výsledky srovnání, vygenerované grafy a tabulky. |

Příklad spuštění:

```bash
py clean.py --results
```

### datasetGenerator

Vygeneruje datové soubory pro debugování uvedené v konfiguračním souboru `src/config/debugDatasets.json`. O konfiguraci tohoto skriptu se více dočtete v kapitole `Datové soubory pro debugování` níže v této dokumentaci.

Příklad spuštění:

```bash
py datasetGenerator.py
```

### datasetToText

Převede vybraný datový soubor ze složky `src/data` do textového formátu. Výstupní textový soubor zapíše pod jménem datového souboru do stejné složky.

| Parametr, zkratka        | Význam                                                       |
| ------------------------ | ------------------------------------------------------------ |
| &#x2011;&#x2011;name, -n | Název datového souboru bez přípony. Pokud není uveden, výchozím souborem je `angular-small`. |

Příklad spuštění:

```bash
py datasetToText --name euclidean-medium
```

### formatCMakeTemplates

Vygeneruje soubor `src/index/CMakeLists.txt` a doplní do něj správnou definici maker tak, aby došlo pouze ke kompilaci těch funkcí, pro které je k dispozici vhodné SIMD rozšíření instrukční sady procesoru.

Příklad spuštění:

```bash
py formatCMakeTemplates.py
```

### generateTables

Vygeneruje LaTeX tabulky podobné těm, které jsou v bakalářské práci, ale pouze v případě, že jsou pro ně dostupné naměřené výsledky. Tyto výsledky lze získat spuštěním následujících příkazů. Avšak tato měření mohou trvat více než 12 hodin.

```bash
py runBenchmarks.py -a ..\config\heuristic.yaml ..\config\naive.yaml -d lastfm-64-dot -r 5
py runBenchmarks.py -a ..\config\heuristic.yaml ..\config\prefetch.yaml -d glove-50-angular -r 5
py runBenchmarks.py -a ..\config\original.yaml ..\config\prefetch.yaml -d sift-128-euclidean -r 5
py generateTables.py
```

Vygenerované tabulky jsou dostupné ve složce `src/figures`.

### latexTable

Vygeneruje jednu LaTeX tabulku na základě výsledků srovnání implementací.

| Parametr, zkratka                     | Význam                                                       |
| ------------------------------------- | ------------------------------------------------------------ |
| &#x2011;&#x2011;algorithms, &#x2011;a | Vyžadován. Seznam implementací oddělený mezerami.            |
| &#x2011;&#x2011;dataset, &#x2011;d    | Vyžadován. Název datového souboru.                           |
| &#x2011;&#x2011;label, &#x2011;la     | Identifikátor tabulky.                                       |
| &#x2011;&#x2011;legend, &#x2011;le    | Názvy implementací v tabulce. Pokud není uveden, budou použity původní názvy. |
| &#x2011;&#x2011;output, &#x2011;o     | Vyžadován. Cesta k výstupnímu souboru.                       |
| &#x2011;&#x2011;percent, &#x2011;p    | Přidá do tabulky sloupec s procentuálním rozdílem časů stavby. |
| &#x2011;&#x2011;recompute, &#x2011;r  | Znovu vypočítá výkonnostní metriky z naměřených výsledků. Tato operace může trvat více než 10 minut. |

Příklad spuštění:

```bash
py latexTable.py -a new-prefetch original -d sift-128-euclidean -le "Nová impl." "Původní impl." -o ..\figures\table.tex -p
```

### runBenchmarks

*Před spuštěním se ujistěte, že je služba Docker zapnutá.*

Spustí srovnání implementací v jednom nebo více Docker kontejnerech, vypočítá výkonnostní metrika, vygeneruje webovou stránku s výsledky a otevře ji v nové kartě internetového prohlížeče. Kód vygenerované stránky lze poté najít ve složce `src/website` a můžete ji opětovně zobrazit otevřením souboru `index.html`.

| Parametr, zkratka                       | Význam                                                       |
| --------------------------------------- | ------------------------------------------------------------ |
| &#x2011;&#x2011;algoDefPaths, &#x2011;a | Vyžadován. Seznam cest ke konfiguračním souborům oddělených mezerami. O konfiguraci se více dočtete v kapitole `Konfigurace srovnání`. |
| &#x2011;&#x2011;datasets, &#x2011;d     | Vyžadován\*. Seznam datových souborů oddělených mezerami.    |
| &#x2011;&#x2011;datasetsPath, &#x2011;p | Vyžadován\*. Cesta k textovému souboru se seznamem datových souborů. |
| &#x2011;&#x2011;force, &#x2011;f        | Spustí již provedená měření znovu.&#x2011;                   |
| &#x2011;&#x2011;runs, &#x2011;r         | Počet opakování měření. Výchozí hodnota je 1.                |
| &#x2011;&#x2011;workers, &#x2011;w      | Počet paralelně spuštěných Docker kontejnerů. Výchozí hodnota je 1. |

Datové soubory využité ke srovnání nejsou ty samé, které jsou využívány k debugování. Jejich seznam najdete v kapitole `Testované datové soubory`.

\* Pouze jeden z parametrů označených hvězdičkou by měl být uveden.

Příklad spuštění:

```bash
py runBenchmarks.py -a ..\config\noBit.yaml -f -p ..\config\datasets.txt -r 5 -w 2
```

### runRecallTable

Postaví index nové implementace a vyhledá v něm nejbližší sousedy s různými hodnotami parametru vyhledávání ef<sub>search</sub>. Poté vypíše tabulku závislosti přesnosti vyhledávání na tomto parametru. Konfigurace tohoto skriptu se nachází v souboru `src/config/recallTable.json` a více se o ní dočtete v kapitole `Konfigurace programů recallTable`.

Příklad spuštění:

```bash
py runRecallTable.py
```

### SIMDCapability

Zobrazí SIMD rozšíření instrukční sady procesoru, která jsou k dispozici. Využívána ostatními skripty pro vygenerování správných maker v jazyce C++.

Příklad spuštění:

```bash
py SIMDCapability.py
```

## Nativní knihovna

C++ řešení vygenerujete pomocí skriptu `RUNME.py` nebo `src/scripts/buildProject.py`. Řešení bude vytvořeno ve složce `src/cmakeBuild`. V každém systému vypadají soubory řešení jinak. Např. při použití Windows s Visual Studiem je řešením `.sln` soubor a projekty jsou `.vcxproj` soubory. Pro spuštění projektů je doporučena konfigurace `Release`. Řešení obsahuje dva projekty.

- *datasetToText* - Vypíše textový popis datové kolekce do souboru. Slouží pro ověření konzistence mezi binárními a HDF5 soubory. Název datového souboru je prvním parametrem programu. Výchozí hodnotou je `angular-small`.
- *recallTable* - Postaví HNSW index a vypíše tabulku závislosti přesnosti na parametru vyhledávání ef<sub>search</sub>. Konfigurace programu se nachází v souboru `src/config/recallTable.json` a více se o ní dočtete v kapitole `Konfigurace programů recallTable`.

## Datové soubory pro debugování

Pro vygenerování jiných datových souborů pro debugování změňte konfiguraci v souboru `src/config/debugDatasets.json` a spusťte skript `src/scripts/datasetGenerator.py`. Konfigurace je JSON soubor s polem objektů, kde každý objekt popisuje jeden datový soubor.

```json
{
    "name": "angular-small",
    "angular": true,
    "dim": 25,
    "k": 10,
    "testCount": 200,
    "trainCount": 20000,
    "seed": 104
}
```

| Klíč       | Typ hodnoty | Význam                                                       |
| :--------- | :---------- | :----------------------------------------------------------- |
| name       | string      | Unikátní název souboru sloužící k identifikaci.              |
| angular    | boolean     | Pokud je nastaven na `true`, využívá soubor kosinusové podobnosti. Jinak využívá Eukleidovské vzdálenosti. |
| dim        | int         | Počet dimenzí prostoru.                                      |
| k          | int         | Počet hledaných nejbližších sousedů dotazovaného prvku.      |
| testCount  | int         | Počet dotazů.                                                |
| trainCount | int         | Počet prvků použitých k sestavení indexu.                    |
| seed       | int         | Nastavení generátoru náhodných čísel.                        |

## Konfigurace programů *recallTable*
Pro změnu datového souboru nebo nastavení indexu v programech `recallTable.cpp` a `recallTable.py` upravte soubor `src/config/recallTable.json`. Konfigurace je JSON soubor s jediným objektem.

```json
{
	"dataset": "angular-small",
	"efConstruction": 200,
	"efSearch": [10, 15, 20, 40, 80, 120, 200],
	"mMax": 16,
	"seed": 200,
	"SIMD": "best",
	"template": "prefetching"
}
```

| Klíč           | Typ hodnoty | Význam                                                       |
| :------------- | :---------- | :----------------------------------------------------------- |
| dataset        | string      | Identifikace datového souboru. Odpovídá klíči `name` v souboru `src/config/debugDatasets.json`. |
| efConstruction | int         | Počet uvažovaných sousedů při vytváření nových hran v indexu. |
| efSearch       | array       | Pole hodnot parametru vyhledávání ef<sub>search</sub>.       |
| mMax           | int         | Maximální povolený počet sousedů jednoho prvku v indexu na vrstvě vyšší než vrstva 0. |
| seed           | int         | Nastavení generátoru náhodných úrovní v indexu.              |
| SIMD           | string      | Upřednostňovaný typ SIMD instrukcí. Možnosti jsou `avx`, `avx512`, `best`, `null`, a `sse`.* |
| template       | string      | Šablona indexu. Možnosti jsou `Heuristic`, `Naive`, `NoBitArray` a `Prefetching`. |

\* Zvolením hodnoty `best` zvolíte nejmodernější dostupné SIMD rozšíření. Hodnotou `null` zakážete použití SIMD instrukcí.

## Šablony nové implementace

| Šablona     | Metoda výběru sousedů | Seznam navštívených vrcholů | Asynchronní přístup do paměti                                |
| ----------- | --------------------- | --------------------------- | ------------------------------------------------------------ |
| Heuristic   | Heuristika            | Bitové pole                 | Ne                                                           |
| Naive       | Naivní algoritmus     | Bitové pole                 | Ne                                                           |
| NoBitArray  | Heuristika            | Obyčejné pole               | Při výpočtu vzdáleností<br />Při načítání dat seznamu navštívených vrcholů |
| Prefetching | Heuristika            | Bitové pole                 | Při výpočtu vzdáleností                                      |

## Testované datové soubory

Pro srovnání implementací je možno využít následujících datových souborů.

| Název                                            | Dimenze | Počet prvků při stavbě | Dotazy | Metrika                 |
| ------------------------------------------------ | ------- | ---------------------- | ------ | ----------------------- |
| deep&#x2011;image&#x2011;96&#x2011;angular       | 96      | 9 990 000              | 10 000 | Kosinusová podobnost    |
| fashion&#x2011;mnist&#x2011;784&#x2011;euclidean | 784     | 60 000                 | 10 000 | Eukleidovská vzdálenost |
| gist&#x2011;960&#x2011;euclidean                 | 960     | 1 000 000              | 1 000  | Eukleidovská vzdálenost |
| glove&#x2011;25&#x2011;angular                   | 25      | 1 183 514              | 10 000 | Kosinusová podobnost    |
| glove&#x2011;50&#x2011;angular                   | 50      | 1 183 514              | 10 000 | Kosinusová podobnost    |
| glove&#x2011;100&#x2011;angular                  | 100     | 1 183 514              | 10 000 | Kosinusová podobnost    |
| glove&#x2011;200&#x2011;angular                  | 200     | 1 183 514              | 10 000 | Kosinusová podobnost    |
| lastfm&#x2011;64&#x2011;dot                      | 64      | 292 385                | 50 000 | Kosinusová podobnost    |
| mnist&#x2011;784&#x2011;euclidean                | 784     | 60 000                 | 10 000 | Eukleidovská vzdálenost |
| nytimes&#x2011;16&#x2011;angular                 | 16      | 290 000                | 10 000 | Kosinusová podobnost    |
| nytimes&#x2011;256&#x2011;angular                | 256     | 290 000                | 10 000 | Kosinusová podobnost    |
| random&#x2011;s&#x2011;100&#x2011;angular        | 100     | 100 000                | 10 000 | Kosinusová podobnost    |
| random&#x2011;s&#x2011;100&#x2011;euclidean      | 100     | 100 000                | 10 000 | Eukleidovská vzdálenost |
| random&#x2011;xs&#x2011;20&#x2011;angular        | 20      | 10 000                 | 10 000 | Kosinusová podobnost    |
| random&#x2011;xs&#x2011;20&#x2011;euclidean      | 20      | 10 000                 | 10 000 | Eukleidovská vzdálenost |
| sift&#x2011;128&#x2011;euclidean                 | 128     | 1 000 000              | 10 000 | Eukleidovská vzdálenost |

Pro spuštění srovnání nad více soubory lze využít textového formátu, kde každý řádek reprezentuje jeden datový soubor. Řádky, které začínají znakem `#` jsou ignorovány. Příklad:

```bash
# deep-image-96-angular
glove-25-angular
sift-128-euclidean
```

## Konfigurace srovnání

Výběr implementací ke srovnání a jejich parametrů zprostředkovávají konfigurační soubory ve formátu YAML. Příklad takového souboru je `src/config/algos.yaml`. Ve složce `src/config` se nacházejí předem vytvořené konfigurace. Jejich význam popisuje následující tabulka.

| Název souboru          | Význam                                                       |
| ---------------------- | ------------------------------------------------------------ |
| 100k&#x2011;large.yaml | 12 stejných konfigurací pro novou a původní implementaci. Vhodné pro malé datové soubory. |
| 100k&#x2011;small.yaml | 4 stejné konfigurace pro novou a původní implementaci. Vhodné pro malé datové soubory. |
| algos.yaml             | 1 konfigurace pro každou šablonu nové a původní implementace. |
| heuristic.yaml         | 12 konfigurací pro šablonu *Heuristic* nové implementace.\*  |
| naive.yaml             | 12 konfigurací pro šablonu *Naive* nové implementace.\*      |
| noBit.yaml             | 12 konfigurací pro šablonu *NoBitArray* nové implementace.\* |
| original.yaml          | 12 konfigurací pro původní implementaci.                     |
| prefetch.yaml          | 12 konfigurací pro šablonu *Prefetching* nové implementace.\* |

\* Popis šablon obsahuje kapitola `Šablony nové implementace`.

Následuje příklad konfigurace. Komentáře označené znakem `#` popisují jednotlivé klíče.

```yaml
algos: # Povinný klíč.
  # Povinné pole názvů porovnávaných implementací.
  - original
  - new-prefetch
build: # Povinný klíč.
  # Povinné pole objektů konfigurace stavby.
  # Pro každou konfiguraci musí být uvedeny hodnoty parametrů efConstruction a mMax.
  - efConstruction: 50
    mMax: 4
  - efConstruction: 100
    mMax: 8
efSearch: [10, 12, 14, 16, 18, 20, 25, 30, 40, 80] # Povinné pole hodnot parametru vyhledávání efSearch.
```

Definované implementace popisuje následující tabulka.

| Název implementace        | Druh implementace | Šablona     | SIMD rozšíření         |
| ------------------------- | ----------------- | ----------- | ---------------------- |
| original                  | Původní hnswlib   |             | Nejmodernější dostupné |
| new&#x2011;avx            | Nová              | Heuristic   | AVX                    |
| new&#x2011;heuristic      | Nová              | Heuristic   | Nejmodernější dostupné |
| new&#x2011;naive          | Nová              | Naive       | Nejmodernější dostupné |
| new&#x2011;no&#x2011;bit  | Nová              | NoBitArray  | Nejmodernější dostupné |
| new&#x2011;no&#x2011;simd | Nová              | Heuristic   | Žádné                  |
| new&#x2011;prefetch       | Nová              | Prefetching | Nejmodernější dostupné |
| new&#x2011;sse            | Nová              | Heuristic   | SSE                    |
