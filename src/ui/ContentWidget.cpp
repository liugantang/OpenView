//
// Created by liugantang on 2025/10/4.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ContentWidget.h" resolved

#include "ContentWidget.h"
#include "ui_ContentWidget.h"
#include "ContentWidgetPrivate.h"
#include "../ActionManager.h"
#include "../FormatUtil.h"

#include <QStandardPaths>
#include <QDesktopServices>
#include <QFileDialog>
#include <QThreadPool>

ContentWidget::ContentWidget(QWidget *parent) : QWidget(parent), ui(new Ui::ContentWidget),
                                                d(std::make_unique<ContentWidgetPrivate>()) {
    // Use PIMPL idiom
    ui->setupUi(this);
    initGui();
    connectActions();
}

ContentWidget::~ContentWidget() {
    delete ui;
}

// Slot to handle the open file action
void ContentWidget::onOpenFile() {
    if (d->imageFileCacheJob) {
        d->imageFileCacheJob->quit();
    }
    // Define file filters for the dialog
    const QString imagesFilter = "Images (*.png *.tiff *.jpg *.jpeg *.bmp *.gif *.webp)";
    const QString archivesFilter = "Archives (*.zip *.7z)";
    const QString allFilesFilter = "All Files (*)";
    const QString filters = imagesFilter + ";;" + archivesFilter + ";;" + allFilesFilter;

    // Show the file dialog to get a file path from the user
    QString selectedFile = QFileDialog::getOpenFileName(
        nullptr,
        tr("Select file"),
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), // Default to Pictures folder
        filters
    );
    // Return if the user canceled the dialog
    if (selectedFile.isEmpty()) {
        return;
    }
    d->imageFileCacheJob.reset(new ImageFileCacheJob(selectedFile));
    connect(d->imageFileCacheJob.get(), &ImageFileCacheJob::imageReady, this, &ContentWidget::onCacheReady);
    QThreadPool::globalInstance()->start(d->imageFileCacheJob.get());
}


// Slot to navigate to the next image
void ContentWidget::onNext() {
   if (d->imageFileCacheJob) {
       d->imageFileCacheJob->next();
   }
}

// Slot to navigate to the previous image
void ContentWidget::onPrevious() {
  if (d->imageFileCacheJob) {
      d->imageFileCacheJob->prev();
  }
}

// Slot to handle errors from background jobs
void ContentWidget::onJobError(QString errMsg) {
    qDebug() << errMsg;
}

void ContentWidget::onCacheReady(QImage cacheImage) {
    updateImage(cacheImage);
}

// Initialize the GUI components for image display
void ContentWidget::initGui() {
    d->view = new QGraphicsView(this);
    d->scene = new QGraphicsScene(d->view);
    d->view->setScene(d->scene);
    d->view->setFrameShape(QFrame::NoFrame);
    d->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Enable anti-aliasing for better rendering quality
    d->view->setRenderHint(QPainter::Antialiasing, true);
    d->view->setRenderHint(QPainter::SmoothPixmapTransform, true);

    // The item that will hold the displayed image
    d->pixmapItem = new QGraphicsPixmapItem();
    d->scene->addItem(d->pixmapItem);
}

// Connect signals from the global ActionManager to this widget's slots
void ContentWidget::connectActions() {
    connect(&ActionManager::instance(), &ActionManager::requestOpenFile, this, &ContentWidget::onOpenFile);
    connect(&ActionManager::instance(), &ActionManager::requestNextImage, this, &ContentWidget::onNext);
    connect(&ActionManager::instance(), &ActionManager::requestPreviousImage, this, &ContentWidget::onPrevious);
}

void ContentWidget::updateImage(const QImage &image) {
    if (image.isNull()) {
        return;
    }
    d->pixmapItem->setPixmap(QPixmap::fromImage(image));
    d->scene->setSceneRect(d->pixmapItem->boundingRect());
    d->view->fitInView(d->pixmapItem, Qt::KeepAspectRatio);
}

// Handle widget resize events
void ContentWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    d->view->resize(this->size());
    if (d->pixmapItem->pixmap().isNull()) {
        return;
    }
    d->view->fitInView(d->pixmapItem, Qt::KeepAspectRatio);
}
