//
// Created by liugantang on 2025/10/4.
//

#ifndef OPENVIEW_FILECACHEJOB_H
#define OPENVIEW_FILECACHEJOB_H
#include "BaseCacheJob.h"

namespace Jobs {
    class FileCacheJob : public Jobs::BaseCacheJob {
        Q_OBJECT

    public:
        FileCacheJob(QString filePath, QObject *parent = nullptr);

        void run() override;

        void next() override;

        void prev() override;

        void index(int index) override;

        bool canNext() override;

        bool canPrev() override;

        bool canIndex(int index) override;


    private:
        void cacheIndex(int index) override;

        QString startPath;
        QStringList listPaths;
    };
}
#endif //OPENVIEW_FILECACHEJOB_H
