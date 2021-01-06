# Generated from examples.pro.

qt_examples_build_begin()

if(TARGET Qt::Positioning)
    add_subdirectory(positioning)
endif()
# TODO: Enable location examples later!
# if(TARGET Qt::Location AND TARGET Qt::Positioning)
#     add_subdirectory(location)
# endif()

qt_examples_build_end()
