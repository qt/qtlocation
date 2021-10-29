qt_internal_add_plugin(QGeoServiceProviderFactoryItemsOverlayPlugin
    OUTPUT_NAME qtgeoservices_itemsoverlay
    CLASS_NAME QGeoServiceProviderFactoryItemsOverlay
    PLUGIN_TYPE geoservices
    SOURCES
        qgeomapitemsoverlay.h qgeomapitemsoverlay.cpp
        qgeomappingmanagerengineitemsoverlay.h qgeomappingmanagerengineitemsoverlay.cpp
        qgeoserviceproviderpluginitemsoverlay.h qgeoserviceproviderpluginitemsoverlay.cpp
    LIBRARIES
        Qt::Core
        Qt::LocationPrivate
        Qt::PositioningPrivate
)

qt_internal_extend_target(QGeoServiceProviderFactoryItemsOverlayPlugin
    CONDITION QT_FEATURE_location_labs_plugin
    DEFINES
        LOCATIONLABS
)
