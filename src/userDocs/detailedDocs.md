# Podrobná uživatelská dokumentace

Tato dokumentace obsahuje podrobný návod ke všem programům, které obsahuje příloha práce. Popis systémových požadavků, přístupu k programátorské dokumentaci a návod jak spustit ukázku projektu obsahuje dokument `Uživatelská dokumentace.pdf`.

Všechny cesty uvedené v tomto souboru jsou relativní k cestě složky, která obsahuje PDF soubor této dokumentace.

## Nativní knihovna

Pokud neplánujete používat skripty v jazyce Python, můžete sestavit nativní knihovnu samostatně pomocí následujících příkazů. 

```bash
mkdir src/cmakeBuild
cd src/cmakeBuild
cmake -DCMAKE_BUILD_TYPE=Release ../index
cmake --build . --config Release
```

Vygenerované řešení nebude využívat SIMD instrukcí. Pokud těchto instrukcí chcete využít, vygenerujte řešení pomocí skriptu `src/scripts/buildProject.py`.

Řešení bude vytvořeno ve složce `src/cmakeBuild`. V každém systému vypadají soubory řešení jinak. Např. při použití Windows s Visual Studiem je řešením `.sln` soubor a projekty jsou `.vcxproj` soubory. Pro spuštění projektů je doporučena konfigurace `Release`. Řešení obsahuje dva projekty.

- *recallTable* - Postaví HNSW index nové implementace a vypíše tabulku závislosti přesnosti na parametru vyhledávání ef<sub>search</sub>. Více o konfiguraci tohoto projektu v kapitole `JSON konfigurace`.

  | Parametr | Význam                                     | Výchozí hodnota          |
  | -------- | ------------------------------------------ | ------------------------ |
  | První    | Cesta ke konfiguračnímu souboru typu JSON. | `src/config/config.json` |

- *datasetToText* - Vypíše textový popis datové kolekce ze složky `src/data` do souboru. Slouží pro ověření konzistence mezi binárními a HDF5 soubory. Název datového souboru je prvním parametrem programu. Výchozí hodnotou je `angular-small`.

## JSON Konfigurace

Pro změnu sestavovaných konfigurací programy `recallTable.cpp` a `recallTable.py` upravte soubor s konfiguracemi. Výchozím souborem je `src/config/config.json`. V souboru je jediný JSON objekt, který obsahuje dva povinné klíče.

```json
{
	"datasets": [
		{
			"name": "angular-small",
			"angular": true,
			"dim": 25,
			"k": 10,
			"testCount": 200,
			"trainCount": 20000,
			"seed": 104
		}
	],
	"index": [
		{
			"dataset": "angular-small",
			"efConstruction": 200,
			"efSearch": [10, 15, 20, 40, 80, 120, 200],
			"mMax": 16,
			"seed": 200,
			"SIMD": "best",
			"template": "prefetching"
		}
	]
}
```

Klíč `datasets` je pole objektů, kde každý objekt popisuje jeden datový soubor.

| Klíč       | Typ hodnoty | Význam                                                       |
| :--------- | :---------- | :----------------------------------------------------------- |
| name       | string      | Unikátní název souboru sloužící k identifikaci.              |
| angular    | boolean     | Pokud je nastaven na `true`, využívá soubor kosinusové vzdálenosti. Jinak využívá Eukleidovské vzdálenosti. |
| dim        | int         | Počet dimenzí prostoru.                                      |
| k          | int         | Počet hledaných nejbližších sousedů dotazovaného prvku.      |
| testCount  | int         | Počet dotazů.                                                |
| trainCount | int         | Počet prvků použitých k sestavení indexu.                    |
| seed       | int         | Nastavení generátoru náhodných čísel.                        |

Klíč `index` je pole objektů, kde každý objekt popisuje jednu konfiguraci indexu.

| Klíč           | Typ hodnoty   | Význam                                                       |
| :------------- | :------------ | :----------------------------------------------------------- |
| dataset        | string        | Identifikace datového souboru. Odpovídá klíči `name` v tabulce výše. |
| efConstruction | int           | Počet uvažovaných sousedů při vytváření nových hran v indexu. |
| efSearch       | array         | Pole hodnot parametru vyhledávání ef<sub>search</sub>.       |
| mMax           | int           | Maximální povolený počet sousedů jednoho prvku v indexu na vrstvě vyšší než vrstva 0. |
| seed           | int           | Nastavení generátoru náhodných úrovní v indexu.              |
| SIMD           | string / null | Upřednostňovaný typ SIMD instrukcí. Možnosti jsou `"avx"`, `"avx512"`, `"best"`, `null`, a `"sse"`.* |
| template       | string        | Šablona indexu. Možnosti jsou `Heuristic`, `Naive`, `NoBitArray` a `Prefetching`. |

