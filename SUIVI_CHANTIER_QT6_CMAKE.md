# Suivi chantier : CMake + Qt6 + minuscules

Objectif : compiler `quick3d` (lib) et `quick3dtest` (appli) sous Linux avec
CMake + Qt6, même organisation que `galman` (builds shadow, `scripts/linux`).
Qt5 est abandonné. Les `.pro`/`.pri` sont conservés et maintenus à jour
(usage QtCreator) ; la référence de build sous Linux est CMake.

Légende : `[ ]` à faire, `[x]` fait, `[~]` en cours.

## Phase 0 — Préparation

- [x] Inventaire organisation `galman` (`build-debug/`, `build-release/`, `scripts/linux/`)
- [x] Inventaire `quick3d` : lib 116 `.cpp` / 137 `.h`, appli test (5 sources + 1 `.ui`), `qt-plus` (sous-module), `COTS`
- [x] Recensement Qt5-ismes : `QGLWidget` (7), `QGLShaderProgram` (30), `QGLShader` (5), `QGLFramebufferObject` (4), `QRegExp` (10), `qSort` (4), `SkipEmptyParts` (10), `QWheelEvent` (7), `<QtGui/QApplication>` (1)
- [x] Recenser toutes les références de chemins avec casse (fait : aucun `#include "Dir/..."` côté `quick3d`, chemins `qrc` avec préfixe `/`, `Models/`/`Textures/` dans `bin/*.xml`, includes à sous-dossiers côté `qt-plus`)

## Phase 1 — Dossiers en minuscules (`git mv`, fichiers inchangés, contenu `third` intact)

- [x] `Quick3D` -> `quick3d`, `Quick3DTest` -> `quick3dtest`, `COTS` -> `third` (+ `.gitmodules` + gitdir `zlib` réparé, `submodule status` OK)
- [x] `quick3d/Documentation` -> `doc`, `quick3d/Source` -> `source` (+ 11 sous-dossiers), `quick3d/Resources` -> `resources` (+ `shaders`, `glsl1`, `glsl4`, `doc/shaders/2d`), `quick3d/Blender` -> `blender`
- [x] `quick3dtest/bin/Models` -> `models`, `bin/Textures` -> `textures`
- [x] `quick3dtest/Models` -> `models` (+ `textures`), `quick3dtest/Resources` -> `resources` (+ `samples`, `source`)
- [x] `qt-plus` (sous-module, à committer là-bas) : `file`, `geotools` (+ `wmm`), `image`, `poppy`, `qmltree`, `remotecontrol`, `test`, `web` (+ `webcontrols`)
- [x] Références de casse mises à jour (includes, `INCLUDEPATH`, `.pro`/`.pri`, `.qrc`, strings `:/resources/...`, scènes `bin/*.xml`, `doc/*.md`, `AGENTS.md`, `.gitignore`)
- Note : l'outil d'édition réduit `$$` en `$` dans les `newString` avec `replaceAll` — vérifier `$$PWD` après chaque édition de `.pro`, ou passer par `perl`.

## Phase 2 — CMake + `scripts/linux` (modèle `galman`, builds shadow)

