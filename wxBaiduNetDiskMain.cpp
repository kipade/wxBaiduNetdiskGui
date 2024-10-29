/***************************************************************
 * Name:      wxBaiduNetDiskMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2024-10-27
 * Copyright:  ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#include "wxBaiduNetDiskMain.h"
#include <wx/txtstrm.h>
#include <wx/filename.h>
#include <map>
#include "icon1.xpm"
#include "icon3.xpm"

//helper functions
enum wxbuildinfoformat
{
    short_f, long_f
};

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

BEGIN_EVENT_TABLE(wxBaiduNetDiskFrame, wxFrame)
    EVT_CLOSE(wxBaiduNetDiskFrame::OnClose)
    EVT_MENU(idMenuQuit, wxBaiduNetDiskFrame::OnQuit)
    EVT_MENU(idMenuAbout, wxBaiduNetDiskFrame::OnAbout)
    EVT_LIST_ITEM_ACTIVATED(wxID_ANY, wxBaiduNetDiskFrame::OnListItemActived)
    EVT_LIST_ITEM_CHECKED(wxID_ANY, wxBaiduNetDiskFrame::OnListItemChecked)
    EVT_LIST_ITEM_UNCHECKED(wxID_ANY, wxBaiduNetDiskFrame::OnListItemUnChecked)
    EVT_BUTTON(id_btn_dir_prev, wxBaiduNetDiskFrame::OnBtnDirectoryUp)
    EVT_BUTTON(id_btn_download, wxBaiduNetDiskFrame::OnBtnDownload)
    EVT_BUTTON(id_btn_upload, wxBaiduNetDiskFrame::OnBtnUploadFile)
    EVT_BUTTON(id_btn_upload_dir, wxBaiduNetDiskFrame::OnBtnUploadDir)
    EVT_BUTTON(id_btn_mkdir, wxBaiduNetDiskFrame::OnBtnMkdir)
    EVT_BUTTON(id_btn_rm, wxBaiduNetDiskFrame::OnBtnRm)
    EVT_CHECKBOX(id_btn_sel_all, wxBaiduNetDiskFrame::OnBtnSelectAll)
END_EVENT_TABLE()

wxBaiduNetDiskFrame::wxBaiduNetDiskFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title)
{
//auto func = std::bind(&wxBaiduNetDiskFrame::print_message, this, std::placeholders::_1);
    // auto func = std::bind(&wxBaiduNetDiskFrame::ProcessNetdiskRespLs, this, std::placeholders::_1);
    m_pcsRespProcessor[wxT("ls")] =std::bind(&wxBaiduNetDiskFrame::ProcessNetdiskRespLs, this, std::placeholders::_1);
    m_pcsRespProcessor[wxT("pwd")] =std::bind(&wxBaiduNetDiskFrame::ProcessNetdiskRespPwd, this, std::placeholders::_1);
#if wxUSE_MENUS
    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
    mbar->Append(fileMenu, _("&File"));

    wxMenu* helpMenu = new wxMenu(_T(""));
    helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
    mbar->Append(helpMenu, _("&Help"));

    SetMenuBar(mbar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(2);
    SetStatusText(_("Hello Code::Blocks user!"),0);
    SetStatusText(wxbuildinfo(short_f), 1);
#endif // wxUSE_STATUSBAR

    m_imagesSmall.push_back( wxIcon( icon1_xpm) );
    m_imagesSmall.push_back( wxIcon( icon3_xpm) );

    wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

    //路径
    wxBoxSizer *pathSizer = new wxBoxSizer(wxHORIZONTAL);
    pathSizer->Add(new wxStaticText(this, wxID_ANY, wxT("当前路径:")));
    txt_path = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    btn_path_prev = new wxButton(this, id_btn_dir_prev, wxT("向上一层"));
    btn_download = new wxButton(this, id_btn_download, wxT("下载"));
    btn_upload = new wxButton(this, id_btn_upload, wxT("上传文件"));
    btn_upload_dir = new wxButton(this, id_btn_upload_dir, wxT("上传目录"));
    btn_mkdir = new wxButton(this, id_btn_mkdir, wxT("创建目录"));
    btn_rm = new wxButton(this, id_btn_rm, wxT("删除项目"));
    chk_sel_all = new wxCheckBox(this, id_btn_sel_all, wxT(""));
    pathSizer->Add(txt_path, 1, wxRIGHT, 1);
    topSizer->Add(pathSizer, 0, wxEXPAND|wxALL, 5);

    wxBoxSizer *btn_sizer = new wxBoxSizer(wxHORIZONTAL);
    btn_sizer->Add(chk_sel_all, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL, 12);
    btn_sizer->Add(btn_path_prev);
    btn_sizer->Add(btn_download);
    btn_sizer->Add(btn_upload);
    btn_sizer->Add(btn_upload_dir);
    btn_sizer->Add(btn_mkdir);
    btn_sizer->Add(btn_rm);

    topSizer->Add(btn_sizer);




    //m_panel = new wxPanel(this, wxID_ANY);
    m_logWindow = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_READONLY | wxTE_MULTILINE | wxSUNKEN_BORDER);

    m_logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_logWindow));

    RecreateList(wxLC_REPORT | wxLC_SINGLE_SEL);

    // Make the list control big enough to show its initial contents.
    //m_listCtrl->SetInitialSize(FromDIP(wxSize(600, 300)));

    topSizer->Add(m_listCtrl, 1, wxEXPAND, 1);
    topSizer->Add(m_logWindow, 0, wxEXPAND);

    ProcessNetdiskLs();

    SetMinClientSize(wxSize(1200, 760));

    SetSizer(topSizer);
    topSizer->Fit(this);
}

void wxBaiduNetDiskFrame::RecreateList(long flags, bool withText)
{
    // we could avoid recreating it if we don't set/clear the wxLC_VIRTUAL
    // style, but it is more trouble to do it than not
    {

        m_listCtrl = new MyListCtrl(this, LIST_CTRL,
                                    wxDefaultPosition, wxDefaultSize,
                                    flags |
                                    wxBORDER_THEME | wxLC_EDIT_LABELS);

        m_listCtrl->EnableCheckBoxes(true);

        switch ( flags & wxLC_MASK_TYPE )
        {
        /*         case wxLC_LIST:
                     InitWithListItems();
                     break;

                 case wxLC_ICON:
                     InitWithIconItems(withText);
                     break;

                 case wxLC_SMALL_ICON:
                     InitWithIconItems(withText, true);
                     break;
        */
        case wxLC_REPORT:
            /*if ( flags & wxLC_VIRTUAL )
                InitWithVirtualItems();
            else*/
            InitWithReportItems();
            break;

        default:
            wxFAIL_MSG( "unknown listctrl mode" );
        }

    }
    m_logWindow->Clear();
}

