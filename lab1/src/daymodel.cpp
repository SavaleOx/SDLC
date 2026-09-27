#include "daymodel.h"
#include "intervalutils.h"
#include <QtMath>
#include <algorithm>

DayModel::DayModel(QObject* parent) : QObject(parent) {}

// ============================================================
//              ВАЛИДАЦИЯ ВВЕДЁННЫХ ДАННЫХ
// ============================================================

bool DayModel::validateInput(QString& error) const
{
    return validateSchedule(m_schedule, error);
}

bool DayModel::validateSchedule(const DaySchedule& s, QString& error)
{
    if (!s.wakeUp.isValid() || !s.sleep.isValid() ||
        !s.workStart.isValid() || !s.workEnd.isValid() ||
        !s.restStart.isValid() || !s.restEnd.isValid()) {
        error = "Все поля должны быть заполнены корректным временем.";
        return false;
    }

    int sleepDuration = intervalMinutes(s.sleep, s.wakeUp);
    if (sleepDuration < 240) {
        error = "Слишком мало сна. Продолжительность сна должна быть не менее 4 часов.";
        return false;
    }
    if (sleepDuration > 720) {
        error = "Слишком много сна. Продолжительность сна должна быть не более 12 часов.";
        return false;
    }

    int workDuration = intervalMinutes(s.workStart, s.workEnd);
    if (workDuration == 0) {
        error = "Начало и конец работы совпадают.";
        return false;
    }
    if (workDuration > 16 * 60) {
        error = "Рабочий интервал больше 16 часов — проверьте данные.";
        return false;
    }

    int restDuration = intervalMinutes(s.restStart, s.restEnd);
    if (restDuration == 0) {
        error = "Начало и конец отдыха совпадают.";
        return false;
    }
    if (restDuration > 16 * 60) {
        error = "Отдых больше 16 часов — проверьте данные.";
        return false;
    }

    if (intervalsOverlap(s.workStart, s.workEnd, s.restStart, s.restEnd)) {
        error = "Время работы и отдыха не должно пересекаться.";
        return false;
    }
    if (intervalsOverlap(s.workStart, s.workEnd, s.sleep, s.wakeUp)) {
        error = "Работа пересекается со сном.";
        return false;
    }
    if (intervalsOverlap(s.restStart, s.restEnd, s.sleep, s.wakeUp)) {
        error = "Отдых пересекается со сном.";
        return false;
    }

    error.clear();
    return true;
}

// ============================================================
//            ПОСТРОЕНИЕ «РАСПИСАНИЯ ДЛЯ СЧАСТЬЯ»
// ============================================================