- [x] `CMakeLists.txt` racine, `qt-plus/CMakeLists.txt` (91 sources, `QTPLUS_LIBRARY`), `quick3d/CMakeLists.txt` (116 sources + `CZip` + `unzip11`, `QUICK3D_LIB`, `zlib` système, SFML exclu sur Linux), `quick3dtest/CMakeLists.txt` (appli + 2 `.ui` + `.qrc`, postfix `d` en debug comme les `.pro`)
- [x] `scripts/linux/build.sh`, `clean.sh`, `deps.sh`, `run.sh` (binaire `quick3dtest/Quick3DTest[d]`, log `tmp/run.log`, args après `--` transmis à l'appli), `chmod +x`
- [x] Stratégie runtime data : à affiner en phase 5 (`CRessourcesManager` cherche relatif à l'exécutable + `qrc` ; `run.sh` transmet `--scene`)
- [x] `.gitignore` : `build-*/` (déjà `*build-*`), `/build/`, `tmp/` déjà couvert
- [x] Premier `cmake -S . -B build-debug` qui configure sans erreur (dépendances installées via `deps.sh`)

## Phase 3 — Port Qt6 `qt-plus` (sous-module, Qt6 uniquement)

- [x] `QMutex::Recursive`/`RecursionMode` supprimés de Qt6 -> membres `QRecursiveMutex` (9 sites, `QMutexLocker` est template en Qt6 : OK) ; surcharge `CTracableMutex(eMode)` sans appelants supprimée
- [x] `qsrand`/`qrand`/`toTime_t` -> `QRandomGenerator` (`CTDMADevice`, `ROKE` ; `s_bSeedInit` supprimé)
- [x] `QRegExp` -> `QRegularExpression` (10 sites : `split`, `indexIn`/`cap` -> `match`/`captured`, `exactMatch` -> motif ancré `\A(?:...)\z`)
- [x] `QColor::getHsvF(qreal*)` -> `float*` (`float` locaux), `QImage::alphaChannel()/setAlphaChannel()` -> `convertedTo(Format_Alpha8)` + `QPainter DestinationIn`
- [x] `QString::SkipEmptyParts` -> `Qt::SkipEmptyParts`, `QByteArray::append(QString)` -> `.toLatin1()` (convention existante du fichier)
- [x] `CSoundSynth` : `QAudioDeviceInfo`/`QAudioOutput`+format -> `QMediaDevices::defaultAudioOutput()` + `QAudioSink(device, format)` (`Int16`), membre renommé `m_tAudioSink`
- [x] `qt-plus` compile en lib sous Qt6 (`build-debug/qt-plus/libqt-plusd.so`)

## Phase 4 — Port Qt6 lib `quick3d`

- [x] `CGLWidgetScene` : `QGLWidget` -> `QOpenGLWidget` (format via `QSurfaceFormat` + `setFormat`, `grabFramebuffer()`)
- [x] `QGLShaderProgram`/`QGLShader` -> `QOpenGLShaderProgram`/`QOpenGLShader`, `QGLFramebufferObject` -> `QOpenGLFramebufferObject` (ctor 5 args conservé, existe en Qt6), `QGLWidget::convertToGLFormat` -> helper local `convertToGLFormat()` (`RGBA8888` + miroir vertical), `updateGL()` -> `update()`
- [x] `QMutex::Recursive` -> `QRecursiveMutex` (12 membres), `qSort` -> `std::sort`, `QString::Skip/KeepEmptyParts` -> `Qt::`, `Qt::MidButton` -> `Qt::MiddleButton`, `QWheelEvent::delta()` -> `angleDelta().y()`
- [x] `QString('0' + code)` ambigu -> `QChar(static_cast<ushort>(...))` (`CGeoloc`, `CTiledMaterial` : même extrait dupliqué, à factoriser plus tard) ; `CAverager::getAverage` : `T(count)` ambigu (`qsizetype`) -> `T(static_cast<int>(count))` (fix générique dans `qt-plus`)
- [x] `qt-plus` expose aussi `source/cpp/web` + `source/cpp/geotools` (comme le `.pro`) pour `CViewport.h`
- [x] La lib `quick3d` linke (`build-debug/quick3d/libQuick3Dd.so`)

## Phase 5 — Appli `quick3dtest` Qt6

- [x] `main.cpp`, `Quick3DTest.ui`, `CUnitTests` : aucun port nécessaire au-delà de `qrand()` -> `QRandomGenerator::bounded()` ; `CGeometryGenerator.cpp` manquant dans le `CMakeLists` ajouté
- [x] L'appli linke (`build-debug/quick3dtest/Quick3DTestd`), `--unit-tests` passe en `offscreen` (exit 0)
- [x] `run.sh` testé (`-d -- --unit-tests`, log `tmp/run.log`) ; runtime data : `CRessourcesManager` cherche relatif à l'exécutable + `qrc`, scènes via `--scene <fichier>` — affiner si besoin à l'usage GUI

## Phase 6 — Finalisation

- [ ] `AGENTS.md` : commandes build, noms dossiers, Qt6 (plus de Qt5/`qmake`)
- [ ] `README.md` : instruction build CMake (plus de `Quick3D.pro` / Qt Creator sans shadow build)
- [ ] `.pro`/`.pri` conservés et maintenus à jour pour QtCreator (ne pas supprimer ; le build Linux de référence reste CMake)
- [ ] Commit(s) : `quick3d` + sous-module `qt-plus` (seulement sur ta demande explicite)

## Points à valider avec toi

1. Commits dans le sous-module `qt-plus` : OK de committer/pousser là-bas quand ce sera prêt ?
2. `GLSL1`/`GLSL4` : dossiers passés en minuscules aussi (`glsl1`/`glsl4`) avec maj des références, ou gardés tels quels ?
3. `package.sh` / `install-desktop.sh` / versionnage type `galman` : confirmé hors scope, plus tard si besoin.
