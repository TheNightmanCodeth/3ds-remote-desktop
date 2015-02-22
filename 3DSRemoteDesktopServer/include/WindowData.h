#ifndef _WINDOWDATA_H_
#define _WINDOWDATA_H_

#include <windef.h>

class CWindowData
{
    HWND m_WindowHandle;
    char* m_szWindowText;
    RECT m_ScreenPos;
	RECT m_ScreenRect;

	unsigned int m_unWindowWidth;
	unsigned int m_unWindowHeight;

    CWindowData();
public:
    CWindowData(HWND hWnd);
    ~CWindowData();

    void UpdateWindowInfo();
	void ForceWindowOnTop(bool bForce);

    HWND GetHandle() { return m_WindowHandle;}
    const char* const GetWindowText() { return m_szWindowText;}
    RECT GetWindowRect() { return m_ScreenRect;}

    unsigned int GetWidth() { return m_unWindowWidth;}
    unsigned int GetHeight() { return m_unWindowHeight;}
};

#endif // _WINDOWDATA_H_
