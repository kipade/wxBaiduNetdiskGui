#include "mylistctrl.h"

const wxChar *SMALL_VIRTUAL_VIEW_ITEMS[][2] =
{
    { wxT("Cat"), wxT("meow") },
    { wxT("Cow"), wxT("moo") },
    { wxT("Crow"), wxT("caw") },
    { wxT("Dog"), wxT("woof") },
    { wxT("Duck"), wxT("quack") },
    { wxT("Mouse"), wxT("squeak") },
    { wxT("Owl"), wxT("hoo") },
    { wxT("Pig"), wxT("oink") },
    { wxT("Pigeon"), wxT("coo") },
    { wxT("Sheep"), wxT("baaah") },
};

int wxCALLBACK
MyCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{
    bool ascending = (sortData == 1);
    // inverse the order
    if (item1 < item2)
        return ascending ? -1 : 1;
    if (item1 > item2)
        return ascending ? 1 : -1;

    return 0;
}

// ----------------------------------------------------------------------------
// MyListCtrl
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyListCtrl, wxListCtrl)
    EVT_LIST_BEGIN_DRAG(LIST_CTRL, MyListCtrl::OnBeginDrag)
    EVT_LIST_BEGIN_RDRAG(LIST_CTRL, MyListCtrl::OnBeginRDrag)
    EVT_LIST_BEGIN_LABEL_EDIT(LIST_CTRL, MyListCtrl::OnBeginLabelEdit)
    EVT_LIST_END_LABEL_EDIT(LIST_CTRL, MyListCtrl::OnEndLabelEdit)
    EVT_LIST_DELETE_ITEM(LIST_CTRL, MyListCtrl::OnDeleteItem)
    EVT_LIST_DELETE_ALL_ITEMS(LIST_CTRL, MyListCtrl::OnDeleteAllItems)
    EVT_LIST_ITEM_SELECTED(LIST_CTRL, MyListCtrl::OnSelected)
    EVT_LIST_ITEM_DESELECTED(LIST_CTRL, MyListCtrl::OnDeselected)
    EVT_LIST_KEY_DOWN(LIST_CTRL, MyListCtrl::OnListKeyDown)
    EVT_LIST_ITEM_ACTIVATED(LIST_CTRL, MyListCtrl::OnActivated)
    EVT_LIST_ITEM_FOCUSED(LIST_CTRL, MyListCtrl::OnFocused)
    EVT_LIST_ITEM_CHECKED(LIST_CTRL, MyListCtrl::OnChecked)
    EVT_LIST_ITEM_UNCHECKED(LIST_CTRL, MyListCtrl::OnUnChecked)

    EVT_LIST_ITEM_RIGHT_CLICK(LIST_CTRL, MyListCtrl::OnItemRightClick)

    EVT_LIST_COL_CLICK(LIST_CTRL, MyListCtrl::OnColClick)
    EVT_LIST_COL_RIGHT_CLICK(LIST_CTRL, MyListCtrl::OnColRightClick)
    EVT_LIST_COL_BEGIN_DRAG(LIST_CTRL, MyListCtrl::OnColBeginDrag)
    EVT_LIST_COL_DRAGGING(LIST_CTRL, MyListCtrl::OnColDragging)
    EVT_LIST_COL_END_DRAG(LIST_CTRL, MyListCtrl::OnColEndDrag)

    EVT_LIST_CACHE_HINT(LIST_CTRL, MyListCtrl::OnCacheHint)

#if USE_CONTEXT_MENU
    EVT_CONTEXT_MENU(MyListCtrl::OnContextMenu)
#endif
    EVT_CHAR(MyListCtrl::OnChar)

    EVT_RIGHT_DOWN(MyListCtrl::OnRightClick)
wxEND_EVENT_TABLE()

void MyListCtrl::OnCacheHint(wxListEvent& event)
{
    wxLogMessage( "OnCacheHint: cache items %ld..%ld",
                  event.GetCacheFrom(), event.GetCacheTo() );
}

void MyListCtrl::SetColumnImage(int col, int image)
{
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(image);
    SetColumn(col, item);
}

