//
// Created by liugantang on 2025/10/4.
//

#include "BaseCacheJob.h"

namespace Jobs {
    BaseCacheJob::BaseCacheJob(QObject *parent) {
        setAutoDelete(false);
    }

    BaseCacheJob::~BaseCacheJob() {
        quit();
    }

    void BaseCacheJob::quit() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            isQuit = true;
            indexChanged = true;
        }
        conv.notify_all();
    }
} // Jobs
