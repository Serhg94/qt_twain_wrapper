#include "stdafx.h"
#include "twaincpp.h"
#include <QtGlobal>
#include <limits.h>
#include <QDebug>
/*
Constructor:
    Parameters : HWND
                Window to subclass

*/
CTwain::CTwain(HWND hWnd)
{
    m_hMessageWnd = 0;
    m_hTwainDLL = NULL;
    m_pDSMProc = NULL;
    m_bSourceSelected = false;
    m_bDSOpen = m_bDSMOpen = false;
    m_bSourceEnabled = false;
    m_bModalUI = true;
    m_nImageCount = TWCPP_ANYCOUNT;
    dib = new CDIB();
//    if(hWnd)
//    {
//        InitTwain(hWnd);
//    }
}
CTwain::~CTwain()
{
    ReleaseTwain();
}
/*
Initializes TWAIN interface . Is already called from the constructor.
It should be called again if ReleaseTwain is called.
  hWnd is the window which has to subclassed in order to recieve
  Twain messaged. Normally - this would be your main application window.
*/
bool CTwain::InitTwain(HWND hWnd)
{

char libName[512];
    if(IsValidDriver())
    {
        return true;
    }
    memset(&m_AppId,0,sizeof(m_AppId));
//    if(!IsWindow(hWnd))
//    {
//        return false;
//    }
    m_hMessageWnd = hWnd;
    strcpy(libName,"TWAIN_32.DLL");

    m_hTwainDLL  = LoadLibraryA(libName);
    if(m_hTwainDLL != NULL)
    {  //qWarning("valid driver");
        if(!(m_pDSMProc = (DSMENTRYPROC)GetProcAddress(m_hTwainDLL,(LPCSTR)"DSM_Entry")))
        {
            FreeLibrary(m_hTwainDLL);
            m_hTwainDLL = NULL;
        }
    }
    if(IsValidDriver())
    {

        GetIdentity();
        m_bDSMOpen= CallTwainProc(&m_AppId,NULL,DG_CONTROL,DAT_PARENT,MSG_OPENDSM,(TW_MEMREF)&m_hMessageWnd);
        return true;
    }
    else
    {
        return false;
    }
}
/*
Releases the twain interface . Need not be called unless you
want to specifically shut it down.
*/
void CTwain::ReleaseTwain()
{
    if(IsValidDriver())
    {
        CloseDSM();
        FreeLibrary(m_hTwainDLL);
        m_hTwainDLL = NULL;
        m_pDSMProc = NULL;
    }
}
/*
Returns true if a valid driver has been loaded
*/
bool CTwain::IsValidDriver() const
{
    return (m_hTwainDLL && m_pDSMProc);
}
/*
Entry point into Twain. For a complete description of this
routine  please refer to the Twain specification 1.8
*/
bool CTwain::CallTwainProc(pTW_IDENTITY pOrigin,pTW_IDENTITY pDest,
                       TW_UINT32 DG,TW_UINT16 DAT,TW_UINT16 MSG,
                       TW_MEMREF pData)
{
    if(IsValidDriver())
    {
    USHORT ret_val;
        ret_val = (*m_pDSMProc)(pOrigin,pDest,DG,DAT,MSG,pData);
        m_returnCode = ret_val;
        if(ret_val == TWRC_FAILURE)
        {
            (*m_pDSMProc)(pOrigin,pDest,DG_CONTROL,DAT_STATUS,MSG_GET,&m_Status);
        }
        return (ret_val == TWRC_SUCCESS);
    }
    else
    {
        m_returnCode = TWRC_FAILURE;
        return false;
    }
}
/*
This function should ideally be overridden in the derived class . If only a
few fields need to be updated , call CTawin::GetIdentity first in your
derived class
*/
void CTwain::GetIdentity()
{
    // Expects all the fields in m_AppId to be set except for the id field.
    m_AppId.Id = 0; // Initialize to 0 (Source Manager
    // will assign real value)
    m_AppId.Version.MajorNum = 3; //Your app's version number
    m_AppId.Version.MinorNum = 5;
    m_AppId.Version.Language = TWLG_USA;
    m_AppId.Version.Country = TWCY_USA;
    strcpy (m_AppId.Version.Info, "3.5");
    m_AppId.ProtocolMajor = TWON_PROTOCOLMAJOR;
    m_AppId.ProtocolMinor = TWON_PROTOCOLMINOR;
    m_AppId.SupportedGroups = DG_IMAGE | DG_CONTROL;
    strcpy (m_AppId.Manufacturer, "Electronika");
    strcpy (m_AppId.ProductFamily, "Generic");
    strcpy (m_AppId.ProductName, "Twain Scan Library");
}
/*
Called to display a dialog box to select the Twain source to use.
This can be overridden if a list of all sources is available
to the application. These sources can be enumerated by Twain.
it is not yet supportted by CTwain.
*/
bool CTwain::SelectSource()
{
    memset(&m_Source,0,sizeof(m_Source));
    if(!SourceSelected())
    {
        SelectDefaultSource();
    }
    if(CallTwainProc(&m_AppId,NULL,DG_CONTROL,DAT_IDENTITY,MSG_USERSELECT,&m_Source))
    {
        m_bSourceSelected = true;
    }
    return m_bSourceSelected;
}
/*
Called to select the default source
*/
bool CTwain::SelectDefaultSource()
{
    m_bSourceSelected = CallTwainProc(&m_AppId,NULL,DG_CONTROL,DAT_IDENTITY,MSG_GETDEFAULT,&m_Source);
    return m_bSourceSelected;
}
/*
Closes the Data Source
*/
void CTwain::CloseDS()
{
    if(DSOpen())
    {
        DisableSource();
        CallTwainProc(&m_AppId,NULL,DG_CONTROL,DAT_IDENTITY,MSG_CLOSEDS,(TW_MEMREF)&m_Source);
        m_bDSOpen = false;
    }
}
/*
Closes the Data Source Manager
*/
void CTwain::CloseDSM()
{
    if(DSMOpen())
    {
        CloseDS();
        CallTwainProc(&m_AppId,NULL,DG_CONTROL,DAT_PARENT,MSG_CLOSEDSM,(TW_MEMREF)&m_hMessageWnd);
        m_bDSMOpen = false;
    }
}
/*
Returns true if the Data Source Manager is Open
*/
bool CTwain::DSMOpen() const
{
   //qWarning("DSMOpen");
    return IsValidDriver() && m_bDSMOpen;

}
/*
Returns true if the Data Source is Open
*/
bool CTwain::DSOpen() const
{  //qWarning("DSOpen");
    return IsValidDriver() && DSMOpen() && m_bDSOpen;
}
/*
Opens a Data Source supplied as the input parameter
*/
bool CTwain::OpenSource(TW_IDENTITY *pSource)
{
    if(pSource)
    {
        m_Source = *pSource;
        //qWarning("OpenSource - pSource");
    }
    if(DSMOpen())
    {
      //qWarning("OpenSource - DSMOpen");
        if(!SourceSelected())
        {
        //qWarning("OpenSource - SourceSelected");
            SelectDefaultSource();
        }
        m_bDSOpen = CallTwainProc(&m_AppId,NULL,DG_CONTROL,DAT_IDENTITY,MSG_OPENDS,(TW_MEMREF)&m_Source);
    }
    return DSOpen();
}
/*
Should be called from the main message loop of the application. Can always be called,
it will not process the message unless a scan is in progress.
*/
//bool CTwain::ProcessMessage(MSG msg)
//{
// qWarning("ProcessMessage");
//    if(SourceEnabled())
//    {
//    TW_UINT16  twRC = TWRC_NOTDSEVENT;
//    TW_EVENT twEvent;
//        twEvent.pEvent = (TW_MEMREF)&msg;
//        //memset(&twEvent, 0, sizeof(TW_EVENT));
//        twEvent.TWMessage = MSG_NULL;