void MyListCtrl::OnColClick(wxListEvent& event)
{
    int col = event.GetColumn();

    if ( col == -1 )
    {
        return; // clicked outside any column.
    }

    // sort on item data (SetItemData)
    const bool ascending = GetUpdatedAscendingSortIndicator(col);
    if ( SortItems(MyCompareFunction, ascending) )
    {
        ShowSortIndicator(col, ascending);
    }

    // set or unset image
    static bool x = false;
    x = !x;
    SetColumnImage(col, x ? 0 : -1);

    wxLogMessage( "OnColumnClick at %d.", col );
}

void MyListCtrl::OnColRightClick(wxListEvent& event)
{
    int col = event.GetColumn();
    if ( col != -1 )
    {
        SetColumnImage(col, -1);
    }

    // Show popupmenu at position
    wxMenu menu("Test");
    menu.Append(wxID_ANY, "&About");
    PopupMenu(&menu, event.GetPoint());

    wxLogMessage( "OnColumnRightClick at %d.", event.GetColumn() );
}

void MyListCtrl::LogColEvent(const wxListEvent& event, const wxString& name)
{
    const int col = event.GetColumn();

    wxLogMessage("%s: column %d (width = %d or %d).",
                 name,
                 col,
                 event.GetItem().GetWidth(),
                 GetColumnWidth(col));
}

void MyListCtrl::OnColBeginDrag(wxListEvent& event)
{
    LogColEvent( event, "OnColBeginDrag" );

    if ( event.GetColumn() == 0 )
    {
        wxLogMessage("Resizing this column shouldn't work.");

        event.Veto();
    }
}

void MyListCtrl::OnColDragging(wxListEvent& event)
{
    LogColEvent( event, "OnColDragging" );
}

void MyListCtrl::OnColEndDrag(wxListEvent& event)
{
    LogColEvent( event, "OnColEndDrag" );
}

void MyListCtrl::OnBeginDrag(wxListEvent& event)
{
    const wxPoint& pt = event.m_pointDrag;

    int flags;
    wxLogMessage( "OnBeginDrag at (%d, %d), item %ld.",
                  pt.x, pt.y, HitTest(pt, flags) );
}

void MyListCtrl::OnBeginRDrag(wxListEvent& event)
{
    wxLogMessage( "OnBeginRDrag at %d,%d.",
                  event.m_pointDrag.x, event.m_pointDrag.y );
}

void MyListCtrl::OnBeginLabelEdit(wxListEvent& event)
{
    wxLogMessage( "OnBeginLabelEdit: %s", event.m_item.m_text);

    wxTextCtrl * const text = GetEditControl();
    if ( !text )
    {
        wxLogMessage("BUG: started to edit but no edit control");
    }
    else
    {
        wxLogMessage("Edit control value: \"%s\"", text->GetValue());
    }
}

void MyListCtrl::OnEndLabelEdit(wxListEvent& event)
{
    wxLogMessage( "OnEndLabelEdit: %s",
        (
            event.IsEditCancelled() ?
            wxString("[cancelled]") :
            event.m_item.m_text
        )
    );
}

void MyListCtrl::OnDeleteItem(wxListEvent& event)
{
    LogEvent(event, "OnDeleteItem");
    wxLogMessage( "Number of items when delete event is sent: %d", GetItemCount() );
}

void MyListCtrl::OnDeleteAllItems(wxListEvent& event)
{
    LogEvent(event, "OnDeleteAllItems");
}

void MyListCtrl::OnSelected(wxListEvent& event)
{
    LogEvent(event, "OnSelected");

    if ( GetWindowStyle() & wxLC_REPORT )
    {
        wxListItem info;
        info.m_itemId = event.m_itemIndex;
        info.m_col = 1;
        info.m_mask = wxLIST_MASK_TEXT;
        if ( GetItem(info) )
        {
            wxLogMessage("Value of the 2nd field of the selected item: %s",
                         info.m_text);
        }
        else
        {
            wxFAIL_MSG("wxListCtrl::GetItem() failed");
        }
    }
}

void MyListCtrl::OnDeselected(wxListEvent& event)
{
    LogEvent(event, "OnDeselected");
}

void MyListCtrl::OnActivated(wxListEvent& event)
{
    LogEvent(event, "OnActivated");
    event.Skip();
}

void MyListCtrl::OnFocused(wxListEvent& event)
{
    LogEvent(event, "OnFocused");

    event.Skip();
}

void MyListCtrl::OnItemRightClick(wxListEvent& event)
{
    LogEvent(event, "OnItemRightClick");

    event.Skip();
}

