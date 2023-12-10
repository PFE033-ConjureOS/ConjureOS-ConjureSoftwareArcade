//
// Created by jonny on 2023-12-09.
//

#include "ScoreLine.h"

#include <utility>


namespace model {

    ScoreLineData::ScoreLineData() = default;

    ScoreLineData::ScoreLineData(int player_id, int score, QDateTime score_date)
            : player_id(player_id), score(score), score_date(std::move(score_date)) {}

    ScoreLine::ScoreLine(QObject *parent)
            : QObject(parent) {}


    ScoreLine::ScoreLine(int player_id, int score, QDateTime score_date, QObject *parent)
            : QObject(parent), m_scoreLineData(player_id, score, score_date) {}

}
