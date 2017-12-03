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
#include "twaincpp.h"
#include <limits.h>
#include <QDebug>

#include "qtwain.h"

QTwain::QTwain(QObject* parent)
    : QObject()
{
    native_twain = new CTwain();
    connect(native_twain, SIGNAL(ImageTaken(HBITMAP)), this, SLOT(scanTaken(HBITMAP)));
} // !QTwain()

QTwain::~QTwain()
{	
    native_twain->ReleaseTwain();
    delete native_twain;
} // !~QTwain()

void QTwain::scanTaken(HBITMAP scan)
{
    emit pixmapReceived(scan);
}

bool QTwain::selectSource()
{
    return (native_twain->SelectSource() == true);
} // !selectSource()


bool QTwain::getScans(unsigned int maxNumImages)
{
	unsigned int nMaxNum = 1;
	if (maxNumImages == UINT_MAX)
		nMaxNum = TWCPP_ANYCOUNT;
		
    if (native_twain->Acquire(nMaxNum) == true)
    {
        //emit pixmapReceived(native_twain->GetScanImage());
        return true;
    }
    else return false;
} // !acquire()

void QTwain::getScan()
{
    getScans(1);
}

bool QTwain::isValidDriver() const
{
    return (native_twain->IsValidDriver() == true);
} // !isValidDriver()


//bool QTwain::processMessage(MSG& msg)
//{
//	if (msg.message == 528) // don't really know why...
//		return false;
//	if (m_hMessageWnd == 0)
//		return false;
//	return (ProcessMessage(msg) == true);
//} // !processMessage()


bool QTwain::initTwain(int hdl)
{
    //WId id = ->winId();
    native_twain->InitTwain((HWND)hdl);
    return native_twain->IsValidDriver();
} // !onSetParent()