//        CallTwainProc(&m_AppId,&m_Source,DG_CONTROL,DAT_EVENT,MSG_PROCESSEVENT,(TW_MEMREF)&twEvent);
//        if(GetRC() != TWRC_NOTDSEVENT)
//        {
//            TranslateMessage(twEvent);
//        }
//        //return false;
//        return (twRC==TWRC_DSEVENT);
//    }
//    return false;
//}
/*
Queries the capability of the Twain Data Source
*/
bool CTwain::GetCapability(TW_CAPABILITY& twCap,TW_UINT16 cap,TW_UINT16 conType)
{
    if(DSOpen())
    {
        twCap.Cap = cap;
        twCap.ConType = conType;
        twCap.hContainer = NULL;
        if(CallTwainProc(&m_AppId,&m_Source,DG_CONTROL,DAT_CAPABILITY,MSG_GET,(TW_MEMREF)&twCap))
        {
            return true;
        }
    }
    return false;
}
/*
Queries the capability of the Twain Data Source
*/
bool CTwain::GetCapability(TW_UINT16 cap,TW_UINT32& value)
{
TW_CAPABILITY twCap;
    if(GetCapability(twCap,cap))
    {
    pTW_ONEVALUE pVal;
        pVal = (pTW_ONEVALUE )GlobalLock(twCap.hContainer);
        if(pVal)
        {
            value = pVal->Item;
            GlobalUnlock(pVal);
            GlobalFree(twCap.hContainer);
            return true;
        }
    }
    return false;
}
/*
Sets the capability of the Twain Data Source
*/
bool CTwain::SetCapability(TW_UINT16 cap,TW_UINT16 value,bool sign)
{
    if(DSOpen())
    {
    TW_CAPABILITY twCap;
    pTW_ONEVALUE pVal;
    bool ret_value = false;
        twCap.Cap = cap;
        twCap.ConType = TWON_ONEVALUE;

        twCap.hContainer = GlobalAlloc(GHND,sizeof(TW_ONEVALUE));
        if(twCap.hContainer)
        {
            pVal = (pTW_ONEVALUE)GlobalLock(twCap.hContainer);
            pVal->ItemType = sign ? TWTY_INT16 : TWTY_UINT16;
            pVal->Item = (TW_UINT32)value;
            GlobalUnlock(twCap.hContainer);
            ret_value = SetCapability(twCap);
            GlobalFree(twCap.hContainer);
        }
        return ret_value;
    }
    return false;
}
/*
Sets the capability of the Twain Data Source
*/
bool CTwain::SetCapability(TW_CAPABILITY& cap)
{
    if(DSOpen())
    {
        return CallTwainProc(&m_AppId,&m_Source,DG_CONTROL,DAT_CAPABILITY,MSG_SET,(TW_MEMREF)&cap);
    }
    return false;
}
/*
Sets the number of images which can be accpeted by the application at one time
*/
bool CTwain::SetImageCount(TW_INT16 nCount)
{
    if(SetCapability(CAP_XFERCOUNT,(TW_UINT16)nCount,true))
    {
        m_nImageCount = nCount;
        return true;
        //qDebug() << 1 <<GetRC();
    }
    else
    {
        //qDebug() << 2<< GetRC();
        if(GetRC() == TWRC_CHECKSTATUS)
        {
        TW_UINT32 count;
            if(GetCapability(CAP_XFERCOUNT,count))
            {
                nCount = (TW_INT16)count;
                if(SetCapability(CAP_XFERCOUNT,nCount))
                {
                    m_nImageCount = nCount;
                    return true;
                }
            }
        }
    }
    return false;
}
/*
Called to enable the Twain Acquire Dialog. This too can be
overridden but is a helluva job .
*/
bool CTwain::EnableSource(bool showUI)
{
    if(DSOpen() && !SourceEnabled() )
    {
        //qWarning("EnableSource - DSOpen et SourceEnabled");
        TW_USERINTERFACE twUI;
        twUI.ShowUI = showUI;
        twUI.hParent = (TW_HANDLE)m_hMessageWnd;
        if(CallTwainProc(&m_AppId,&m_Source,DG_CONTROL,DAT_USERINTERFACE,MSG_ENABLEDS,(TW_MEMREF)&twUI))
        {
         //qWarning("EnableSource - CallTwainProc");
            m_bSourceEnabled = true;
            m_bModalUI = twUI.ModalUI;
        }
        else
        {

            m_bSourceEnabled = false;
            m_bModalUI = true;
        }
        return m_bSourceEnabled;
    }
    return false;
}
/*
Called to acquire images from the source. parameter numImages i the
numberof images that you an handle concurrently
*/
bool CTwain::Acquire(int numImages)
{
    if(DSOpen() || OpenSource())
    {
    //qWarning("1!");
        if(SetImageCount(numImages))
        {
        //qWarning("2!");
            if(EnableSource(false))
            {
            //qWarning("3!");
            //ReleaseTwain();
            TransferImage();
            DisableSource();
//            TW_IMAGEINFO info;
//            GetImageInfo(info);
//            GetImage(info))
            //ReleaseTwain();
                return true;
            }
        }
    }
    return false;
}
/*
 Called to disable the source.
*/
bool CTwain::DisableSource()
{
    if(SourceEnabled())
    {
    TW_USERINTERFACE twUI;
        if(CallTwainProc(&m_AppId,&m_Source,DG_CONTROL,DAT_USERINTERFACE,MSG_DISABLEDS,&twUI))
        {
            m_bSourceEnabled = false;
            return true;
        }
    }
    return false;
}
/*
Called by ProcessMessage to Translate a TWAIN message
*/
//void CTwain::TranslateMessage(TW_EVENT& twEvent)
//{
//    qWarning("something");
//    switch(twEvent.TWMessage)
//    {
//    case MSG_XFERREADY:
//            TransferImage();
//            qWarning("translate1");
//            break;
//    case MSG_CLOSEDSREQ:
//            if(CanClose())
//            {
//                CloseDS();
//            }
//            qWarning("translate2");
//            break;
//    }
//}
/*
Gets Imageinfo for an image which is about to be transferred.
*/
bool CTwain::GetImageInfo(TW_IMAGEINFO& info)
{
    if(SourceEnabled())
    {
        return CallTwainProc(&m_AppId,&m_Source,DG_IMAGE,DAT_IMAGEINFO,MSG_GET,(TW_MEMREF)&info);
    }
    return false;
}

