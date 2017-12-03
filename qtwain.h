/*
QTwain interface class set
Copyright (C) 2002-2003         Stephan Stapel
                                stephan.stapel@web.de
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef _QTWAIN_H_
#define _QTWAIN_H_

#include "QTwain_global.h"
#include <windows.h>
#include <QObject>


class CTwain;


const unsigned int AnyCount = UINT_MAX;
class QTwain_EXPORT QTwain : public QObject
{	
    Q_OBJECT

public:
    QTwain(QObject* parent = 0);
    ~QTwain();

    bool selectSource();
    bool getScans(unsigned int maxNumImages = AnyCount);
    bool isValidDriver() const;
    bool initTwain(int hdl = 0);

public slots:
    void getScan();
signals:
    void pixmapReceived(HBITMAP scan);

private slots:
    void scanTaken(HBITMAP scan);

protected:
    CTwain *native_twain;

};
#endif // !_QTWAIN_H_
