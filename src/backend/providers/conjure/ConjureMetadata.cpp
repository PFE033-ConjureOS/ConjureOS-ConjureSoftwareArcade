//
// Created by jonny on 2023-10-17.
// Started from duplication of PegasusMetadata.cpp
//

#include "ConjureMetadata.h"

#include "AppSettings.h"
#include "Log.h"
#include "PegasusAssets.h"
#include "model/gaming/Assets.h"
#include "model/gaming/Collection.h"
#include "model/gaming/Game.h"
#include "parsers/MetaFile.h"
#include "providers/SearchContext.h"
#include "providers/conjure/ConjureFilter.h"
#include "types/AssetType.h"
#include "utils/PathTools.h"
#include "providers/ProviderUtils.h"

#include <QUrl>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <array>
#include <QUuid>

namespace {
constexpr size_t ISSUE_LOG_LIMIT = 100;

QStringList tokenize_by_comma(const QString &str) {
    QStringList list = str.split(QLatin1Char(','), Qt::SkipEmptyParts);
    for (QString &item: list)
        item = item.trimmed();

    return list;
}
} // namespace


namespace providers {
namespace conjure {
ParserState::ParserState(const QString &path_ref)
    : path(path_ref), dir(QFileInfo(path_ref).absoluteDir()) {}

enum class CollAttrib : unsigned char {
    SHORT_NAME,
    DIRECTORIES,
    EXTENSIONS,
    FILES,
    REGEX,
    SHORT_DESC,
    LONG_DESC,
    LAUNCH_CMD,
    LAUNCH_WORKDIR,
    SORT_BY,
};

enum class GameAttrib : unsigned char {
    GAME,
    FILES,
    VERSION,
    DEVELOPERS,
    PUBLISHERS,
    GENRES,
    TAGS,
    PLAYER_COUNT,
    SHORT_DESC,
    LONG_DESC,
    RELEASE,
    MODIFICATION,
    RATING,
    LEADERBOARD,
    THUMBNAIL,
    IMAGE,
    LAUNCH_CMD,
    LAUNCH_WORKDIR,
    SORT_BY,
};

Metadata::Metadata(QString log_tag)
    : m_log_tag(std::move(log_tag)),
    m_primary_key_collection("collection"),
    m_primary_key_game("id"),
    m_json_cache_dir(QStringLiteral("conjure")),
    m_coll_attribs{
                   {QStringLiteral("shortname"),         CollAttrib::SHORT_NAME},
                   {QStringLiteral("launch"),            CollAttrib::LAUNCH_CMD},
                   {QStringLiteral("command"),           CollAttrib::LAUNCH_CMD},
                   {QStringLiteral("workdir"),           CollAttrib::LAUNCH_WORKDIR},
                   {QStringLiteral("cwd"),               CollAttrib::LAUNCH_WORKDIR},
                   {QStringLiteral("directory"),         CollAttrib::DIRECTORIES},
                   {QStringLiteral("directories"),       CollAttrib::DIRECTORIES},
                   {QStringLiteral("extension"),         CollAttrib::EXTENSIONS},
                   {QStringLiteral("extensions"),        CollAttrib::EXTENSIONS},
                   {QStringLiteral("file"),              CollAttrib::FILES},
                   {QStringLiteral("files"),             CollAttrib::FILES},
                   {QStringLiteral("regex"),             CollAttrib::REGEX},
                   {QStringLiteral("ignore-extension"),  CollAttrib::EXTENSIONS},
                   {QStringLiteral("ignore-extensions"), CollAttrib::EXTENSIONS},
                   {QStringLiteral("ignore-file"),       CollAttrib::FILES},
                   {QStringLiteral("ignore-files"),      CollAttrib::FILES},
                   {QStringLiteral("ignore-regex"),      CollAttrib::REGEX},
                   {QStringLiteral("summary"),           CollAttrib::SHORT_DESC},
                   {QStringLiteral("description"),       CollAttrib::LONG_DESC},
                   // sort name variations
                   {QStringLiteral("sortby"),            CollAttrib::SORT_BY},
                   {QStringLiteral("sort_by"),           CollAttrib::SORT_BY},
                   {QStringLiteral("sort-by"),           CollAttrib::SORT_BY},
                   }, m_game_attribs{
                     {QStringLiteral("game"),          GameAttrib::GAME},
                     {QStringLiteral("file"),          GameAttrib::FILES},
                     {QStringLiteral("files"),         GameAttrib::FILES},
                     {QStringLiteral("version"),       GameAttrib::VERSION},
                     {QStringLiteral("launch"),        GameAttrib::LAUNCH_CMD},
                     {QStringLiteral("command"),       GameAttrib::LAUNCH_CMD},
                     {QStringLiteral("workdir"),       GameAttrib::LAUNCH_WORKDIR},
                     {QStringLiteral("cwd"),           GameAttrib::LAUNCH_WORKDIR},
                     {QStringLiteral("developer"),     GameAttrib::DEVELOPERS},
                     {QStringLiteral("developers"),    GameAttrib::DEVELOPERS},
                     {QStringLiteral("publisher"),     GameAttrib::PUBLISHERS},
                     {QStringLiteral("publishers"),    GameAttrib::PUBLISHERS},
                     {QStringLiteral("genre"),         GameAttrib::GENRES},
                     {QStringLiteral("genres"),        GameAttrib::GENRES},
                     {QStringLiteral("tag"),           GameAttrib::TAGS},
                     {QStringLiteral("tags"),          GameAttrib::TAGS},
                     {QStringLiteral("players"),       GameAttrib::PLAYER_COUNT},
                     {QStringLiteral("summary"),       GameAttrib::SHORT_DESC},
                     {QStringLiteral("description"),   GameAttrib::LONG_DESC},
                     {QStringLiteral("release"),       GameAttrib::RELEASE},
                     {QStringLiteral("modification"),  GameAttrib::MODIFICATION},
                     {QStringLiteral("rating"),        GameAttrib::RATING},
                     {QStringLiteral("leaderboard"),   GameAttrib::LEADERBOARD},
                     {QStringLiteral("thumbnailpath"), GameAttrib::THUMBNAIL},
                     {QStringLiteral("thumbnailPath"), GameAttrib::THUMBNAIL},
                     {QStringLiteral("thumbnail"),     GameAttrib::THUMBNAIL},
                     {QStringLiteral("imagePath"),     GameAttrib::IMAGE},
                     {QStringLiteral("imagepath"),     GameAttrib::IMAGE},
                     {QStringLiteral("image"),         GameAttrib::IMAGE},
                     // sort title variations
                     {QStringLiteral("sorttitle"),     GameAttrib::SORT_BY},
                     {QStringLiteral("sortname"),      GameAttrib::SORT_BY},
                     {QStringLiteral("sort_title"),    GameAttrib::SORT_BY},
                     {QStringLiteral("sort_name"),     GameAttrib::SORT_BY},
                     {QStringLiteral("sort-title"),    GameAttrib::SORT_BY},
                     {QStringLiteral("sort-name"),     GameAttrib::SORT_BY},
                     {QStringLiteral("sortby"),        GameAttrib::SORT_BY},
                     {QStringLiteral("sort_by"),       GameAttrib::SORT_BY},
                     {QStringLiteral("sort-by"),       GameAttrib::SORT_BY},
                     }, rx_asset_key(QStringLiteral(R"(^assets?\.(.+)$)")),
    rx_count_range(QStringLiteral("^(\\d+)(-(\\d+))?$")), rx_percent(QStringLiteral("^\\d+%$")),
    rx_float(QStringLiteral("^\\d(\\.\\d+)?$")),
    rx_date(QStringLiteral("^(\\d{4})(-(\\d{1,2}))?(-(\\d{1,2}))?$")),
    rx_unescaped_newline(QStringLiteral(R"((?<!\\)\\n)")),
    rx_uri(QStringLiteral("^[a-zA-Z][a-zA-Z0-9+\\-.]+:.+")) {}

void Metadata::print_error(ParserState &ps, const metafile::Error &err) const {
    ps.found_issues++;
    if (ps.found_issues <= ISSUE_LOG_LIMIT) {
        Log::error(m_log_tag, LOGMSG("`%1`, line %2: %3")
                                  .arg(::pretty_path(ps.path), QString::number(err.line), err.message));
    }
}


void Metadata::print_warning(ParserState &ps, const metafile::Entry &entry, const QString &msg) const {
    ps.found_issues++;
    if (ps.found_issues <= ISSUE_LOG_LIMIT) {
        Log::warning(m_log_tag, LOGMSG("`%1`, line %2: %3")
                                    .arg(::pretty_path(ps.path), QString::number(entry.line), msg));
    }
}

const QString &Metadata::first_line_of(ParserState &ps, const metafile::Entry &entry) const {
    Q_ASSERT(!entry.key.isEmpty());
    Q_ASSERT(!entry.values.empty());
    Q_ASSERT(!entry.values.front().isEmpty());

    if (entry.values.size() > 1) {
        print_warning(ps, entry,
                      LOGMSG("Expected a single line value for `%1`, but got more; the rest of the lines will be ignored")
                          .arg(entry.key));
    }

    return entry.values.front();
}

void Metadata::replace_newlines(QString &text) const {
    text.replace(rx_unescaped_newline, QStringLiteral("\n"))  // '\n' -> [newline]
        .replace(QLatin1String(R"(\\n)"), QLatin1String(R"(\n)"));  // '\\n' -> '\n'
}


void Metadata::apply_collection_entry(ParserState &ps, const metafile::Entry &entry) const {
    Q_ASSERT(ps.cur_coll);
    Q_ASSERT(!ps.filters.empty());
    Q_ASSERT(!ps.cur_game);


    const auto attrib_it = m_coll_attribs.find(entry.key);
    if (attrib_it == m_coll_attribs.cend()) {
        print_warning(ps, entry, LOGMSG("Unrecognized collection property `%1`, ignored").arg(entry.key));
        return;
    }

    FileFilterGroup &filter_group = entry.key.startsWith(QLatin1String("ignore-"))
                                        ? ps.filters.back().exclude
                                        : ps.filters.back().include;

    switch (attrib_it->second) {
    case CollAttrib::SHORT_NAME:
        ps.cur_coll->setShortName(first_line_of(ps, entry));
        break;
    case CollAttrib::LAUNCH_CMD:
        ps.cur_coll->setCommonLaunchCmd(metafile::merge_lines(entry.values));
        break;
    case CollAttrib::LAUNCH_WORKDIR:
        ps.cur_coll->setCommonLaunchWorkdir(first_line_of(ps, entry));
        break;
    case CollAttrib::DIRECTORIES:
        for (const QString &line: entry.values) {
            const QFileInfo finfo(ps.dir, line);
            if (!finfo.isDir()) {
                print_warning(ps, entry, LOGMSG("Directory path `%1` doesn't seem to exist").arg(
                                             ::pretty_path(finfo)));
                continue;
            }

            QString path = ::clean_abs_path(finfo);
            ps.filters.back().directories.emplace_back(std::move(path));
        }
        break;
    case CollAttrib::EXTENSIONS: {
        QStringList new_exts = ::tokenize_by_comma(first_line_of(ps, entry).toLower());
        filter_group.extensions.insert(
            filter_group.extensions.end(),
            std::make_move_iterator(new_exts.begin()),
            std::make_move_iterator(new_exts.end()));
    }
    break;
    case CollAttrib::FILES:
        for (const QString &path: entry.values)
            filter_group.files.emplace_back(path);
        break;
    case CollAttrib::REGEX: {
        QRegularExpression new_rx(first_line_of(ps, entry));
        if (!new_rx.isValid()) {
            print_warning(ps, entry, LOGMSG("Invalid regular expression"));
            return;
        }

        filter_group.regex = new_rx;
    }
    break;
    case CollAttrib::SHORT_DESC: {
        QString text = metafile::merge_lines(entry.values);
        replace_newlines(text);
        ps.cur_coll->setSummary(std::move(text));
    }
    break;
    case CollAttrib::LONG_DESC: {
        QString text = metafile::merge_lines(entry.values);
        replace_newlines(text);
        ps.cur_coll->setDescription(std::move(text));
    }
    break;
    case CollAttrib::SORT_BY:
        ps.cur_coll->setSortBy(first_line_of(ps, entry));
        break;
    }
}

void Metadata::apply_game_entry(ParserState &ps, const metafile::Entry &entry, SearchContext &sctx) const {
    // NOTE: m_cur_coll may be null when the entry is defined before any collection
    Q_ASSERT(ps.cur_game);

    const auto attrib_it = m_game_attribs.find(entry.key);
    if (attrib_it == m_game_attribs.cend()) {
        print_warning(ps, entry, LOGMSG("Unrecognized game property `%1`, ignored").arg(entry.key));
        return;
    }

    switch (attrib_it->second) {

    case GameAttrib::GAME: {
        ps.cur_game->setTitle(first_line_of(ps, entry));
    }
    break;
    case GameAttrib::FILES:
        for (const QString &line: entry.values) {
            const bool is_uri = rx_uri.match(line).hasMatch();
            if (is_uri) {
                model::Game *const game_ptr = sctx.game_by_uri(line);
                if (game_ptr == ps.cur_game) {
                    print_warning(ps, entry, LOGMSG("Duplicate URI entry detected: `%1`").arg(line));
                    continue;
                }
                if (game_ptr != nullptr && game_ptr != ps.cur_game) {
                    print_warning(ps, entry,
                                  LOGMSG("This URI already belongs to a different game: `%1`").arg(line));
                    continue;
                }

                Q_ASSERT(game_ptr == nullptr);
                sctx.game_add_uri(*ps.cur_game, line);
            } else {
                QFileInfo finfo(ps.dir, line);
                if (AppSettings::general.verify_files && !finfo.exists()) {
                    print_warning(ps, entry,
                                  LOGMSG("Game file `%1` doesn't seem to exist").arg(::pretty_path(finfo)));
                    continue;
                }

                QString path = ::clean_abs_path(finfo);
                model::Game *const game_ptr = sctx.game_by_filepath(path);
                if (game_ptr == ps.cur_game) {
                    print_warning(ps, entry, LOGMSG("Duplicate file entry detected: `%1`").arg(line));
                    continue;
                }
                if (game_ptr != nullptr && game_ptr != ps.cur_game) {
                    print_warning(ps, entry,
                                  LOGMSG("This file already belongs to a different game: `%1`").arg(line));
                    continue;
                }

                Q_ASSERT(game_ptr == nullptr);
                sctx.game_add_filepath(*ps.cur_game, std::move(path));
            }
        }
        break;
    case GameAttrib::VERSION: {
        QString text_version = metafile::merge_lines(entry.values);
        replace_newlines(text_version);
        ps.cur_game->setVersion(text_version);
    }
    break;
    case GameAttrib::DEVELOPERS:
        for (const QString &line: entry.values)
            ps.cur_game->developerList().append(line);
        break;
    case GameAttrib::PUBLISHERS:
        for (const QString &line: entry.values)
            ps.cur_game->publisherList().append(line);
        break;
    case GameAttrib::GENRES:
        for (const QString &line: entry.values)
            ps.cur_game->genreList().append(line);
        break;
    case GameAttrib::TAGS:
        for (const QString &line: entry.values)
            ps.cur_game->tagList().append(line);
        break;
    case GameAttrib::PLAYER_COUNT: {
        const auto rx_match = rx_count_range.match(first_line_of(ps, entry));
        if (rx_match.hasMatch()) {
            const short a = rx_match.capturedRef(1).toShort();
            const short b = rx_match.capturedRef(3).toShort();
            ps.cur_game->setPlayerCount(std::max(a, b));
        }
    }
    break;
    case GameAttrib::SHORT_DESC: {
        QString text = metafile::merge_lines(entry.values);
        replace_newlines(text);
        ps.cur_game->setSummary(std::move(text));
    }
    break;
    case GameAttrib::LONG_DESC: {
        QString text = metafile::merge_lines(entry.values);
        replace_newlines(text);
        ps.cur_game->setDescription(std::move(text));
    }
    break;
    case GameAttrib::RELEASE: {
        const auto rx_match = rx_date.match(first_line_of(ps, entry));
        if (!rx_match.hasMatch()) {
            print_warning(ps, entry,
                          LOGMSG("Incorrect date format, should be YYYY, YYYY-MM or YYYY-MM-DD"));
            return;
        }

        const int y = qMax(1, rx_match.captured(1).toInt());
        const int m = qBound(1, rx_match.captured(3).toInt(), 12);
        const int d = qBound(1, rx_match.captured(5).toInt(), 31);
        QDate date(y, m, d);
        if (!date.isValid()) {
            print_warning(ps, entry, LOGMSG("Invalid date"));
            return;
        }

        ps.cur_game->setReleaseDate(std::move(date));
    }
    break;
    case GameAttrib::MODIFICATION: {
        const auto rx_match = rx_date.match(first_line_of(ps, entry));
        if (!rx_match.hasMatch()) {
            print_warning(ps, entry,
                          LOGMSG("Incorrect date format, should be YYYY, YYYY-MM or YYYY-MM-DD"));
            return;
        }

        const int y = qMax(1, rx_match.captured(1).toInt());
        const int m = qBound(1, rx_match.captured(3).toInt(), 12);
        const int d = qBound(1, rx_match.captured(5).toInt(), 31);
        QDate date(y, m, d);
        if (!date.isValid()) {
            print_warning(ps, entry, LOGMSG("Invalid date"));
            return;
        }

        ps.cur_game->setLastUpdatedDate(std::move(date));
    }
    break;
    case GameAttrib::RATING: {
        const QString &line = first_line_of(ps, entry);

        const auto rx_match_a = rx_percent.match(line);
        if (rx_match_a.hasMatch()) {
            ps.cur_game->setRating(line.leftRef(line.length() - 1).toFloat() / 100.f);
            return;
        }
        const auto rx_match_b = rx_float.match(line);
        if (rx_match_b.hasMatch()) {
            ps.cur_game->setRating(line.toFloat());
            return;
        }

        print_warning(ps, entry, LOGMSG("Failed to parse the rating value"));
    }
    break;
    case GameAttrib::LEADERBOARD: {
        QString text = metafile::merge_lines(entry.values);

        bool hasLeaderboard = text.contains("True") || text.contains("true");

        ps.cur_game->setHasLeaderboard(hasLeaderboard);

        if (!hasLeaderboard) break; //set leaderboard to game if properties leaderboard is true
        fetch_leaderboard(*ps.cur_game, sctx);

    }
    break;
    case GameAttrib::THUMBNAIL: {
        QString text = metafile::merge_lines(entry.values);
        ps.cur_game->assetsMut().add_uri(AssetType::CARTRIDGE, assetline_to_url(ps, entry,
                                                                                text)); //TODO update thumbnail to be another type of assets??
    }
    break;
    case GameAttrib::IMAGE: {
        QString text = metafile::merge_lines(entry.values);
        ps.cur_game->assetsMut().add_uri(AssetType::UI_BANNER, assetline_to_url(ps, entry,
                                                                                text));//TODO update BANNER to be another type of assets??
    }
    break;
    case GameAttrib::LAUNCH_CMD:
        ps.cur_game->setLaunchCmd(metafile::merge_lines(entry.values));
        break;
    case GameAttrib::LAUNCH_WORKDIR:
        ps.cur_game->setLaunchWorkdir(first_line_of(ps, entry));
        break;
    case GameAttrib::SORT_BY:
        ps.cur_game->setSortBy(first_line_of(ps, entry));
        break;
    }
}

// Returns true if the entry is an extra entry
bool Metadata::apply_extra_entry_maybe(ParserState &ps, const metafile::Entry &entry) const {
    Q_ASSERT(ps.cur_coll || ps.cur_game);

    if (!entry.key.startsWith(QLatin1String("x-")))
        return false;

    const QString key = entry.key.mid(2); /* len of 'x-' */
    if (key.isEmpty()) {
        print_warning(ps, entry, LOGMSG("Invalid extra field, entry ignored"));
        return true;
    }

    QVariantMap &extra_map = ps.cur_game
                                 ? ps.cur_game->extraMapMut()
                                 : ps.cur_coll->extraMapMut();

    QStringList values;
    values.reserve(entry.values.size());
    for (const QString &value: entry.values)
        values.append(value);

    extra_map.insert(std::move(key), std::move(values));
    return true;
}

QString Metadata::assetline_to_url(ParserState &ps, const metafile::Entry &entry, const QString &value) const {
    Q_ASSERT(!value.isEmpty());

    if (value.startsWith(QLatin1String("http://")) || value.startsWith(QLatin1String("https://")))
        return value;

    const QFileInfo finfo(ps.dir, value);
    if (AppSettings::general.verify_files && !finfo.exists()) {
        print_warning(ps, entry, LOGMSG("Asset file `%1` doesn't seem to exist").arg(finfo.absoluteFilePath()));
        return QString();
    }

    return QUrl::fromLocalFile(finfo.absoluteFilePath()).toString();
}

// Returns true if the entry is an asset entry
bool Metadata::apply_asset_entry_maybe(ParserState &ps, const metafile::Entry &entry) const {
    Q_ASSERT(ps.cur_coll || ps.cur_game);

    const auto rx_match = rx_asset_key.match(entry.key);
    if (!rx_match.hasMatch())
        return false;

    const QString asset_key = rx_match.captured(1);
    const AssetType asset_type = pegasus_assets::str_to_type(asset_key);
    if (asset_type == AssetType::UNKNOWN) {
        print_warning(ps, entry, LOGMSG("Unknown asset type `%1`, entry ignored").arg(asset_key));
        return true;
    }

    model::Assets &assets = ps.cur_game
                                ? ps.cur_game->assetsMut()
                                : ps.cur_coll->assetsMut();
    for (const QString &line: entry.values)
        assets.add_uri(asset_type, assetline_to_url(ps, entry, line));

    return true;
}

void Metadata::apply_entry(ParserState &ps, const metafile::Entry &entry, SearchContext &sctx) const {
    Q_ASSERT(!entry.key.isEmpty());
    Q_ASSERT(!entry.values.empty());
    Q_ASSERT(!entry.values.front().isEmpty());

    // TODO: set cur_* by the return value of emplace
    if (entry.key == m_primary_key_collection) {
        const QString &name = first_line_of(ps, entry);

        ps.cur_coll = sctx.get_or_create_collection(name);
        ps.cur_coll->setCommonLaunchCmdBasedir(ps.dir.path());
        ps.cur_game = nullptr;

        ps.all_colls.emplace_back(ps.cur_coll);
        ps.filters.emplace_back(ps.cur_coll, ps.dir.path());
        return;
    }

    if (entry.key == m_primary_key_game) {
        ps.cur_game = sctx.create_game();
        ps.cur_game->setId(first_line_of(ps, entry));
        ps.cur_game->setLaunchCmdBasedir(ps.dir.path());

        // Add to the ones found so far
        for (model::Collection *const coll: ps.all_colls)
            sctx.game_add_to(*ps.cur_game, *coll);

        return;
    }


    if (!ps.cur_coll && !ps.cur_game) {
        print_warning(ps, entry, LOGMSG("No `collection` or `game` defined yet, entry ignored"));
        return;
    }

    if (apply_extra_entry_maybe(ps, entry))
        return;
    if (apply_asset_entry_maybe(ps, entry))
        return;

    if (ps.cur_game)
        apply_game_entry(ps, entry, sctx);
    else
        apply_collection_entry(ps, entry);
}


std::vector<FileFilter> Metadata::apply_metafile(const QString &metafile_path, SearchContext &sctx) const {
    ParserState ps(metafile_path);

    const auto on_error = [&](const metafile::Error &error) {
        print_error(ps, error);
    };
    const auto on_entry = [&](const metafile::Entry &entry) {
        apply_entry(ps, entry, sctx);
    };

    if (!metafile::read_file(metafile_path, on_entry, on_error)) {
        Log::error(m_log_tag, LOGMSG("Failed to read metadata file `%1`")
                                  .arg(::pretty_path(metafile_path)));
    }
    if (ps.found_issues > ISSUE_LOG_LIMIT) {
        Log::warning(m_log_tag,
                     LOGMSG("%1 other issues omitted").arg(QString::number(ps.found_issues - ISSUE_LOG_LIMIT)));
    }

    return std::move(ps.filters);
}

bool apply_json_C(model::Game &game, const QJsonDocument &json) {

    if (json.isNull()) {
        return false;
    }
    QJsonObject jsonObject = json.object();

    const QJsonArray scoresArray = jsonObject["scores"].toArray();

    std::vector<model::ScoreLine *> leaderboard;
    leaderboard.reserve(scoresArray.size());

    for (const auto &entry: scoresArray) {

        const QJsonObject entryObject = entry.toObject();
        int playerId = entryObject["playerId"].toInt();
        const int score = entryObject["score"].toInt();
        const QDateTime date = QDateTime::fromString(entryObject["date"].toString(), Qt::ISODateWithMs);

        QString username = jsonObject["players"].toObject()[QString::number(playerId)].toObject()["username"].toString();
        leaderboard.push_back(new model::ScoreLine(username, score, date));
    }

    game.setLeaderboard(std::move(leaderboard));

    return true;
}


void Metadata::fetch_leaderboard(model::Game &game, SearchContext &sctx) const {

    const QString domain = sctx.conjure_domain;

    model::Game *const game_ptr = &game;

    const QString gameId = game_ptr->id();

    const QString url_str = QStringLiteral("http://%1/games/%2/scores/detailed").arg(domain, gameId);

    const QUrl url(url_str, QUrl::StrictMode);

    Q_ASSERT(url.isValid());

    if (Q_UNLIKELY(!url.isValid()))
        return;

    using JsonCallback = std::function<bool(model::Game &, const QJsonDocument &)>;

    const std::tuple<QUrl, JsonCallback> request = std::make_tuple(url, apply_json_C);

    QString log_tag = m_log_tag;
    QString json_cache_dir = m_json_cache_dir;

    const JsonCallback &json_callback = std::get<1>(request);

    sctx.schedule_download_conjure(std::get<0>(request),
                                   [log_tag, json_cache_dir, game_ptr, json_callback, gameId](
                                       QNetworkReply *const reply) {

                                       if (reply->error()) {
                                           Log::warning(log_tag,
                                                        LOGMSG("Fetching scores for `%1` failed: %2")
                                                            .arg(game_ptr->title(),
                                                                 reply->errorString()));
                                           return;
                                       }

                                       const QByteArray raw_data = reply->readAll();
                                       const QJsonDocument json = QJsonDocument::fromJson(raw_data);
                                       if (json.isNull()) {
                                           Log::warning(log_tag, LOGMSG(
                                                                     "Failed to parse the response of the server for game '%1'"
                                                                     ).arg(game_ptr->title()));
                                           return;
                                       }

                                       const bool success = json_callback(*game_ptr, json);
                                       if (success) {
                                           providers::cache_json(log_tag, json_cache_dir, gameId,
                                                                 json.toJson(
                                                                     QJsonDocument::Compact));
                                       }
                                   });
}
} // namespace conjure
} // namespace providers