/*
Trasnfers the image or cancels the transfer depending on the state of the
TWAIN system
*/
void CTwain::TransferImage()
{
TW_IMAGEINFO info;
bool bContinue=true;
    while(bContinue)
    {
        if(GetImageInfo(info))
        {
            int permission;
            permission = ShouldTransfer(info);
            switch(permission)
            {
            case TWCPP_CANCELTHIS:
                    bContinue=EndTransfer();
                    break;
            case TWCPP_CANCELALL:
                    CancelTransfer();
                    bContinue=false;
                    break;
            case TWCPP_DOTRANSFER:
                    bContinue=GetImage(info);
                    break;
            }
        }
    }
}
/*
Ends the current transfer.
Returns true if the more images are pending
*/
bool CTwain::EndTransfer()
{
TW_PENDINGXFERS twPend;
    if(CallTwainProc(&m_AppId,&m_Source,DG_CONTROL,DAT_PENDINGXFERS,MSG_ENDXFER,(TW_MEMREF)&twPend))
    {
        return twPend.Count != 0;
    }
    return false;
}
/*
Aborts all transfers
*/
void CTwain::CancelTransfer()
{
TW_PENDINGXFERS twPend;
    CallTwainProc(&m_AppId,&m_Source,DG_CONTROL,DAT_PENDINGXFERS,MSG_RESET,(TW_MEMREF)&twPend);
}
/*
Calls TWAIN to actually get the image
*/
bool CTwain::GetImage(TW_IMAGEINFO& info)
{
HANDLE hBitmap;
    CallTwainProc(&m_AppId,&m_Source,DG_IMAGE,DAT_IMAGENATIVEXFER,MSG_GET,&hBitmap);
    //qWarning("calltwain");
    switch(m_returnCode)
    {
    case TWRC_XFERDONE:
            CopyImage(hBitmap,info);
            break;
    case TWRC_CANCEL:
            break;
    case TWRC_FAILURE:
            CancelTransfer();
            return false;
    }
    GlobalFree(hBitmap);
    return EndTransfer();
}


