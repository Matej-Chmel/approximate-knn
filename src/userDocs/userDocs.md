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
