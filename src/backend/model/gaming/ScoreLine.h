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
    Q_PROPERTY(QString playerId READ playerId CONSTANT)
    Q_PROPERTY(int score READ score CONSTANT)
    Q_PROPERTY(QDateTime date READ scoreDate CONSTANT)

    const QString &playerId() const { return m_scoreLineData.playerId; }

    const int score() const { return m_scoreLineData.score; }

    const QDateTime &scoreDate() const { return m_scoreLineData.score_date; }

signals:
    void scoreLinesChanged();

public:
    explicit ScoreLine(QString, int, QDateTime);

private:
    ScoreLineData m_scoreLineData;
};

bool sort_scores(const model::ScoreLine* const, const model::ScoreLine* const);
}
