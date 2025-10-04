//
// Created by liugantang on 2025/10/4.
//

#ifndef OPENVIEW_CONTENTWIDGET_H
#define OPENVIEW_CONTENTWIDGET_H

#include <QWidget>


class ContentWidgetPrivate;
QT_BEGIN_NAMESPACE

namespace Ui {
    class ContentWidget;
}

QT_END_NAMESPACE

class ContentWidget : public QWidget {
    Q_OBJECT

public:
    explicit ContentWidget(QWidget *parent = nullptr);

    ~ContentWidget() override;
private slots:
    void onOpenFile();
    void onNext();
    void onPrevious();
    void onJobError(QString errMsg);
    void onCacheReady(QImage cacheImage);
private:
    void initGui();
    void connectActions();
    void updateImage(const QImage &image);
    void resizeEvent(QResizeEvent *event) override;
    Ui::ContentWidget *ui;
    std::unique_ptr<ContentWidgetPrivate> d;
};


#endif //OPENVIEW_CONTENTWIDGET_H