// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.0
//! [Plugin import]
import QtLocation 5.3
//! [Plugin import]

Item {
    //! [Plugin locale]
    //single locale
    Plugin {
        locales: "en_US"
    }

    //multiple locales
    Plugin {
        locales: ["fr_FR","en_US"]
    }
    //! [Plugin locale]
}
