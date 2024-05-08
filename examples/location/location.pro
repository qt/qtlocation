TEMPLATE = subdirs

qtHaveModule(quick) {
    SUBDIRS += places \
               places_list \
               places_map \
               mapviewer \
               minimal_map \
               itemview_transitions \
               planespotter

    QT_FOR_CONFIG += location-private
    qtHaveModule(widgets):qtConfig(location-labs-plugin): SUBDIRS += geojson_viewer
}