\* Zvolením hodnoty `best` zvolíte nejmodernější dostupné SIMD rozšíření. Hodnotou `null` zakážete použití SIMD instrukcí.

## Šablony nové implementace

| Šablona     | Metoda výběru sousedů | Seznam navštívených vrcholů | Asynchronní přístup do paměti                                |
| ----------- | --------------------- | --------------------------- | ------------------------------------------------------------ |
| Heuristic   | Heuristika            | Bitové pole                 | Ne                                                           |
| Naive       | Naivní algoritmus     | Bitové pole                 | Ne                                                           |
| NoBitArray  | Heuristika            | Obyčejné pole               | Při výpočtu vzdáleností<br />Při načítání dat seznamu navštívených vrcholů |
| Prefetching | Heuristika            | Bitové pole                 | Při výpočtu vzdáleností                                      |

## Sestavení rozhraní v jazyce Python

Pokud chcete využít skriptů jazyka Python, musíte vygenerovat správné virtuální prostředí. Pokud jste již alespoň jednou spustili ukázku pomocí skriptu `RUNME.py`, můžete tuto sekci přeskočit.

Spusťte skript `src/scripts/buildProject.py` pomocí interpretu Python verze 3.9. Tento skript vygeneruje virtuální prostředí, stáhne potřebné balíčky a zkompiluje knihovnu nové implementace indexu HNSW.

*OS Windows*

```none
py -3.9 .\src\scripts\buildProject.py
```

*OS Linux*

```none
python3.9 ./src/scripts/buildProject.py
```

## Virtuální prostředí

Pokud není uvedeno jinak, skripty uvnitř složky `src/scripts` vždy spouštějte pomocí vygenerovaného virtuální prostředí. Prostředí aktivujete pomocí aktivačního skriptu. Výběr skriptu závisí na použitém OS a interpretu.

| OS | Interpret | Cesta k aktivačnímu skriptu |
| :-- | :-- | :-- |
| Linux | * | ./.venv/bin/activate |
| Windows | Batch | .\\.venv\Scripts\activate.bat |
| Windows | Powershell | .\\.venv\Scripts\Activate.ps1 |

\* Pokud používáte OS Linux specifikujte zvolený interpret při volání aktivačního skriptu.

```none
source ./.venv/bin/activate
```

## Srovnání implementací

Skript `src/runBenchmarks.py` spustí srovnání implementací v jednom nebo více Docker kontejnerech, vypočítá výkonnostní metriky, vygeneruje webovou stránku s výsledky a otevře ji v nové kartě internetového prohlížeče. Kód vygenerované stránky lze poté najít ve složce `src/website` a můžete ji opětovně zobrazit otevřením souboru `src/website/index.html`.

*Před spuštěním se ujistěte, že je služba Docker zapnutá.*

| Parametr, zkratka                       | Význam                                                       |
| --------------------------------------- | ------------------------------------------------------------ |
| &#x2011;&#x2011;algoDefPaths, &#x2011;a | Vyžadován. Seznam cest ke konfiguračním souborům oddělených mezerami. O konfiguraci se více dočtete níže v kapitole `Konfigurace srovnání`. |
| &#x2011;&#x2011;datasets, &#x2011;d     | Vyžadován\*. Seznam datových souborů oddělených mezerami.    |
| &#x2011;&#x2011;datasetsPath, &#x2011;p | Vyžadován\*. Cesta k textovému souboru se seznamem datových souborů. |
| &#x2011;&#x2011;force, &#x2011;f        | Spustí již provedená měření znovu.                           |
| &#x2011;&#x2011;runs, &#x2011;r         | Počet opakování měření. Výchozí hodnota je 1.                |
| &#x2011;&#x2011;workers, &#x2011;w      | Počet paralelně spuštěných Docker kontejnerů. Výchozí hodnota je 1. |

\* Pouze jeden z parametrů označených hvězdičkou by měl být uveden. Datové soubory využité ke srovnání nejsou ty samé, které jsou využívány k testování nativní knihovny. Jejich seznam najdete v kapitole `Porovnávané datové soubory`. Příklad spuštění:

```bash
python runBenchmarks.py -a ../config/noBitVsPrefetch.yaml -f -p ../config/selectedDatasets.txt -r 5 -w 2
```

## Konfigurace srovnání

Výběr implementací ke srovnání a jejich parametrů zprostředkovávají konfigurační soubory ve formátu YAML. Příklad takového souboru je `src/config/algos.yaml`. Ve složce `src/config` se nacházejí předem vytvořené konfigurace.

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

