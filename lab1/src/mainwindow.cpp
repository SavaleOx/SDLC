#include "mainwindow.h"
#include "daycontroller.h"
#include "daymodel.h"
#include "inputdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QFrame>

// ============================================================
//                       СТИЛИ
// ============================================================

static const QString kAppStyle = R"(
    QMainWindow, QWidget {
        background-color: #2B2F33;
        color: #E6E6E6;
    }
    QGroupBox {
        background-color: #343A40;
        border: 1px solid #3E464E;
        border-radius: 10px;
        margin-top: 14px;
        padding: 12px;
        font-size: 13px;
        font-weight: bold;
        color: #4FC3F7;
    }
    QGroupBox::title {
        subcontrol-origin: margin;
        left: 14px;
        padding: 0 6px 0 6px;
    }
    QPushButton {
        background-color: #4FC3F7;
        color: #1B1F23;
        border: none;
        border-radius: 8px;
        padding: 12px 22px;
        font-size: 14px;
        font-weight: bold;
    }
    QPushButton:hover { background-color: #81D4FA; }
    QPushButton:pressed { background-color: #29B6F6; }
)";

// ============================================================
//                       КОНСТРУКТОР
// ============================================================

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    setWindowTitle("Идеальный день");
    resize(900, 750);
    setStyleSheet(kAppStyle);
    setupUI();
}

MainWindow::~MainWindow() {}

void MainWindow::setController(DayController* controller)
{
    m_controller = controller;

    // Активная модель: подписываемся на её сигналы
    if (m_controller) {
        connect(m_controller->model(), &DayModel::scheduleChanged,
            this, &MainWindow::onModelScheduleChanged);
        connect(m_controller->model(), &DayModel::analysisChanged,
            this, &MainWindow::onModelAnalysisChanged);
        connect(m_controller->model(), &DayModel::dataCleared,
            this, &MainWindow::clearDisplay);

        // View → Controller: нажатие кнопки
        connect(this, &MainWindow::inputRequested,
            m_controller, &DayController::onInputRequested);
    }
}


void MainWindow::setupUI()
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* layout = new QVBoxLayout(central);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    // ---------- Заголовок ----------
    QLabel* title = new QLabel("Утилита «Идеальный день»", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: #4FC3F7; font-size: 22px; font-weight: bold;");
    layout->addWidget(title);

    QLabel* subtitle = new QLabel(
        "Введите время подъёма, сна, работы и отдыха — программа оценит, "
        "насколько ваш день идеален, и предложит расписание «для счастья».",
        this);
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setWordWrap(true);
    subtitle->setStyleSheet("color: #90A4AE; font-size: 12px;");
    layout->addWidget(subtitle);

    // ---------- Кнопка «Ввести данные» ----------
    QHBoxLayout* btnLayout = new QHBoxLayout;
    inputButton = new QPushButton("Ввести данные", this);
    inputButton->setMinimumHeight(46);
    connect(inputButton, &QPushButton::clicked,
        this, &MainWindow::onInputButtonClicked);
    btnLayout->addStretch();
    btnLayout->addWidget(inputButton);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    // ---------- Расписание пользователя ----------
    QGroupBox* scheduleBox = new QGroupBox("Ваше расписание", this);
    QVBoxLayout* scheduleLayout = new QVBoxLayout(scheduleBox);

    wakeUpLabel = new QLabel("Подъём:         —", scheduleBox);
    workLabel = new QLabel("Работа:         —", scheduleBox);
    restLabel = new QLabel("Отдых:          —", scheduleBox);
    sleepLabel = new QLabel("Сон:            —", scheduleBox);

    for (QLabel* l : { wakeUpLabel, workLabel, restLabel, sleepLabel }) {
        l->setStyleSheet(
            "QLabel { font-family: 'Consolas', monospace;"
            "         font-size: 13px; color: #E6E6E6; padding: 2px; }");
        scheduleLayout->addWidget(l);
    }
    layout->addWidget(scheduleBox);

    // ---------- Результат анализа ----------
    QGroupBox* resultBox = new QGroupBox("Результат анализа", this);
    QVBoxLayout* resultLayout = new QVBoxLayout(resultBox);

    scoreLabel = new QLabel("Оценка: ", resultBox);
    scoreLabel->setStyleSheet(
        "QLabel { font-size: 16px; font-weight: bold; color: #FFB74D; }");
    resultLayout->addWidget(scoreLabel);

    verdictLabel = new QLabel("", resultBox);
    verdictLabel->setWordWrap(true);
    verdictLabel->setStyleSheet(
        "QLabel { font-size: 14px; color: #E6E6E6; padding: 4px 0; }");
    resultLayout->addWidget(verdictLabel);

    problemsLabel = new QLabel(resultBox);
    problemsLabel->setWordWrap(true);
    problemsLabel->setStyleSheet(
        "QLabel { font-size: 12px; color: #FFAB91; padding: 4px 0; }");
    resultLayout->addWidget(problemsLabel);

    adviceLabel = new QLabel(resultBox);
    adviceLabel->setWordWrap(true);
    adviceLabel->setStyleSheet(
        "QLabel { font-size: 12px; color: #A5D6A7; padding: 4px 0; }");
    resultLayout->addWidget(adviceLabel);

    layout->addWidget(resultBox);

    // ---------- Идеальное расписание ----------
    QGroupBox* idealBox = new QGroupBox("Расписание «для счастья»", this);
    QVBoxLayout* idealLayout = new QVBoxLayout(idealBox);

    idealScheduleLabel = new QLabel(
        "Нажмите «Ввести данные», чтобы получить рекомендации.", idealBox);
    idealScheduleLabel->setWordWrap(true);
    idealScheduleLabel->setStyleSheet(
        "QLabel { font-family: 'Consolas', monospace;"
        "         font-size: 13px; color: #81D4FA; padding: 4px; }");
    idealLayout->addWidget(idealScheduleLabel);

    layout->addWidget(idealBox);
    layout->addStretch();
}


