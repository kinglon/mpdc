#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShortcut>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initWindow();

private slots:
    void onCollectBtnClicked(bool checked);

    void onCtrlDShortcut();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void AddCollectLog(const QString& log);

private:
    Ui::MainWindow* ui;

    // CTRL+D快捷键
    QShortcut* m_ctrlDShortcut = nullptr;
};
#endif // MAINWINDOW_H
