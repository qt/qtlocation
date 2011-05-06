QT_LOCATION_VERSION = $$QT_VERSION
QT_LOCATION_MAJOR_VERSION = $$QT_MAJOR_VERSION
QT_LOCATION_MINOR_VERSION = $$QT_MINOR_VERSION
QT_LOCATION_PATCH_VERSION = $$QT_PATCH_VERSION

QT.location.name = QtLocation
QT.location.bins = $$QT_MODULE_BIN_BASE
QT.location.includes = $$QT_MODULE_INCLUDE_BASE $$QT_MODULE_INCLUDE_BASE/QtLocation
QT.location.private_includes = $$QT_MODULE_INCLUDE_BASE/QtLocation/private
QT.location.sources = $$QT_MODULE_BASE/src/location
QT.location.libs = $$QT_MODULE_LIB_BASE
QT.location.plugins = $$QT_MODULE_PLUGIN_BASE
QT.location.imports = $$QT_MODULE_IMPORT_BASE
QT.location.depends = gui sql network
QT.location.DEFINES = QT_LOCATION_LIB

QT_CONFIG += location
