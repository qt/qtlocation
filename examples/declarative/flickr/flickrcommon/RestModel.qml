/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7

XmlListModel {
    property double latitude: 0
    property double longitude: 0
    // The flickr rest API with position data
    source: "http://api.flickr.com/services/rest/?min_taken_date=2000-01-01+0:00:00&extras=date_taken&method=flickr.photos.search&per_page=30&sort=date-taken-desc&api_key=e36784df8a03fea04c22ed93318b291c&lat="+latitude+"&lon="+longitude;
    query: "/rsp/photos/photo"
    XmlRole { name: "title"; query: "@title/string()" }
    XmlRole { name: "datetaken"; query: "@datetaken/string()" }
    XmlRole { name: "farm"; query: "@farm/string()" }
    XmlRole { name: "server"; query: "@server/string()" }
    XmlRole { name: "id"; query: "@id/string()" }
    XmlRole { name: "secret"; query: "@secret/string()" }
}

/*

QString url = "http://farm";
            url.append(child.attribute("farm"));
            url.append(".static.flickr.com/");
            url.append(child.attribute("server"));
            url.append("/");
            url.append(child.attribute("id"));
            url.append("_");
            url.append(child.attribute("secret"));


XmlListModel {
    property string tags : ""

    function commasep(x)
    {
        return x.replace(' ',',');
    }

    //m_latitude = 61.4500;
    //m_longitude = 23.8502;
    //urlstring.append("&lat=");
    //urlstring.append(QString::number(m_latitude));
    //urlstring.append("&lon=");

    source: "http://api.flickr.com/services/rest/photos_public.gne?"+(tags ? "tags="+commasep(tags)+"&" : "")+"format=rss2"
    // +"&lat=61"+"&lon=23"
    // original source: "http://api.flickr.com/services/feeds/photos_public.gne?"+(tags ? "tags="+commasep(tags)+"&" : "")+"format=rss2"
    query: "/rss/channel/item"
    namespaceDeclarations: "declare namespace media=\"http://search.yahoo.com/mrss/\";"

    XmlRole { name: "title"; query: "title/string()" }
    XmlRole { name: "imagePath"; query: "media:thumbnail/@url/string()" }
    XmlRole { name: "url"; query: "media:content/@url/string()" }
    XmlRole { name: "description"; query: "description/string()" }
    XmlRole { name: "tags"; query: "media:category/string()" }
    XmlRole { name: "photoWidth"; query: "media:content/@width/string()" }
    XmlRole { name: "photoHeight"; query: "media:content/@height/string()" }
    XmlRole { name: "photoType"; query: "media:content/@type/string()" }
    XmlRole { name: "photoAuthor"; query: "author/string()" }
    XmlRole { name: "photoDate"; query: "pubDate/string()" }
}
*/
