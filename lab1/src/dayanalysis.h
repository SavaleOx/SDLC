#ifndef DAYANALYSIS_H
#define DAYANALYSIS_H

#include <QString>
#include <QStringList>
#include "dayschedule.h"


struct DayAnalysis {
    bool valid = false;
    double score = 0.0;         // 0..100 — насколько день «идеален»
    QString verdict;            // текстовый вердикт
    QStringList problems;       // список замечаний
    QStringList advice;         // рекомендации
    DaySchedule idealSchedule;  // альтернативное «расписание для счастья»
};

#endif // DAYANALYSIS_H