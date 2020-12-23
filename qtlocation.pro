# Only QtPositioning build is supported for now.
# Porting QtLocation to Qt6 requires heavy refactoring and will be done later.
# If you want to enable QtLocation build (for development reasons, probably),
# remove the SKIP_QT_LOCATION variable definition in src/src.pro

requires(!wasm)
load(configure)
qtCompileTest(gypsy)
qtCompileTest(winrt)

load(qt_parts)

DISTFILES += sync.profile configure.json
