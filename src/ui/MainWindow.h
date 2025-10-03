//
// Created by liugantang on 2025/9/29.
//

#ifndef OPENVIEW_MAINWINDOW_H
#define OPENVIEW_MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE

namespace Ui {
    class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private:
    void initGui();
    void initFileMenu();

    Ui::MainWindow *ui;
};


#endif //OPENVIEW_MAINWINDOW_H
