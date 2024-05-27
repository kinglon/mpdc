#ifndef DEBUGDIALOG_H
#define DEBUGDIALOG_H

#include <QDialog>

namespace Ui {
class DebugDialog;
}

class DebugDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DebugDialog(QWidget *parent = nullptr);
    ~DebugDialog();

private slots:
    void onLoadBtnClicked(bool checked);

    void onRunJsBtnClicked(bool checked);

    void onCaptureBtnlicked(bool checked);

    void onChangeSizeBtnlicked(bool checked);

    void onRunJsCodeFinish(const QVariant& result);

private:
    Ui::DebugDialog *ui;
};

#endif // DEBUGDIALOG_H
