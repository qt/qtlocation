qt_feature("geoservices_osm" PRIVATE
    LABEL "Provides access to OpenStreetMap geoservices"
    CONDITION TRUE
)

qt_feature("geoservices_esri" PRIVATE
    LABEL "Provides access to OpenStreetMap geoservices"
    CONDITION FALSE
)

qt_feature("geoservices_mapbox" PRIVATE
    LABEL "Provides access to OpenStreetMap geoservices"
    CONDITION FALSE
)

qt_feature("geoservices_nokia" PRIVATE
    LABEL "Provides access to OpenStreetMap geoservices"
    CONDITION FALSE
)
