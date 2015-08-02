//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   xlMenu.h
//    Author:      Streamlet
//    Create Time: 2011-05-08
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------

#ifndef __XLMENU_H_C208C619_17E7_4147_BBFD_C815FAFA7324_INCLUDED__
#define __XLMENU_H_C208C619_17E7_4147_BBFD_C815FAFA7324_INCLUDED__


#include "../../Common/Meta/xlUtility.h"
#include "../xlWin32Ver.h"
#include <Windows.h>

namespace xl
{
    class Menu : public NonCopyable
    {
    public:
        Menu() :
            m_hMenu(nullptr)
        {

        }

        Menu(HMENU hMenu) :
            m_hMenu(nullptr)
        {
            Attach(hMenu);
        }

        ~Menu()
        {
            Destroy();
            Detach();
        }

        bool Create()
        {
            if (m_hMenu != nullptr)
            {
                return false;
            }

            m_hMenu = CreateMenu();

            if (m_hMenu == nullptr)
            {
                return false;
            }

            return true;
        }

        bool CreatePopup()
        {
            if (m_hMenu != nullptr)
            {
                return false;
            }

            m_hMenu = CreatePopupMenu();

            if (m_hMenu == nullptr)
            {
                return false;
            }

            return true;
        }

        bool Destroy()
        {
            if (m_hMenu == nullptr)
            {
                return false;
            }

            if (!DestroyMenu(m_hMenu))
            {
                return false;
            }

            return true;
        }

        bool Attach(HMENU hMenu)
        {
            if (m_hMenu != nullptr)
            {
                return false;
            }

            m_hMenu = hMenu;

            return true;
        }

        HMENU Detach()
        {
            HMENU hMenu = m_hMenu;

            if (m_hMenu != nullptr)
            {
                m_hMenu = nullptr;
            }

            return hMenu;
        }

        operator HMENU() const
        {
            return m_hMenu;
        }

    protected:
        HMENU m_hMenu;
    };

} // namespace xl


#endif // #ifndef __XLMENU_H_C208C619_17E7_4147_BBFD_C815FAFA7324_INCLUDED__
