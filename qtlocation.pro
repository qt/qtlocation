load(configure)
qtCompileTest(locationd)
qtCompileTest(geoclue)
qtCompileTest(gypsy)

load(qt_parts)

module_qtlocation_doc.subdir = doc/src
module_qtlocation_doc.target = sub-doc
module_qtlocation_doc.depends = sub_src

SUBDIRS += module_qtlocation_doc
