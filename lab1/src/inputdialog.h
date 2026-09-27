#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>
#include "dayschedule.h"

class QTimeEdit;
class QDialogButtonBox;
class QLabel;

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InputDialog(QWidget* parent = nullptr);

    // Восстановить последние данные при повторном открытии
    void setSchedule(const DaySchedule& s);
    DaySchedule schedule() const;

private slots:
    void onAccept();

private:
    void setupUI();

    QTimeEdit* wakeUpEdit;
    QTimeEdit* sleepEdit;
    QTimeEdit* workStartEdit;
    QTimeEdit* workEndEdit;
    QTimeEdit* restStartEdit;
    QTimeEdit* restEndEdit;
    QLabel* errorLabel;
    QDialogButtonBox* buttons;
};

#endif // INPUTDIALOG_H