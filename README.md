# Aproximace KNN problému

Vlastní implementace techniky HNSW pro vyhledávání *k* nejbližších sousedů v prostoru a její srovnání s původní implementací [hnswlib](https://github.com/nmslib/hnswlib/tree/7cc0ecbd43723418f43b8e73a46debbbc3940346).

Uživatelská dokumentace se nachází v souboru `Uživatelská dokumentace.pdf`. Také ji můžete zobrazit v Markdown formátu ze souboru [userDocs.md](src/userDocs/userDocs.md).

## Software třetích stran
- Srovnávací metoda převzata z [ann-benchmarks](https://github.com/erikbern/ann-benchmarks/tree/2b40b3ea988c77822cbe3a1df2b8d047805a2282) a poté upravena. [Licence](src/benchmarks/LICENSE_ann-benchmarks).
- Vzdálenostní funkce v souborech [euclideanDistance.hpp](src/index/chm/euclideanDistance.hpp) a [innerProduct.hpp](src/index/chm/innerProduct.hpp) byly převzaty z [hnswlib](https://github.com/nmslib/hnswlib/tree/7cc0ecbd43723418f43b8e73a46debbbc3940346) a poté upraveny. [Licence](src/index/LICENSE_hnswlib).
- Konfigurační soubory typu JSON čteny pomocí knihovny [nlohmann/json](https://github.com/nlohmann/json/tree/a94430615d8360272151f602b8c9eeb58509ecde). [Licence](src/index/libs/json.hpp).