void wxBaiduNetDiskFrame::InitWithReportItems()
{
    m_listCtrl->SetSmallImages(m_imagesSmall);

    // note that under MSW for SetColumnWidth() to work we need to create the
    // items with images initially even if we specify dummy image id
    wxListItem itemCol;
    itemCol.SetText(wxT(""));
    itemCol.SetImage(-1);
    itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
    m_listCtrl->InsertColumn(0, itemCol);
    m_listCtrl->SetColumnWidth( 0, 40 );

    itemCol.SetText(wxT("文件名"));
    itemCol.SetAlign(wxLIST_FORMAT_LEFT);
    m_listCtrl->InsertColumn(1, itemCol);
    m_listCtrl->SetColumnWidth( 1, 400 );

    itemCol.SetText(wxT("修改时间"));
    itemCol.SetImage(-1);
    itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
    m_listCtrl->InsertColumn(2, itemCol);

    itemCol.SetText(wxT("类型"));
    itemCol.SetImage(-1);
    m_listCtrl->InsertColumn(3, itemCol);

    itemCol.SetText(wxT("大小"));
    itemCol.SetImage(-1);
    m_listCtrl->InsertColumn(4, itemCol);


#if 0
    int m_numListItems = 2;
    if ( m_numListItems <= 0 )
    {
        m_listCtrl->SetColumnWidth( 0, 100 );
        m_listCtrl->SetColumnWidth( 1, wxLIST_AUTOSIZE );
        m_listCtrl->SetColumnWidth( 2, wxLIST_AUTOSIZE_USEHEADER );

        return;
    }

    // to speed up inserting we hide the control temporarily
    m_listCtrl->Hide();

    wxStopWatch sw;

    for ( int i = 0; i < m_numListItems; i++ )
    {
        m_listCtrl->InsertItemInReportView(i);
    }

    m_logWindow->WriteText(wxString::Format("%d items inserted in %ldms\n",
                                            m_numListItems, sw.Time()));
    m_listCtrl->Show();

    // we leave all mask fields to 0 and only change the colour
    wxListItem item;
    item.m_itemId = 0;
    item.SetTextColour(*wxRED);
    m_listCtrl->SetItem( item );

    if ( m_numListItems > 2 )
    {
        item.m_itemId = 2;
        item.SetTextColour(*wxGREEN);
        m_listCtrl->SetItem( item );
    }

    if ( m_numListItems > 4 )
    {
        item.m_itemId = 4;
        item.SetTextColour(*wxLIGHT_GREY);
        item.SetFont(*wxITALIC_FONT);
        item.SetBackgroundColour(*wxRED);
        m_listCtrl->SetItem( item );
    }

    m_listCtrl->SetTextColour(*wxBLUE);

    if ( m_numListItems > 1 )
    {
        // Set images in columns
        m_listCtrl->SetItemColumnImage(1, 1, 0);
    }

    if ( m_numListItems > 3 )
    {
        wxListItem info;
        info.SetImage(0);
        info.SetId(3);
        info.SetColumn(2);
        m_listCtrl->SetItem(info);
    }

    // test SetItemFont too
    m_listCtrl->SetItemFont(0, *wxITALIC_FONT);
#endif
    m_listCtrl->SetColumnWidth( 0, 40 );
    m_listCtrl->SetColumnWidth( 1, 440 );
    m_listCtrl->SetColumnWidth( 2, 200 );
    m_listCtrl->SetColumnWidth( 3, 80 );

//    m_listCtrl->SetColumnWidth( 2, wxLIST_AUTOSIZE_USEHEADER );
}


