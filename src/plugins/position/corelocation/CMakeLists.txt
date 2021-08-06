# Generated from corelocation.pro.

#####################################################################
## QGeoPositionInfoSourceFactoryCL Plugin:
#####################################################################

qt_internal_add_plugin(QGeoPositionInfoSourceFactoryCL
    OUTPUT_NAME qtposition_cl
    TYPE position
    SOURCES
        qgeopositioninfosource_cl.mm qgeopositioninfosource_cl_p.h
        qgeopositioninfosourcefactory_cl.h qgeopositioninfosourcefactory_cl.mm
    LIBRARIES
        ${FWCoreLocation}
        ${FWFoundation}
        Qt::Core
        Qt::CorePrivate
        Qt::Positioning
)

# special case begin
set_target_properties(QGeoPositionInfoSourceFactoryCL
    PROPERTIES
        DISABLE_PRECOMPILE_HEADERS ON
)
# special case end

#### Keys ignored in scope 1:.:.:corelocation.pro:<TRUE>:
# OTHER_FILES = "plugin.json"

## Scopes:
#####################################################################

#### Keys ignored in scope 2:.:.:corelocation.pro:NOT APPLE:
# DISTFILES = "$$OBJECTIVE_SOURCES"
