//
// Created by liugantang on 2025/10/4.
//

#ifndef OPENVIEW_FILECACHEJOB_H
#define OPENVIEW_FILECACHEJOB_H
#include <QObject>
#include <QRunnable>
#include <QStringList>
#include <QImage>
#include <QHash>

namespace Jobs {

    class FileCacheJob : public QObject, public QRunnable {
        Q_OBJECT

    public:
        explicit FileCacheJob(const std::shared_ptr<QStringList>& fileList, QList<int> indicesToLoad, QObject *parent = nullptr);

        void run() override;

    signals:
        void imagesReady(QHash<int, QImage> newImages);
        void error(const QString &errorMessage);

    private:
        std::shared_ptr<QStringList> m_fileList;
        QList<int> m_indicesToLoad;
    };
};
#endif //OPENVIEW_FILECACHEJOB_H
