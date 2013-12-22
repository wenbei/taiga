/*
** Taiga
** Copyright (C) 2010-2013, Eren Okka
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

#include "base/common.h"
#include "base/file.h"
#include "base/foreach.h"
#include "base/string.h"
#include "library/anime_db.h"
#include "library/anime_episode.h"
#include "library/history.h"
#include "taiga/http.h"
#include "taiga/resource.h"
#include "taiga/script.h"
#include "taiga/settings.h"
#include "taiga/taiga.h"
#include "win/win_taskbar.h"
#include "ui/dlg/dlg_anime_info.h"
#include "ui/dlg/dlg_anime_list.h"
#include "ui/dlg/dlg_history.h"
#include "ui/dlg/dlg_input.h"
#include "ui/dlg/dlg_main.h"
#include "ui/dlg/dlg_search.h"
#include "ui/dlg/dlg_season.h"
#include "ui/dlg/dlg_settings.h"
#include "ui/dlg/dlg_stats.h"
#include "ui/dlg/dlg_torrent.h"
#include "ui/dlg/dlg_update.h"
#include "ui/menu.h"
#include "ui/theme.h"
#include "ui/ui.h"
#include "win/win_taskdialog.h"

namespace ui {

void ChangeStatusText(const string_t& status) {
  MainDialog.ChangeStatus(status);
}

void ClearStatusText() {
  MainDialog.ChangeStatus(L"");
}

void EnableDialogInput(Dialog dialog, bool enable) {
  switch (dialog) {
    case kDialogMain:
      MainDialog.EnableInput(enable);
      break;
  }
}

////////////////////////////////////////////////////////////////////////////////

void OnHttpError(const taiga::HttpClient& http_client, const string_t& error) {
  switch (http_client.mode()) {
    case taiga::kHttpSilent:
    case taiga::kHttpServiceGetMetadataById:
    case taiga::kHttpServiceSearchTitle:
    case taiga::kHttpGetLibraryEntryImage:
      return;
    case taiga::kHttpServiceAuthenticateUser:
    case taiga::kHttpServiceGetLibraryEntries:
    case taiga::kHttpServiceAddLibraryEntry:
    case taiga::kHttpServiceDeleteLibraryEntry:
    case taiga::kHttpServiceUpdateLibraryEntry:
      ChangeStatusText(error);
      MainDialog.EnableInput(true);
      break;
    case taiga::kHttpFeedCheck:
    case taiga::kHttpFeedCheckAuto:
    case taiga::kHttpFeedDownload:
    case taiga::kHttpFeedDownloadAll:
      ChangeStatusText(error);
      TorrentDialog.EnableInput();
      break;
    case taiga::kHttpTwitterRequest:
    case taiga::kHttpTwitterAuth:
    case taiga::kHttpTwitterPost:
      ChangeStatusText(error);
      break;
    case taiga::kHttpTaigaUpdateCheck:
    case taiga::kHttpTaigaUpdateDownload:
      MessageBox(UpdateDialog.GetWindowHandle(), 
                 error.c_str(), L"Update", MB_ICONERROR | MB_OK);
      UpdateDialog.PostMessage(WM_CLOSE);
      return;
  }

  TaskbarList.SetProgressState(TBPF_NOPROGRESS);
}

void OnHttpHeadersAvailable(const taiga::HttpClient& http_client) {
  switch (http_client.mode()) {
    case taiga::kHttpSilent:
      return;
    case taiga::kHttpTaigaUpdateCheck:
    case taiga::kHttpTaigaUpdateDownload:
      if (http_client.content_length() > 0) {
        UpdateDialog.progressbar.SetMarquee(false);
        UpdateDialog.progressbar.SetRange(0, http_client.content_length());
      } else {
        UpdateDialog.progressbar.SetMarquee(true);
      }
      if (http_client.mode() == taiga::kHttpTaigaUpdateDownload) {
        UpdateDialog.SetDlgItemText(IDC_STATIC_UPDATE_PROGRESS,
                                    L"Downloading latest update...");
      }
      break;
    default:
      TaskbarList.SetProgressState(http_client.content_length() > 0 ?
                                   TBPF_NORMAL : TBPF_INDETERMINATE);
      break;
  }
}

void OnHttpProgress(const taiga::HttpClient& http_client) {
  wstring status;

  switch (http_client.mode()) {
    case taiga::kHttpSilent:
    case taiga::kHttpServiceGetMetadataById:
    case taiga::kHttpServiceSearchTitle:
    case taiga::kHttpGetLibraryEntryImage:
      return;
    case taiga::kHttpServiceAuthenticateUser:
      status = L"Reading account information...";
      break;
    case taiga::kHttpServiceGetLibraryEntries:
      status = L"Downloading anime list...";
      break;
    case taiga::kHttpServiceAddLibraryEntry:
    case taiga::kHttpServiceDeleteLibraryEntry:
    case taiga::kHttpServiceUpdateLibraryEntry:
      status = L"Updating list...";
      break;
    case taiga::kHttpFeedCheck:
    case taiga::kHttpFeedCheckAuto:
      status = L"Checking new torrents...";
      break;
    case taiga::kHttpFeedDownload:
    case taiga::kHttpFeedDownloadAll:
      status = L"Downloading torrent file...";
      break;
    case taiga::kHttpTwitterRequest:
      status = L"Connecting to Twitter...";
      break;
    case taiga::kHttpTwitterAuth:
      status = L"Authorizing Twitter...";
      break;
    case taiga::kHttpTwitterPost:
      status = L"Updating Twitter status...";
      break;
    case taiga::kHttpTaigaUpdateCheck:
    case taiga::kHttpTaigaUpdateDownload:
      if (http_client.content_length() > 0)
        UpdateDialog.progressbar.SetPosition(http_client.current_length());
      return;
  }

  if (http_client.content_length() > 0) {
    float current_length = static_cast<float>(http_client.current_length());
    float content_length = static_cast<float>(http_client.content_length());
    int percentage = static_cast<int>((current_length / content_length) * 100);
    status += L" (" + ToWstr(percentage) + L"%)";
    TaskbarList.SetProgressValue(static_cast<ULONGLONG>(current_length),
                                 static_cast<ULONGLONG>(content_length));
  } else {
    status += L" (" + ToSizeString(http_client.current_length()) + L")";
  }

  ChangeStatusText(status);
}

void OnHttpReadComplete(const taiga::HttpClient& http_client) {
  TaskbarList.SetProgressState(TBPF_NOPROGRESS);
}

////////////////////////////////////////////////////////////////////////////////

void OnLibraryChange() {
  ClearStatusText();

  AnimeListDialog.RefreshList();
  AnimeListDialog.RefreshTabs();
  HistoryDialog.RefreshList();
  SearchDialog.RefreshList();

  MainDialog.EnableInput(true);
}

void OnLibraryEntryChange(int id) {
  if (AnimeDialog.GetCurrentId() == id)
    AnimeDialog.Refresh(false, true, false, false);

  if (AnimeListDialog.IsWindow())
    AnimeListDialog.RefreshListItem(id);

  if (NowPlayingDialog.GetCurrentId() == id)
    NowPlayingDialog.Refresh(false, true, false, false);

  if (SeasonDialog.IsWindow())
    SeasonDialog.RefreshList(true);
}

void OnLibraryEntryDelete(int id) {
  if (AnimeDialog.GetCurrentId() == id)
    AnimeDialog.Destroy();

  AnimeListDialog.RefreshList();
  AnimeListDialog.RefreshTabs();

  SearchDialog.RefreshList();

  if (SeasonDialog.IsWindow())
    SeasonDialog.RefreshList(true);

  if (CurrentEpisode.anime_id == id)
    CurrentEpisode.Set(anime::ID_NOTINLIST);
}

void OnLibraryEntryImageChange(int id) {
  if (AnimeDialog.GetCurrentId() == id)
    AnimeDialog.Refresh(true, false, false, false);

  if (AnimeListDialog.IsWindow())
    AnimeListDialog.RefreshListItem(id);

  if (NowPlayingDialog.GetCurrentId() == id)
    NowPlayingDialog.Refresh(true, false, false, false);

  if (SeasonDialog.IsWindow())
    SeasonDialog.RefreshList(true);
}

void OnLibrarySearchTitle(const string_t& results) {
  std::vector<string_t> split_vector;
  Split(results, L",", split_vector);

  std::vector<int> ids;
  foreach_(it, split_vector) {
    int id = ToInt(*it);
    ids.push_back(id);
    OnLibraryEntryChange(id);
  }

  SearchDialog.ParseResults(ids);
}

void OnLibraryUpdateFailure(int id, const string_t& reason) {
  auto anime_item = AnimeDatabase.FindItem(id);

  std::wstring text;
  if (anime_item)
    text += L"Title: " + anime_item->GetTitle() + L"\n";
  if (!reason.empty())
    text += L"Reason: " + reason + L"\n";
  text += L"Click to try again.";

  Taiga.current_tip_type = taiga::kTipTypeUpdateFailed;

  Taskbar.Tip(L"", L"", 0);  // clear previous tips
  Taskbar.Tip(text.c_str(), L"Update failed", NIIF_ERROR);

  ChangeStatusText(L"Update failed: " + reason);
}

////////////////////////////////////////////////////////////////////////////////

void OnHistoryAddItem(const HistoryItem& history_item) {
  OnHistoryChange();

  if (history_item.mode == taiga::kHttpServiceAddLibraryEntry ||
      history_item.mode == taiga::kHttpServiceDeleteLibraryEntry ||
      history_item.status ||
      history_item.enable_rewatching) {
    AnimeListDialog.RefreshList();
    AnimeListDialog.RefreshTabs();
  } else {
    AnimeListDialog.RefreshListItem(history_item.anime_id);
  }

  if (!Taiga.logged_in) {
    auto anime_item = AnimeDatabase.FindItem(history_item.anime_id);
    ChangeStatusText(L"\"" + anime_item->GetTitle() +
                     L"\" is queued for update.");
  }
}

void OnHistoryChange() {
  HistoryDialog.RefreshList();
  MainDialog.treeview.RefreshHistoryCounter();
  NowPlayingDialog.Refresh(false, false, false);
}

int OnHistoryProcessConfirmationQueue(anime::Episode& episode) {
  auto anime_item = AnimeDatabase.FindItem(episode.anime_id);

  win::TaskDialog dlg;
  wstring title = L"Anime title: " + anime_item->GetTitle();
  dlg.SetWindowTitle(APP_TITLE);
  dlg.SetMainIcon(TD_ICON_INFORMATION);
  dlg.SetMainInstruction(L"Do you want to update your anime list?");
  dlg.SetContent(title.c_str());
  dlg.SetVerificationText(L"Don't ask again, update automatically");
  dlg.UseCommandLinks(true);

  int number = GetEpisodeHigh(episode.number);
  if (number == 0)
    number = 1;
  if (anime_item->GetEpisodeCount() == 1)
    episode.number = L"1";

  if (anime_item->GetEpisodeCount() == number) {  // Completed
    dlg.AddButton(L"Update and move\n"
                  L"Update and set as completed", IDCANCEL);
  } else if (anime_item->GetMyStatus() != anime::kWatching) {  // Watching
    dlg.AddButton(L"Update and move\n"
                  L"Update and set as watching", IDCANCEL);
  }
  wstring button = L"Update\n"
                   L"Update episode number from " +
                   ToWstr(anime_item->GetMyLastWatchedEpisode()) +
                   L" to " + ToWstr(number);
  dlg.AddButton(button.c_str(), IDYES);
  dlg.AddButton(L"Cancel\n"
                L"Don't update anything", IDNO);

  dlg.Show(g_hMain);
  if (dlg.GetVerificationCheck())
    Settings.Set(taiga::kSync_Update_AskToConfirm, false);
  return dlg.GetSelectedButtonID();
}

////////////////////////////////////////////////////////////////////////////////

void OnAnimeWatchingStart(const anime::Item& anime_item,
                          const anime::Episode& episode) {
  NowPlayingDialog.SetCurrentId(anime_item.GetId());
  
  int list_status = anime_item.GetMyStatus();
  if (anime_item.GetMyRewatching())
    list_status = anime::kWatching;
  if (list_status != anime::kNotInList) {
    AnimeListDialog.RefreshList(list_status);
    AnimeListDialog.RefreshTabs(list_status);
  }
  int list_index = AnimeListDialog.GetListIndex(anime_item.GetId());
  if (list_index > -1) {
    AnimeListDialog.listview.SetItemIcon(list_index, ui::kIcon16_Play);
    AnimeListDialog.listview.RedrawItems(list_index, list_index, true);
    AnimeListDialog.listview.EnsureVisible(list_index);
  }

  MainDialog.UpdateTip();
  MainDialog.UpdateTitle();
  if (Settings.GetBool(taiga::kSync_Update_GoToNowPlaying))
    MainDialog.navigation.SetCurrentPage(SIDEBAR_ITEM_NOWPLAYING);

  if (Settings.GetBool(taiga::kSync_Notify_Recognized)) {
    Taiga.current_tip_type = taiga::kTipTypeNowPlaying;
    std::wstring tip_text =
        ReplaceVariables(Settings[taiga::kSync_Notify_Format], episode);
    Taskbar.Tip(L"", L"", 0);
    Taskbar.Tip(tip_text.c_str(), L"Now Playing", NIIF_INFO);
  }
}

void OnAnimeWatchingEnd(const anime::Item& anime_item,
                        const anime::Episode& episode) {
  NowPlayingDialog.SetCurrentId(anime::ID_UNKNOWN);

  MainDialog.UpdateTip();
  MainDialog.UpdateTitle();

  int list_index = AnimeListDialog.GetListIndex(anime_item.GetId());
  if (list_index > -1) {
    int icon_index = StatusToIcon(anime_item.GetAiringStatus());
    AnimeListDialog.listview.SetItemIcon(list_index, icon_index);
    AnimeListDialog.listview.RedrawItems(list_index, list_index, true);
  }
}

////////////////////////////////////////////////////////////////////////////////

void OnRootFoldersEmpty() {
  win::TaskDialog dlg(APP_TITLE, TD_ICON_INFORMATION);
  dlg.SetMainInstruction(L"Would you like to set root anime folders first?");
  dlg.SetContent(L"You need to have at least one root folder set before "
                 L"scanning available episodes.");
  dlg.AddButton(L"Yes", IDYES);
  dlg.AddButton(L"No", IDNO);
  dlg.Show(g_hMain);
  if (dlg.GetSelectedButtonID() == IDYES)
    ExecuteAction(L"Settings", SECTION_LIBRARY, PAGE_LIBRARY_FOLDERS);
}

////////////////////////////////////////////////////////////////////////////////

void OnFeedCheck(bool success) {
  ChangeStatusText(success ?
      L"There are new torrents available!" : L"No new torrents found.");

  TorrentDialog.RefreshList();
  TorrentDialog.EnableInput();
}

void OnFeedDownload(bool success) {
  if (success)
    TorrentDialog.RefreshList();

  ChangeStatusText(L"Successfully downloaded all torrents.");
  TorrentDialog.EnableInput();
}

bool OnFeedNotify(const Feed& feed) {
  std::wstring tip_text;
  std::wstring tip_title = L"New torrents available";
  std::wstring tip_format = L"%title%$if(%episode%, #%episode%)\n";

  foreach_(it, feed.items)
    if (it->state == FEEDITEM_SELECTED)
      tip_text += L"\u00BB " + ReplaceVariables(tip_format, it->episode_data);

  if (tip_text.empty())
    return false;

  tip_text += L"Click to see all.";
  tip_text = LimitText(tip_text, 255);
  Taiga.current_tip_type = taiga::kTipTypeTorrent;
  Taskbar.Tip(L"", L"", 0);
  Taskbar.Tip(tip_text.c_str(), tip_title.c_str(), NIIF_INFO);

  return true;
}

////////////////////////////////////////////////////////////////////////////////

bool OnTwitterRequest(string_t& auth_pin) {
  ClearStatusText();

  InputDialog dlg;
  dlg.title = L"Twitter Authorization";
  dlg.info = L"Please enter the PIN shown on the page after logging into "
             L"Twitter:";
  dlg.Show();

  if (dlg.result == IDOK && !dlg.text.empty()) {
    auth_pin = dlg.text;
    return true;
  }

  return false;
}

void OnTwitterAuth(bool success) {
  ChangeStatusText(success ?
      L"Taiga is now authorized to post to this Twitter account: " +
      Settings[taiga::kShare_Twitter_Username] :
      L"Twitter authorization failed.");

  SettingsDialog.RefreshTwitterLink();
}

void OnTwitterPost(bool success, const string_t& error) {
  ChangeStatusText(success ?
      L"Twitter status updated." :
      L"Twitter status update failed. (" + error + L")");
}

////////////////////////////////////////////////////////////////////////////////

void OnLogin() {
  ChangeStatusText(L"Logged in as " +
                   Settings[taiga::kSync_Service_Mal_Username]);

  Menus.UpdateAll();

  MainDialog.UpdateTip();
  MainDialog.UpdateTitle();
  MainDialog.EnableInput(true);
}

void OnLogout() {
}

void OnUpdate() {
  UpdateDialog.PostMessage(WM_CLOSE);
}

}  // namespace ui