| Název implementace        | Druh implementace                                            | Šablona     | SIMD rozšíření         |
| ------------------------- | ------------------------------------------------------------ | ----------- | ---------------------- |
| original                  | Původní [hnswlib](https://github.com/nmslib/hnswlib/releases/tag/v0.6.2) |             | Nejmodernější dostupné |
| new&#x2011;avx            | Nová                                                         | Heuristic   | AVX nebo AVX2          |
| new&#x2011;avx&#x2011;512 | Nová                                                         | Heuristic   | AVX&#x2011;512         |
| new&#x2011;heuristic      | Nová                                                         | Heuristic   | Nejmodernější dostupné |
| new&#x2011;naive          | Nová                                                         | Naive       | Nejmodernější dostupné |
| new&#x2011;no&#x2011;bit  | Nová                                                         | NoBitArray  | Nejmodernější dostupné |
| new&#x2011;no&#x2011;simd | Nová                                                         | Heuristic   | Žádné                  |
| new&#x2011;prefetch       | Nová                                                         | Prefetching | Nejmodernější dostupné |
| new&#x2011;sse            | Nová                                                         | Heuristic   | SSE                    |

Popis šablon obsahuje kapitola `Šablony nové implementace` výše.

## Porovnávané datové soubory

Pro srovnání implementací je možno využít následujících datových souborů.

| Název                                            | Dimenze | Počet prvků při stavbě | Dotazy | Metrika                 |
| ------------------------------------------------ | ------- | ---------------------- | ------ | ----------------------- |
| fashion&#x2011;mnist&#x2011;784&#x2011;euclidean | 784     | 60 000                 | 10 000 | Eukleidovská vzdálenost |
| glove&#x2011;25&#x2011;angular                   | 25      | 1 183 514              | 10 000 | Kosinusová vzdálenost    |
| glove&#x2011;50&#x2011;angular                   | 50      | 1 183 514              | 10 000 | Kosinusová vzdálenost    |
| glove&#x2011;100&#x2011;angular                  | 100     | 1 183 514              | 10 000 | Kosinusová vzdálenost    |
| glove&#x2011;200&#x2011;angular                  | 200     | 1 183 514              | 10 000 | Kosinusová vzdálenost    |
| lastfm&#x2011;64&#x2011;dot                      | 64      | 292 385                | 50 000 | Kosinusová vzdálenost    |
| mnist&#x2011;784&#x2011;euclidean                | 784     | 60 000                 | 10 000 | Eukleidovská vzdálenost |
| nytimes&#x2011;16&#x2011;angular                 | 16      | 290 000                | 10 000 | Kosinusová vzdálenost    |
| nytimes&#x2011;256&#x2011;angular                | 256     | 290 000                | 10 000 | Kosinusová vzdálenost    |
| random&#x2011;s&#x2011;100&#x2011;angular        | 100     | 100 000                | 10 000 | Kosinusová vzdálenost    |
| random&#x2011;s&#x2011;100&#x2011;euclidean      | 100     | 100 000                | 10 000 | Eukleidovská vzdálenost |
| random&#x2011;xs&#x2011;20&#x2011;angular        | 20      | 10 000                 | 10 000 | Kosinusová vzdálenost    |
| random&#x2011;xs&#x2011;20&#x2011;euclidean      | 20      | 10 000                 | 10 000 | Eukleidovská vzdálenost |
| sift&#x2011;128&#x2011;euclidean                 | 128     | 1 000 000              | 10 000 | Eukleidovská vzdálenost |

Pro spuštění srovnání nad více soubory lze využít textového formátu, kde každý řádek reprezentuje jeden datový soubor. Řádky, které začínají znakem `#` jsou ignorovány. Příklad:

```bash
# glove-25-angular
nytimes-256-angular
sift-128-euclidean
```

## Seznam dalších skriptů

Následuje seznam dalších skriptů ve složce `src/scripts`.

| Název skriptu        | Stručný popis skriptu                                        |
| -------------------- | ------------------------------------------------------------ |
| buildProject         | Vytvoří virtuální prostředí, datové soubory pro testování, nativní C++ řešení a jeho Python rozhraní. |
| clean                | Odstraní vygenerované soubory a vrátí projekt do původního stavu. |
| datasetGenerator     | Vygeneruje datové soubory pro testování.                     |
| datasetToText        | Převede datový soubor do textového formátu.                  |
| formatCMakeTemplates | Vygeneruje `src/index/CMakeLists.txt`.                       |
| generateTables       | Vygeneruje LaTeX tabulky podobné těm, které jsou uvedeny v bakalářské práci. |
| latexTable           | Vygeneruje LaTeX tabulku na základě výsledků srovnání.       |
| runRecallTable       | Postaví nový index a zobrazí tabulku závislosti přesnosti na parametru vyhledávání ef<sub>search</sub>. |
| SIMDCapability       | Zobrazí dostupná SIMD rozšíření instrukční sady procesoru.   |

## Podrobný popis skriptů

U každého skriptu je uveden jeho účel, parametry a příklad spuštění. Pokud skript obsahuje alespoň jeden parametr, pak použitím parametru `--help` nebo `-h` zobrazíte nápovědu v anglickém jazyce. Každý skript se nachází ve složce `src/scripts`.

### buildProject

*Tento skript lze spustit bez virtuálního prostředí.*

Vytvoří virtuální prostředí interpretu Python, stáhne potřebné moduly, vygeneruje datové soubory pro testování, nativní C++ řešení pro knihovnu nového indexu, rozhraní v jazyce Python pro nový index a otestuje funkčnost tohoto indexu spuštěním skriptu `runRecallTable`.

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

*Tento skript lze spustit bez virtuálního prostředí.*

Odstraní datové soubory pro testování, C++ nativní řešení a Python rozhraní. Pokud je spuštěn mimo virtuální prostředí ve složce `.venv`, pak odstraní toto prostředí. Naměřené výsledky odstraněny nebudou, pokud o to uživatel nepožádá. Skript nikdy neodstraní velké datové kolekce ve složce `src/benchmarks/data`.

| Parametr, zkratka                  | Význam                                                       |
| ---------------------------------- | ------------------------------------------------------------ |
| &#x2011;&#x2011;results, &#x2011;r | Odstraní naměřené výsledky srovnání, vygenerované grafy a tabulky. |

Příklad spuštění:

```bash
python clean.py --results
```

### datasetGenerator

Vygeneruje datové soubory pro testování. O konfiguraci tohoto skriptu se více dočtete výše v kapitole `JSON Konfigurace`.

| Parametr, zkratka                 | Význam                                     | Výchozí hodnota          |
| --------------------------------- | ------------------------------------------ | ------------------------ |
| &#x2011;&#x2011;config, &#x2011;c | Cesta ke konfiguračnímu souboru typu JSON. | `src/config/config.json` |

Příklad spuštění:

```bash
python datasetGenerator.py -c ../config/dimensions.json
```

### datasetToText

Převede vybraný datový soubor ze složky `src/data` do textového formátu. Výstupní textový soubor zapíše pod jménem datového souboru do stejné složky.

| Parametr, zkratka        | Význam                                                       |
| ------------------------ | ------------------------------------------------------------ |
| &#x2011;&#x2011;name, -n | Název datového souboru bez přípony. Pokud není uveden, výchozím souborem je `angular-small`. |

Příklad spuštění:

```bash
python datasetToText --name euclidean-medium
```

### formatCMakeTemplates

Vygeneruje soubor `src/index/CMakeLists.txt` a doplní do něj správnou definici maker tak, aby došlo pouze ke kompilaci těch funkcí, pro které je k dispozici vhodné SIMD rozšíření instrukční sady procesoru.

Příklad spuštění:

```bash
python formatCMakeTemplates.py
```

### generateTables

Vygeneruje LaTeX tabulky podobné těm, které jsou uvedeny v bakalářské práci, ale pouze v případě, že jsou pro ně dostupné naměřené výsledky. Tyto výsledky lze získat spuštěním následujících příkazů. Avšak tato měření mohou trvat více než 12 hodin.

```bash
python runBenchmarks.py -a ../config/heuristicVsNaive.yaml -d lastfm-64-dot -r 5
python runBenchmarks.py -a ../config/heuristicVsPrefetch.yaml -d glove-50-angular -r 5
python runBenchmarks.py -a ../config/originalVsPrefetch.yaml -d sift-128-euclidean -r 5
python generateTables.py
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
python latexTable.py -a new-prefetch original -d sift-128-euclidean -le "Nová impl." "Původní impl." -o ../figures/table.tex -p
```

### runRecallTable

Postaví index nové implementace a vyhledá v něm nejbližší sousedy s různými hodnotami parametru vyhledávání ef<sub>search</sub>. Poté vypíše tabulku závislosti přesnosti vyhledávání na tomto parametru. O konfiguraci tohoto skriptu se více dočtete výše v kapitole `JSON Konfigurace`.

| Parametr, zkratka                 | Význam                                     | Výchozí hodnota          |
| --------------------------------- | ------------------------------------------ | ------------------------ |
| &#x2011;&#x2011;config, &#x2011;c | Cesta ke konfiguračnímu souboru typu JSON. | `src/config/config.json` |

Příklad spuštění:

```bash
python runRecallTable.py -c ../config/dimensions.json
```

### SIMDCapability

Zobrazí dostupná SIMD rozšíření instrukční sady procesoru. Tento skript je také využíván ostatními skripty pro vygenerování správných maker pro konfiguraci programu CMake.

Příklad spuštění:

```bash
python SIMDCapability.py
```
