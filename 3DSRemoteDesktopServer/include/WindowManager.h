#ifndef _WINDOWMANAGER_H_
#define _WINDOWMANAGER_H_

#include <windows.h>
#include <stdio.h>
#include <Vector>

#include "WindowData.h"

class CWindowManager
{
    friend BOOL CALLBACK enumWindowsProc( HWND hWnd, LPARAM lParam);

    CWindowData* m_pDesktopWindow;
    std::vector<CWindowData*> m_WindowList;
    bool m_bGeneratedWindowList;

public:
    class iterator
    {
        friend CWindowManager;
        CWindowManager* m_pWindowManager;
        std::vector<CWindowData*>::iterator m_pWindowIter;
    protected:
        //iterator();
    public:
        iterator next();
        iterator prev();
        CWindowData* operator->() const;
        bool operator!=(iterator other);
    };

    CWindowData* GetDesktopWindow() { return m_pDesktopWindow;};

    iterator begin();
    iterator end();

    unsigned int size();

    CWindowManager();
    ~CWindowManager();
    void RefreshWindowList();
    void ClearWindowList();

    bool IsGenerated() { return m_bGeneratedWindowList;}

    CWindowData* operator[](int nIndex) const;
};

#endif // _WINDOWMANAGER_H_
