
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

#include "kmahjonggbackground.h"
#include <kstandarddirs.h>
#include <klocale.h>
#include <kconfig.h>
#include <QImage>
#include <QFile>
#include <QPixmap>
#include <QPixmapCache>
#include <QPainter>
#include <QtDebug>

KMahjonggBackground::KMahjonggBackground(): tile(true) {
    isSVG = false;

    static bool _inited = false;
    if (_inited)
        return;
    KGlobal::dirs()->addResourceType("kmahjonggbackground", KStandardDirs::kde_default("data") + QString::fromLatin1("kmahjongglib/backgrounds/"));

    KGlobal::locale()->insertCatalog("libkmahjongglib");
    _inited = true;
}

KMahjonggBackground::~KMahjonggBackground() {
}

bool KMahjonggBackground::loadDefault()
{
    QString idx = "default.desktop";

    QString bgPath = KStandardDirs::locate("kmahjonggbackground", idx);
qDebug() << "Inside LoadDefault(), located background at " << bgPath;
    if (bgPath.isEmpty()) {
		return false;
    }
    return load(bgPath, 0, 0);
}

#define kBGVersionFormat 1

bool KMahjonggBackground::load(const QString &file, short width, short height) {
qDebug() << "Background loading";
    //tiled for now
    tile = true;

    QString graphicsPath;
    qDebug() << "Attempting to load .desktop at " << file;

    // verify if it is a valid file first and if we can open it
    QFile bgfile(file);
    if (!bgfile.open(QIODevice::ReadOnly)) {
      return (false);
    }
    bgfile.close();

    KConfig bgconfig(file, KConfig::OnlyLocal);
    bgconfig.setGroup(QString::fromLatin1("KMahjonggBackground"));

    QString themeName = bgconfig.readEntry("Name"); // Returns translated data
    //Version control
    int bgversion = bgconfig.readEntry("VersionFormat",0);
    //Format is increased when we have incompatible changes, meaning that older clients are not able to use the remaining information safely
    if (bgversion > kBGVersionFormat) {
        return false;
    }

    QString graphName = bgconfig.readEntry("FileName");

    graphicsPath = KStandardDirs::locate("kmahjonggbackground", graphName);
qDebug() << "Using background at " << graphicsPath;
    filename = graphicsPath;

    if (graphicsPath.isEmpty()) return (false);

    w      = bgconfig.readEntry("Width",0);
    h      = bgconfig.readEntry("Height",0);

    svg.load(graphicsPath);
    if (svg.isValid()) {
	isSVG = true;
        //tile?
        if (tile) {
           //use default tile dimensions
           w = svg.defaultSize().width(); 
           h = svg.defaultSize().height(); 
        }
    } else {
        qDebug() << "could not load svg";
        return( false );
    }

  // call out to scale/tile the source image into the background image
  //in case of SVG we will be already at the right size
  filename = file;
   return true;
}

// slot used when tile/scale option changes
void KMahjonggBackground::scaleModeChanged() {
	//TODO implement tiling
	//sourceToBackground();
	//scale();
}

void KMahjonggBackground::sizeChanged(int newW, int newH) {
        //in tiled mode we do not care about the whole field size
        if (tile) return;

	if (newW == w && newH == h)
		return;
	w = newW;
	h = newH;
	//scale();
}

void KMahjonggBackground::scale() {
qDebug() << "Background scaling";
    if( isSVG) {
	if (svg.isValid()) {
	        sourceImage =  QImage(w, h ,QImage::Format_RGB32);
		sourceImage.fill(0);
	        QPainter p(&sourceImage);
	        svg.render(&p);
	    }
    }
    backgroundPixmap = QPixmap::fromImage(sourceImage);
}

QString KMahjonggBackground::pixmapCacheNameFromElementId(QString & elementid) {
	return elementid+QString("W%1H%2").arg(w).arg(h);
}

QPixmap KMahjonggBackground::renderBG(short width, short height) {
    QImage qiRend(QSize(width, height),QImage::Format_ARGB32_Premultiplied);
    qiRend.fill(0);

    if (svg.isValid()) {
            QPainter p(&qiRend);
	    svg.render(&p);
    }
    return QPixmap::fromImage(qiRend);
}

QBrush & KMahjonggBackground::getBackground() {
 	if (!QPixmapCache::find(pixmapCacheNameFromElementId(filename), backgroundPixmap)) {
     		backgroundPixmap = renderBG(w, h);
     		QPixmapCache::insert(pixmapCacheNameFromElementId(filename), backgroundPixmap);
 	}
	backgroundBrush = QBrush(backgroundPixmap);
        return backgroundBrush;
}

/*
void Background::sourceToBackground() {
  //Sanity check, did we load anything?
  if (!sourceImage) return;

  // Deallocate the old image and create the new one
  delete backgroundImage;
  // the new version of kmahjongg uses true color images
  // to avoid the old color limitation.
  backgroundImage = new QImage(w, h, QImage::Format_RGB32);

  // Now we decide if we should scale the incoming image
  // or if we tile. First we check for an exact match which
  // should be true for all images created specifically for us.
  if ((sourceImage->width() == w) && (sourceImage->height() == h)) {
    *backgroundImage = *sourceImage;

  // Save a copy of the background as a pixmap for easy and quick
  // blitting.
  *backgroundPixmap = QPixmap::fromImage(*backgroundImage);

    return;
  }
    
  if (tile) {
    // copy new to background wrapping on w and height
    for (int y=0; y<backgroundImage->height(); y++) {
      QRgb *dest = (QRgb *) backgroundImage->scanLine(y);
      QRgb *src = (QRgb *) sourceImage->scanLine(y % sourceImage->height());
      for (int x=0; x< backgroundImage->width(); x++) {
	*dest = *(src + (x % sourceImage->width()));
	dest++;
      }
    }
  } else {
    *backgroundImage = sourceImage->scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    // Just incase the image is loaded 8 bit
    if (backgroundImage->depth() != 32)
      *backgroundImage = backgroundImage->convertToFormat(QImage::Format_RGB32);
  }

  // Save a copy of the background as a pixmap for easy and quick
  // blitting.
  *backgroundPixmap = QPixmap::fromImage(*backgroundImage);

  return;
}*/