static unsigned char masktable[] = { 0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01 };
void Create24Bit(CDIB& source,CDIB& dest)
{
    int pal;
    dest.Create(source.Width(),source.Height(),24);
    pal = source.GetPaletteSize();
    BYTE palet[768];
    for(int i=0; i < pal; i++)
    {
        COLORREF col = source.PaletteColor(i);
        palet[i*3+2] = GetRValue(col);
        palet[i*3+1] = GetGValue(col);
        palet[i*3+0] = GetBValue(col);
    }
    int j;
    int i;
    BYTE *src,*dst,*ptr;
    for(i=0; i < source.Height(); i++)
    {
        src = source.GetLinePtr(i);
        dst = dest.GetLinePtr(i);
        ptr = dst;
        int index;
        for(j=0; j < source.Width(); j++,ptr+=3)
        {
            switch(pal)
            {
            case 2:
                if(src[j>>3] & masktable[j&7])
                {
                    index = 1;
                }
                else
                {
                    index = 0;
                }
                break;
            case 16:
                if(j & 1)
                {
                    index = src[j>>1] & 0x0f;
                }
                else
                {
                    index = (src[j>>1] >> 4) & 0x0f;
                }
                break;
            case 256:
                index = src[j];
                break;
            }
            Q_ASSERT(index < pal);
            memcpy(ptr,palet+index*3,3);
        }
        index = (ptr - dst)/3;
        Q_ASSERT(index <= source.Width());
    }
} // !Create24Bit()

