//
// Created by liugantang on 2025/10/4.
//

#ifndef OPENVIEW_CONTENTWIDGETPRIVATE_H
#define OPENVIEW_CONTENTWIDGETPRIVATE_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include "FileCacheJob.h"
#include "ImageFileCacheJob.h"

class ContentWidgetPrivate {
public:
    QGraphicsPixmapItem *pixmapItem;
    QGraphicsScene *scene;
    QGraphicsView *view;
    std::unique_ptr<ImageFileCacheJob> imageFileCacheJob;
};
#endif //OPENVIEW_CONTENTWIDGETPRIVATE_H
