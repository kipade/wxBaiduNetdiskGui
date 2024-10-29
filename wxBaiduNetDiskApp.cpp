/***************************************************************
 * Name:      wxBaiduNetDiskApp.cpp
 * Purpose:   Code for Application Class
 * Author:     ()
 * Created:   2024-10-27
 * Copyright:  ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#include "wxBaiduNetDiskApp.h"
#include "wxBaiduNetDiskMain.h"

IMPLEMENT_APP(wxBaiduNetDiskApp);

bool wxBaiduNetDiskApp::OnInit()
{
    wxBaiduNetDiskFrame* frame = new wxBaiduNetDiskFrame(0L, _("wxWidgets Application Template"));
    
    frame->Show();
    
    return true;
}