void MyListCtrl::OnChecked(wxListEvent& event)
{
    LogEvent(event, "OnChecked");

    if ( IsVirtual() )
    {
        m_checked.SelectItem(event.GetIndex(), true);
        RefreshItem(event.GetIndex());
    }

    event.Skip();
}

void MyListCtrl::OnUnChecked(wxListEvent& event)
{
    LogEvent(event, "OnUnChecked");

    if ( IsVirtual() )
    {
        m_checked.SelectItem(event.GetIndex(), false);
        RefreshItem(event.GetIndex());
    }

    event.Skip();
}

void MyListCtrl::OnListKeyDown(wxListEvent& event)
{
    long item;

    if ( !wxGetKeyState(WXK_SHIFT) )
    {
        LogEvent(event, "OnListKeyDown");
        event.Skip();
    }

    switch ( event.GetKeyCode() )
    {
        case 'C': // colorize
            {
                wxListItem info;
                info.m_itemId = event.GetIndex();
                if ( info.m_itemId == -1 )
                {
                    // no item
                    break;
                }

                GetItem(info);

                wxItemAttr *attr = info.GetAttributes();
                if ( !attr || !attr->HasTextColour() )
                {
                    info.SetTextColour(*wxCYAN);

                    SetItem(info);

                    RefreshItem(info.m_itemId);
                }
            }
            break;

        case 'N': // next
            item = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
            if ( item++ == GetItemCount() - 1 )
            {
                item = 0;
            }

            wxLogMessage("Focusing item %ld", item);

            SetItemState(item, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
            EnsureVisible(item);
            break;

        case 'R': // show bounding rectangle
            {
                item = event.GetIndex();
                wxRect r;
                if ( !GetItemRect(item, r) )
                {
                    wxLogError("Failed to retrieve rect of item %ld", item);
                    break;
                }

                wxLogMessage("Bounding rect of item %ld is (%d, %d)-(%d, %d)",
                             item, r.x, r.y, r.x + r.width, r.y + r.height);
            }
            break;

        case '1': // show sub item bounding rectangle for the given column
        case '2': // (and icon/label rectangle if Shift/Ctrl is pressed)
        case '3':
        case '4': // this column is invalid but we want to test it too
            if ( InReportView() )
            {
                int subItem = event.GetKeyCode() - '1';
                item = event.GetIndex();
                wxRect r;

                int code = wxLIST_RECT_BOUNDS;
                if ( wxGetKeyState(WXK_SHIFT) )
                    code = wxLIST_RECT_ICON;
                else if ( wxGetKeyState(WXK_CONTROL) )
                    code = wxLIST_RECT_LABEL;

                if ( !GetSubItemRect(item, subItem, r, code) )
                {
                    wxLogError("Failed to retrieve rect of item %ld column %d", item, subItem + 1);
                    break;
                }

                wxString part;
                switch ( code )
                {
                    case wxLIST_RECT_BOUNDS:
                        part = "total rectangle";
                        break;

                    case wxLIST_RECT_ICON:
                        part = "icon";
                        break;

                    case wxLIST_RECT_LABEL:
                        part = "label";
                        break;
                }

                wxLogMessage("Bounding rect of the %s of the item %ld column %d is (%d, %d)-(%d, %d)",
                             part, item, subItem + 1, r.x, r.y, r.x + r.width, r.y + r.height);
            }
            break;

        case 'U': // update
            if ( !IsVirtual() )
                break;

            if ( m_updated != -1 )
                RefreshItem(m_updated);

            m_updated = event.GetIndex();
            if ( m_updated != -1 )
            {
                // we won't see changes to this item as it's selected, update
                // the next one (or the first one if we're on the last item)
                if ( ++m_updated == GetItemCount() )
                    m_updated = 0;

                wxLogMessage("Updating colour of the item %ld", m_updated);
                RefreshItem(m_updated);
            }
            break;

        case 'D': // delete
            item = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            while ( item != -1 )
            {
                DeleteItem(item);

                wxLogMessage("Item %ld deleted", item);

                // -1 because the indices were shifted by DeleteItem()
                item = GetNextItem(item - 1,
                                   wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            }
            break;

        case 'I': // insert
            if ( GetWindowStyle() & wxLC_REPORT )
            {
                if ( GetWindowStyle() & wxLC_VIRTUAL )
                {
                    SetItemCount(GetItemCount() + 1);
                }
                else // !virtual
                {
                    int idx = event.GetIndex();
                    if ( idx == -1 )
                        idx = 0;
                    InsertItemInReportView(idx);
                }
                break;
            }
            wxFALLTHROUGH;

        default:
            event.Skip();
    }
}

void MyListCtrl::OnChar(wxKeyEvent& event)
{
    wxLogMessage("Got char event.");

    event.Skip();
}

void MyListCtrl::OnRightClick(wxMouseEvent& event)
{
    if ( !event.ControlDown() )
    {
        event.Skip();
        return;
    }

    int flags;
    long subitem;
    long item = HitTest(event.GetPosition(), flags, &subitem);

    wxString where;
    switch ( flags )
    {
        case wxLIST_HITTEST_ABOVE: where = "above"; break;
        case wxLIST_HITTEST_BELOW: where = "below"; break;
        case wxLIST_HITTEST_NOWHERE: where = "nowhere near"; break;
        case wxLIST_HITTEST_ONITEMICON: where = "on icon of"; break;
        case wxLIST_HITTEST_ONITEMLABEL: where = "on label of"; break;
        case wxLIST_HITTEST_TOLEFT: where = "to the left of"; break;
        case wxLIST_HITTEST_TORIGHT: where = "to the right of"; break;
        default: where = "not clear exactly where on"; break;
    }

    wxLogMessage("Right click %s item %ld, subitem %ld",
                 where, item, subitem);
}

void MyListCtrl::LogEvent(const wxListEvent& event, const wxString& eventName)
{
    wxLogMessage("Item %ld: %s (item text = %s, data = %ld)",
                 event.GetIndex(), eventName,
                 event.GetText(), static_cast<long>(event.GetData()));
}

wxString MyListCtrl::OnGetItemText(long item, long column) const
{
    if ( GetItemCount() == WXSIZEOF(SMALL_VIRTUAL_VIEW_ITEMS) )
    {
        return SMALL_VIRTUAL_VIEW_ITEMS[item][column];
    }
    else // "big" virtual control
    {
        return wxString::Format("Column %ld of item %ld", column, item);
    }
}

bool MyListCtrl::OnGetItemIsChecked(long item) const
{
    return m_checked.IsSelected(item);
}

int MyListCtrl::OnGetItemColumnImage(long item, long column) const
{
    if (!column)
        return 0;

    if (!(item % 3) && column == 1)
        return 0;

    return -1;
}

wxItemAttr *MyListCtrl::OnGetItemAttr(long item) const
{
    // test to check that RefreshItem() works correctly: when m_updated is
    // set to some item and it is refreshed, we highlight the item
    if ( item == m_updated )
    {
        static wxItemAttr s_attrHighlight(*wxRED, wxNullColour, wxNullFont);
        return &s_attrHighlight;
    }

    return wxListCtrl::OnGetItemAttr(item);
}

void MyListCtrl::InsertItemInReportView(int i)
{
    wxString buf;
    buf.Printf("This is item %d", i);
    long tmp = InsertItem(i, buf, 0);
    SetItemData(tmp, i);

    buf.Printf("Col 1, item %d", i);
    SetItem(tmp, 1, buf);
    SetItemImage(tmp%1, 1);

    //buf.Printf("Item %d in column 2", i);
    //SetItem(tmp, 2, buf);
}

#if USE_CONTEXT_MENU
void MyListCtrl::OnContextMenu(wxContextMenuEvent& event)
{
    if (GetEditControl() == nullptr)
    {
        wxPoint point = event.GetPosition();
        // If from keyboard
        if ( (point.x == -1) && (point.y == -1) )
        {
            wxSize size = GetSize();
            point.x = size.x / 2;
            point.y = size.y / 2;
        }
        else
        {
            point = ScreenToClient(point);
        }
        int flags;
        ShowContextMenu(point, HitTest(point, flags));
    }
    else
    {
        // the user is editing:
        // allow the text control to display its context menu
        // if it has one (it has on Windows) rather than display our one
        event.Skip();
    }
}
#endif

void MyListCtrl::ShowContextMenu(const wxPoint& pos, long item)
{
    wxMenu menu;
    menu.Append(wxID_ANY, wxString::Format("Menu for item %ld", item));
    menu.Append(wxID_ABOUT, "&About");
    menu.AppendSeparator();
    menu.Append(wxID_EXIT, "E&xit");

    PopupMenu(&menu, pos.x, pos.y);
}

