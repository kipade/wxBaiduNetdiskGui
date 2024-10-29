/***************************************************************
 * Name:      wxBaiduNetDiskMain.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2024-10-27
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef WXBAIDUNETDISKMAIN_H
#define WXBAIDUNETDISKMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/process.h>
#include <map>
#include <functional>
#include "mylistctrl.h"
#include "wxBaiduNetDiskApp.h"

class wxBaiduNetDiskFrame: public wxFrame
{
    public:
        wxBaiduNetDiskFrame(wxFrame *frame, const wxString& title);
        ~wxBaiduNetDiskFrame();
    private:
        enum
        {
            idMenuQuit = 1000,
            idMenuAbout,
            id_btn_dir_prev,
            id_btn_download,
            id_btn_upload,
            id_btn_upload_dir,
            id_btn_mkdir,
            id_btn_rm,
            id_btn_sel_all
        };
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnListItemActived(wxListEvent& event);
        void OnListItemChecked(wxListEvent& event);
        void OnListItemUnChecked(wxListEvent& event);
        void OnBtnDirectoryUp(wxCommandEvent& event);
        void OnBtnDownload(wxCommandEvent& event);
        void OnBtnUploadFile(wxCommandEvent& event);
        void OnBtnUploadDir(wxCommandEvent& event);
        void OnBtnMkdir(wxCommandEvent& event);
        void OnBtnRm(wxCommandEvent& event);
        void OnBtnSelectAll(wxCommandEvent& event);

        DECLARE_EVENT_TABLE()

        void RecreateList(long flags, bool withText=true);
        void InitWithReportItems();
        void ProcessNetdiskLs();
        bool ExecuteNetdiskCmd(const wxString& cmd, wxString& resp);
        bool CheckNetdiskResp(const wxString& cmd, const wxString& resp);
        bool ProcessNetdiskResp(const wxString& cmd, const wxString& resp);

        bool ProcessNetdiskRespLs(const wxString& resp);
        bool ProcessNetdiskRespPwd(const wxString& resp);
        bool InsertNewFileItem(const wxString& resp);
        bool DownloadItem(long i);
        bool UploadItem(const wxString& path);
        bool RemoveItem(long i);


    private:
        wxTextCtrl* txt_path;
        wxTextCtrl* m_logWindow;
        wxButton *btn_path_prev, *btn_download, *btn_upload, *btn_upload_dir, *btn_mkdir, *btn_rm;
        wxCheckBox* chk_sel_all;
        MyListCtrl* m_listCtrl;
        wxLog *m_logOld;
        wxVector<wxBitmapBundle> m_imagesSmall;
        wxPanel* m_panel;
        int checked_items_cnt;

        std::map<wxString, std::function<bool(const wxString&)>> m_pcsRespProcessor;
};


#endif // WXBAIDUNETDISKMAIN_H
