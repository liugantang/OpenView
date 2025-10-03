//
// Created by liugantang on 2025/10/4.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ContentWidget.h" resolved

#include "ContentWidget.h"
#include "ui_ContentWidget.h"
#include "ContentWidgetPrivate.h"
#include "../ActionManager.h"

#include <QStandardPaths>
#include <QDesktopServices>
#include <QFileDialog>
#include <QThreadPool>
#include <ui_MainWindow.h>

#include "MainWindow.h"
#include "../FormatUtil.h"

ContentWidget::ContentWidget(QWidget *parent) : QWidget(parent), ui(new Ui::ContentWidget),
                                                d(std::make_unique<ContentWidgetPrivate>()) {
    ui->setupUi(this);
    initGui();
    connectActions();
}

ContentWidget::~ContentWidget() {
    delete ui;
}

void ContentWidget::onOpenFile() {
    if (d->fileCacheJob) {
        d->fileCacheJob->quit();
    }

    const QString imagesFilter = "Images (*.png *.tiff *.jpg *.jpeg *.bmp *.gif *.webp)";
    const QString archivesFilter = "Archives (*.zip *.rar *.7z *.tar *.gz)";
    const QString allFilesFilter = "All Files (*)";
    //merge filter
    const QString filters = imagesFilter + ";;" + archivesFilter + ";;" + allFilesFilter;

    QString selectedFile = QFileDialog::getOpenFileName(
        nullptr,
        tr("Select file"),
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
        filters
    );
    if (selectedFile.isEmpty()) {
        return;
    }
    QString suffixFile = QFileInfo(selectedFile).suffix();
    if (FormatUtil::supportImages().contains(suffixFile)) {
        d->fileCacheJob.reset(new Jobs::FileCacheJob(selectedFile));
        connect(d->fileCacheJob.get(), &Jobs::FileCacheJob::cached, this, &ContentWidget::onCached);
        QThreadPool::globalInstance()->start(d->fileCacheJob.get());
    }
}

void ContentWidget::onCached(QPixmap pixmap) {
    d->pixmapItem->setPixmap(pixmap);
    d->pixmapItem->setScale(1.0);

    QRectF sceneRect = d->scene->sceneRect();
    QRectF itemRect = d->pixmapItem->boundingRect();
    qreal scaledItemWidth = itemRect.width() * d->pixmapItem->scale();
    qreal scaledItemHeight = itemRect.height() * d->pixmapItem->scale();

    qreal x = (sceneRect.width() - scaledItemWidth) / 2.0;
    qreal y = (sceneRect.height() - scaledItemHeight) / 2.0;

    d->pixmapItem->setPos(x, y);
}

void ContentWidget::onNext() {
    if (d->fileCacheJob) {
        d->fileCacheJob->next();
    }
}

void ContentWidget::onPrevious() {
    if (d->fileCacheJob) {
        d->fileCacheJob->prev();
    }
}


void ContentWidget::initGui() {
    d->view = new QGraphicsView(this);
    d->scene = new QGraphicsScene(d->view);
    d->view->setScene(d->scene);
    d->view->setFrameShape(QFrame::NoFrame);
    d->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->view->setRenderHint(QPainter::Antialiasing, true);
    d->view->setRenderHint(QPainter::SmoothPixmapTransform, true);

    d->pixmapItem = new QGraphicsPixmapItem();
    d->scene->addItem(d->pixmapItem);
}

void ContentWidget::connectActions() {
    connect(&ActionManager::instance(), &ActionManager::requestOpenFile, this, &ContentWidget::onOpenFile);
    connect(&ActionManager::instance(), &ActionManager::requestNextImage, this, &ContentWidget::onNext);
    connect(&ActionManager::instance(), &ActionManager::requestPreviousImage, this, &ContentWidget::onPrevious);
}

void ContentWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    d->view->resize(this->size());
    d->scene->setSceneRect(d->view->rect());
}
