# TODO: figure out if we need any condition for that.
# Previously was: "condition": "config.opengl"
qt_feature("location-labs-plugin" PRIVATE
    LABEL "Provides experimental QtLocation QML types"
    CONDITION TRUE
)

qt_feature("geoservices_osm" PRIVATE
    LABEL "Provides access to OpenStreetMap geoservices"
    CONDITION TRUE
)
