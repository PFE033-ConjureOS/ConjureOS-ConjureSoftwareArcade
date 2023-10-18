// Pegasus Frontend
// Copyright (C) 2017-2020  Mátyás Mustoha
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.


#include <QtTest/QtTest>

#include "Log.h"
#include "model/gaming/Assets.h"
#include "model/gaming/Collection.h"
#include "model/gaming/Game.h"
#include "model/gaming/GameFile.h"
#include "providers/SearchContext.h"
#include "providers/conjure/ConjureProvider.h"
#include "utils/HashMap.h"

#include <QString>
#include <QStringList>


namespace {

#define PATHMSG(msg, path) qUtf8Printable(QStringLiteral(msg).arg(QDir::toNativeSeparators(QStringLiteral(path))))
#define PATHMSG2(msg, path1, path2) qUtf8Printable(QStringLiteral(msg).arg(QDir::toNativeSeparators(QStringLiteral(path1)), QDir::toNativeSeparators(QStringLiteral(path2))))

const model::Collection* get_collection_ptr(const std::vector<model::Collection*>& list, const QString& name)
{
    const auto it = std::find_if(list.cbegin(), list.cend(),
        [&name](const model::Collection* const ptr){ return ptr->name() == name; });
    return it != list.cend()
        ? *it
        : nullptr;
}

const model::Collection& get_collection(const std::vector<model::Collection*>& list, const QString& name)
{
    const model::Collection* ptr = get_collection_ptr(list, name);
    Q_ASSERT(ptr != nullptr);
    return *ptr;
}

bool has_collection(const std::vector<model::Collection*>& list, const QString& name)
{
    return get_collection_ptr(list, name) != nullptr;
}


const model::Game* get_game_ptr_by_file_path(const std::vector<model::Game*>& list, const QString& path)
{
    const auto it = std::find_if(
        list.cbegin(),
        list.cend(),
        [&path](const model::Game* const game){ return std::any_of(
            game->filesModel()->entries().cbegin(),
            game->filesModel()->entries().cend(),
            [&path](const model::GameFile* gf){ return gf->path() == path; });
        });
    return it != list.cend()
        ? *it
        : nullptr;
}

const model::Game& get_game_by_file_path(const std::vector<model::Game*>& list, const QString& path)
{
    const model::Game* ptr = get_game_ptr_by_file_path(list, path);
    Q_ASSERT(ptr != nullptr);
    return *ptr;
}

bool has_game_file(const std::vector<model::Game*>& list, const QString& path)
{
    return get_game_ptr_by_file_path(list, path) != nullptr;
}

void verify_collected_files(
    const std::vector<model::Collection*>& collections,
    const HashMap<QString, QStringList>& expected_collection_files)
{
    for (const auto& [collname, expected_files] : expected_collection_files) {
        QVERIFY(has_collection(collections, collname));
        const model::Collection& coll = get_collection(collections, collname);

        for (const QString& abs_path : expected_files)
            QVERIFY(has_game_file(coll.gameList()->entries(), abs_path));
    }
}

const model::Game* get_game_ptr_by_title(const std::vector<model::Game*>& list, const QString& title)
{
    const auto it = std::find_if(
        list.cbegin(),
        list.cend(),
        [&title](const model::Game* const game){ return game->title() == title; });
    return it != list.cend()
        ? *it
        : nullptr;
}

} // namespace


class test_ConjureProvider : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        Log::init_qttest();
    }

    void empty();
    void autoparenting();
};

void test_ConjureProvider::empty()
{
    QTest::ignoreMessage(QtInfoMsg, "Conjure Metafiles: No Conjure game found");

    providers::SearchContext sctx({ QStringLiteral(":/empty") });
    providers::conjure::ConjureProvider().run(sctx);
    const auto [collections, games] = sctx.finalize(this);

    QVERIFY(games.empty());
    QVERIFY(collections.empty());
}


QTEST_MAIN(test_ConjureProvider)
#include "test_ConjureProvider.moc"