DaySchedule DayModel::buildIdealSchedule() const
{
    const DaySchedule& s = m_schedule;

    // ---- Якоря ----
    DaySchedule ideal;
    ideal.wakeUp    = s.wakeUp;
    ideal.workStart = s.workStart;

    // ---- 1. Сон: 8 часов назад от подъёма ----
    const int kIdealSleepMinutes = 8 * 60;
    int wakeMin  = timeToMinutes(ideal.wakeUp);
    ideal.sleep  = minutesToTime(wakeMin - kIdealSleepMinutes);

    // ---- 2. Работа: режем переработку > 8.5 ч ----
    int workDur = intervalMinutes(s.workStart, s.workEnd);
    int targetWorkDur = workDur;
    if (workDur > 8 * 60 + 30) {
        targetWorkDur = 8 * 60 + 30;
    }
    if (targetWorkDur != workDur) {
        ideal.workEnd = minutesToTime(timeToMinutes(ideal.workStart) + targetWorkDur);
    } else {
        ideal.workEnd = s.workEnd;
    }

    // ---- 3. Отдых ----
    const int kIdealRestMinutes = 2 * 60;
    const int kMinRestMinutes   = 60;
    const int kGapBeforeSleep   = 30;

    // 3a. Пользовательский отдых уже хорош?
    int userRestDur = intervalMinutes(s.restStart, s.restEnd);
    bool userRestFits =
        userRestDur >= kIdealRestMinutes &&
        !intervalsOverlap(s.restStart, s.restEnd, ideal.workStart, ideal.workEnd) &&
        !intervalsOverlap(s.restStart, s.restEnd, ideal.sleep,    ideal.wakeUp);

    if (userRestFits) {
        ideal.restStart = s.restStart;
        ideal.restEnd   = s.restEnd;
    } else {
        // 3b. Пробуем растянуть пользовательский отдых до 2ч, сохранив начало.
        QTime fixedStart = s.restStart;
        QTime fixedEnd   = minutesToTime(timeToMinutes(s.restStart) + kIdealRestMinutes);

        if (!intervalsOverlap(fixedStart, fixedEnd, ideal.workStart, ideal.workEnd) &&
            !intervalsOverlap(fixedStart, fixedEnd, ideal.sleep,    ideal.wakeUp)) {
            ideal.restStart = fixedStart;
            ideal.restEnd   = fixedEnd;
        } else {
            // 3c. Ищем лучшее свободное окно.
            int workEndMin    = timeToMinutes(ideal.workEnd);
            int workStartMin  = timeToMinutes(ideal.workStart);
            int sleepStartMin = timeToMinutes(ideal.sleep);
            int wakeUpMin     = timeToMinutes(ideal.wakeUp);

            int windowAfter  = (sleepStartMin - workEndMin + 1440) % 1440;
            int windowBefore = (workStartMin - wakeUpMin + 1440) % 1440;

            int window = 0, startAnchor = 0;
            if (windowAfter >= windowBefore) {
                window = windowAfter;
                startAnchor = workEndMin;
            } else {
                window = windowBefore;
                startAnchor = wakeUpMin;
            }

            int available = qMax(0, window - kGapBeforeSleep);
            int restDur;
            if (available < kIdealRestMinutes) restDur = available;
            else                                restDur = kIdealRestMinutes;

            ideal.restStart = minutesToTime(startAnchor);
            ideal.restEnd   = minutesToTime(startAnchor + restDur);
        }
    }

    // ---- 4. Финальная валидация ----
    QString err;
    if (!validateSchedule(ideal, err)) {
        DaySchedule fallback;
        fallback.sleep     = QTime(23, 0);
        fallback.wakeUp    = QTime(7, 0);
        fallback.workStart = QTime(9, 0);
        fallback.workEnd   = QTime(18, 0);
        fallback.restStart = QTime(19, 0);
        fallback.restEnd   = QTime(21, 0);
        return fallback;
    }

    return ideal;
}

// ============================================================
//          АНАЛИЗ ДНЯ
// ============================================================

