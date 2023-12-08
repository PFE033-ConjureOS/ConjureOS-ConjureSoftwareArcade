// Pegasus Frontend
// Copyright (C) 2017-2019  Mátyás Mustoha
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


#pragma once

#include "types/AssetType.h"
#include "utils/HashMap.h"
#include "utils/MoveOnly.h"

#include <QStringList>
#include <QObject>


namespace model {
    class Leaderboard : public QObject {
        Q_OBJECT

    public:
#define GEN(qmlname, enumname) \
    const QString& qmlname() const { return getFirst(AssetType::enumname); } \
    const QStringList& qmlname##List() const { return get(AssetType::enumname); } \
    Q_PROPERTY(QString qmlname READ qmlname CONSTANT) \
    Q_PROPERTY(QStringList qmlname##List READ qmlname##List CONSTANT) \

#undef GEN

    public:
        explicit Leaderboard(QObject* parent);

    private:

       // HashMap<QStringList, qint64> m_scores;  // playerId, score
    };

} // namespace model
