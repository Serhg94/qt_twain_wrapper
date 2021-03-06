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
#include <QPainter>
#include "qtwainmainwindow.h"
#include "qtwaininterface.h"
#include "qtwain.h"
#include "dib.h"
QTwainMainWindow::QTwainMainWindow(QWidget* parent, Qt::WindowFlags f )
	: QMainWindow(parent, f)
{
	m_pTwain         = new QTwain(0);
    m_pWidget        = new QWidget(this);	// do this as your first call!
	m_pVBox          = new QGridLayout();
	m_pWidget->setLayout(m_pVBox);
	m_pAcquireButton = new QPushButton("Acquire image",m_pWidget);
	m_pSelectButton  = new QPushButton("Select source",m_pWidget);
	m_pLabel = new QLabel();
	m_pVBox->addWidget(m_pAcquireButton,0,0,0);
    m_pVBox->addWidget(m_pSelectButton,0,1,0);
	m_pVBox->addWidget(m_pLabel,1,0,2,2,0);
	 m_pWidget->setMinimumSize(500,500);
	m_pPixmap        = 0;
	
 
    
	QObject::connect(m_pSelectButton, SIGNAL(clicked()),
	                 this, SLOT(onSelectButton()));
	QObject::connect(m_pAcquireButton, SIGNAL(clicked()),
	                 this, SLOT(onAcquireButton()));
	QObject::connect(m_pTwain, SIGNAL(dibAcquired(CDIB*)),
		             this, SLOT(onDibAcquired(CDIB*)));
	
	setMinimumSize(800, 600);
	
	setGeometry ( 300,300 ,800,600 ) ;
} // !QTwainMainWindow()
QTwainMainWindow::~QTwainMainWindow()
{
	delete m_pTwain;
} // !~QTwainMainWindow()
void QTwainMainWindow::showEvent(QShowEvent* thisEvent)
{
	// set the parent here to be sure to have a really
	// valid window as the twain parent!
	m_pTwain->setParent(this);
} // !showEvent()
bool QTwainMainWindow::nativeEvent(const QByteArray &eventType, MSG* pMsg, long* result)
{ qWarning("QTwainMainWindow::winEvent");
	//return (m_pTwain->processMessage(*pMsg) == true);
	m_pTwain->processMessage(*pMsg); 
	return false;
} // !winEvent()
void QTwainMainWindow::resizeEvent(QResizeEvent* thisEvent)
{
	//this->setGeometry(0, 0, 170, 70);
} // !resizeEvent()
void QTwainMainWindow::onSelectButton()
{
	m_pTwain->selectSource();
} // !onSelectButton()
void QTwainMainWindow::onAcquireButton()
{
	if (!m_pTwain->acquire())
	{
		qWarning("acquire() call not successful!");
	}
} // !onAcquireButton()
void QTwainMainWindow::onDibAcquired(CDIB* pDib)
{
	if (m_pPixmap)
		delete m_pPixmap;
	m_pPixmap = QTwainInterface::convertToPixmap(pDib);	
	
   m_pLabel->setPixmap(*m_pPixmap);
	delete pDib;
	
	//repaint();
} // !onDibAcquired()
void QTwainMainWindow::paintEvent(QPaintEvent* thisEvent)
{
	if (m_pPixmap)
	{
	  qWarning("paintEvent");
		//QPainter p(m_pWidget);
		//m_pVBox->addWidget(p,1,0,0);
		/*p.drawPixmap(0, 71, 
			         *m_pPixmap,
					 0, 0,
					 width(), height() - 71);
					 */
	}
} // !paintEvent()
