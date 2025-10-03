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
    QString currImagePath;
    std::unique_ptr<Jobs::FileCacheJob> fileCacheJob;
};
#endif //OPENVIEW_CONTENTWIDGETPRIVATE_H
