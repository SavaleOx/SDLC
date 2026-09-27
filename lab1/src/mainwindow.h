#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dayschedule.h"
#include "dayanalysis.h"

class DayController;
class QLabel;
class QPushButton;
class QGroupBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    // Контроллер устанавливается извне (main.cpp)
    void setController(DayController* controller);

signals:
    void inputRequested();  // View сообщает контроллеру: «нажата кнопка Ввести данные»

private slots:
    void onInputButtonClicked();
    void onModelScheduleChanged(const DaySchedule& s);
    void onModelAnalysisChanged(const DayAnalysis& a);

private:
    void setupUI();
    void displaySchedule(const DaySchedule& s);
    void displayAnalysis(const DayAnalysis& a);
    void clearDisplay();

    // ---------- Виджеты ----------
    QPushButton* inputButton;

    QLabel* wakeUpLabel;
    QLabel* sleepLabel;
    QLabel* workLabel;
    QLabel* restLabel;

    QLabel* scoreLabel;
    QLabel* verdictLabel;
    QLabel* problemsLabel;
    QLabel* adviceLabel;
    QLabel* idealScheduleLabel;

    // ---------- Контроллер ----------
    DayController* m_controller = nullptr;
};

#endif // MAINWINDOW_H