#include "debugdialog.h"
#include "ui_debugdialog.h"
#include "browserwindow.h"
#include "Utility/ImPath.h"
#include <QDesktopServices>

DebugDialog::DebugDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowModality(Qt::WindowModal);

    connect(ui->loadBtn, &QPushButton::clicked, this, &DebugDialog::onLoadBtnClicked);
    connect(ui->runJSBtn, &QPushButton::clicked, this, &DebugDialog::onRunJsBtnClicked);
    connect(ui->captureBtn, &QPushButton::clicked, this, &DebugDialog::onCaptureBtnlicked);
    connect(ui->enableBtn, &QPushButton::clicked, this, &DebugDialog::onEnableBtnlicked);
}

DebugDialog::~DebugDialog()
{
    delete ui;
}

void DebugDialog::onLoadBtnClicked(bool checked)
{
    (void)checked;
    BrowserWindow::getInstance()->showMaximized();
    BrowserWindow::getInstance()->load(QUrl(ui->urlEdit->text()));
    connect(BrowserWindow::getInstance(), &BrowserWindow::loadFinished, [](){
        qInfo("finish to load url");
    });
}

void DebugDialog::onRunJsBtnClicked(bool checked)
{
    (void)checked;
}

void DebugDialog::onCaptureBtnlicked(bool checked)
{
    (void)checked;
    std::wstring imageSavePath = CImPath::GetCachePath() + L"mpdc_capture_image.png";
    QString qimageSavePath = QString::fromStdWString(imageSavePath);
    if (!BrowserWindow::getInstance()->captrueImage(qimageSavePath))
    {
        qCritical("failed to capture image");
        return;
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(qimageSavePath));
}

void DebugDialog::onEnableBtnlicked(bool checked)
{
    (void)checked;
    BrowserWindow::getInstance()->setEnabled(true);
}