void DayModel::analyze()
{
    DayAnalysis a;

    if (!m_hasData) { m_analysis = a; return; }

    QString err;
    if (!validateInput(err)) {
        a.valid = false;
        a.verdict = "Ошибка в данных: " + err;
        m_analysis = a;
        return;
    }

    a.valid = true;
    const DaySchedule& s = m_schedule;

    int sleepDuration = intervalMinutes(s.sleep, s.wakeUp);
    int workDuration  = intervalMinutes(s.workStart, s.workEnd);
    int restDuration  = intervalMinutes(s.restStart, s.restEnd);

    // ============ 1. СОН ============
    double sleepDurationScore = 20.0 * plateau(sleepDuration,
                                               7 * 60, 9 * 60, 3 * 60);

    double sleepTimingScore;
    {
        int sleepStartMin = timeToMinutes(s.sleep);
        int diff = (sleepStartMin - 23 * 60 + 1440) % 1440;
        if (diff > 720) diff -= 1440;
        sleepTimingScore = 10.0 * qMax(0.0, 1.0 - qAbs(diff) / 180.0);
    }

    double sleepQualityScore = 5.0;
    if (s.sleep.hour() >= 7 && s.sleep.hour() < 17) sleepQualityScore -= 3.0;
    if (s.wakeUp.hour() < 5)                          sleepQualityScore -= 2.0;
    sleepQualityScore = qMax(0.0, sleepQualityScore);

    double sleepScore = sleepDurationScore + sleepTimingScore + sleepQualityScore;

    // ============ 2. РАБОТА ============
    double workDurationScore = 15.0 * plateau(workDuration,
                                              6 * 60, 8 * 60, 2 * 60);
    if (workDuration > 10 * 60) workDurationScore = qMax(0.0, workDurationScore - 3.0);

    double workTimingScore = 10.0;
    {
        auto parts = splitInterval(s.workStart, s.workEnd);
        int nightOverlap = 0;
        for (const auto& p : parts) {
            int lo = qMax(p.first, 0);
            int hi = qMin(p.second, 6 * 60);
            if (hi > lo) nightOverlap += (hi - lo);
        }
        if (nightOverlap > 0) {
            workTimingScore = qMax(0.0, 10.0 - nightOverlap / 60.0 * 2.5);
        }
    }
    double workScore = workDurationScore + workTimingScore;

    // ============ 3. ОТДЫХ ============
    double restDurationScore = 12.0 * plateau(restDuration,
                                              90, 180, 60);

    double restPositionScore = 8.0;
    {
        int restStartMin = timeToMinutes(s.restStart);
        int restEndMin   = timeToMinutes(s.restEnd);
        int workEndMin   = timeToMinutes(s.workEnd);
        int sleepStartMin = timeToMinutes(s.sleep);

        int gapFromWork = (restStartMin - workEndMin + 1440) % 1440;
        int gapToSleep  = (sleepStartMin - restEndMin + 1440) % 1440;

        double penalty = 0.0;
        if (gapFromWork < 15) penalty += 2.0;
        if (gapToSleep  < 15) penalty += 2.0;
        if (gapFromWork > 6 * 60) penalty += 2.0;

        restPositionScore = qMax(0.0, restPositionScore - penalty);
    }
    double restScore = restDurationScore + restPositionScore;

    // ============ 4. БАЛАНС ДНЯ ============
    double sleepShareScore = 8.0 * plateau(sleepDuration, 7 * 60, 9 * 60, 2 * 60);

    double workRestRatioScore = 7.0;
    if (restDuration > 0) {
        double ratio = double(workDuration) / restDuration;
        if      (ratio < 1.0)  workRestRatioScore = 3.0;
        else if (ratio < 2.0)  workRestRatioScore = 5.0;
        else if (ratio <= 4.0) workRestRatioScore = 7.0;
        else if (ratio <= 6.0) workRestRatioScore = 4.0;
        else                   workRestRatioScore = 2.0;
    } else workRestRatioScore = 0.0;

    double freeTimeScore = 5.0;
    {
        int occupied = sleepDuration + workDuration + restDuration;
        int freeTime = 1440 - occupied;
        if      (freeTime < 60)  freeTimeScore = 0.0;
        else if (freeTime < 120) freeTimeScore = 2.0;
        else if (freeTime < 180) freeTimeScore = 4.0;
        else                     freeTimeScore = 5.0;
    }

    double balanceScore = sleepShareScore + workRestRatioScore + freeTimeScore;

    // ============ 5. ШТРАФЫ ============
    double penalty = 0.0;
    if (sleepDuration < 5 * 60) penalty += 5.0;
    if (restDuration > 5 * 60)  penalty += 3.0;
    if (s.sleep.hour() >= 6 && s.sleep.hour() <= 10) penalty += 2.0;

    // ============ 6. ИТОГ ============
    a.score = qBound(0.0, sleepScore + workScore + restScore + balanceScore - penalty, 100.0);

    // ============ 7. ВЕРДИКТ ============
    struct Cat { QString name; double value; double max; };
    QVector<Cat> cats = {
        {"сон",    sleepScore,   35.0},
        {"работа", workScore,    25.0},
        {"отдых",  restScore,    20.0},
        {"баланс", balanceScore, 20.0},
    };
    std::sort(cats.begin(), cats.end(),
              [](const Cat& a, const Cat& b) {
                  return a.value / a.max < b.value / b.max;
              });
    const Cat& weakest = cats.first();
    double weakestRatio = weakest.value / weakest.max;

    QString base;
    if      (a.score >= 90) base = "Отличный день!";
    else if (a.score >= 75) base = "Хороший день.";
    else if (a.score >= 55) base = "Средний день.";
    else if (a.score >= 35) base = "Слабый день.";
    else                    base = "Очень плохой день.";

    QString focus;
    if (weakestRatio < 0.5) {
        if      (weakest.name == "сон")    focus = " Особенно страдает сон.";
        else if (weakest.name == "работа") focus = " Особенно тяжёлая работа.";
        else if (weakest.name == "отдых")  focus = " Особенно мало отдыха.";
        else                                focus = " Особенно плохой баланс дня.";
    } else if (weakestRatio < 0.75) {
        focus = QString(" Слабое место — %1.").arg(weakest.name);
    }

    QString emoji;
    if      (a.score >= 90) emoji = " 💚";
    else if (a.score >= 75) emoji = " 💛";
    else if (a.score >= 55) emoji = " 🧡";
    else if (a.score >= 35) emoji = " ❤️";
    else                    emoji = " 💔";

    a.verdict = base + focus + emoji;

    // ============ 8. ПРОБЛЕМЫ И СОВЕТЫ ============
    // --- Сон ---
    if (sleepDuration < 7 * 60) {
        a.problems << QString("Вы спите %1 ч — недостаточно для восстановления (норма 7–9 ч).")
            .arg(sleepDuration / 60.0, 0, 'f', 1);
        a.advice   << "Старайтесь спать не менее 7 часов.";
    } else if (sleepDuration > 9 * 60) {
        a.problems << QString("Вы спите %1 ч — многовато, может снижать продуктивность.")
            .arg(sleepDuration / 60.0, 0, 'f', 1);
        a.advice   << "Оптимальная продолжительность сна — 7–9 часов.";
    }
    if (sleepTimingScore < 5.0) {
        int sH = s.sleep.hour();
        if (sH >= 7 && sH < 17) {
            a.problems << "Вы ложитесь спать днём — это сбивает циркадный ритм.";
            a.advice   << "Старайтесь ложиться в интервале 22:00–00:30.";
        } else if (sH >= 3 && sH < 5) {
            a.problems << "Слишком позднее засыпание (после 3 ночи).";
            a.advice   << "Сдвиньте отход ко сну на 22:30–23:30.";
        } else if (sH >= 20 && sH < 22) {
            a.problems << "Слишком ранний отход ко сну — может указывать на недосып.";
            a.advice   << "Если хотите спать в 20–21ч, проверьте здоровье и режим.";
        }
    }

    // --- Работа ---
    if (workDuration > 10 * 60) {
        a.problems << QString("Рабочий день %1 ч — переработка.")
            .arg(workDuration / 60.0, 0, 'f', 1);
        a.advice   << "Сократите рабочий день до 8 часов.";
    } else if (workDuration > 8 * 60 + 30) {
        a.problems << QString("Рабочий день %1 ч — немного больше нормы (8 ч).")
            .arg(workDuration / 60.0, 0, 'f', 1);
    }
    if (workTimingScore < 7.0) {
        a.problems << "Часть работы приходится на ночное время (00:00–06:00).";
        a.advice   << "По возможности перенесите работу на дневные часы.";
    }
    if (workDuration > 0 && workDuration < 4 * 60) {
        a.problems << QString("Очень короткий рабочий день (%1 ч).")
            .arg(workDuration / 60.0, 0, 'f', 1);
    }

    // --- Отдых ---
    if (restDuration < 60) {
        a.problems << "Отдых короче 1 часа — мало для восстановления.";
        a.advice   << "Выделяйте не менее 1–2 часов на отдых и хобби.";
    } else if (restDuration < 90) {
        a.problems << "Отдых меньше 1.5 ч — на грани.";
        a.advice   << "Старайтесь выделять хотя бы 1.5–2 часа.";
    } else if (restDuration > 5 * 60) {
        a.problems << "Слишком длинный отдых — возможно, стоит распределить его блоками.";
        a.advice   << "Разбейте отдых на 2–3 блока в течение дня.";
    }
    if (restPositionScore < 6.0) {
        a.problems << "Отдых расположен вплотную к работе или ко сну.";
        a.advice   << "Оставьте 30–60 мин между работой, отдыхом и сном.";
    }

    // --- Баланс ---
    if (workRestRatioScore < 5.0) {
        a.problems << "Сильный перекос между работой и отдыхом.";
        a.advice   << "Стремитесь к соотношению работы и отдыха 2:1–4:1.";
    }
    if (freeTimeScore < 5.0) {
        a.problems << "Слишком мало свободного времени вне сна, работы и отдыха.";
        a.advice   << "Оставьте 2–3 часа в сутки на быт и личные дела.";
    }

    // ============ 9. ИДЕАЛЬНОЕ РАСПИСАНИЕ ============
    a.idealSchedule = buildIdealSchedule();

    m_analysis = a;
}

// ============================================================
//                  СЛОТЫ ИЗМЕНЕНИЯ ДАННЫХ
// ============================================================

void DayModel::setSchedule(const DaySchedule& s)
{
    m_schedule = s;
    m_hasData = true;
    emit scheduleChanged(m_schedule);
    analyze();
    emit analysisChanged(m_analysis);
}

void DayModel::clear()
{
    m_schedule = DaySchedule();
    m_analysis = DayAnalysis();
    m_hasData = false;
    emit dataCleared();
}