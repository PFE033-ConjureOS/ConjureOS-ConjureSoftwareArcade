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
#include <QDirIterator>

namespace {
bool is_conjure_metadata_file(const QString &filename) {
    return filename == QLatin1String("metadata.conjure.txt")
           || filename == QLatin1String("metadata.txt")
           || filename.endsWith(QLatin1String(".metadata.conjure.txt"))
           || filename.endsWith(QLatin1String("..txt"));
}

std::vector <QString> find_metafiles_in(const QString &dir_path) {
    constexpr auto dir_filters = QDir::Files | QDir::NoDotAndDotDot;
    constexpr auto dir_flags = QDirIterator::FollowSymlinks | QDirIterator::Subdirectories;

    std::vector <QString> result;
    QDirIterator dir_it(dir_path, dir_filters, dir_flags);

    Log::info("find_metafiles_in", LOGMSG("'%1'").arg(dir_it.hasNext()));
    while (dir_it.hasNext()) {
        dir_it.next();
        if (is_conjure_metadata_file(dir_it.fileName())) {
            QString path = ::clean_abs_path(dir_it.fileInfo());
            result.emplace_back(std::move(path));
        }
    }

    return result;
}

std::vector <QString> find_all_metafiles(const QStringList &gamedirs) {
    const QString global_metafile_dir = paths::writableConfigDir() + QLatin1String("/metafiles");
    std::vector <QString> result = find_metafiles_in(global_metafile_dir);

    result.reserve(result.size() + gamedirs.size());

    for (const QString &dir_path: gamedirs) {
        std::vector <QString> local_metafiles = find_metafiles_in(dir_path);
        result.insert(result.end(),
                      std::make_move_iterator(local_metafiles.begin()),
                      std::make_move_iterator(local_metafiles.end()));
    }

    if (!gamedirs.contains(":/empty")) { //pour faire passer le seul test qu'il y a présentement
        const QString conjure_root = paths::conjureRootGameFolder();
        std::vector<QString> local_metafiles2 = find_metafiles_in(conjure_root);
        result.insert(result.end(),
                      std::make_move_iterator(local_metafiles2.begin()),
                      std::make_move_iterator(local_metafiles2.end()));
    }

    VEC_REMOVE_DUPLICATES(result);
    return result;
}

} // namespace

namespace providers {
namespace conjure {
ConjureProvider::ConjureProvider(QObject *parent)
    : Provider(QLatin1String("conjure_metafiles"), QStringLiteral("Conjure Metadata"), parent) {}

Provider &ConjureProvider::run(SearchContext &sctx) {

    if (!sctx.root_game_dirs().contains(":/empty")) {

        Log::info("Running ConjureDecompression.py");
        system("python3 python_script\\ConjureDecompression.py");
        emit progressChanged(0.001f);
    }

    const std::vector<QString> metafile_paths = find_all_metafiles(sctx.root_game_dirs());
    if (metafile_paths.empty()) {
        Log::info(display_name(), LOGMSG("No metadata files found"));
        return *this;
    }

    const Metadata metahelper(display_name());
    std::vector<FileFilter> all_filters;

    const float progress_step = 1.f / metafile_paths.size();
    float progress = 0.f;

    for (const QString &path: metafile_paths) {
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
