//
// Created by jonny on 2023-12-09.
//

#include "ScoreLine.h"

#include <utility>


namespace model {

    ScoreLineData::ScoreLineData(QString playerId, int score, QDateTime score_date)
            : playerId(std::move(playerId)), score(score), score_date(std::move(score_date)) {}

    ScoreLine::ScoreLine(QString playerId, int score, QDateTime score_date)
            : m_scoreLineData(std::move(playerId), score, std::move(score_date)) {}
}