wxBaiduNetDiskFrame::~wxBaiduNetDiskFrame()
{
}

void wxBaiduNetDiskFrame::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void wxBaiduNetDiskFrame::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void wxBaiduNetDiskFrame::OnAbout(wxCommandEvent &event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

static bool MakeSurePwdResp(const wxString& resp)
{
    return resp.Length() > 0 && resp.at(0) == '/';
}

static bool MakeSureLsResp(const wxString& resp)
{
    return resp.Length() > 1 && resp.Find(wxT("当前目录: ")) != wxNOT_FOUND;
}

typedef bool (*PFCheckRespStr)(const wxString&);

std::map<wxString, PFCheckRespStr> RespChecker =
{
    {wxT("pd"), MakeSurePwdResp},
    {wxT("ls"), MakeSureLsResp}
};

bool wxBaiduNetDiskFrame::CheckNetdiskResp(const wxString& cmd, const wxString& resp)
{
    auto it = RespChecker.find(cmd);
    if(it != RespChecker.end())
    {
        return it->second(resp);
    }
    return false;
}

bool wxBaiduNetDiskFrame::ExecuteNetdiskCmd(const wxString& cmd, wxString& resp)
{
    wxProcess *process { new wxProcess(wxPROCESS_REDIRECT) };
    wxExecute(wxT("baidupcs ") + cmd, wxEXEC_ASYNC, process);
    wxInputStream *iStream { process->GetInputStream() };
    //wxString resp;
    wxTextInputStream tiStream { *iStream };
    resp = tiStream.ReadLine();
    //*outputCtrl << output + "\n\n";
    while (!iStream->Eof())
    {
        resp.Append(wxT("\n")).Append(tiStream.ReadLine());
    }
    //wxMessageBox(resp);
    return CheckNetdiskResp(cmd, resp);
}

bool wxBaiduNetDiskFrame::ProcessNetdiskRespLs(const wxString& resp)
{
    const wxString start_str = wxT("当前目录: ");
    size_t pos = resp.Find(start_str);

    if(pos != (size_t)wxNOT_FOUND)
    {
        pos += start_str.Length();
        do
        {
            auto ch = resp.at(pos);
            if(ch == ' ' || ch == '\t')
            {
                pos ++;
            }
            else
            {
                break;
            }
        }
        while(pos < resp.Length());
        auto pos2 = pos;
        while(pos2 < resp.Length())
        {
            auto ch = resp.at(pos2);
            if(ch == ' ' || ch == '\t' || ch == '\n')
            {
                break;
            }
            pos2++;
        }
        m_listCtrl->DeleteAllItems();
        if(pos != pos2)
        {
            auto cur_path = resp.Mid(pos, pos2 - pos);
            txt_path->SetValue(cur_path);
            pos = resp.find_first_of('\n', pos2);
            if(pos != (size_t)wxNOT_FOUND)
            {
                pos++;
                while(pos < resp.Length())
                {
                    wxString line;
                    auto pos2 = resp.find_first_of('\n', pos);
                    if(pos2 != (size_t)wxNOT_FOUND)
                    {
                        line = resp.Mid(pos, pos2-pos);
                    }
                    else
                    {
                        line = resp.Mid(pos);
                    }
                    InsertNewFileItem(line);
                    pos = pos2 + 1;
                }
            }
        }


    }
    return false;
}

bool wxBaiduNetDiskFrame::InsertNewFileItem(const wxString& line)
{
    size_t pos = 0;
    while(pos < line.Length())
    {
        auto ch = line.at(pos);
        if(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
        {
            pos ++;
        }
        else
        {
            break;
        }
    }
    if(pos < line.Length())
    {
        auto ch = line.at(pos);
        if(ch >= '0' && ch <= '9')
        {
            auto pos2 = pos;
            while(pos2 < line.Length() && (line.at(pos2) >= '0' && line.at(pos2) <= '9')) pos2++;
            if(pos2 < line.Length())
            {
                wxString index = line.Mid(pos, pos2 - pos);
                pos = line.find_first_not_of(' ', pos2);
                if(pos != (size_t)wxNOT_FOUND)
                {
                    pos2 = line.find_first_of(' ', pos + 1);
                    if(pos2 != (size_t)wxNOT_FOUND)
                    {
                        wxString size = line.Mid(pos, pos2 - pos);

                        pos = line.find_first_not_of(' ', pos2 + 1);
                        if(pos != (size_t)wxNOT_FOUND)
                        {
                            pos2 = line.find_first_of(' ', pos + 1);
                            if(pos2 != (size_t)wxNOT_FOUND)
                            {
                                pos2 = line.find_first_not_of(' ', pos2 + 1);
                                if(pos2 != (size_t)wxNOT_FOUND)
                                {
                                    pos2 = line.find_first_of(' ', pos2 + 1);
                                    wxString date = line.Mid(pos, pos2 - pos);

                                    pos = line.find_first_not_of(' ', pos2 + 1);
                                    if(pos != (size_t)wxNOT_FOUND)
                                    {
                                        wxString name;
                                        pos2 = line.find_first_of(' ', pos + 1);
                                        if(pos2 == (size_t)wxNOT_FOUND)
                                        {
                                            name = line.Mid(pos, pos2 - pos);
                                        }
                                        else
                                        {
                                            pos2 = line.find_first_of('\n', pos2 + 1);
                                            if(pos2 != (size_t)wxNOT_FOUND)
                                            {
                                                name = line.Mid(pos, pos2 - pos);
                                            }
                                            else
                                            {
                                                name = line.Mid(pos);
                                            }
                                        }
                                        int icon_idx = 0;
                                        name.Trim();
                                        wxString type = wxT("文件");
                                        if(size.Cmp(wxT("-")) == 0)
                                        {
                                            icon_idx = 1;
                                            type = wxT("目录");

                                            if(name.at(name.Length() - 1) == '/')
                                            {
                                                name = name.Left(name.Length() - 1);
                                            }
                                        }
                                        auto item_idx = m_listCtrl->GetItemCount();
                                        long new_id = m_listCtrl->InsertItem(item_idx, name, 1);
                                        m_listCtrl->SetItem(new_id, 1, name);
                                        m_listCtrl->SetItem(new_id, 2, date);
                                        m_listCtrl->SetItem(new_id, 3, type);
                                        m_listCtrl->SetItem(new_id, 4, size);
                                        m_listCtrl->SetItemImage(new_id, icon_idx, 1);
                                        m_listCtrl->SetItemData(new_id, icon_idx);

                                        wxListItem info;
                                        info.SetImage(icon_idx);
                                        info.SetId(new_id);
                                        info.SetColumn(1);
                                        m_listCtrl->SetItem(info);


                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool wxBaiduNetDiskFrame::ProcessNetdiskRespPwd(const wxString& resp)
{
    return false;
}
/*
std::map<wxString, std::function<bool(const wxString&)>> wxBaiduNetDiskFrame::m_pcsRespProcessor = {
    //{wxT("ls"), &wxBaiduNetDiskFrame::ProcessNetdiskRespLs}
    {("ls"), &wxBaiduNetDiskFrame::ProcessNetdiskRespLs}
};*/
bool wxBaiduNetDiskFrame::ProcessNetdiskResp(const wxString& cmd, const wxString& resp)
{
    auto it = m_pcsRespProcessor.find(cmd);
    if(it != m_pcsRespProcessor.end())
    {
        return it->second(resp);
    }
    return false;
}

void wxBaiduNetDiskFrame::ProcessNetdiskLs()
{
    wxString resp;
    wxString cmd = wxT("ls");
    bool ret = ExecuteNetdiskCmd(cmd, resp);
    if(ret == true)
    {
        ProcessNetdiskResp(cmd, resp);
    }
    checked_items_cnt = 0;
}

void wxBaiduNetDiskFrame::OnListItemActived(wxListEvent& event)
{
    auto item = event.GetItem();
    wxString name = item.GetText();
    long icon_idx = item.GetData();
    if(icon_idx == 1)
    {
        wxString new_cmd = wxT("cd ") + txt_path->GetValue() + wxT("/") + name;
        wxString resp;
        ExecuteNetdiskCmd(new_cmd, resp);
        ProcessNetdiskLs();
    }
    else
    {
        wxString cmd = wxT("d ") + name;
        wxString resp;
        ExecuteNetdiskCmd(cmd, resp);
        if(resp.Find(wxT("下载结束") ) != wxNOT_FOUND)
        {
            wxMessageBox(wxT("下载完成,文件保存在下载目录"), wxT("成功") );
        }
        else
        {
            wxMessageBox(wxT("下载失败,请检查磁盘是否可用,或者当前网络!"), wxT("失败"), wxYES|wxICON_ERROR);
        }
    }
}

void wxBaiduNetDiskFrame::OnBtnDirectoryUp(wxCommandEvent& event)
{
    wxString resp;
    wxString cmd = wxT("cd ..");
    ExecuteNetdiskCmd("cd ..", resp);
    ProcessNetdiskLs();
}

void wxBaiduNetDiskFrame::OnBtnDownload(wxCommandEvent& event)
{

    int success = 0;
    auto n = m_listCtrl->GetItemCount();
    for(int i = 0; i < n; ++i)
    {
        // auto item = m_listCtrl->GetItem(i);
        if (m_listCtrl->IsItemChecked(i))
        {
            success += (DownloadItem(i) == true ? 1 : 0);
        }
    }
    wxString msg = wxString::Format(wxT("任务完成, %d 项成功下载"), success);
    wxMessageBox(msg);

}

bool wxBaiduNetDiskFrame::DownloadItem(long i)
{
    wxListItem currentItem;
    currentItem.SetId(i);
    if (m_listCtrl->GetItem(currentItem))
    {
        wxString name = currentItem.GetText();
        //wxLogInfo("Item %d Text: %s", i, name);
        wxString cmd = wxT("d \"") + name + wxT("\"");
        wxString resp;
        ExecuteNetdiskCmd(cmd, resp);
         if(resp.Find(wxT("下载结束") ) != wxNOT_FOUND)
         {
            return true;
         }

        // 如果需要获取其他列的数据，可以类似地使用SetId和GetItem
        // 例如获取第二列的数据
        /*currentItem.SetId(i);
        currentItem.SetColumn(1);
        if (listCtrl->GetItem(currentItem))
        {
            wxString column2Text = currentItem.GetText();
            wxLogInfo("Item %d Column 2 Text: %s", i, column2Text);
        }*/
    }
    return false;
}

void wxBaiduNetDiskFrame::OnBtnUploadFile(wxCommandEvent& event)
{
    wxString wildcards =
                    wxString::Format
                    (
                        wxT("所有文件 (%s)|%s"),
                        wxFileSelectorDefaultWildcardStr,
                        wxFileSelectorDefaultWildcardStr
                    );
    wxFileDialog dialog(this, wxT("选择需上传的文件..."),
                        wxEmptyString, wxEmptyString, wildcards,
                        wxFD_OPEN|wxFD_MULTIPLE);

    dialog.Centre(wxCENTER_ON_SCREEN);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxArrayString paths, filenames;

        dialog.GetPaths(paths);
        //dialog.GetFilenames(filenames);

        wxString msg(wxT("u ")), s;
        size_t count = paths.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            s.Printf("\"%s\" ", paths[n]);
            msg += s;
        }
        msg.Append(txt_path->GetValue());
        wxString resp;
        ExecuteNetdiskCmd(msg, resp);
        //wxMessageBox(resp);
        if(resp.Find(wxT("上传文件成功")) != wxNOT_FOUND)
        {
            wxMessageBox(wxT("上传成功(若多选,则可能有部分文件上传失败,请复查)! "));
            ProcessNetdiskLs();
        }

    }
}

void wxBaiduNetDiskFrame:: OnBtnUploadDir(wxCommandEvent& event)
{
    wxString dirHome;
    wxGetHomeDir(&dirHome);

    wxDirDialog dialog(this, "Testing directory picker", dirHome, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString cmd = wxT("u \"") + dialog.GetPath() + wxT("\"  \"") + txt_path->GetValue() + wxT("\"");
        wxString resp;
        ExecuteNetdiskCmd(cmd, resp);
        if(resp.Find(wxT("上传文件成功")) != wxNOT_FOUND)
        {
            wxMessageBox(wxT("上传目录成功!"));
            if(resp.Find(wxT("上传文件失败")) != wxNOT_FOUND)
            {
                wxMessageBox(wxT("虽然目录上传成功,但存在部分文件上传失败,请核查后重试"), wxT("提示"), wxYES|wxICON_WARNING);
            }
            wxFileName fn(dialog.GetPath());
            cmd = wxT("cd \"") + fn.GetName() + wxT("\"");
            ExecuteNetdiskCmd(cmd, resp);
            ProcessNetdiskLs();
        }
    }
}

void wxBaiduNetDiskFrame:: OnBtnMkdir(wxCommandEvent& event)
{
        wxTextEntryDialog dialog(this,
                             wxT("将在当前目录中创建新目录, 请输入目录名:"),
                             wxT("请输入目录名"),
                             "",
                             wxOK | wxCANCEL);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString cmd = wxT("mkdir \"") + dialog.GetValue() + wxT("\"");
        wxString resp;
        ExecuteNetdiskCmd(cmd, resp);
        if(resp.Find(wxT("创建目录成功")) != wxNOT_FOUND)
        {
            wxMessageBox(wxT("创建新目录成功!"));
            cmd = wxT("cd \"") + dialog.GetValue() + wxT("\"");
            ExecuteNetdiskCmd(cmd, resp);
            ProcessNetdiskLs();
        }
        else
        {
            wxMessageBox(wxT("创建目录失败, 请检查目标是否已经存在或者网络"), wxT("错误"), wxOK | wxICON_ERROR);
        }

    }
}

void wxBaiduNetDiskFrame::OnBtnRm(wxCommandEvent& event)
{
    int success = 0;
    auto n = m_listCtrl->GetItemCount();
    for(int i = 0; i < n; ++i)
    {
        // auto item = m_listCtrl->GetItem(i);
        if (m_listCtrl->IsItemChecked(i))
        {
            success += (RemoveItem(i) == true ? 1 : 0);
        }
    }
    wxString msg = wxString::Format(wxT("任务完成, %d 项成功删除"), success);
    wxMessageBox(msg);
    if(success > 0)
    {
        ProcessNetdiskLs();
    }
}

bool wxBaiduNetDiskFrame::RemoveItem(long i)
{
    wxListItem currentItem;
    currentItem.SetId(i);
    if (m_listCtrl->GetItem(currentItem))
    {
        wxString name = currentItem.GetText();
        //wxLogInfo("Item %d Text: %s", i, name);
        wxString cmd = wxT("rm \"") + name + wxT("\"");
        wxString resp;
        ExecuteNetdiskCmd(cmd, resp);
         if(resp.Find(wxT("操作成功") ) != wxNOT_FOUND)
         {
            return true;
         }
    }
    return false;
    }

    void wxBaiduNetDiskFrame::OnBtnSelectAll(wxCommandEvent& event)
    {
        bool check = true;
        if(checked_items_cnt == m_listCtrl->GetItemCount())
        {
            check = false;
        }
        for(int i = 0; i < m_listCtrl->GetItemCount(); ++i)
        {
            m_listCtrl->CheckItem(i, check);
        }
    }

    void wxBaiduNetDiskFrame::OnListItemChecked(wxListEvent& event)
    {
        checked_items_cnt++;
        if(checked_items_cnt == m_listCtrl->GetItemCount())
        {
            chk_sel_all->SetValue(true);
        }
    }

      void wxBaiduNetDiskFrame::OnListItemUnChecked(wxListEvent& event)
    {
        checked_items_cnt--;
        chk_sel_all->SetValue(false);
    }
