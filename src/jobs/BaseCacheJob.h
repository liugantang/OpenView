//
// Created by liugantang on 2025/10/4.
//

#ifndef OPENVIEW_FINDOTHERFILESJOB_H
#define OPENVIEW_FINDOTHERFILESJOB_H
#include <condition_variable>
#include <QObject>
#include <QRunnable>
#include <QPixmap>

namespace Jobs {
    class BaseCacheJob : public QObject, public QRunnable {
        Q_OBJECT

    public:
        BaseCacheJob(QObject *parent = nullptr);

        ~BaseCacheJob();

        virtual void run() = 0;

        virtual bool canNext() = 0;

        virtual void next() = 0;

        virtual bool canPrev() = 0;

        virtual void prev() = 0;

        virtual bool canIndex(int index) = 0;

        virtual void index(int index) = 0;

        void quit();

    signals:
        void indexed(int index);

        void cached(QPixmap pixmap);

    protected:
        virtual void cacheIndex(int index) = 0;

        int currIndex = 0;
        bool isQuit = false;
        bool indexChanged = false;
        QPixmap pixmapCurr;
        QPixmap pixmapPrev;
        QPixmap pixmapNext;
        std::mutex mutex;
        std::condition_variable conv;
    };
} // Jobs

#endif //OPENVIEW_FINDOTHERFILESJOB_H
