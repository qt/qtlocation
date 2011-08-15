include(../../../staticconfig.pri)

TEMPLATE = subdirs

contains(mobility_modules,contacts): SUBDIRS += qtcontactsdocsample
contains(mobility_modules,versit): SUBDIRS += qtversitdocsample
contains(mobility_modules,organizer): SUBDIRS += qtorganizerdocsample

contains(mobility_modules,location): SUBDIRS += qtlandmarksdocsample
contains(mobility_modules,sensors): SUBDIRS += sensors
contains(mobility_modules,feedback): SUBDIRS += qtfeedbackdocsample
!win32:contains(mobility_modules,connectivity): SUBDIRS += connectivity
!symbian:contains(mobility_modules,multimedia): SUBDIRS += multimedia-snippets
