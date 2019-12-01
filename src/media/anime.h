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

#include <array>
#include <map>
#include <string>
#include <vector>

#include "base/time.h"
#include "sync/service.h"

namespace anime {

enum AnimeId {
  ID_NOTINLIST = -1,
  ID_UNKNOWN = 0
};

enum class AgeRating {
  Unknown,
  G,
  PG,
  PG13,
  R17,
  R18,
};

enum class SeriesStatus {
  Unknown,
  FinishedAiring,
  Airing,
  NotYetAired
};

enum class SeriesType {
  Unknown,
  Tv,
  Ova,
  Movie,
  Special,
  Ona,
  Music,
};

enum class MyStatus {
  NotInList,
  Watching,
  PlanToWatch,
  Completed,
  OnHold,
  Dropped,
};

enum class TitleLanguage {
  Romaji,
  English,
  Native,
};

constexpr std::array<SeriesStatus, 3> kSeriesStatuses{
  SeriesStatus::FinishedAiring,
  SeriesStatus::Airing,
  SeriesStatus::NotYetAired,
};

constexpr std::array<SeriesType, 6> kSeriesTypes{
  SeriesType::Tv,
  SeriesType::Ova,
  SeriesType::Movie,
  SeriesType::Special,
  SeriesType::Ona,
  SeriesType::Music,
};

constexpr std::array<MyStatus, 5> kMyStatuses{
  MyStatus::Watching,
  MyStatus::Completed,
  MyStatus::PlanToWatch,
  MyStatus::OnHold,
  MyStatus::Dropped,
};

constexpr int kUnknownEpisodeCount = -1;
constexpr int kUnknownEpisodeLength = -1;
constexpr double kUnknownScore = 0.0;
constexpr int kUserScoreMax = 100;

struct Titles {
  std::wstring romaji;
  std::wstring english;
  std::wstring japanese;
  std::vector<std::wstring> synonyms;
};

struct SeriesInformation {
  int id = AnimeId::ID_UNKNOWN;
  std::map<sync::ServiceId, std::wstring> uids;
  sync::ServiceId source = sync::ServiceId::Unknown;
  std::time_t last_modified = 0;
  int episode_count = kUnknownEpisodeCount;
  int episode_length = kUnknownEpisodeLength;
  AgeRating age_rating = AgeRating::Unknown;
  SeriesStatus status = SeriesStatus::Unknown;
  SeriesType type = SeriesType::Unknown;
  Date start_date;
  Date end_date;
  float score = 0.0f;
  int popularity_rank = 0;
  std::wstring image_url;
  std::wstring slug;
  std::wstring synopsis;
  Titles titles;
  std::vector<std::wstring> genres;
  std::vector<std::wstring> producers;
};

struct MyInformation {
  std::wstring id;
  int watched_episodes = 0;
  int score = 0;
  MyStatus status = MyStatus::NotInList;
  int rewatched_times = 0;
  bool rewatching = false;
  int rewatching_ep = 0;
  Date date_start;
  Date date_finish;
  std::wstring last_updated;
  std::wstring tags;
  std::wstring notes;
};

struct LocalInformation {
  int last_aired_episode = 0;
  std::vector<bool> available_episodes;
  std::wstring next_episode_path;
  time_t next_episode_time = 0;
  std::wstring folder;
  std::vector<std::wstring> synonyms;
  bool playing = false;
  bool use_alternative = false;
};

}  // namespace anime
