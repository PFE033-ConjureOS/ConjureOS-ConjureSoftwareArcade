//
// Created by jonny on 2023-10-17.
// Started from duplication of ConjureProvider.cpp
//

#include "ConjureProvider.h"

#include "Log.h"
#include "Paths.h"
#include "providers/ProviderUtils.h"
#include "providers/SearchContext.h"
#include "providers/conjure/ConjureMetadata.h"
#include "providers/conjure/ConjureFilter.h"
#include "utils/StdHelpers.h"
#include "utils/PathTools.h"

#include <QDir>
#include <QSettings>
#include <QStandardPaths>
#include <QStringBuilder>

namespace {
    QString find_conjure_data_dir() {
        std::vector<QString> possible_dirs;

        possible_dirs.emplace_back(QLatin1String("`%1`\\ConjureGames").arg(QStandardPaths::DocumentsLocation));

        for (const QString &dir_path: possible_dirs) {
            if (QFileInfo::exists(dir_path))
                return dir_path;
        }

        return {};
    }
} // namespace

namespace providers {
    namespace conjure {
        ConjureProvider::ConjureProvider(QObject *parent)
                : Provider(QLatin1String("conjure_metafiles"), QStringLiteral("Conjure Metafiles"),PROVIDER_FLAG_INTERNAL, parent) {}


        Provider &ConjureProvider::run(SearchContext &sctx) {
            const QString conjure_dir_path = find_conjure_data_dir();
            if (conjure_dir_path.isEmpty()) {
                Log::info(display_name(), LOGMSG("No Conjure game found"));
                return *this;
            }

            const Metadata metahelper(display_name());
            std::vector<FileFilter> all_filters;

            const float progress_step = 1.f / conjure_dir_path.size();
            float progress = 0.f;

            for (const QString &path: conjure_dir_path) {
                Log::info(display_name(), LOGMSG("Found `%1`").arg(::pretty_path(path)));

                std::vector<FileFilter> filters = metahelper.apply_metafile(path, sctx);
                all_filters.insert(all_filters.end(),
                                   std::make_move_iterator(filters.begin()),
                                   std::make_move_iterator(filters.end()));

                progress += progress_step;
                emit progressChanged(progress);
            }

            for (FileFilter &filter: all_filters) {
                apply_filter(filter, sctx);

                for (QString &dir_path: filter.directories)
                    sctx.pegasus_add_game_dir(dir_path);
            }

            return *this;
        }
    } // namespace conjure
} // namespace providers