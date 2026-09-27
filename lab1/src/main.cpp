#include <QApplication>
#include "mainwindow.h"
#include "daymodel.h"
#include "daycontroller.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    DayModel    model;                       // Model
    MainWindow  view;                        // View
    DayController controller(&view, &model); // Controller

    // Связываем View ↔ Controller через указатель
    view.setController(&controller);

    view.show();
    return app.exec();
}