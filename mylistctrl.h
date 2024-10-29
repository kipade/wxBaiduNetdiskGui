#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "wx/selstore.h"

#define LIST_CTRL 2000

class MyListCtrl: public wxListCtrl
{
public:
    MyListCtrl(wxWindow *parent,
               const wxWindowID id,
               const wxPoint& pos,
               const wxSize& size,
               long style)
        : wxListCtrl(parent, id, pos, size, style)
        {
            m_updated = -1;

        }
    MyListCtrl(const MyListCtrl&) = delete;
    MyListCtrl &operator=(const MyListCtrl&) = delete;

    // add one item to the listctrl in report mode
    void InsertItemInReportView(int i);

    void OnColClick(wxListEvent& event);
    void OnColRightClick(wxListEvent& event);
    void OnColBeginDrag(wxListEvent& event);
    void OnColDragging(wxListEvent& event);
    void OnColEndDrag(wxListEvent& event);
    void OnBeginDrag(wxListEvent& event);
    void OnBeginRDrag(wxListEvent& event);
    void OnBeginLabelEdit(wxListEvent& event);
    void OnEndLabelEdit(wxListEvent& event);
    void OnDeleteItem(wxListEvent& event);
    void OnDeleteAllItems(wxListEvent& event);
    void OnSelected(wxListEvent& event);
    void OnDeselected(wxListEvent& event);
    void OnListKeyDown(wxListEvent& event);
    void OnActivated(wxListEvent& event);
    void OnFocused(wxListEvent& event);
    void OnItemRightClick(wxListEvent& event);
    void OnChecked(wxListEvent& event);
    void OnUnChecked(wxListEvent& event);
    void OnCacheHint(wxListEvent& event);

    void OnChar(wxKeyEvent& event);

#if USE_CONTEXT_MENU
    void OnContextMenu(wxContextMenuEvent& event);
#endif

    void OnRightClick(wxMouseEvent& event);

private:
    void ShowContextMenu(const wxPoint& pos, long item);
    void SetColumnImage(int col, int image);

    void LogEvent(const wxListEvent& event, const wxString& eventName);
    void LogColEvent(const wxListEvent& event, const wxString& eventName);

    virtual wxString OnGetItemText(long item, long column) const override;
    virtual bool OnGetItemIsChecked(long item) const override;
    virtual int OnGetItemColumnImage(long item, long column) const override;
    virtual wxItemAttr *OnGetItemAttr(long item) const override;

    long m_updated;

    // checked boxes in virtual list
    wxSelectionStore m_checked;

    wxDECLARE_EVENT_TABLE();
};

