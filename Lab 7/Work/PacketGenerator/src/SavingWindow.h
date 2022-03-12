#pragma once

#include <QDialog>

namespace Ui {
class SavingWindow;
}

class SavingWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SavingWindow( QWidget* parent = nullptr );
    ~SavingWindow();

private:
    void onFinish( int result );
    bool savePacket();

private:
    Ui::SavingWindow *ui;

    const QString PATH_SAVE = "SavePackets/";
};
