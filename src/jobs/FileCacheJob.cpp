//
// Created by liugantang on 2025/10/4.
//

#include "FileCacheJob.h"
#include <QFileInfo>
#include <QDir>

#include "../FormatUtil.h"

namespace Jobs {
    FileCacheJob::FileCacheJob(QString filePath, QObject *parent) : BaseCacheJob(parent),
                                                                    startPath(std::move(filePath)) {
    }


    void FileCacheJob::run() {
        {
            std::unique_lock<std::mutex> lock(mutex);
            QString fileDir = QFileInfo(startPath).absolutePath();
            QDir dir(fileDir);
            auto infoList = dir.entryInfoList(QDir::AllEntries|QDir::NoDotAndDotDot, QDir::SortFlag::Name);
            for (int i = 0; i < infoList.size(); i++) {
                if (isQuit) {
                    return;
                }
                const auto &info = infoList.at(i);
                if (info.isDir()) {
                    QDir dirChild(info.absoluteFilePath());
                    infoList.append(dirChild.entryInfoList(QDir::AllEntries|QDir::NoDotAndDotDot, QDir::SortFlag::Name));
                }
                if (info.isFile() && FormatUtil::supportImages().contains(info.suffix().toLower())) {
                    listPaths.append(info.absoluteFilePath());
                    if (startPath == info.absoluteFilePath()) {
                        currIndex = i;
                        indexChanged = true;
                    }
                }
            }
        }

        int oldIndex = currIndex;
        while (true) {
            std::unique_lock<std::mutex> lock(mutex);
            conv.wait(lock, [this] { return indexChanged == true; });
            if (isQuit) {
                return;
            }
            indexChanged = false;
            if (oldIndex - 1 == currIndex) {
                cached(pixmapPrev);
                cacheIndex(currIndex);
            } else if (oldIndex + 1 == currIndex) {
                cached(pixmapNext);
                cacheIndex(currIndex);
            } else {
                cacheIndex(currIndex);
                cached(pixmapCurr);
            }
            oldIndex = currIndex;
        }
    }

    void FileCacheJob::next() {
        if (canNext()) {
            std::lock_guard<std::mutex> lock(mutex);
            currIndex++;
            indexChanged = true;
        }
        conv.notify_all();
    }

    void FileCacheJob::prev() {
        if (canPrev()) {
            std::unique_lock lock(mutex);
            currIndex--;
            indexChanged = true;
        }
        conv.notify_all();
    }

    void FileCacheJob::index(int index) {
        if (canIndex(index)) {
            std::lock_guard lock(mutex);
            currIndex = index;
            indexChanged = true;
        }
        conv.notify_all();
    }

    bool FileCacheJob::canNext() {
        std::lock_guard lock(mutex);
        return currIndex < listPaths.size() - 1 && currIndex >= 0;
    }

    bool FileCacheJob::canPrev() {
        std::lock_guard lock(mutex);
        return currIndex < listPaths.size() && currIndex > 0;
    }

    bool FileCacheJob::canIndex(int index) {
        std::lock_guard lock(mutex);
        return index >= 0 && index < listPaths.size();
    }

    void FileCacheJob::cacheIndex(int index) {
        if (index >= 0 && index < listPaths.size() - 1 ) {
            pixmapNext = QPixmap(listPaths.at(index + 1));
        }
        if (index > 0 && index < listPaths.size()) {
            pixmapPrev = QPixmap(listPaths.at(index - 1));
        }
        if (index >= 0 && index < listPaths.size()) {
            pixmapCurr = QPixmap(listPaths.at(index));
        }
    }
}