HBITMAP convertToHBITMAP(CDIB* pDib)//,const unsigned int nWidth,const unsigned int nHeight)
{
    if (!pDib)
        return 0;
    unsigned int nUseWidth;
    unsigned int nUseHeight;
    nUseWidth = pDib->Width();
    nUseHeight = pDib->Height();
    HANDLE hdib=pDib->DIBHandle();
    pDib->DestroyDIB();
    HPALETTE pepe =CreateDIBPalette(hdib);
    HBITMAP ima=DIBToBitmap(hdib,pepe);
    GlobalFree(hdib);
    return ima;
} // !convertToPixmap()

//QPixmap* convertToPixmap(CDIB* pDib)//,const unsigned int nWidth,const unsigned int nHeight)
//{
//    if (!pDib)
//        return 0;
//    unsigned int nUseWidth;
//    unsigned int nUseHeight;
////	if (nUseWidth == 0)
//    nUseWidth = pDib->Width();
////	if (nUseHeight == 0)
//    nUseHeight = pDib->Height();
//    //QPixmap* retval = new QPixmap(nUseWidth, nUseHeight);
//    HANDLE hdib=pDib->DIBHandle();
//    HPALETTE pepe =CreateDIBPalette(hdib);
//    HBITMAP ima =DIBToBitmap(hdib,pepe);
// //   QPixmap* retval2= new QPixmap(QPixmap::fromWinHBITMAP ( ima,QPixmap::NoAlpha));
//    QPixmap* retval2= new QPixmap();
//    return retval2;
//} // !convertToPixmap()


//QImage* convertToImage(CDIB* pDib, unsigned int nWidth, unsigned int nHeight)
//{
//    if (!pDib)
//        return 0;
//    unsigned int nUseWidth  = nWidth;
//    unsigned int nUseHeight = nHeight;
//    if (nUseWidth == 0)
//        nUseWidth = pDib->Width();
//    if (nUseHeight == 0)
//        nUseHeight = pDib->Height();
//    QPixmap pixmap(pDib->Width(), pDib->Height());

//    QImage* retval = new QImage( pixmap.toImage().scaled(nUseWidth, nUseHeight,Qt::KeepAspectRatio,Qt::SmoothTransformation) );
//    return retval;
//} // !convertToImage()

// internal, implementation of CTwain function
void CTwain::CopyImage(HANDLE hBitmap, TW_IMAGEINFO& info)
{
    //CDIB* dib = new CDIB();
    //if (dib!=NULL)
    //    delete dib;
    CDIB temp_dib;
    temp_dib.CreateFromHandle(hBitmap, info.BitsPerPixel);
    if (info.BitsPerPixel == 24)
        *dib = temp_dib;
    else
        Create24Bit(temp_dib, *dib);
    emit ImageTaken(convertToHBITMAP(dib));
    //qDebug() <<1111;
    //emit pixmapReceived(convertToPixmap(dib));
    //emit pixmapReceived(convertToHBITMAP(dib));

//	if (emitPixmaps())
//	{
//		QPixmap* retval = convertToPixmap(dib);
//		emit pixmapAcquired(retval);
//		delete dib;
//	}
//	else
//	{
//		emit dibAcquired(dib);
//	}
} // !CopyImage()


