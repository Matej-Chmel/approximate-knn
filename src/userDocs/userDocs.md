# Uživatelská dokumentace

Tato dokumentace obsahuje návod jak spustit ukázku nové implementace indexu HNSW, která je součástí bakalářské práce na téma "Aproximace KNN problému".

Všechny cesty uvedené v tomto souboru jsou relativní k cestě složky, která obsahuje PDF soubor této dokumentace.

## Systémové požadavky
- 64bitový operační systém Linux nebo Windows 10
- CMake, verze 3.0 nebo vyšší
- Docker
- Internetové připojení
- Překladač C++17
- Python, verze 3.9

Cesty k požadovaným programům musí být obsaženy v proměnné prostředí `PATH`.

### Další potřebné balíčky na OS Linux
 - build-essential
 - python3.9-dev
 - python3.9-venv

Pokud používáte APT, využijte následující kód.

```none
apt install build-essential cmake python3.9-dev python3.9-venv
```

## Ukázka

1. Ujistěte se, že je služba Docker zapnutá.
2. Spusťte skript `RUNME.py` pomocí interpretu Python verze 3.9.

*OS Linux*
```none
python3.9 docs/openDocs.py
```

*OS Windows*
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

## Programátorská dokumentace

C++ kód nového indexu naleznete ve složce `src/index/chm`. Webovou stránku s programátorskou dokumentací nového indexu HNSW zobrazíte pomocí skriptu `docs/openDocs.py`. Spustíte jej pomocí následujícího příkazu.

*OS Linux*
```none
python3 docs/openDocs.py
```

*OS Windows*
```none
py .\docs\openDocs.py
```

Dokumentaci můžete také zobrazit otevřením souboru `docs/html/index.html` v internetovém prohlížeči.

## Podrobná dokumentace

Více informací obsahuje dokument `Podrobná uživatelská dokumentace.pdf`.
