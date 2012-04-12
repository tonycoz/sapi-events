This is a folded, spindled and mutilated version of your code, in
particular look at the do_sapi_stuff() function and TTSAppStatusMessage()
 method for the types of things you'll need to do.

#include "sapistuff.h"

/* this can be C or C++, as long as the interface header above is only C
   
   You can use C++ in XS too, but it's underdocumented, and annoying.

   You might event put all of this in a separate DLL, but then you
   need to link that in too.
*/
// sapi3.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
//#define _ASSERT(x)
//#include <sphelper.h>
//#include <atlbase.h>
#include <windows.h>
#include <stdio.h>
#include <sapi.h>
#define _UNICODE
#include <strsafe.h>

class CTTSHandler : ISpNotifyCallback
{
public:
  CTTSHandler(sapi_cb_t cb, void *ctx);
    ~CTTSHandler(void);
    HRESULT Initialize();
    HRESULT DoSpeak();
    HRESULT Uninitialize();

private:
    HRESULT STDMETHODCALLTYPE NotifyCallback(WPARAM wParam, LPARAM lParam);
    void TTSAppStatusMessage(LPCTSTR str);

  //    CComPtr<ISpAudio>   m_cpOutAudio;
  ISpAudio *m_cpOutAudio;
//    CComPtr<ISpVoice> m_cpVoice;

	ISpVoice *  m_cpVoice;
    HANDLE m_hSpeakDone;
  sapi_cb_t cb;
  void *ctx;;
};

extern "C" void
do_sapi_stuff(const char *text, sapi_cb_t cb, void *ctx)
{




//	puts("foo\n");

  CTTSHandler myobj(cb, ctx);
	myobj.Initialize();
//	myobj.DoSpeak();
	myobj.Uninitialize();


//	system("pause");


//	return 0;
}

CTTSHandler::CTTSHandler(sapi_cb_t cb_, void *ctx_) : m_hSpeakDone(INVALID_HANDLE_VALUE),
						      cb(cb_), ctx(ctx_)
{
}

CTTSHandler::~CTTSHandler(void)
{
}

HRESULT CTTSHandler::Initialize()
{
//    HRESULT hr = m_cpVoice.CoCreateInstance( CLSID_SpVoice );

	    if (FAILED(::CoInitialize(NULL)))
        return FALSE;

 HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&m_cpVoice);

    if ( SUCCEEDED( hr ) )
    {
        SpCreateDefaultObjectFromCategoryId( SPCAT_AUDIOOUT, &m_cpOutAudio );
    }
    if( SUCCEEDED( hr ) )
    {
        hr = m_cpVoice->SetOutput( m_cpOutAudio, FALSE );
    }
    if ( SUCCEEDED( hr ) )
    {
        hr = m_cpVoice->SetNotifyCallbackInterface(this, 0, 0);
    }
    // We're interested in all TTS events
    if( SUCCEEDED( hr ) )
    {
        hr = m_cpVoice->SetInterest( SPFEI_ALL_TTS_EVENTS, SPFEI_ALL_TTS_EVENTS );
    }
    if (SUCCEEDED(hr))
    {
        m_hSpeakDone = ::CreateEvent(NULL, TRUE, FALSE, NULL);     // anonymous event used to wait for speech done
    }

// if (SUCCEEDED(hr))
  //  {
	 hr = m_cpVoice->Speak( L"This is a reasonably long string that should take a while to speak.  This is some more text.", SPF_ASYNC |SPF_IS_NOT_XML, 0 );
