# special case begin
add_subdirectory(3rdparty/clip2tri)
add_subdirectory(3rdparty/poly2tri)
add_subdirectory(3rdparty/clipper)
add_subdirectory(positioning)
add_subdirectory(plugins)
if(TARGET Qt::Quick)
    add_subdirectory(positioningquick)
endif()
# special case end
