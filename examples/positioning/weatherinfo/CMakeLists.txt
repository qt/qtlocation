# Generated from weatherinfo.pro.

cmake_minimum_required(VERSION 3.16)
project(weatherinfo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
  set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/positioning/weatherinfo")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Network)
find_package(Qt6 COMPONENTS Positioning)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Quick)

qt_add_executable(weatherinfo
    appmodel.cpp appmodel.h
    providerbackend.cpp providerbackend.h
    openweathermapbackend.cpp openweathermapbackend.h
    weatherapibackend.cpp weatherapibackend.h
    main.cpp
)
set_target_properties(weatherinfo PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(weatherinfo PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Network
    Qt::Positioning
    Qt::Qml
    Qt::Quick
)

qt_add_qml_module(weatherinfo
    URI WeatherInfo
    VERSION 1.0
    QML_FILES
        components/BigForecastIcon.qml
        components/ForecastIcon.qml
        components/WeatherIcon.qml
        weatherinfo.qml
    RESOURCES
        icons/weather-few-clouds.png
        icons/weather-fog.png
        icons/weather-haze.png
        icons/weather-icy.png
        icons/weather-overcast.png
        icons/weather-showers.png
        icons/weather-sleet.png
        icons/weather-snow.png
        icons/weather-storm.png
        icons/weather-sunny-very-few-clouds.png
        icons/weather-sunny.png
        icons/weather-thundershower.png
        icons/weather-showers-scattered.png
    NO_RESOURCE_TARGET_PATH
)

install(TARGETS weatherinfo
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
