package HabitRabbit_UI_Model {

    import ScalarValues::*;
    import HabitRabbit_Domain::*;

    /* ============================================================
     * 1. Общие типы пользовательского интерфейса
     * ============================================================ */

    enum def UiStatusKind {
        enum Completed;
        enum NotCompleted;
        enum Missing;
    }

    enum def HabitType {
        enum OneTime;
        enum Quantitative;
    }

    enum def RepetitionMode {
        enum Daily;
        enum Custom;
    }

    enum def StatsViewKind {
        enum ScoreChart;
        enum BarResultsChart;
        enum RelativeResultsChart;
    }

    /* ============================================================
     * 2. Внешние/доменные сущности, с которыми работает UI
     * ============================================================ */

    item def HabitItem {
        attribute id : String;
        attribute title : String;
        attribute startDate : String;
        attribute habitType : HabitType;
        attribute repetitionMode : RepetitionMode;
        attribute unit : String [0..1];
        attribute dailyGoal : Real [0..1];
    }

    item def HabitStatusItem {
        attribute habitId : String;
        attribute date : String;
        attribute status : UiStatusKind;
    }

    item def CalendarColumnItem {
        attribute date : String;
        attribute visible : Boolean;
    }

    /* ============================================================
     * 3. Диалоги и окна
     * ============================================================ */

    part def MainWindow {
        /* Главное окно: таблица привычек и команды */
        port addHabitCommand : AddHabitCommandPort;
        port openStatsCommand : OpenStatsCommandPort;
        port openSettingsCommand : OpenSettingsCommandPort;
        port syncLocalCommand : SyncLocalCommandPort;
        port syncRemoteCommand : SyncRemoteCommandPort;

        part habitTable : HabitTableView;
        part menuBar : MenuBar;
        part toolbar : Toolbar;
    }

    part def HabitTableView {
        /* Табличное представление привычек с календарными статусами */
        port horizontalScroll : HorizontalScrollPort;
        part statusLegend : StatusLegendView;
        part rows : HabitRowView [0..*];
    }

    part def HabitRowView {
        attribute habit : HabitItem;
        part calendarCells : StatusCellView [0..*];
    }

    part def StatusCellView {
        attribute date : String;
        attribute status : UiStatusKind;
        attribute icon : String;
    }

    part def StatusLegendView {
        attribute completedIcon : String;
        attribute notCompletedIcon : String;
        attribute missingIcon : String;
    }

    part def MenuBar {
        port addHabitCommand : AddHabitCommandPort;
        port openSettingsCommand : OpenSettingsCommandPort;
        port openHelpCommand : OpenHelpCommandPort;
        port syncLocalCommand : SyncLocalCommandPort;
        port syncRemoteCommand : SyncRemoteCommandPort;
    }

    part def Toolbar {
        port addHabitCommand : AddHabitCommandPort;
        port openSettingsCommand : OpenSettingsCommandPort;
    }

    part def AddHabitDialog {
        /* REQ-INT-002, REQ-FUNC-001..006 */
        attribute titleValue : String;
        attribute startDateValue : String;
        attribute habitTypeValue : HabitType;
        attribute repetitionModeValue : RepetitionMode;
        attribute unitValue : String [0..1];
        attribute dailyGoalValue : Real [0..1];

        port openCustomRepetitionCommand : OpenCustomRepetitionPort;
        port confirmCommand : ConfirmHabitPort;
        port cancelCommand : CancelHabitPort;
    }

    part def CustomRepetitionDialog {
        /* REQ-INT-003, REQ-FUNC-006 */
        attribute selectedParameters : String;
        port confirmCommand : ConfirmRepetitionPort;
        port cancelCommand : CancelRepetitionPort;
    }

    part def StatsWindow {
        /* REQ-INT-004, REQ-FUNC-012..014 */
        attribute selectedHabit : HabitItem;
        port selectViewCommand : SelectStatsViewPort;
        port openScoreChartSettingsCommand : OpenScoreChartSettingsPort;

        part scoreChartView : ScoreChartView;
        part barResultsView : BarResultsChartView;
        part relativeResultsView : RelativeResultsChartView;
    }

    part def ScoreChartView {
        attribute settings : String;
    }

    part def BarResultsChartView;

    part def RelativeResultsChartView;

    part def ScoreChartSettingsDialog {
        /* REQ-FUNC-014 */
        attribute parameters : String;
        port applyCommand : ApplyScoreChartSettingsPort;
        port cancelCommand : CancelScoreChartSettingsPort;
    }

    part def SettingsDialog {
        /* REQ-INT-005, REQ-FUNC-015 */
        attribute availableParameters : String [0..*];
        port applyCommand : ApplySettingsPort;
        port cancelCommand : CancelSettingsPort;
    }

    /* ============================================================
     * 4. Порты команд и событий
     * ============================================================ */

    port def AddHabitCommandPort {
        in event openAddHabitDialog;
    }

    port def OpenStatsCommandPort {
        in event openStats;
    }

    port def OpenSettingsCommandPort {
        in event openSettings;
    }

    port def OpenHelpCommandPort {
        in event openHelp;
    }

    port def SyncLocalCommandPort {
        in event syncLocal;
        out event syncLocalResult : String;
    }

    port def SyncRemoteCommandPort {
        in event syncRemote;
        out event syncRemoteResult : String;
    }

    port def OpenCustomRepetitionPort {
        in event openCustomRepetition;
    }

    port def ConfirmHabitPort {
        in event confirmHabit;
        out event habitCreated : HabitItem;
    }

    port def CancelHabitPort {
        in event cancelHabit;
    }

    port def ConfirmRepetitionPort {
        in event confirmRepetition;
        out event repetitionParameters : String;
    }

    port def CancelRepetitionPort {
        in event cancelRepetition;
    }

    port def SelectStatsViewPort {
        in event selectView(viewKind : StatsViewKind);
    }

    port def OpenScoreChartSettingsPort {
        in event openScoreChartSettings;
    }

    port def ApplyScoreChartSettingsPort {
        in event applyScoreChartSettings(settings : String);
    }

    port def CancelScoreChartSettingsPort {
        in event cancelScoreChartSettings;
    }

    port def ApplySettingsPort {
        in event applySettings;
    }

    port def CancelSettingsPort {
        in event cancelSettings;
    }

    port def HorizontalScrollPort {
        in event scrollHorizontally(delta : Integer);
    }

    /* ============================================================
     * 5. Связи между UI-компонентами
     * ============================================================ */

    connection def MainWindowToAddHabitDialog {
        end source : MainWindow::addHabitCommand;
        end target : AddHabitDialog::openCustomRepetitionCommand;
    }

    connection def AddHabitDialogToCustomRepetition {
        end source : AddHabitDialog::openCustomRepetitionCommand;
        end target : CustomRepetitionDialog::confirmCommand;
    }

    connection def MainWindowToStatsWindow {
        end source : MainWindow::openStatsCommand;
        end target : StatsWindow::selectViewCommand;
    }

    connection def StatsWindowToScoreChartSettings {
        end source : StatsWindow::openScoreChartSettingsCommand;
        end target : ScoreChartSettingsDialog::applyCommand;
    }

    connection def MainWindowToSettingsDialog {
        end source : MainWindow::openSettingsCommand;
        end target : SettingsDialog::applyCommand;
    }

    /* ============================================================
     * 6. Требования UI (трассировка на SRS)
     * ============================================================ */

    requirement def ReqMainWindowDisplay {
        id : "REQ-INT-001";
        text : "Система должна отображать главное окно с таблицей привычек и датированными статусами.";
    }

    requirement def ReqAddHabitDialog {
        id : "REQ-INT-002";
        text : "Система должна предоставлять диалог для ввода основных параметров новой привычки.";
    }

    requirement def ReqCustomRepetitionDialog {
        id : "REQ-INT-003";
        text : "При выборе пользовательского повторения система должна предоставлять отдельный диалог настройки повторяемости.";
    }

    requirement def ReqStatsWindow {
        id : "REQ-INT-004";
        text : "Система должна предоставлять отдельное окно статистики с несколькими графическими представлениями данных привычки.";
    }

    requirement def ReqSettingsDialog {
        id : "REQ-INT-005";
        text : "Система должна предоставлять пользователю доступ к диалогу настроек.";
    }

    requirement def ReqAddHabitOpen {
        id : "REQ-FUNC-001";
        text : "Система должна позволять пользователю открыть диалог добавления привычки из главного окна.";
    }

    requirement def ReqHabitTitleAndStartDate {
        id : "REQ-FUNC-002";
        text : "При создании привычки пользователь должен иметь возможность задать её название и дату начала.";
    }

    requirement def ReqHabitType {
        id : "REQ-FUNC-003";
        text : "Система должна поддерживать типы OneTime и Quantitative.";
    }

    requirement def ReqQuantitativeParams {
        id : "REQ-FUNC-004";
        text : "Для привычки типа Quantitative система должна предоставлять поля единиц измерения и дневной цели.";
    }

    requirement def ReqDailyRepetition {
        id : "REQ-FUNC-005";
        text : "Система должна позволять использовать ежедневное повторение привычки.";
    }

    requirement def ReqCustomRepetition {
        id : "REQ-FUNC-006";
        text : "Система должна позволять настраивать пользовательское повторение через отдельный диалог.";
    }

    requirement def ReqHabitCalendarDisplay {
        id : "REQ-FUNC-007";
        text : "Система должна отображать привычки в табличной модели с календарными статусами по датам.";
    }

    requirement def ReqStatusVisuals {
        id : "REQ-FUNC-008";
        text : "Система должна визуально различать выполненный, невыполненный и отсутствующий статус привычки.";
    }

    requirement def ReqCalendarScroll {
        id : "REQ-FUNC-009";
        text : "Система должна позволять просматривать разные части календарной таблицы при горизонтальной прокрутке.";
    }

    requirement def ReqOpenStats {
        id : "REQ-FUNC-012";
        text : "Система должна позволять открыть окно статистики для просмотра данных привычки.";
    }

    requirement def ReqStatsViews {
        id : "REQ-FUNC-013";
        text : "Окно статистики должно предоставлять график оценки/счёта, столбчатый график результатов и относительный график результатов.";
    }

    requirement def ReqScoreChartSettings {
        id : "REQ-FUNC-014";
        text : "Для графика оценки/счёта система должна предоставлять предусмотренные реализацией настройки.";
    }

    requirement def ReqOpenSettings {
        id : "REQ-FUNC-015";
        text : "Система должна позволять открыть диалог настроек из главного окна.";
    }

    requirement def ReqLocalSync {
        id : "REQ-FUNC-016";
        text : "Если соответствующая команда присутствует в текущей сборке, система должна предоставлять сценарий локальной синхронизации файла базы данных.";
    }

    requirement def ReqRemoteSync {
        id : "REQ-FUNC-017";
        text : "Если удалённая синхронизация включена в текущую сборку, система должна предоставлять команду передачи файла базы данных через scp.";
    }

    /* ============================================================
     * 7. Трассировка требований на элементы модели
     * ============================================================ */

    satisfy ReqMainWindowDisplay by MainWindow;
    satisfy ReqAddHabitDialog by AddHabitDialog;
    satisfy ReqCustomRepetitionDialog by CustomRepetitionDialog;
    satisfy ReqStatsWindow by StatsWindow;
    satisfy ReqSettingsDialog by SettingsDialog;

    satisfy ReqAddHabitOpen by MainWindow;
    satisfy ReqHabitTitleAndStartDate by AddHabitDialog;
    satisfy ReqHabitType by AddHabitDialog;
    satisfy ReqQuantitativeParams by AddHabitDialog;
    satisfy ReqDailyRepetition by AddHabitDialog;
    satisfy ReqCustomRepetition by CustomRepetitionDialog;

    satisfy ReqHabitCalendarDisplay by HabitTableView;
    satisfy ReqStatusVisuals by StatusCellView;
    satisfy ReqCalendarScroll by HabitTableView;

    satisfy ReqOpenStats by MainWindow;
    satisfy ReqStatsViews by StatsWindow;
    satisfy ReqScoreChartSettings by ScoreChartSettingsDialog;

    satisfy ReqOpenSettings by MainWindow;
    satisfy ReqLocalSync by MainWindow;
    satisfy ReqRemoteSync by MainWindow;

    /* ============================================================
     * 8. Ограничения и допущения
     * ============================================================ */

    constraint def SingleUserOffline {
        text : "UI рассчитан на одного пользователя и работу без сети; удалённая синхронизация зависит от настроенного SSH/scp.";
    }

    constraint def StatusVisualDistinction {
        text : "Состояния Completed, NotCompleted и Missing должны визуально различаться в таблице.";
    }

    constraint def QuantitativeFieldsVisibility {
        text : "Поля unit и dailyGoal отображаются только при выбранном типе Quantitative.";
    }
}