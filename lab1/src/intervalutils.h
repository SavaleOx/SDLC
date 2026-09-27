#ifndef INTERVALUTILS_H
#define INTERVALUTILS_H

#include <QTime>
#include <QVector>
#include <QPair>


inline int intervalMinutes(const QTime& start, const QTime& end)
{
    int s = start.hour() * 60 + start.minute();
    int e = end.hour() * 60 + end.minute();
    return (e > s) ? (e - s) : (24 * 60 - s + e);
}

inline QVector<QPair<int, int>> splitInterval(const QTime& start, const QTime& end)
{
    int s = start.hour() * 60 + start.minute();
    int e = end.hour() * 60 + end.minute();

    if (e > s) {
        return { {s, e} };
    }

    QVector<QPair<int, int>> parts;
    if (s < 1440) parts.append({ s, 1440 });
    if (e > 0)    parts.append({ 0, e });
    return parts;
}

inline bool intervalsOverlap(const QTime& a1, const QTime& a2,
                             const QTime& b1, const QTime& b2)
{
    auto A = splitInterval(a1, a2);
    auto B = splitInterval(b1, b2);
    for (const auto& x : A)
        for (const auto& y : B)
            if (x.first < y.second && y.first < x.second)
                return true;
    return false;
}


inline QTime minutesToTime(int minutes)
{
    minutes = ((minutes % 1440) + 1440) % 1440;   // корректно для отрицательных
    return QTime(minutes / 60, minutes % 60);
}

inline double plateau(double x, double lo, double hi, double soft)
{
    if (x >= lo && x <= hi) return 1.0;
    if (x < lo) {
        double d = lo - x;
        return (d >= soft) ? 0.0 : 1.0 - d / soft;
    }
    // x > hi
    double d = x - hi;
    return (d >= soft) ? 0.0 : 1.0 - d / soft;
}

inline double lerp(double x, double x0, double y0, double x1, double y1)
{
    if (x <= x0) return y0;
    if (x >= x1) return y1;
    return y0 + (y1 - y0) * (x - x0) / (x1 - x0);
}

// QTime → минуты от полуночи.
inline int timeToMinutes(const QTime& t)
{
    return t.hour() * 60 + t.minute();
}

#endif // INTERVALUTILS_H