// }
	if (FAILED(hr))
    {
        TTSAppStatusMessage(  _T("speak failed\r\n") );
    }
    else
    {
        BOOL fContinue = TRUE;
        while (fContinue)
        {
            DWORD dwWaitId = ::MsgWaitForMultipleObjectsEx(1, &m_hSpeakDone, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            switch (dwWaitId)
            {
            case WAIT_OBJECT_0:
                {
                    fContinue = FALSE;
                }
                break;

            case WAIT_OBJECT_0 + 1:
                {
                    MSG Msg;
                    while (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
                    {
                        ::TranslateMessage(&Msg);
                        ::DispatchMessage(&Msg);
                    }
                }
                break;

            case WAIT_TIMEOUT:
                {
                    hr = S_FALSE;
                    fContinue = FALSE;
                }
                break;

            default:// Unexpected error
                {
		  TTSAppStatusMessage(_T("Unexpected error returned from MsgWaitForMultipleObj"));
                    hr = HRESULT_FROM_WIN32(::GetLastError());
                    fContinue = FALSE;
                }
                break;
            }
        }
    }






    return hr;
}


HRESULT CTTSHandler::Uninitialize()
{
    m_cpVoice = NULL;
    return S_OK;
}


void CTTSHandler::TTSAppStatusMessage(LPCTSTR szMessage )
{
  SEvent evt;
  evt.event_name = szMessage;
    wprintf_s(L"%s", szMessage);
    cb(ctx, &evt);
}

/////////////////////////////////////////////////////////////////
HRESULT STDMETHODCALLTYPE CTTSHandler::NotifyCallback(WPARAM, LPARAM)
/////////////////////////////////////////////////////////////////
//
// Handles the WM_TTSAPPCUSTOMEVENT application defined message and all
// of it's appropriate SAPI5 events.
//
{

  //CSpEvent        event;  // helper class in sphelper.h for events that releases any 
    // allocated memory in it's destructor - SAFER than SPEVENT
  SPEVENT event;
    int             i = 0;
    HRESULT         hr = S_OK;

    while( m_cpVoice->GetEvents(1, &event, NULL) == S_OK )
    {
        switch( event.eEventId )
        {
        case SPEI_START_INPUT_STREAM:
            TTSAppStatusMessage(  _T("StartStream event\r\n") );
            break; 

        case SPEI_END_INPUT_STREAM:
            TTSAppStatusMessage(  _T("EndStream event\r\n") );
            SetEvent(m_hSpeakDone);
            break;     

        case SPEI_VOICE_CHANGE:
            TTSAppStatusMessage(  _T("Voicechange event\r\n") );
            break;

        case SPEI_TTS_BOOKMARK:
            {
                // Get the string associated with the bookmark
                // and add the null terminator.
                TCHAR szBuff2[MAX_PATH] = _T("Bookmark event: ");

                size_t cEventString = wcslen( (const WCHAR *)event.lParam ) + 1;
                WCHAR *pwszEventString = new WCHAR[ cEventString ];
                if ( pwszEventString )
                {
		  wcscpy_s( pwszEventString, cEventString, (const WCHAR *)event.lParam );
		  _tcscat( szBuff2, pwszEventString );
                    delete[] pwszEventString;
                }

                _tcscat( szBuff2, _T("\r\n") );
                TTSAppStatusMessage(  szBuff2 );
            }
            break;

        case SPEI_WORD_BOUNDARY:
            TTSAppStatusMessage(  _T("Wordboundary event\r\n") );
            break;

        case SPEI_PHONEME:
            TTSAppStatusMessage(  _T("Phoneme event\r\n") );
            break;

        case SPEI_VISEME:
            TTSAppStatusMessage(  _T("Viseme event\r\n") );
            break;

        case SPEI_SENTENCE_BOUNDARY:
            TTSAppStatusMessage(  _T("Sentence event\r\n") );
            break;

        case SPEI_TTS_AUDIO_LEVEL:
            WCHAR wszBuff[MAX_PATH];
            swprintf_s(wszBuff, _countof(wszBuff), L"Audio level: %d\r\n", (ULONG)event.wParam);
            TTSAppStatusMessage(  CW2T(wszBuff) );
            break;

        case SPEI_TTS_PRIVATE:
            TTSAppStatusMessage(  _T("Private engine event\r\n") );
            break;

        default:
            TTSAppStatusMessage(  _T("Unknown message\r\n") );
            break;
        }
    }
    return hr;
}
