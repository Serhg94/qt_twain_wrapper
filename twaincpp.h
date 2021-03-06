#ifndef __TWAINCPP_
#define __TWAINCPP_
#include <windows.h>
#include "dib.h"
#include "dibutil.h"
#include "dibapi.h"
#include "twain.h"

#include <QObject>

#define TWCPP_ANYCOUNT		(-1)
#define TWCPP_CANCELTHIS	(1)
#define TWCPP_CANCELALL		(2)
#define TWCPP_DOTRANSFER	(0)

class CTwain : public QObject
{
    Q_OBJECT

public:
	CTwain(HWND hWnd = NULL);
    ~CTwain();
	bool InitTwain(HWND hWnd);
	void ReleaseTwain();
	/*  
	  This routine must be implemented by the dervied class 
	  After setting the required values in the m_AppId structure,
	  the derived class should call the parent class implementation
	  Refer Pg: 51 of the Twain Specification version 1.8
	*/
    CDIB* dib = NULL;
    HBITMAP map;


    void GetIdentity();
    bool SelectSource();
    bool OpenSource(TW_IDENTITY *pSource=NULL);
    int  ShouldTransfer(TW_IMAGEINFO& info) { return TWCPP_DOTRANSFER;}
	
	bool ProcessMessage(MSG msg);
	bool SelectDefaultSource();
	bool IsValidDriver() const;
    bool SourceSelected() const {return m_bSourceSelected;}
	bool DSMOpen() const;
	bool DSOpen() const;
    bool SourceEnabled() const { return m_bSourceEnabled;}
    bool ModalUI() const { return m_bModalUI; }
    TW_INT16 GetRC() const { return m_returnCode; }
    TW_STATUS GetStatus() const { return m_Status; }
	bool SetImageCount(TW_INT16 nCount = 1);
	bool Acquire(int numImages=1);
protected:
	bool CallTwainProc(pTW_IDENTITY pOrigin,pTW_IDENTITY pDest,
					   TW_UINT32 DG,TW_UINT16 DAT,TW_UINT16 MSG,
					   TW_MEMREF pData);
	void CloseDSM();
	void CloseDS();
	bool GetCapability(TW_CAPABILITY& twCap,TW_UINT16 cap,TW_UINT16 conType=TWON_DONTCARE16);
	bool GetCapability(TW_UINT16 cap,TW_UINT32& value);
	bool SetCapability(TW_UINT16 cap,TW_UINT16 value,bool sign=false);
	bool SetCapability(TW_CAPABILITY& twCap);
	bool EnableSource(bool showUI = true);
	bool GetImageInfo(TW_IMAGEINFO& info);
    bool DisableSource();
    //virtual bool CanClose()  { return true; }
    //void TranslateMessage(TW_EVENT& twEvent);
	void TransferImage();
	bool EndTransfer();
	void CancelTransfer();
	bool ShouldContinue();
	bool GetImage(TW_IMAGEINFO& info);
    void CopyImage(HANDLE hBitmap,TW_IMAGEINFO& info);
protected:
	HINSTANCE m_hTwainDLL;
	DSMENTRYPROC m_pDSMProc;
	TW_IDENTITY m_AppId;
	TW_IDENTITY m_Source;
	TW_STATUS m_Status;
	TW_INT16  m_returnCode;
	HWND m_hMessageWnd;
	bool m_bSourceSelected;
	bool m_bDSMOpen;
	bool m_bDSOpen;
	bool m_bSourceEnabled;
	bool m_bModalUI;
	int m_nImageCount;
signals:
    void ImageTaken(HBITMAP scan);
};
#endif
