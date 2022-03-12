#pragma once

#include <QDialog>
#include <QRadioButton>

#include "PcapLiveDeviceList.h"
#include "SystemUtils.h"

namespace Ui {
class ChangingDeviceWindow;
}

typedef std::vector< pcpp::PcapLiveDevice* > Devices;

class ChangingDeviceWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ChangingDeviceWindow( QWidget* parent, Devices& devices );
    ~ChangingDeviceWindow();

    int getChoice() const
    {
        return m_choice;
    }

private:
    Ui::ChangingDeviceWindow *ui;
    QList< QRadioButton* > m_buttonList;
    int m_choice = 0;
};
