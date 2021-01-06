# Generated from clip2tri.pro.

#####################################################################
## Bundled_Clip2Tri Generic Library:
#####################################################################

qt_internal_add_3rdparty_library(Bundled_Clip2Tri
    QMAKE_LIB_NAME _clip2tri
    STATIC
    SKIP_AUTOMOC # special case
    EXCEPTIONS
    SOURCES
        clip2tri.cpp clip2tri.h
    INCLUDE_DIRECTORIES
        ../clipper
        ../poly2tri
    LIBRARIES
        # Remove: L/lib
        Bundled_Clipper # special case
        Bundled_Poly2Tri # special case
)
qt_disable_warnings(Bundled_Clip2Tri)

# special case begin

# Generating an alias target, so that the unneeded include dependency is not
# generated in STATIC build.
# Otherwise the Bundled_Clip2Tri target will match to one of the
# ${known_modules}, and will be added to generate a wierd
# #include <QtBundled_Clip2Tri/QtBundled_Clip2Tri>
# in the QtPositioning/QtPositioningDepends.
# The root cause for such behavior are
# function(qt_internal_create_module_depends_file target)
# and
# function(qt_internal_write_depends_file module)
add_library(Clip2Tri::Clip2Tri ALIAS Bundled_Clip2Tri)

# special case end

## Scopes:
#####################################################################

#### Keys ignored in scope 3:.:.:clip2tri.pro:GCC:
# QMAKE_CFLAGS_OPTIMIZE_FULL = "-ffast-math"

#### Keys ignored in scope 4:.:.:clip2tri.pro:NOT CLANG AND NOT ICC AND NOT rim_qcc:
# QMAKE_CXXFLAGS_WARN_ON = "-Wno-error=return-type"
