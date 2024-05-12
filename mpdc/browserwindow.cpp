#include "browserwindow.h"
#include "ui_browserwindow.h"
#include <QImage>
#include <QWebEngineSettings>
#include "Utility/ImPath.h"

QWebEnginePage* WebEnginePage::createWindow(WebWindowType)
{
    WebEnginePage *page = new WebEnginePage(this);
    connect(page, &QWebEnginePage::urlChanged, this, &WebEnginePage::onUrlChanged);
    return page;
}

void WebEnginePage::onUrlChanged(const QUrl & url)
{
    if (WebEnginePage *page = qobject_cast<WebEnginePage *>(sender()))
    {
        load(url);
        page->deleteLater();
    }
}

BrowserWindow::BrowserWindow(QWidget *parent) :
    QMainWindow(parent),
    m_webView(new QWebEngineView(this))
{
    setEnabled(false);
    setWindowTitle(QString::fromStdWString(L"浏览器"));
    m_webView->setPage(new WebEnginePage(m_webView));
    m_webView->resize(m_webViewSize);
    setWindowState(windowState() | Qt::WindowMaximized);
    connect(m_webView->page(), &QWebEnginePage::loadFinished,this, &BrowserWindow::onLoadFinished);
}

BrowserWindow::~BrowserWindow()
{

}

BrowserWindow* BrowserWindow::getInstance()
{
    static BrowserWindow* instance = new BrowserWindow();
    return instance;
}

void BrowserWindow::load(const QUrl& url)
{
    m_webView->load(url);
}

QString BrowserWindow::getUrl()
{
    return m_webView->page()->url().toString();
}

bool BrowserWindow::captrueImage(const QString& savePath)
{
    // 如果最小化就最大化展示，因为最小化时捕获不到图片
    if (isMinimized())
    {
        showMaximized();
    }

    // 截屏
    QImage image(m_webView->size(), QImage::Format_ARGB32);
    QPainter painter(&image);
    m_webView->render(&painter);
    painter.end();
    if (!image.save(savePath))
    {
        qCritical("failed to save the capturing image to %s", savePath.toStdString().c_str());
        return false;
    }
    else
    {
        return true;
    }
}

void BrowserWindow::runJsCode(const QString& jsCode)
{
    m_webView->page()->runJavaScript(jsCode, [this](const QVariant &result) {
        emit runJsCodeFinished(result);
    });
}

void BrowserWindow::onLoadFinished(bool ok)
{
    emit loadFinished(ok);
}

void BrowserWindow::closeEvent(QCloseEvent *event)
{
    if (m_canClose)
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}
