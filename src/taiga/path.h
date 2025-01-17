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

namespace sync {
enum class ServiceId;
}

namespace taiga {

enum class Path {
  Data,
  Database,
  DatabaseAnime,
  DatabaseAnimeRelations,
  DatabaseManga,
  DatabaseImage,
  Feed,
  FeedHistory,
  Media,
  Settings,
  Test,
  TestRecognition,
  Theme,
  ThemeCurrent,
  User,
  UserHistory,
  UserLibrary,
  UserLibraryManga
};

std::wstring GetUserDirectoryName(const sync::ServiceId service_id);
std::wstring GetUserDirectoryName();
std::wstring GetPath(Path path);

}  // namespace taiga
