﻿#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QCloseEvent>

namespace Ui {
class BrowserWindow;
}

class BrowserWindow : public QMainWindow
{
    Q_OBJECT

private:
    explicit BrowserWindow(QWidget *parent = nullptr);
    ~BrowserWindow();

public:
    static BrowserWindow* getInstance();

public:
    // 设置可以关闭退出
    void setCanClose() { m_canClose = true; }

    // 加载网页
    void load(const QUrl& url);

    // 截图
    bool captrueImage(const QString& savePath);

    // 执行JS脚本
    void runJsCode(const QString& jsCode);

signals:
    // 加载网页完成
    void loadFinished(bool ok);

    // 执行JS脚本的结果
    void runJsCodeFinished(const QVariant& result);

private slots:
    void onLoadFinished(bool ok);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QWebEngineView* m_webView = nullptr;

    QSize m_webViewSize = QSize(1920, 1080);

    bool m_canClose = false;
};

#endif // BROWSERWINDOW_H
