
#include "Leaderboard.h"

#include "model/gaming/ScoreLine.h"

namespace {
    enum Roles {
        Self = Qt::UserRole,
        PlayerId,
        Score,
        Date,
    };
}

namespace model {
    Leaderboard::Leaderboard(QObject *parent)
            : TypeListModel(parent) {}


    QHash<int, QByteArray> Leaderboard::roleNames() const {
        static QHash<int, QByteArray> ROLE_NAMES{
                {Roles::Self,     QByteArrayLiteral("modelData")},
                {Roles::PlayerId, QByteArrayLiteral("playerId")},
                {Roles::Score,    QByteArrayLiteral("score")},
                {Roles::Date,     QByteArrayLiteral("date")},
        };
        return ROLE_NAMES;
    }


    QVariant Leaderboard::data(const QModelIndex &index, int role) const {
        if (!index.isValid())
            return {};

        model::ScoreLine *const scoreLine_ptr = m_entries.at(index.row());
        const model::ScoreLine &scoreLine = *scoreLine_ptr;
        switch (role) {
            case Roles::Self:
                return QVariant::fromValue(scoreLine_ptr);
            case Roles::PlayerId:
                return scoreLine.playerId();
            case Roles::Score:
                return scoreLine.score();
            case Roles::Date:
                return scoreLine.scoreDate();
            default:
                return {};
        }
    }
}
