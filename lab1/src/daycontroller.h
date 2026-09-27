#ifndef DAYCONTROLLER_H
#define DAYCONTROLLER_H

#include <QObject>
#include "daymodel.h"

class MainWindow;


class DayController : public QObject
{
    Q_OBJECT

public:
    DayController(MainWindow* view, DayModel* model, QObject* parent = nullptr);

    DayModel* model() const { return m_model; }

public slots:
    // Вызывается, когда пользователь нажал «Ввести данные»
    void onInputRequested();

private:
    MainWindow* m_view;
    DayModel*   m_model;
};

#endif // DAYCONTROLLER_H