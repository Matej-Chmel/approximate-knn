# Aproximace KNN problému

Příloha bakalářské práce na téma "Aproximace KNN problému".

Obsahem přílohy je nová implementace indexu HNSW ve složce [src/index/chm](src/index/chm), kód srovnání s původní implementací [hnswlib](https://github.com/nmslib/hnswlib/releases/tag/v0.6.2) a dokumentace.

Návod jak spustit ukázku obsahuje soubor `Uživatelská dokumentace.pdf`. Podrobný návod ke všem programům v této příloze obsahuje soubor `Podrobná uživatelská dokumentace.pdf`. Uživatelské dokumentace můžete také zobrazit v Markdown formátu ze složky [src/userDocs](src/userDocs).

Programátorskou dokumentaci generovanou programem [Doxygen](https://www.doxygen.nl/index.html) naleznete v souboru [docs/html/index.html](docs/html/index.html).

## Software třetích stran
- Srovnávací metoda převzata z [ann-benchmarks](https://github.com/erikbern/ann-benchmarks/tree/2b40b3ea988c77822cbe3a1df2b8d047805a2282) a poté upravena. [Licence](src/benchmarks/LICENSE_ann-benchmarks).
- Metriky vzdálenosti v souborech [euclideanDistance.hpp](src/index/chm/euclideanDistance.hpp) a [innerProduct.hpp](src/index/chm/innerProduct.hpp) byly převzaty z [hnswlib](https://github.com/nmslib/hnswlib/tree/7cc0ecbd43723418f43b8e73a46debbbc3940346). [Licence](src/index/LICENSE_hnswlib). Seznam změn:
	- Volba využitého SIMD rozšíření zohledňuje preference uživatele.
	- Počet složek vektorů, které lze paralelně zpracovat, metrika nepočítá.
	- Každé funkci je přiřazen objekt, který ji zastupuje a ukládá název funkce.
- Konfigurační soubory typu JSON čteny pomocí knihovny [nlohmann/json](https://github.com/nlohmann/json/tree/a94430615d8360272151f602b8c9eeb58509ecde). [Licence](src/index/libs/json.hpp#L1).
