//
// Created by jonny on 2023-12-09.
//

#pragma once

#include "utils/HashMap.h"
#include "utils/MoveOnly.h"

#include <QString>
#include <QDateTime>
#include <QObject>

namespace model {
struct ScoreLineData {
    explicit ScoreLineData(QString, int, QDateTime);

    QString playerId;
    int score = 0;
    QDateTime score_date;
};

class ScoreLine : public QObject {
    Q_OBJECT

public:

#define GETTER(type, name, field) \
    type name() const { return m_scoreLineData.field; }
        GETTER(const QString&, playerId, playerId)
        GETTER(const int, score, score)
        GETTER(const QDateTime&, scoreDate, score_date)
#undef GETTER

#define SETTER(type, name, field) \
    ScoreLine& set##name(type val) { m_scoreLineData.field = std::move(val); return *this; }
        SETTER(QString, PlayerId, playerId)
        SETTER(int, Score, score)
        SETTER(QDateTime, ScoreDate, score_date)
#undef SETTER

    Q_PROPERTY(QString playerId READ playerId CONSTANT)
    Q_PROPERTY(int score READ score CONSTANT)
    Q_PROPERTY(QDateTime date READ scoreDate CONSTANT)


signals:
    void scoreLinesChanged();

public:
    explicit ScoreLine(QString, int, QDateTime);

    void finalize();

private:
    ScoreLineData m_scoreLineData;
};

bool sort_scores(const model::ScoreLine* const, const model::ScoreLine* const);
}
