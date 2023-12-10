//
// Created by jonny on 2023-12-09.
//

#pragma once

#include <QString>
#include <QFileInfo>
#include <QDateTime>

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
        const QString &playerId() const { return m_scoreLineData.playerId; }

        const int score() const { return m_scoreLineData.score; }

        const QDateTime &scoreDate() const { return m_scoreLineData.score_date; }

        ScoreLine &setScoreLine(int score) {
            m_scoreLineData.score = score;
            return *this;
        }

    public:
        explicit ScoreLine(QString, int, QDateTime);

    private:
        ScoreLineData m_scoreLineData;
    };
}