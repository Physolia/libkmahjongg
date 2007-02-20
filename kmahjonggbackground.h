/*
    Copyright (C) 1997 Mathias Mueller   <in5y158@public.uni-hamburg.de>
    Copyright (C) 2006 Mauricio Piacentini   <mauricio@tabuleiro.com>

    Libkmahjongg is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef _KMAHJONGGBACKGROUND_H_
#define _KMAHJONGGBACKGROUND_H_
#include <QString>
#include <QImage>
#include <QBrush>
#include <QPixmap>
#include "ksvgrenderer.h"
#include "libkmahjongg_export.h"

class KMAHJONGGLIB_EXPORT KMahjonggBackground  
{


  public:
    KMahjonggBackground();
    ~KMahjonggBackground();
    bool tile;
    bool isSVG;

    bool loadDefault();
    bool load(const QString &file, short width, short height);
    void sizeChanged(int newW, int newH);
    void scaleModeChanged();
    QBrush & getBackground();
    QString path() {return filename;};
 private:
    //void sourceToBackground();
    void scale();
    QString pixmapCacheNameFromElementId(QString & elementid);
    QPixmap renderBG(short width, short height);

    //int tileMode;    // scale background = 0, tile = 1
    //QImage backgroundImage;
    QImage sourceImage;
    QPixmap backgroundPixmap;
    QBrush backgroundBrush;
    QString filename;
    short w;
    short h;

    KSvgRenderer svg;
};

#endif