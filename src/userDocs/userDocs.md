# Uživatelská dokumentace

Tato dokumentace obsahuje návod jak spustit ukázku nové implementace indexu HNSW, která je součástí bakalářské práce na téma "Aproximace KNN problému".

## Potřebné programy
- Docker
- Překladač C++17
- Python 3.9

## Sestavení

Ujistěte se, že služba Docker je zapnutá. Spusťte skript `RUNME.py` pomocí interpretu Python verze 3.9. Na Windows například takto:

```none
py -3.9 RUNME.py
```

Tento skript provede následující.

- Vytvoří virtuální prostředí interpretu Python ve složce `.venv`.
- Vytvoří nativní C++ řešení ve složce `src/cmakeBuild`.
- Spustí porovnání původní a nové implementace indexu HNSW.
- Otevře stránku s výsledky v jedné kartě internetového prohlížeče.
- Otevře stránku s dokumentací indexu ve druhé kartě.

## Výsledky srovnání

Výsledky jsou zaznačeny do grafů, které zobrazuje webová stránka. Ta je generována ve složce `src/website`. Původní implementace je v grafech označována slovem `original`, nová implementace slovem `new`.

## Virtuální prostředí

Skripty uvnitř složky `src/scripts` vždy spouštějte pomocí vygenerovaného virtuální prostředí. Prostředí aktivujete pomocí aktivačního skriptu ve složce `.venv/Scripts`. Výběr skriptu závisí na použitém OS a interpretu.

| OS | Interpret | Cesta k aktivačnímu skriptu |
| :-- | :-- | :-- |
| Linux |  | ./.venv/Scripts/activate |
| Windows | Batch | .\\.venv\Scripts\activate.bat |
| Windows | Powershell | .\\.venv\Scripts\Activate.ps1 |

## Seznam skriptů

Následuje seznam skriptů ve složce `src/scripts`, které umožňují uživateli provést více operací než úvodní skript.

| Název skriptu        | Stručný popis skriptu                                        |
| -------------------- | ------------------------------------------------------------ |
| buildProject         | Vytvoří virtuální prostředí, nativní C++ řešení a jeho Python rozhraní. |
| clean                | Odstraní vygenerované soubory a vrátí projekt do původního stavu. |
| datasetGenerator     | Vygeneruje datové soubory pro debugování.                    |
| datasetToText        | Převede datový soubor do textového formátu.                  |
| formatCMakeTemplates | Vygeneruje CMakeLists.txt.                                   |
| generateTables       | Vygeneruje LaTeX tabulky podobné těm, které jsou v bakalářské práci. |
| latexTable           | Vygeneruje LaTeX tabulku na základě výsledků srovnání.       |
| runBenchmarks        | Spustí srovnání, vygeneruje a otevře webovou stránku s výsledky. |
| runRecallTable       | Postaví nový index a zobrazí tabulku závislosti přesnosti na parametru vyhledávání ef<sub>search</sub>. |
| SIMDCapability       | Zobrazí SIMD rozšíření instrukční sady procesoru, která jsou k dispozici. |
