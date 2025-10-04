//
// Created by liugantang on 2025/10/4.
//

#ifndef OPENVIEW_CONTENTWIDGETPRIVATE_H
#define OPENVIEW_CONTENTWIDGETPRIVATE_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include "FileCacheJob.h"

class ContentWidgetPrivate {
public:
    QGraphicsPixmapItem *pixmapItem;
    QGraphicsScene *scene;
    QGraphicsView *view;

    QHash<int, QImage> hashPersistentCache;
    std::shared_ptr<QStringList> fileList;
    int currentIndex = -1;
    bool m_isLoading = false;
    const int cacheWindow = 10; // load 10 images every time.
    const int maxCacheSize = 50; // max keep 50 images in memory.
};
#endif //OPENVIEW_CONTENTWIDGETPRIVATE_H
