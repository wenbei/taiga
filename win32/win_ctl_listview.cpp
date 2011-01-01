/*
** Taiga, a lightweight client for MyAnimeList
** Copyright (C) 2010-2011, Eren Okka
** 
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "win_control.h"

// =============================================================================

CListView::CListView() {
  m_iSortColumn = 0;
  m_iSortOrder = -1;
  m_iSortType = 0;
}

// =============================================================================

void CListView::PreCreate(CREATESTRUCT &cs) {
  cs.dwExStyle = WS_EX_CLIENTEDGE;
  cs.lpszClass = WC_LISTVIEW;
  cs.style     = WS_CHILD | WS_TABSTOP | WS_VISIBLE | LVS_ALIGNLEFT | LVS_AUTOARRANGE | 
                 LVS_REPORT | LVS_SHAREIMAGELISTS | LVS_SINGLESEL;
}

BOOL CListView::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) {
  ListView_SetExtendedListViewStyle(hwnd, LVS_EX_AUTOSIZECOLUMNS | LVS_EX_DOUBLEBUFFER | 
    LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_LABELTIP);
  return TRUE;
}

// =============================================================================

int CListView::InsertColumn(int nIndex, int nWidth, int nWidthMin, int nAlign, LPCWSTR szText) {
  LVCOLUMN lvc = {0};
  lvc.cx       = nWidth;
  lvc.cxMin    = nWidthMin;
  lvc.fmt      = nAlign;
  lvc.mask     = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | (nWidthMin ? LVCF_MINWIDTH : NULL);
  lvc.pszText  = const_cast<LPWSTR>(szText);

  if (GetWinVersion() < WINVERSION_VISTA) {
    lvc.cx = lvc.cxMin;
  }

  return ListView_InsertColumn(m_hWindow, nIndex, &lvc);
}

// =============================================================================

int CListView::EnableGroupView(bool bValue) {
  return ListView_EnableGroupView(m_hWindow, bValue);
}

int CListView::InsertGroup(int nIndex, LPCWSTR szText, bool bCollapsed) {
  LVGROUP lvg = {0};
  lvg.cbSize    = sizeof(LVGROUP);
  lvg.iGroupId  = nIndex;
  lvg.mask      = LVGF_HEADER | LVGF_GROUPID;
  lvg.pszHeader = const_cast<LPWSTR>(szText);
  
  if (GetWinVersion() >= WINVERSION_VISTA) {
    lvg.mask |= LVGF_STATE;
    lvg.state = LVGS_COLLAPSIBLE;
    if (bCollapsed) lvg.state |= LVGS_COLLAPSED;
  }
  
  return ListView_InsertGroup(m_hWindow, nIndex, &lvg);
}

BOOL CListView::IsGroupViewEnabled() {
  return ListView_IsGroupViewEnabled(m_hWindow);
}

int CListView::SetGroupText(int nIndex, LPCWSTR szText) {
  LVGROUP lvg = {0};
  lvg.cbSize  = sizeof(LVGROUP);
  lvg.mask      = LVGF_HEADER;
  lvg.pszHeader = const_cast<LPWSTR>(szText);
  return ListView_SetGroupInfo(m_hWindow, nIndex, &lvg);
}

// =============================================================================

HIMAGELIST CListView::CreateDragImage(int iItem, LPPOINT lpptUpLeft) {
  return ListView_CreateDragImage(m_hWindow, iItem, lpptUpLeft);
}

BOOL CListView::DeleteAllItems() {
  return ListView_DeleteAllItems(m_hWindow);
}

BOOL CListView::DeleteItem(int iItem) {
  return ListView_DeleteItem(m_hWindow, iItem);
}

BOOL CListView::EnsureVisible(int i) {
  return ListView_EnsureVisible(m_hWindow, i, false);
}

BOOL CListView::GetCheckState(UINT iIndex) {
  return ListView_GetCheckState(m_hWindow, iIndex);
}

HWND CListView::GetHeader() {
  return ListView_GetHeader(m_hWindow);
}

int CListView::GetItemCount() {
  return ListView_GetItemCount(m_hWindow);
}

LPARAM CListView::GetItemParam(int i) {  
  LVITEM lvi = {0};
  lvi.iItem  = i;
  lvi.mask   = LVIF_PARAM;

  if (ListView_GetItem(m_hWindow, &lvi)) {
    return lvi.lParam;
  } else {
    return NULL;
  }
}

void CListView::GetItemText(int iItem, int iSubItem, LPWSTR pszText, int cchTextMax) {
  ListView_GetItemText(m_hWindow, iItem, iSubItem, pszText, cchTextMax);
}

void CListView::GetItemText(int iItem, int iSubItem, wstring& str, int cchTextMax) {
  vector<wchar_t> buffer(cchTextMax);
  ListView_GetItemText(m_hWindow, iItem, iSubItem, &buffer[0], cchTextMax);
  str.assign(&buffer[0]);
}

INT CListView::GetNextItem(int iStart, UINT flags) {
  return ListView_GetNextItem(m_hWindow, iStart, flags);
}

INT CListView::GetNextItemIndex(int iItem, int iGroup, LPARAM flags) {
  LVITEMINDEX lvii;
  lvii.iItem = iItem;
  lvii.iGroup = iGroup;
  if (ListView_GetNextItemIndex(m_hWindow, &lvii, flags)) {
    return lvii.iItem;
  } else {
    return -1;
  }
}

UINT CListView::GetSelectedCount() {
  return ListView_GetSelectedCount(m_hWindow);
}

INT CListView::GetSelectionMark() {
  return ListView_GetSelectionMark(m_hWindow);
}

BOOL CListView::GetSubItemRect(int iItem, int iSubItem, LPRECT lpRect) {
  return ListView_GetSubItemRect(m_hWindow, iItem, iSubItem, LVIR_BOUNDS, lpRect);
}

int CListView::HitTest(bool return_subitem) {
  LVHITTESTINFO lvhi;
  ::GetCursorPos(&lvhi.pt);
  ::ScreenToClient(m_hWindow, &lvhi.pt);
  ListView_SubItemHitTestEx(m_hWindow, &lvhi);
  return return_subitem ? lvhi.iSubItem : lvhi.iItem;
}

int CListView::InsertItem(int nIndex, int nGroup, int nIcon, LPCWSTR pszText, LPARAM lParam) {
  LVITEM lvi   = {0};
  lvi.iGroupId = nGroup;
  lvi.iImage   = nIcon;
  lvi.iItem    = nIndex;
  lvi.lParam   = lParam;
  lvi.pszText  = const_cast<LPWSTR>(pszText);

  if (pszText != NULL) lvi.mask |= LVIF_TEXT;
  if (nGroup > -1) lvi.mask |= LVIF_GROUPID;
  if (nIcon  > -1) lvi.mask |= LVIF_IMAGE;
  if (lParam != 0) lvi.mask |= LVIF_PARAM;

  return ListView_InsertItem(m_hWindow, &lvi);
}

BOOL CListView::RedrawItems(int iFirst, int iLast, bool repaint) {
  BOOL return_value = ListView_RedrawItems(m_hWindow, iFirst, iLast);
  if (return_value && repaint) ::UpdateWindow(m_hWindow);
  return return_value;
}

void CListView::SetCheckState(int iIndex, BOOL fCheck) {
  ListView_SetItemState(m_hWindow, iIndex, INDEXTOSTATEIMAGEMASK((fCheck==TRUE) ? 2 : 1), LVIS_STATEIMAGEMASK);
}

BOOL CListView::SetColumnWidth(int iCol, int cx) {
  // LVSCW_AUTOSIZE or LVSCW_AUTOSIZE_USEHEADER can be used as cx
  return ListView_SetColumnWidth(m_hWindow, iCol, cx);
}

void CListView::SetExtendedStyle(DWORD dwExStyle) {
  ListView_SetExtendedListViewStyle(m_hWindow, dwExStyle);
}

void CListView::SetImageList(HIMAGELIST hImageList) {
  ListView_SetImageList(m_hWindow, hImageList, LVSIL_SMALL);
}

BOOL CListView::SetItem(int nIndex, int nSubItem, LPCWSTR szText) {
  LVITEM lvi   = {0};
  lvi.iItem    = nIndex;
  lvi.iSubItem = nSubItem;
  lvi.mask     = LVIF_TEXT;
  lvi.pszText  = (LPWSTR)szText;

  return ListView_SetItem(m_hWindow, &lvi);
}

BOOL CListView::SetItemIcon(int nIndex, int nIcon) {
  LVITEM lvi = {0};
  lvi.iImage = nIcon;
  lvi.iItem  = nIndex;
  lvi.mask   = LVIF_IMAGE;

  return ListView_SetItem(m_hWindow, &lvi);
}

void CListView::SetSelectedItem(int iIndex) {
  ListView_SetItemState(m_hWindow, iIndex, LVIS_SELECTED, LVIS_SELECTED);
}

// =============================================================================

int CListView::GetSortColumn() { return m_iSortColumn; }
int CListView::GetSortOrder() { return m_iSortOrder; }
int CListView::GetSortType() { return m_iSortType; }

void CListView::Sort(int iColumn, int iOrder, int iType, PFNLVCOMPARE pfnCompare) {
  m_iSortColumn = iColumn;
  m_iSortOrder = iOrder;
  m_iSortType = iType;

  ListView_SortItemsEx(m_hWindow, pfnCompare, this);

  if (GetWinVersion() < WINVERSION_VISTA) {
    HDITEM hdi   = {0};
    hdi.mask     = HDI_FORMAT;
    HWND hHeader = ListView_GetHeader(m_hWindow);
    for (int i = 0; i < Header_GetItemCount(hHeader); ++i) {
      Header_GetItem(hHeader, i, &hdi);
      hdi.fmt &= ~(HDF_SORTDOWN | HDF_SORTUP);
      Header_SetItem(hHeader, i, &hdi);
    }
    Header_GetItem(hHeader, iColumn, &hdi);
    hdi.fmt |= (iOrder > -1 ? HDF_SORTUP : HDF_SORTDOWN);
    Header_SetItem(hHeader, iColumn, &hdi);
  }
}