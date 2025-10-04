#include "FileCacheJob.h"
#include <QtConcurrent>

namespace Jobs {
    FileCacheJob::FileCacheJob(const std::shared_ptr<QStringList> &fileList, QList<int> indicesToLoad, QObject *parent)
        : QObject(parent), m_fileList(fileList), m_indicesToLoad(std::move(indicesToLoad)) {
        // 使用 move
        setAutoDelete(true);
    }

    void FileCacheJob::run() {
        if (m_indicesToLoad.isEmpty() || !m_fileList) { return; }

        struct LoadedImage {
            int index;
            QImage image;
        };

        auto future = QtConcurrent::mapped(m_indicesToLoad, [this](int index) -> LoadedImage {
            return {index, QImage((*m_fileList)[index])};
        });

        future.waitForFinished();

        QHash<int, QImage> newImages;
        newImages.reserve(future.resultCount());
        for (const auto &result: future.results()) {
            if (!result.image.isNull()) {
                newImages.insert(result.index, std::move(result.image)); // move QImage
            }
        }

        if (!newImages.isEmpty()) {
            emit imagesReady(std::move(newImages)); // move QHash
        }
    }
}
