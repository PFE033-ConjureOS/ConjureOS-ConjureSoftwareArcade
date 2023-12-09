//
// Created by jonny on 2023-12-08.
//

#pragma once

#include "model/ObjectListModel.h"

namespace model { class ScoreLine; }

namespace model {
    class Leaderboard : public TypeListModel<model::ScoreLine> {
    public:
        explicit Leaderboard(QObject* parent = nullptr);

        QHash<int, QByteArray> roleNames() const override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    private:
        void connectEntry(model::ScoreLine* const) override;

        void onEntryPropertyChanged(const QVector<int>& roles);
    };
} // namespace model


