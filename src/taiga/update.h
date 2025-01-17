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

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include "base/rss.h"

namespace hypr::detail {
struct Transfer;
}

namespace taiga {

namespace detail {

class UpdateHelper {
public:
  void Cancel();
  void Check();
  bool Download();
  bool IsRestartRequired() const;

  std::wstring GetCurrentAnimeRelationsModified() const;

  class Item : public rss::Item {
  public:
    std::wstring taiga_anime_relations_location;
    std::wstring taiga_anime_relations_modified;
  };
  std::vector<Item> items;

private:
  void CheckAnimeRelations();
  bool IsAnimeRelationsAvailable() const;
  bool ParseData(std::wstring data);
  bool RunInstaller();
  bool OnTransfer(const hypr::detail::Transfer& transfer);

  std::wstring download_path_;
  std::unique_ptr<Item> current_item_;
  std::unique_ptr<Item> latest_item_;
  std::atomic_bool transfer_cancelled_ = false;
  bool restart_required_ = false;
  bool update_available_ = false;
  std::wstring client_uid_;
};

}  // namespace detail

inline detail::UpdateHelper updater;

}  // namespace taiga
