//
// Created by liugantang on 2025/10/4.
//

#ifndef OPENVIEW_IMAGECACHEJOB_H
#define OPENVIEW_IMAGECACHEJOB_H

#include <condition_variable>
#include <QHash>
#include <QImage>
#include <QRunnable>
#include <QObject>

class ImageFileCacheJob : public QObject,public QRunnable {
    Q_OBJECT

public:
    ImageFileCacheJob(QString imagePath, QObject *parent = nullptr);

    ~ImageFileCacheJob();

    void run() override;

    void next();

    void prev();

    void quit();
signals:
    void imageReady(QImage image);

private:
    QString startPath;
    QStringList fileList;
    int currIndex;
    std::mutex mutex;
    bool isEnd = false;
    bool isChanged = false;
    std::condition_variable conv;
};


#endif //OPENVIEW_IMAGECACHEJOB_H
