
QML_IMPORT_PATH += $$PWD

RESOURCES += \
    $$PWD/common.qrc

commonresources.files += $$PWD/resources/*

qmlcomponents.files += \
    $$PWD/QtLocation/examples/components/TextWithLabel.qml \
    $$PWD/QtLocation/examples/components/Button.qml \
    $$PWD/QtLocation/examples/components/Checkbox.qml \
    $$PWD/QtLocation/examples/components/Fader.qml \
    $$PWD/QtLocation/examples/components/Optionbutton.qml \
    $$PWD/QtLocation/examples/components/Slider.qml \
    $$PWD/QtLocation/examples/components/TitleBar.qml \
    $$PWD/QtLocation/examples/components/ButtonRow.qml \
    $$PWD/QtLocation/examples/components/Menu.qml \
    $$PWD/QtLocation/examples/components/IconButton.qml \
    $$PWD/QtLocation/examples/components/BusyIndicator.qml
OTHER_FILES += $$qmlcomponents.files

qmlcomponentsstyle.files += \
    $$PWD/QtLocation/examples/components/style/Style.qml \
    $$PWD/QtLocation/examples/components/style/ButtonStyle.qml \
    $$PWD/QtLocation/examples/components/style/HMenuItemStyle.qml \
    $$PWD/QtLocation/examples/components/style/VMenuItemStyle.qml
OTHER_FILES += $$qmlcomponentsstyle.files

qmldialogs.files += \
    $$PWD/QtLocation/examples/dialogs/Dialog.qml
OTHER_FILES += $$qmldialogs.files

qmldir.files += $$PWD/QtLocation/examples/qmldir
OTHER_FILES += $$qmldir.files
