//
// Created by liugantang on 2025/10/4.
//

#include "FormatUtil.h"

const QStringList& FormatUtil::supportImages() {
    static QStringList supportImages{
        "jpeg",
        "jpg",
        "png",
        "webp",
        "bmp",
        "tiff",
        "gif",
    };
    return supportImages;
}
