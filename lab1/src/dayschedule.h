#ifndef DAYSCHEDULE_H
#define DAYSCHEDULE_H

#include <QTime>


struct DaySchedule {
    QTime wakeUp;      // подъём
    QTime sleep;       // сон
    QTime workStart;   // начало работы
    QTime workEnd;     // конец работы
    QTime restStart;   // начало отдыха
    QTime restEnd;     // конец отдыха
};

#endif // DAYSCHEDULE_H