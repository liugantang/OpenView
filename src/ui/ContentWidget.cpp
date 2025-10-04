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
                                                d(std::make_unique<ContentWidgetPrivate>()) { // Use PIMPL idiom
    ui->setupUi(this);
    initGui();
    connectActions();
}

ContentWidget::~ContentWidget() {
    delete ui;
}

// Slot to handle the open file action
void ContentWidget::onOpenFile() {
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

    QFileInfo info(selectedFile);
    QString suffixFile = info.suffix();

    // Check if the selected file is a supported image format
    if (FormatUtil::supportImages().contains(suffixFile)) {
        QDir dir(info.absolutePath());
        // Get a list of all files in the same directory
        auto infoList = dir.entryInfoList(QDir::Files, QDir::SortFlag::Name);

        // Reset the file list and cache
        d->fileList.reset(new QStringList());
        d->hashPersistentCache.clear();
        for (int i = 0; i < infoList.size(); ++i) {
            d->fileList->append(infoList.at(i).filePath());
        }

        // Find the index of the selected file
        d->currentIndex = d->fileList->indexOf(selectedFile);
        // Start a background job to cache images around the current one
        createCacheJob(d->currentIndex);
    }
}

// Slot to display an image by its index, primarily using the cache
void ContentWidget::onCached(int index) {
    d->currentIndex = index;
    QImage image;
    {
        // Retrieve the image from the persistent cache
        image = d->hashPersistentCache.value(index);
    }

    // If the image is not in the cache, trigger a job to load it
    if (image.isNull()) {
        createCacheJob(d->currentIndex);
        return;
    }

    // Display the image
    d->pixmapItem->setPixmap(QPixmap::fromImage(image));
    d->pixmapItem->setScale(1.0);

    // Center the image in the graphics view
    QRectF sceneRect = d->scene->sceneRect();
    QRectF itemRect = d->pixmapItem->boundingRect();
    qreal scaledItemWidth = itemRect.width() * d->pixmapItem->scale();
    qreal scaledItemHeight = itemRect.height() * d->pixmapItem->scale();

    qreal x = (sceneRect.width() - scaledItemWidth) / 2.0;
    qreal y = (sceneRect.height() - scaledItemHeight) / 2.0;

    d->pixmapItem->setPos(x, y);
}

// Slot to navigate to the next image
void ContentWidget::onNext() {
    if ( d->fileList->isEmpty() || d->currentIndex >= d->fileList->size() - 1) {
        return; // Do nothing if at the end of the list
    }
    onCached(d->currentIndex + 1);
    createCacheJob(d->currentIndex); // Pre-cache images around the new index
}

// Slot to navigate to the previous image
void ContentWidget::onPrevious() {
    if ( d->fileList->isEmpty() || d->currentIndex <= 0) {
        return; // Do nothing if at the beginning of the list
    }
    onCached(d->currentIndex - 1);
    createCacheJob(d->currentIndex); // Pre-cache images around the new index
}

// Slot to handle errors from background jobs
void ContentWidget::onJobError(QString errMsg) {
    qDebug() << errMsg;
}

// Slot to handle the result from a successful cache job
void ContentWidget::onCacheReady(QHash<int, QImage> cacheImages) {
    {
        // Merge the newly loaded images into the main cache
        d->hashPersistentCache.insert(cacheImages);
        qDebug() << "Cache merged. Total cached items:" << d->hashPersistentCache.size();
    }
    evictCache(); // Clean up the cache if it's too large
    d->m_isLoading = false; // Mark loading as complete
    onCached(d->currentIndex); // Display the current image, which should now be cached
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

// Evicts items from the cache if it exceeds the maximum size
void ContentWidget::evictCache() {
    // Keep removing items until the cache size is within the limit
    while (d->hashPersistentCache.size() > d->maxCacheSize) {
        int furthestKey = -1;
        int maxDistance = -1;

        // Find the cached image that is furthest away from the current index
        for (int key: d->hashPersistentCache.keys()) {
            int distance = qAbs(key - d->currentIndex);
            if (distance > maxDistance) {
                maxDistance = distance;
                furthestKey = key;
            }
        }

        // Remove the furthest item
        if (furthestKey != -1) {
            d->hashPersistentCache.remove(furthestKey);
            qDebug() << "Evicted image at index" << furthestKey << "from cache. New size:" << d->hashPersistentCache.size();
        } else {
            break; // Should not happen if size > maxCacheSize, but as a safeguard
        }
    }
}

// Creates and starts a background job to cache images
void ContentWidget::createCacheJob(int centerIndex) {
    // Prevent starting a new job if one is already running
    if (d->m_isLoading) {
        return;
    }

    // Define the window of images to cache around the center index
    int startIndex = qMax(0, centerIndex - d->cacheWindow);
    int endIndex = qMin(d->fileList->size() - 1, centerIndex + d->cacheWindow);

    // Identify which images in the window are not already in the cache
    QList<int> indicesToLoad;
    {
        for (int i = startIndex; i <= endIndex; ++i) {
            if (!d->hashPersistentCache.contains(i)) {
                indicesToLoad.append(i);
            }
        }
    }

    // If all needed images are already cached, just display the image and return
    if (indicesToLoad.isEmpty()) {
        d->m_isLoading = false;
        onCached(centerIndex);
        return;
    }

    d->m_isLoading = true;
    // Create a new job with the list of files and indices to load
    auto *job = new Jobs::FileCacheJob(d->fileList, indicesToLoad);
    // Connect signals from the job to this widget's slots
    connect(job, &Jobs::FileCacheJob::imagesReady, this, &ContentWidget::onCacheReady);
    connect(job, &Jobs::FileCacheJob::error, this, &ContentWidget::onJobError);
    // Start the job in a background thread using Qt's global thread pool
    QThreadPool::globalInstance()->start(job);
}

// Handle widget resize events
void ContentWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    // Ensure the graphics view and scene resize with the widget
    d->view->resize(this->size());
    d->scene->setSceneRect(d->view->rect());
}