# Uživatelská dokumentace

Tato dokumentace obsahuje návod jak spustit ukázku nové implementace indexu HNSW, která je součástí bakalářské práce na téma "Aproximace KNN problému".

Všechny cesty uvedené v tomto souboru jsou relativní k cestě složky, která obsahuje PDF soubor této dokumentace.

## Potřebné programy
- Docker
- Překladač C++17
- Python 3.9

## Ukázka

1. Ujistěte se, že služba Docker je zapnutá.
2. Spusťte skript `RUNME.py` pomocí interpretu Python verze 3.9. Na Windows například takto:

```none
py -3.9 RUNME.py
```

Tento skript provede následující.

- Vytvoří virtuální prostředí interpretu Python ve složce `.venv`.
- Vytvoří nativní C++ řešení ve složce `src/cmakeBuild`.
- Spustí porovnání původní a nové implementace indexu HNSW nad malými kolekcemi se 100 000 prvky.
- Otevře stránku s výsledky v jedné kartě internetového prohlížeče.
- Otevře stránku s dokumentací indexu ve druhé kartě.

## Výsledky srovnání

Výsledky jsou zaznačeny do grafů, které zobrazuje webová stránka. Ta je generována ve složce `src/website`. Původní implementace je v grafech označována slovem `original`, nová implementace slovem `new`.

## Podrobná dokumentace

Více informací obsahuje dokument `Podrobná uživatelská dokumentace.pdf`.

## Programátorská dokumentace

Programátorskou dokumentaci obsahuje soubor `docs/html/index.html`, který lze otevřít v internetovém prohlížeči.
