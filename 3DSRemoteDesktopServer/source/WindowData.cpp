#include "WindowData.h"

#include <windows.h>

CWindowData::CWindowData() {}
CWindowData::CWindowData(HWND hWnd)
{
    int length = GetWindowTextLength( hWnd );
    m_szWindowText = new char[ length + 1 ];
    memset( m_szWindowText, 0, ( length + 1 ) * sizeof( char ) );

    m_WindowHandle = hWnd;
    GetWindowText( m_WindowHandle, m_szWindowText, length + 1 );
    UpdateWindowInfo();
}

CWindowData::~CWindowData()
{
    if(m_szWindowText)
    {
        free(m_szWindowText);
    }
}


void CWindowData::UpdateWindowInfo()
{
    GetClientRect(m_WindowHandle, &m_ScreenRect);
    memcpy(&m_ScreenPos, &m_ScreenRect, sizeof(RECT));
    MapWindowPoints(GetParent(m_WindowHandle), (m_WindowHandle), (LPPOINT)&m_ScreenPos, 2);

    m_ScreenRect.left -= m_ScreenPos.left;
    m_ScreenRect.top -= m_ScreenPos.top;
    m_ScreenRect.right -= m_ScreenPos.left;
    m_ScreenRect.bottom -= m_ScreenPos.top;
}


void CWindowData::ForceWindowOnTop(bool bForce)
{
    UpdateWindowInfo();
    if(bForce)
    {
        SetFocus(m_WindowHandle);
        SetWindowPos(m_WindowHandle, HWND_TOPMOST,
                 m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right - m_ScreenRect.left, m_ScreenRect.bottom - m_ScreenRect.top,
                 0);
    }
    else
    {
        SetWindowPos(m_WindowHandle, HWND_NOTOPMOST,
                 m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right - m_ScreenRect.left, m_ScreenRect.bottom - m_ScreenRect.top,
                 0);
    }

}
