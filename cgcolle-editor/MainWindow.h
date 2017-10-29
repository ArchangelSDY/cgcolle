#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>

#include "CGColleV1File.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual void keyPressEvent(QKeyEvent *ev) override;

private slots:
    void openFile();
    void saveFile();
    void showEntry();
    void showCompositeRule();
    void syncTypeOffsetLayerId();

private:
    Ui::MainWindow *ui;
    QScopedPointer<CGColleV1File> m_file;
};

#endif // MAINWINDOW_H
