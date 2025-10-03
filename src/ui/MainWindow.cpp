//
// Created by liugantang on 2025/9/29.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "../ActionManager.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    initGui();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onActionExitTriggered() {
    QApplication::quit();
}

void MainWindow::initGui() {
    initFileMenu();
}

void MainWindow::initFileMenu() {
    QMenu *fileMenu = new QMenu(tr("&File"),this);
    ui->menubar->addMenu(fileMenu);
    fileMenu->addAction(ActionManager::instance().action(ActionOpenFile));
    fileMenu->addAction(ActionManager::instance().action(ActionOpenFolder));
    fileMenu->addAction(ActionManager::instance().action(ActionClose));
    fileMenu->addAction(ActionManager::instance().action(ActionSelectImage));
    fileMenu->addAction(ActionManager::instance().action(ActionShowInExplorer));
    fileMenu->addSeparator();
    fileMenu->addAction(ActionManager::instance().action(ActionExit));

    connect(&ActionManager::instance(), &ActionManager::requestExit, this, &MainWindow::onActionExitTriggered);
}
