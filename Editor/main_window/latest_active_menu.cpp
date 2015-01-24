#include "mainwindow.h"

/*
 * QMdiArea::activeSubWindow doesn't return a valid window when the main window is minized.
 * This class should help to record the latest actie window anyway.
 */

void MainWindow::recordSwitchedWindow(QMdiSubWindow *window)
{
    latest = window;
}

void MainWindow::recordRemovedWindow(QObject *possibleDeletedWindow)
{
    if((QObject*)possibleDeletedWindow == latest)
        latest = 0;
}
