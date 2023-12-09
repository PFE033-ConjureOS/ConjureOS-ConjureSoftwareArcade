//
// Created by jonny on 2023-12-08.
//

#pragma once

#include <QObject>
#include <QDateTime>

namespace model {
    struct ScoreLineData {
        explicit ScoreLineData();
        explicit ScoreLineData(int,int,QDateTime);

        int playerId;
        int score = 0;
        QDateTime date;
    };

    class ScoreLine : public QObject {
    Q_OBJECT

    public:
#define GETTER(type, name, field) \
    type name() const { return m_scoreLineData.field; }

        GETTER(const int, playerId, playerId)

        GETTER(const int, score, score)

        GETTER(const QDateTime&, date, date)

#undef GETTER


#define SETTER(type, name, field) \
    ScoreLine& set##name(type val) { m_scoreLineData.field = std::move(val); return *this; }

        SETTER(int, playerId, playerId)

        SETTER(int, score, score)

        SETTER(QDateTime, date, date)

#undef SETTER

#define SETTER(type, name, field) \
    ScoreLine& set##name(type val) { m_data.field = std::move(val); return *this; }
#undef SETTER

        Q_PROPERTY(int playerId READ playerId CONSTANT)
        Q_PROPERTY(int score READ score CONSTANT)
        Q_PROPERTY(QDateTime date READ date CONSTANT)


    private:
        ScoreLineData m_scoreLineData;

    public:
        explicit ScoreLine(int playerId, int score, QDateTime date, QObject *parent = nullptr);

    signals:

        void scorelineChanged();

    public:
        explicit ScoreLine(QObject *parent = nullptr);
    };
}



