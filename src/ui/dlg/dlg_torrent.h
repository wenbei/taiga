/*
** Taiga
** Copyright (C) 2010-2019, Eren Okka
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

#pragma once

#include <string>

#include <windows/win/common_controls.h>
#include <windows/win/dialog.h>

namespace ui {

class TorrentDialog : public win::Dialog {
public:
  TorrentDialog() {};
  ~TorrentDialog() {};

  INT_PTR DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  void OnContextMenu(HWND hwnd, POINT pt);
  BOOL OnInitDialog();
  LRESULT OnNotify(int idCtrl, LPNMHDR pnmh);
  void OnSize(UINT uMsg, UINT nType, SIZE size);

  void EnableInput(bool enable = true);
  void RefreshList();
  void Search(std::wstring url, int anime_id);
  void Search(std::wstring url, std::wstring title);
  void SetTimer(int ticks);

private:
  class ListView : public win::ListView {
  public:
    int last_checked_item;
  } list_;
  win::Rebar rebar_;
  win::Toolbar toolbar_;
};

extern TorrentDialog DlgTorrent;

}  // namespace ui
