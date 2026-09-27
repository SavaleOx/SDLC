#include "daycontroller.h"
#include "mainwindow.h"
#include "inputdialog.h"

#include <QDialog>
#include <QMessageBox>

DayController::DayController(MainWindow* view, DayModel* model, QObject* parent)
    : QObject(parent), m_view(view), m_model(model)
{
    Q_ASSERT(view);
    Q_ASSERT(model);
}


void DayController::onInputRequested()
{
    InputDialog dlg(m_view);

    // Восстанавливаем последние введённые данные
    if (m_model->hasData())
        dlg.setSchedule(m_model->schedule());
    else
        dlg.setSchedule(DaySchedule());   // дефолтные значения

    if (dlg.exec() != QDialog::Accepted)
        return;

    DaySchedule s = dlg.schedule();

    // Передаём данные в модель. Активная модель сама:
    //   1) emit-нет scheduleChanged
    //   2) пересчитает анализ
    //   3) emit-нет analysisChanged
    // View подписан на эти сигналы и обновится автоматически.
    m_model->setSchedule(s);

    if (!m_model->analysis().valid) {
        QMessageBox::warning(m_view, "Ошибка ввода",
            m_model->analysis().verdict);
    }
}