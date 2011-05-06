/******************************************************************************
 * Project:  PROJ.4
 * Purpose:  Implementation of pj_open_lib(), and pj_set_finder().  These
 *           provide a standard interface for opening projections support
 *           data files.
 * Author:   Gerald Evenden, Frank Warmerdam <warmerdam@pobox.com>,
 *           Nokia Corporation (qt-info@nokia.com)
 *
 ******************************************************************************
 * Copyright (c) 1995, Gerald Evenden
 * Copyright (c) 2002, Frank Warmerdam <warmerdam@pobox.com>
 * Copyright (c) 2010, Nokia Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

#define PJ_LIB__
extern "C" {
#include <projects.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
}

#include <QString>
#include <QFile>
#include <QList>

class PJLibTracker
{
public:
    ~PJLibTracker();

    QList<QFile*> files;
};

PJLibTracker::~PJLibTracker()
{
    foreach (QFile *f, files)
        delete f;
}

static PJLibTracker tracker;


void pj_set_finder( const char *(*new_finder)(const char *) )
{
}

void pj_set_searchpath ( int count, const char **path )
{
}

FILE *pj_open_lib(char *name, char *mode)
{
    if (QFile::exists(QString(":/proj_data/%1").arg(name))) {
        QFile *f = new QFile(QString(":/proj_data/%1").arg(name));
        f->open(QIODevice::ReadOnly);
 
        if (f->handle() == -1) {
            delete f;
            return NULL;
        }

        tracker.files.append(f);
        return fdopen(f->handle(), "r");
    } else {
        return NULL;
    }
}
