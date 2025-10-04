//
// Created by liugantang on 2025/10/4.
//

#include "ImageFileCacheJob.h"

#include <mutex>
#include <QDir>
#include <QFileInfo>
#include <QPixmap>
#include <thread>

#include "../FormatUtil.h"

ImageFileCacheJob::ImageFileCacheJob(QString imagePath, QObject *parent) : QObject(parent),
                                                                           startPath(std::move(imagePath)) {
    setAutoDelete(false);
    qDebug() << "ImageFileCacheJob::ImageFileCacheJob";
}

ImageFileCacheJob::~ImageFileCacheJob() {
    quit();
}

void ImageFileCacheJob::run() {
    {
        std::unique_lock<std::mutex> lock(mutex);
        QFileInfo infoStart(startPath);
        QString dirPath = infoStart.absolutePath();
        QDir dir(dirPath);
        auto infoList = dir.entryInfoList(QDir::Files, QDir::SortFlag::Name);
        for (const auto &infoFile: infoList) {
            if (FormatUtil::supportImages().contains(infoFile.suffix().toLower())) {
                fileList.append(infoFile.absoluteFilePath());
            }
        }
        currIndex = fileList.indexOf(startPath);
        isChanged = true;
    }

    while (true) {
        QString cachePath;
        {
            std::unique_lock<std::mutex> lock(mutex);
            conv.wait(lock, [this] { return this->isChanged; });
            if (isEnd) {
                return;
            }

            isChanged = false;
            cachePath = fileList.at(currIndex);
        }
        QImage image(cachePath);
        imageReady(image);
    }
}

void ImageFileCacheJob::next() {
    std::unique_lock<std::mutex> lock(mutex);
    if (currIndex < fileList.size() - 1) {
        ++currIndex;
        isChanged = true;
        qDebug() << "ImageFileCacheJob::next:" << currIndex;
        conv.notify_one();
    }
}

void ImageFileCacheJob::prev() {
    std::unique_lock<std::mutex> lock(mutex);
    if (currIndex > 0) {
        --currIndex;
        isChanged = true;
        qDebug() << "ImageFileCacheJob::prev:" << currIndex;
        conv.notify_one();
    }
}

void ImageFileCacheJob::quit() {
    isChanged = true;
    isEnd = true;
    conv.notify_one();
}
