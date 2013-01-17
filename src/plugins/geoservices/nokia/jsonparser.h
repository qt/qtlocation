/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
** This file is part of the Nokia services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file NOKIA_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Nokia services
** plugin source code.
**
****************************************************************************/

#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QVariant>
#include <QString>
#include <QRegExp>

QT_BEGIN_NAMESPACE

class JSONParser
{
public:
    QString data;
    int pos;


    JSONParser(QString data) : data(data), pos(0) {}

    bool detectString(const QString & string) {
        skipWhitespace();
        return data.mid(pos, string.size()) == string;
    }

    void skipWhitespace() {
        while (pos < data.size() && data[pos].isSpace())
            ++pos;
    }

    char peek() const {
        if (pos < data.size())
            return data[pos].toLatin1();
        else
            return -1;
    }

    QChar peekQ() const {
        if (pos < data.size())
            return data[pos];
        else
            return QChar(short(-1));
    }

    bool next() {
        if (pos < data.size()-1) {
            ++pos;
            return true;
        }
        return false;
    }

    QVariant parse() {
        QVariant ret;

        skipWhitespace();
        switch (peek()) {
        case '{':
            ret = parseHash();
            break;
        case '[':
            ret = parseList();
            break;
        case '"':
            ret = parseString();
            break;
        case 't':
        case 'f':
            ret = parseBoolean();
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '.':
            ret = parseNumber();
            break;
        case -1:
            // we went off the end of the string
            // return an invalid QVariant
            break;
        default:
            ret = parseUnknown();
            break;
        }

        return ret;
    }

    QVariantHash parseHash() {
        bool hascomma = true;
        QVariantHash ret;

        Q_ASSERT(peek() == '{');
        next();
        do {
            skipWhitespace();

            QString key = parseString();
            skipWhitespace();
            Q_ASSERT(peek() == ':');
            next();
            QVariant value = parse();

            ret[key] = value;

            skipWhitespace();
            if (peek() == ',') {
                next();
            }
            else {
                hascomma = false;
            }
        } while (hascomma);
        skipWhitespace();
        //Q_ASSERT(peek() == '}');
        next();

        return ret;
    }

    QVariantList parseList() {
        bool hascomma = true;
        QVariantList ret;

        Q_ASSERT(peek() == '[');
        next();
        do {
            skipWhitespace();
            QVariant value = parse();

            ret << value;

            skipWhitespace();
            if (peek() == ',') {
                next();
            }
            else {
                hascomma = false;
            }
        } while (hascomma);
        skipWhitespace();
        //Q_ASSERT(peek() == ']');
        next();

        return ret;
    }

    QString parseString() {
        QString ret;

        Q_ASSERT(peek() == '"');
        while (true) {
            if (!next()) return ret;
            char c = peek();
            switch (c) {
            case '"':
                next();
                return ret;
            case '\\':
                next();
                // fall-through
            default:
                ret += peekQ();
            }
        }
    }

    QVariant parseBoolean() {
        bool retbool = detectString("true");
        pos += retbool ? 4 : 5;
        return retbool;
    }

    QVariant parseNumber() {
        QRegExp r("[^-0-9.e+]");

        int newPos = r.indexIn(data, pos, QRegExp::CaretAtOffset);
        QVariant ret(data.mid(pos, newPos-pos).toDouble());
        pos = newPos;
        return ret;
    }

    QVariant parseUnknown() {
        QVariant ret(QString("unknown(")+peekQ()+"|"+QString::number((int)peekQ().unicode())+")");
        next();
        return ret;
    }
};

QT_END_NAMESPACE

#endif // JSONPARSER_H
