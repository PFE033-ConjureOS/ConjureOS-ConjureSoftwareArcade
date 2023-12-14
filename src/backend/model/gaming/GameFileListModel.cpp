// Pegasus Frontend
// Copyright (C) 2017-2022  Mátyás Mustoha
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


#include "GameFileListModel.h"

#include "model/gaming/Assets.h"
#include "model/gaming/GameFile.h"


namespace {
enum Roles {
    Self = Qt::UserRole,
    Name,
    Path,
    PlayCount,
    PlayTime,
    LastPlayed,
};
} // namespace


namespace model {
GameFileListModel::GameFileListModel(QObject *parent)
    : TypeListModel(parent) {}


QHash<int, QByteArray> GameFileListModel::roleNames() const {
    static QHash<int, QByteArray> ROLE_NAMES{
                                             {Roles::Self,       QByteArrayLiteral("modelData")},
                                             {Roles::Name,       QByteArrayLiteral("name")},
                                             {Roles::Path,       QByteArrayLiteral("path")},
                                             {Roles::PlayCount,  QByteArrayLiteral("playCount")},
                                             {Roles::PlayTime,   QByteArrayLiteral("playTime")},
                                             {Roles::LastPlayed, QByteArrayLiteral("lastPlayed")},
                                             };
    return ROLE_NAMES;
}


QVariant GameFileListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return {};

    model::GameFile *const gamefile_ptr = m_entries.at(index.row());
    const model::GameFile &gamefile = *gamefile_ptr;
    switch (role) {
    case Roles::Self:
        return QVariant::fromValue(gamefile_ptr);
    case Roles::Name:
        return gamefile.name();
    case Roles::Path:
        return gamefile.path();
    case Roles::PlayCount:
        return gamefile.playCount();
    case Roles::PlayTime:
        return gamefile.playTime();
    case Roles::LastPlayed:
        return gamefile.lastPlayed();
    default:
        return {};
    }
}


void GameFileListModel::connectEntry(model::GameFile *const entry) {
    connect(entry, &model::GameFile::playStatsChanged,
            this, [this]() { onEntryPropertyChanged({Roles::PlayCount, Roles::PlayTime, Roles::LastPlayed}); });
}


void GameFileListModel::onEntryPropertyChanged(const QVector<int> &roles) {
    QObject *const game_ptr = sender();
    const auto it = std::find(m_entries.cbegin(), m_entries.cend(), game_ptr);
    if (it == m_entries.cend())
        return;

    const size_t data_idx = std::distance(m_entries.cbegin(), it);
    const QModelIndex model_idx = index(data_idx);
    emit dataChanged(model_idx, model_idx, roles);
}
} // namespace model
