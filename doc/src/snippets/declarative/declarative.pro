TEMPLATE = aux

content.files = \
   places.qml

# Put content in INSTALLS so that content.files become part of the project tree in Qt Creator,
# but scoped with false as we don't actually want to install them.  They are documentation
# snippets.
false:INSTALLS += content
