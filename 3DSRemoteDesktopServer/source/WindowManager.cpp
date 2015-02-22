#include "WindowManager.h"

#include <windows.h>

#include "WindowData.h"

BOOL CALLBACK enumWindowsProc( HWND hWnd, LPARAM lParam)
{
    CWindowManager* pWM = (CWindowManager*)lParam;
    int length = GetWindowTextLength( hWnd );
    if( 0 == length )
    {
        return true;
    }

    if(!IsWindowVisible(hWnd))
    {
        return true;
    }

    CWindowData* pDesc = new CWindowData(hWnd);
    pWM->m_WindowList.push_back(pDesc);

    return true;
}


CWindowManager::CWindowManager()
{
    m_pDesktopWindow = new CWindowData(::GetDesktopWindow());
    m_bGeneratedWindowList = false;
}

CWindowManager::~CWindowManager()
{
    delete m_pDesktopWindow;
}

void CWindowManager::RefreshWindowList()
{
    EnumWindows( enumWindowsProc, (LPARAM)this);
    m_bGeneratedWindowList = true;
}

void CWindowManager::ClearWindowList()
{
    while(m_WindowList.size() > 0)
    {
        delete m_WindowList.back();
        m_WindowList.pop_back();
    }
    m_bGeneratedWindowList = false;
}


CWindowManager::iterator CWindowManager::iterator::next()
{
    iterator iter;
    iter.m_pWindowManager = m_pWindowManager;
    iter.m_pWindowIter = ++m_pWindowIter;
    return iter;
}

CWindowManager::iterator CWindowManager::iterator::prev()
{
    iterator iter;
    iter.m_pWindowManager = m_pWindowManager;
    iter.m_pWindowIter = --m_pWindowIter;
    return iter;
}

CWindowData* CWindowManager::iterator::operator->() const
{
    return (*m_pWindowIter);
}

bool CWindowManager::iterator::operator!=(CWindowManager::iterator other)
{
    return other.m_pWindowIter != m_pWindowIter;
}

CWindowManager::iterator CWindowManager::begin()
{
    iterator iter;
    iter.m_pWindowManager = this;
    iter.m_pWindowIter = m_WindowList.begin();
    return iter;
}

CWindowManager::iterator CWindowManager::end()
{
    iterator iter;
    iter.m_pWindowManager = this;
    iter.m_pWindowIter = m_WindowList.end();
    return iter;
}

unsigned int CWindowManager::size()
{
    return m_WindowList.size();
}

CWindowData* CWindowManager::operator[](int nIndex) const
{
    return m_WindowList[nIndex];
}
