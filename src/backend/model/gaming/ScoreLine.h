//
// Created by jonny on 2023-12-09.
//

#pragma once

#include <QDateTime>
#include <QStringList>
#include <QObject>

namespace model {
    struct ScoreLineData {
        explicit ScoreLineData();

        explicit ScoreLineData(int, int, QDateTime);

        int player_id{};
        int score{};
        QDateTime score_date;
    };

    class ScoreLine : public QObject {
    Q_OBJECT

    private:
        ScoreLineData m_scoreLineData;

    public:
        Q_PROPERTY(int player_id READ player_id CONSTANT)
        Q_PROPERTY(int score READ score CONSTANT)
        Q_PROPERTY(QDateTime score_date READ score_date CONSTANT)

        explicit ScoreLine(QObject *parent = nullptr);
        explicit ScoreLine(int player_id, int score, QDateTime score_date, QObject *parent = nullptr);
    };
}
