QT.location.VERSION = 5.0.0
QT.location.MAJOR_VERSION = 5
QT.location.MINOR_VERSION = 0
QT.location.PATCH_VERSION = 0

QT.location.name = QtLocation
QT.location.bins = $$QT_MODULE_BIN_BASE
QT.location.includes = $$QT_MODULE_INCLUDE_BASE $$QT_MODULE_INCLUDE_BASE/QtLocation
QT.location.private_includes = $$QT_MODULE_INCLUDE_BASE/QtLocation/$$QT.location.VERSION
QT.location.sources = $$QT_MODULE_BASE/src/location
QT.location.libs = $$QT_MODULE_LIB_BASE
QT.location.plugins = $$QT_MODULE_PLUGIN_BASE
QT.location.imports = $$QT_MODULE_IMPORT_BASE
QT.location.depends = gui 3d sql network
QT.location.DEFINES = QT_LOCATION_LIB

QT_CONFIG += location
