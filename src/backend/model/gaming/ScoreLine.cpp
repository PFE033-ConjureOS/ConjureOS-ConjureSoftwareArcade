//
// Created by jonny on 2023-12-08.
//

#include "ScoreLine.h"

namespace model {
    ScoreLineData::ScoreLineData() = default;

    ScoreLineData::ScoreLineData(int playerId, int score, QDateTime date)
    : playerId(playerId), score(score), date(date) {}

    ScoreLine::ScoreLine(int playerId, int score, QDateTime date, QObject *parent)
            : QObject(parent), m_scoreLineData(playerId, score, date) {}

}
