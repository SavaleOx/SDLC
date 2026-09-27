#include "inputdialog.h"
#include "intervalutils.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QTimeEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>

InputDialog::InputDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("Ввод данных о дне");
    setModal(true);
    resize(420, 380);
    setupUI();
}

void InputDialog::setupUI()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(14);

    QLabel* title = new QLabel("Введите расписание дня", this);
    title->setStyleSheet("color: #4FC3F7; font-size: 16px; font-weight: bold;");
    layout->addWidget(title);

    QFormLayout* form = new QFormLayout;
    form->setSpacing(10);

    auto makeTimeEdit = [this](const QTime& def) {
        QTimeEdit* e = new QTimeEdit(this);
        e->setDisplayFormat("HH:mm");
        e->setTime(def);
        e->setStyleSheet(
            "QTimeEdit { background-color: #343A40; color: #E6E6E6;"
            "             border: 1px solid #3E464E; border-radius: 6px;"
            "             padding: 5px 10px; font-size: 13px; }");
        return e;
        };

    wakeUpEdit    = makeTimeEdit(QTime(7, 0));
    sleepEdit     = makeTimeEdit(QTime(23, 0));
    workStartEdit = makeTimeEdit(QTime(9, 0));
    workEndEdit   = makeTimeEdit(QTime(18, 0));
    restStartEdit = makeTimeEdit(QTime(19, 0));
    restEndEdit   = makeTimeEdit(QTime(21, 0));

    form->addRow("Время подъёма:", wakeUpEdit);
    form->addRow("Начало работы:", workStartEdit);
    form->addRow("Конец работы:", workEndEdit);
    form->addRow("Начало отдыха:", restStartEdit);
    form->addRow("Конец отдыха:", restEndEdit);
    form->addRow("Время сна:", sleepEdit);

    layout->addLayout(form);

    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("color: #F44336; font-size: 12px;");
    errorLabel->setWordWrap(true);
    errorLabel->setVisible(false);
    layout->addWidget(errorLabel);

    layout->addStretch();

    buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setText("Готово");
    buttons->button(QDialogButtonBox::Cancel)->setText("Отмена");

    connect(buttons, &QDialogButtonBox::accepted, this, &InputDialog::onAccept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    layout->addWidget(buttons);

    setStyleSheet(
        "QDialog { background-color: #2B2F33; }"
        "QLabel { color: #E6E6E6; }"
        "QPushButton { background-color: #343A40; color: #E6E6E6;"
        "              border: 1px solid #3E464E; border-radius: 8px;"
        "              padding: 8px 18px; font-size: 13px; }"
        "QPushButton:hover { background-color: #3E464E; border-color: #4FC3F7; }");
}

void InputDialog::setSchedule(const DaySchedule& s)
{
    if (s.wakeUp.isValid())      wakeUpEdit->setTime(s.wakeUp);
    if (s.sleep.isValid())       sleepEdit->setTime(s.sleep);
    if (s.workStart.isValid())   workStartEdit->setTime(s.workStart);
    if (s.workEnd.isValid())     workEndEdit->setTime(s.workEnd);
    if (s.restStart.isValid())   restStartEdit->setTime(s.restStart);
    if (s.restEnd.isValid())     restEndEdit->setTime(s.restEnd);
}

DaySchedule InputDialog::schedule() const
{
    DaySchedule s;
    s.wakeUp    = wakeUpEdit->time();
    s.sleep     = sleepEdit->time();
    s.workStart = workStartEdit->time();
    s.workEnd   = workEndEdit->time();
    s.restStart = restStartEdit->time();
    s.restEnd   = restEndEdit->time();
    return s;
}

void InputDialog::onAccept()
{
    // Мгновенная проверка понятных ошибок.
    // Бизнес-правила (пересечение со сном и т.п.) проверяет модель.
    DaySchedule s = schedule();

    if (intervalMinutes(s.workStart, s.workEnd) == 0) {
        errorLabel->setText("Начало и конец работы совпадают.");
        errorLabel->setVisible(true);
        return;
    }
    if (intervalMinutes(s.restStart, s.restEnd) == 0) {
        errorLabel->setText("Начало и конец отдыха совпадают.");
        errorLabel->setVisible(true);
        return;
    }

    errorLabel->setVisible(false);
    accept();
}