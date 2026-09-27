#ifndef DAYMODEL_H
#define DAYMODEL_H

#include <QObject>
#include "dayschedule.h"
#include "dayanalysis.h"

class DayModel : public QObject
{
    Q_OBJECT

public:
    explicit DayModel(QObject* parent = nullptr);

    const DaySchedule& schedule() const { return m_schedule; }
    const DayAnalysis& analysis() const { return m_analysis; }
    bool hasData() const { return m_hasData; }

public slots:
    void setSchedule(const DaySchedule& s);
    void clear();

signals:
    void scheduleChanged(const DaySchedule& s);
    void analysisChanged(const DayAnalysis& a);
    void dataCleared();

private:
    void analyze();
    bool validateInput(QString& error) const;

    // Строит сбалансированное расписание на основе m_schedule.
    DaySchedule buildIdealSchedule() const;

    // Проверка произвольного расписания (не трогает m_schedule).
    static bool validateSchedule(const DaySchedule& s, QString& error);

    DaySchedule m_schedule;
    DayAnalysis m_analysis;
    bool        m_hasData = false;
};

#endif // DAYMODEL_H