# Suivi chantier : CMake + Qt6 + minuscules

Objectif : compiler `quick3d` (lib) et `quick3dtest` (appli) sous Linux avec
CMake + Qt6, même organisation que `galman` (builds shadow, `scripts/linux`).
Qt5 est abandonné. Les `.pro` restent pour mémoire mais ne servent plus.

Légende : `[ ]` à faire, `[x]` fait, `[~]` en cours.

## Phase 0 — Préparation

- [x] Inventaire organisation `galman` (`build-debug/`, `build-release/`, `scripts/linux/`)
- [x] Inventaire `quick3d` : lib 116 `.cpp` / 137 `.h`, appli test (5 sources + 1 `.ui`), `qt-plus` (sous-module), `COTS`
- [x] Recensement Qt5-ismes : `QGLWidget` (7), `QGLShaderProgram` (30), `QGLShader` (5), `QGLFramebufferObject` (4), `QRegExp` (10), `qSort` (4), `SkipEmptyParts` (10), `QWheelEvent` (7), `<QtGui/QApplication>` (1)
- [ ] Recenser toutes les références de chemins avec casse (`#include "Web/..."`, `INCLUDEPATH`, `.qrc`, chargement `"Models/..."`, `"Shaders/..."`, `"Textures/..."`)

## Phase 1 — Dossiers en minuscules (`git mv`, fichiers inchangés, contenu `COTS` intact)

- [ ] `Quick3D` -> `quick3d`, `Quick3DTest` -> `quick3dtest`, `COTS` -> `third`
- [ ] `quick3d/Documentation` -> `quick3d/doc`, `quick3d/Source` -> `quick3d/source` (+ 11 sous-dossiers : `animation`, `base`, `components`, `input`, `math`, `mesh`, `render`, `terrain`, `utils`, `zip`)
- [ ] `quick3d/Resources` -> `quick3d/resources` (+ `shaders`, `GLSL1`, `GLSL4` : vérifier références avant)
- [ ] `quick3dtest/bin/Models` -> `models`, `bin/Textures` -> `textures`
- [ ] `quick3dtest/Models` -> `models` (+ `Textures` -> `textures`), `quick3dtest/Resources` -> `quick3dtest/resources` (+ `Samples`, `Source`)
- [ ] `qt-plus` (sous-module, à committer là-bas) : `source/cpp/File` -> `file`, `GeoTools` -> `geotools`, `Image` -> `image`, `Poppy` -> `poppy`, `QMLTree` -> `qmltree`, `RemoteControl` -> `remotecontrol`, `Test` -> `test`, `Web` -> `web` (+ `WebControls` -> `webcontrols`), `source/{bison,css,js,misc,qml}` et `test/source` déjà minuscules
- [ ] Mettre à jour toutes les références de casse (includes, `INCLUDEPATH`, `.qrc`, code, `Documentation/*.md`, `AGENTS.md`)

## Phase 2 — CMake + `scripts/linux` (modèle `galman`, builds shadow)

- [ ] `CMakeLists.txt` racine (`cmake_minimum_required`, `project`, `add_subdirectory`)
- [ ] `qt-plus/CMakeLists.txt` dans le sous-module (liste sources depuis `qt-plus.pri`, modules Qt6 ; bison déjà généré : `QMLGrammarParser.cpp` présent, pas de régénération)
- [ ] `quick3d/CMakeLists.txt` : lib (`AUTOMOC`/`AUTORCC`/`AUTOUIC`, Qt6 `Core Gui Widgets Network OpenGL Xml Positioning`, OpenGL système, `zlib` système, `unzip11` depuis `third/`, SFML ignoré sur Linux — usage win32 joystick uniquement)
- [ ] `quick3dtest/CMakeLists.txt` : appli (`main.cpp`, `Quick3DTest.cpp/.h`, `CUnitTests`, `.ui`, ressources, lien lib + `qt-plus`)
- [ ] `scripts/linux/build.sh`, `clean.sh`, `deps.sh`, `run.sh` (adaptés : binaire `quick3dtest`, pas de `VERSION`/`packaging` pour l'instant)
- [ ] Stratégie runtime data : `quick3dtest/bin/models|textures|*.xml` accessibles depuis `build-debug/` / `build-release/` (copie cmake ou `run.sh`)
- [x] Dossier `tmp/` à la racine pour les fichiers temporaires/scratch (gitignoré, mentionné dans `AGENTS.md`, scripts avec `mkdir -p`)
- [ ] `.gitignore` : `build-*/`, `build/`, artefacts cmake (`CMakeLists.txt.user*`, etc.) — `tmp/` déjà couvert
- [ ] Premier `cmake -S . -B build-debug` qui configure sans erreur

## Phase 3 — Port Qt6 `qt-plus` (sous-module, Qt6 uniquement)

- [ ] `QRegExp` -> `QRegularExpression`, `qSort` -> `std::sort`, `QString::SkipEmptyParts` -> `Qt::SkipEmptyParts`
- [ ] `QWheelEvent::delta()` et autres API événements retirées
- [ ] `<QtGui/QApplication>` -> `<QApplication>`, `qInstallMsgHandler`, `QDesktopServices::storageLocation`
- [ ] Correctifs C++17/20 du compilateur moderne, au fil des erreurs
- [ ] `qt-plus` compile en lib sous Qt6 (cible du `add_subdirectory`)

## Phase 4 — Port Qt6 lib `quick3d`

- [ ] `CGLWidgetScene` : `QGLWidget` -> `QOpenGLWidget` (+ `QOpenGLFunctions`)
- [ ] `QGLShaderProgram`/`QGLShader` -> `QOpenGLShaderProgram`/`QOpenGLShader`, `QGLFramebufferObject` -> `QOpenGLFramebufferObject`, `QGLFormat`, `QGL` divers
- [ ] `glext.h`, `CGLExtension.h`, `CMatrix4.h` : vérifier contre Qt6/OpenGL système
- [ ] Reste des Qt5-ismes + correctifs C++17 au fil des erreurs
- [ ] La lib `quick3d` linke

## Phase 5 — Appli `quick3dtest` Qt6

- [ ] `main.cpp`, `Quick3DTest.ui`, `CUnitTests` : port Qt6, `--unit-tests` sans GUI, `--scene <fichier>`
- [ ] L'appli linke, se lance, `--unit-tests` passe (ou échecs listés ici)
- [ ] `run.sh` lance l'appli avec ses données (`--scene` par défaut si pertinent)

## Phase 6 — Finalisation

- [ ] `AGENTS.md` : commandes build, noms dossiers, Qt6 (plus de Qt5/`qmake`)
- [ ] `README.md` : instruction build CMake (plus de `Quick3D.pro` / Qt Creator sans shadow build)
- [ ] Supprimer ou archiver les `.pro`/`.pri` obsolètes (à valider avec toi avant)
- [ ] Commit(s) : `quick3d` + sous-module `qt-plus` (seulement sur ta demande explicite)

## Points à valider avec toi

1. Commits dans le sous-module `qt-plus` : OK de committer/pousser là-bas quand ce sera prêt ?
2. `GLSL1`/`GLSL4` : dossiers passés en minuscules aussi (`glsl1`/`glsl4`) avec maj des références, ou gardés tels quels ?
3. `package.sh` / `install-desktop.sh` / versionnage type `galman` : hors scope pour l'instant, on ajoute si besoin plus tard ?
