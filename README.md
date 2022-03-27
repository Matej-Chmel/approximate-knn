# Aproximace KNN problému

Vlastní implementace techniky HNSW pro vyhledávání *k* nejbližších sousedů v prostoru a její srovnání s původní implementací [hnswlib](https://github.com/nmslib/hnswlib/tree/7cc0ecbd43723418f43b8e73a46debbbc3940346).

## Software třetích stran
- Srovnávací metoda převzata z [ann-benchmarks](https://github.com/erikbern/ann-benchmarks/tree/2b40b3ea988c77822cbe3a1df2b8d047805a2282) a poté upravena. [Licence](benchmarks/LICENSE_ann-benchmarks).
- Vzdálenostní funkce v souborech [euclideanDistance.hpp](index/chm/euclideanDistance.hpp) a [innerProduct.hpp](index/chm/innerProduct.hpp) byly převzaty z [hnswlib](https://github.com/nmslib/hnswlib/tree/7cc0ecbd43723418f43b8e73a46debbbc3940346) a poté upraveny. [Licence](index/LICENSE_hnswlib).

## Sestavení
Spusťte skript [buildProject.py](buildProject.py) pomocí Pythonu verze 3.9.

```batch
py -3.9 buildProject.py
```

Tento skript provede následující:
- Vytvoří virtuální prostředí Pythonu s potřebnými balíčky ve složce `.venv`.
- Vygeneruje malé datové kolekce ve složce `data`.
- Vygeneruje řešení pro nativní knihovnu ve složce `cmakeBuild`.
- Spustí [executables/recallTable.py](executables/recallTable.py) pro ověření funkčnosti Python knihovny.

## Nativní knihovna
Vygenerované řešení ve složce `cmakeBuild` vypadá na každém systému jinak. Např. při použítí Windows s Visual Studiem je řešením `.sln` soubor a projekty jsou `.vcxproj` soubory. Pro spuštění projektů je doporučena konfigurace `Release`. Řešení obsahuje dva projekty.

- *checkDataset* - Vypíše textový popis datové kolekce `data/test.bin` do souboru `data/testCpp.txt`. Slouží pro ověření konzistence mezi binárními a HDF5 soubory.
- *recallTable* - Postaví HNSW index a vypíše tabulku závislosti přesnosti na parametru vyhledávání `efSearch`. Nastavení datové kolekce i indexu lze změnit. [Jak změnit konfiguraci](#nastavení-programů-recall-table).

## Python skripty
Skripty uvnitř složek [benchmarks](benchmarks) a [executables](executables) vždy spouštějte pomocí vygenerovaného virtuální prostředí. Aktivační skripty se nacházejí ve složce `.venv/Scripts`.

|Prostředí|Cesta k aktivačnímu skriptu|
|:--|:--|
|Batch|`.\.venv\Scripts\activate.bat`|
|Linux|`./.venv/Scripts/activate`|
|Powershell|`.\.venv\Scripts\Activate.ps1`|

## Python knihovna
Python balíček `chm_hnsw` je obalem okolo nativní knihovny. Využívají jej dva skripty.

- [executables/checkDataset.py](executables/checkDataset.py) - Vypíše textový popis datové kolekce `data/test.hdf5` do souboru `data/testPy.txt`. Slouží pro ověření konzistence mezi binárními a HDF5 soubory.
- [executables/recallTable.py](executables/recallTable.py) - Postaví HNSW index a vypíše tabulku závislosti přesnosti na parametru vyhledávání `efSearch`. Nastavení datové kolekce i indexu lze změnit. [Jak změnit konfiguraci](#nastavení-programů-recall-table).

## Datové kolekce
Pro vygenerování jiných datových kolekcí změňte konfiguraci v souboru [config/datasetGeneratorConfig.json](config/datasetGeneratorConfig.json) a spusťte skript [executables/datasetGenerator.py](executables/datasetGenerator.py).

### Popis konfigurace
Konfigurace je JSON soubor s polem objektů, kde každý objekt popisuje jednu datovou kolekci.
```json
{
	"name": "angular-20000",
	"angular": true,
	"dim": 16,
	"k": 10,
	"testCount": 200,
	"trainCount": 20000,
	"seed": 150
}
```

|Klíč|Typ hodnoty|Význam|
|:--|:--|:--|
|name|string|Unikátní název datové kolekce sloužící k její identifikaci.|
|angular|boolean|Pokud je nastaven na `true`, využívá datová kolekce kosinusové podobnosti k nalezení sousedů. Jinak využívá eukleidovské vzdálenosti.|
|dim|int|Počet dimenzí.|
|k|int|Počet nejbližších sousedů dotazovaného prvku.|
|testCount|int|Počet dotazů v kolekci.|
|trainCount|int|Počet prvků použitých k sestavení indexu.|
|seed|int|Nastavení generátoru náhodných čísel.|

## Nastavení programů *Recall table*
Pro změnu datové kolekce nebo nastavení indexu v programech [executables/recallTable.cpp](executables/recallTable.cpp) a [executables/recallTable.py](executables/recallTable.py) upravte soubor [config/recallTableConfig.json](config/recallTableConfig.json).

### Popis konfigurace
Konfigurace je JSON soubor s jediným objektem.
```json
{
	"dataset": "angular-20000",
	"efConstruction": 200,
	"efSearch": [10, 50, 100, 500, 1000],
	"mMax": 16,
	"seed": 200,
	"useHeuristic": true
}
```

|Klíč|Typ hodnoty|Význam|
|:--|:--|:--|
|dataset|string|Identifikace datové kolekce. Odpovídá klíči `name` v [konfiguraci generátoru datových kolekcí](#datové-kolekce).|
|efConstruction|int|Počet uvažovaných sousedů při vytváření nových hran v indexu.|
|efSearch|array|Pole hodnot parametru vyhledávání.|
|mMax|int|Maximální povolený počet sousedů jednoho prvku v indexu na vrstvě vyšší než ta nejspodnější.|
|seed|int|Nastavení generátoru náhodných úrovní v indexu.|
|useHeuristic|boolean|Pokud je nastaven na `true`, využívá index heuristiky při výběru sousedů. Jinak využívá naivního algoritmu.|