void MainWindow::onInputButtonClicked()
{
    // View не знает про модель — только emit-ит сигнал.
    // Контроллер сам откроет диалог и передаст данные в модель.
    emit inputRequested();
}


void MainWindow::onModelScheduleChanged(const DaySchedule& s)
{
    displaySchedule(s);
}

void MainWindow::onModelAnalysisChanged(const DayAnalysis& a)
{
    displayAnalysis(a);
}

// ============================================================
//         Отображение данных
// ============================================================

void MainWindow::displaySchedule(const DaySchedule& s)
{
    auto fmtTime = [](const QTime& t) {
        return t.isValid() ? t.toString("HH:mm") : "—";
        };

    wakeUpLabel->setText(QString("Подъём:         %1").arg(fmtTime(s.wakeUp)));
    sleepLabel->setText(QString("Сон:            %1").arg(fmtTime(s.sleep)));

    QString workStr = QString("%1 — %2")
        .arg(fmtTime(s.workStart), fmtTime(s.workEnd));
    workLabel->setText(QString("Работа:         %1").arg(workStr));

    QString restStr = QString("%1 — %2")
        .arg(fmtTime(s.restStart), fmtTime(s.restEnd));
    restLabel->setText(QString("Отдых:          %1").arg(restStr));
}

void MainWindow::displayAnalysis(const DayAnalysis& a)
{
    if (!a.valid) {
        scoreLabel->setText("Оценка: —");
        verdictLabel->setText(a.verdict.isEmpty()
            ? "Данные не введены."
            : a.verdict);
        problemsLabel->clear();
        adviceLabel->clear();
        idealScheduleLabel->setText(
            "Нажмите «Ввести данные», чтобы получить рекомендации.");
        return;
    }

    scoreLabel->setText(QString("Оценка: %1 / 100")
        .arg(a.score, 0, 'f', 1));
    verdictLabel->setText("Вердикт: " + a.verdict);

    if (!a.problems.isEmpty()) {
        problemsLabel->setText("⚠ Замечания:\n• "
            + a.problems.join("\n• "));
    }
    else {
        problemsLabel->setText("⚠ Замечания: нет");
    }

    if (!a.advice.isEmpty()) {
        adviceLabel->setText("💡 Советы:\n• " + a.advice.join("\n• "));
    }
    else {
        adviceLabel->setText("💡 Советы: нет");
    }

    const DaySchedule& i = a.idealSchedule;
    idealScheduleLabel->setText(QString(
        "Сон:            %1 — %2\n"
        "Подъём:         %3\n"
        "Работа:         %4 — %5\n"
        "Отдых:          %6 — %7")
        .arg(i.sleep.toString("HH:mm"),
            i.wakeUp.toString("HH:mm"),
            i.wakeUp.toString("HH:mm"),
            i.workStart.toString("HH:mm"),
            i.workEnd.toString("HH:mm"),
            i.restStart.toString("HH:mm"),
            i.restEnd.toString("HH:mm")));
}

void MainWindow::clearDisplay()
{
    wakeUpLabel->setText("Подъём:         —");
    sleepLabel->setText("Сон:            —");
    workLabel->setText("Работа:         —");
    restLabel->setText("Отдых:          —");

    scoreLabel->setText("Оценка: —");
    verdictLabel->setText("—");
    problemsLabel->clear();
    adviceLabel->clear();
    idealScheduleLabel->setText(
        "Нажмите «Ввести данные», чтобы получить рекомендации.");
}