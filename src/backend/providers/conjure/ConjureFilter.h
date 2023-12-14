//
// Created by jonny on 2023-10-17.
// Started from duplication of ConjureFilter.h
//

#pragma once

#include "utils/MoveOnly.h"

#include <QRegularExpression>
#include <vector>

namespace model { class Collection; }
namespace providers { class SearchContext; }

namespace providers {
namespace conjure {

struct FileFilterGroup {
    std::vector<QString> extensions;
    std::vector<QString> files;
    QRegularExpression regex;

    explicit FileFilterGroup();
    MOVE_ONLY(FileFilterGroup)
};

struct FileFilter {
    //QString collection_key;
    model::Collection *collection;
    std::vector<QString> directories;
    FileFilterGroup include;
    FileFilterGroup exclude;

    explicit FileFilter(model::Collection *const, QString);
    MOVE_ONLY(FileFilter)
};

void apply_filter(FileFilter &, SearchContext &);

} // namespace conjure
} // namespace providers
