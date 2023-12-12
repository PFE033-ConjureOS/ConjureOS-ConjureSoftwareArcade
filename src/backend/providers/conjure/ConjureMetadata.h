//
// Created by jonny on 2023-10-17.
// Started from duplication of ConjureMetadata.h
//

#pragma once

#include "utils/HashMap.h"
#include "utils/NoCopyNoMove.h"
#include "model/gaming/ScoreLine.h"

#include <QDir>
#include <QString>
#include <QRegularExpression>

namespace metafile { struct Entry; }
namespace metafile { struct Error; }
namespace model { class Game; }
namespace model { class Collection; }
namespace providers { class SearchContext; }


namespace providers {
    namespace conjure {

        enum class CollAttrib : unsigned char;
        enum class GameAttrib : unsigned char;
        struct FileFilter;


        struct ParserState {
            const QString &path;
            const QDir dir;
            model::Game *cur_game = nullptr;
            model::Collection *cur_coll = nullptr;
            std::vector<FileFilter> filters;
            std::vector<model::Collection *> all_colls;
            size_t found_issues = 0;

            explicit ParserState(const QString &);
            NO_COPY_NO_MOVE(ParserState)
        };


        class Metadata {
        public:
            explicit Metadata(QString);

            std::vector<FileFilter> apply_metafile(const QString &, SearchContext &) const;
            void fetch_leaderboard(model::Game&, SearchContext&) const;

        private:
            const QString m_log_tag;
            const QString m_json_cache_dir;

            const QLatin1String m_primary_key_collection;
            const QLatin1String m_primary_key_game;

            const HashMap<QString, CollAttrib> m_coll_attribs;
            const HashMap<QString, GameAttrib> m_game_attribs;

            const QRegularExpression rx_asset_key;
            const QRegularExpression rx_count_range;
            const QRegularExpression rx_percent;
            const QRegularExpression rx_float;
            const QRegularExpression rx_date;
            const QRegularExpression rx_unescaped_newline;
            const QRegularExpression rx_uri;

            const QString m_json_suffix;


            void print_error(ParserState &, const metafile::Error &) const;

            void print_warning(ParserState &, const metafile::Entry &, const QString &) const;

            const QString &first_line_of(ParserState &, const metafile::Entry &) const;

            void replace_newlines(QString &) const;

            void apply_collection_entry(ParserState &, const metafile::Entry &) const;

            void apply_game_entry(ParserState &, const metafile::Entry &, SearchContext &) const;

            bool apply_extra_entry_maybe(ParserState &, const metafile::Entry &) const;

            bool apply_asset_entry_maybe(ParserState &, const metafile::Entry &) const;

            void apply_entry(ParserState &, const metafile::Entry &, SearchContext &) const;

            QString assetline_to_url(ParserState &, const metafile::Entry &, const QString &) const;
        };
    } // namespace conjure
} // namespace providers
