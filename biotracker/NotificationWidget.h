#pragma once

#include <QWidget>

#include "biotracker/BioTrackerApp.h"
#include "ui_NotificationWidget.h"
#include "biotracker/util/QtRaiiWrapper.hpp"

namespace BioTracker {
namespace Gui {

class NotificationWidget : public QWidget {
    Q_OBJECT
  public:
    typedef Util::QtRaiiWrapper<Ui::NotificationWidget, QWidget> NotificationUi;

    explicit NotificationWidget(QWidget *parent, Core::BioTrackerApp &facade);

    NotificationUi &getUi() {
        return m_ui;
    }

  public Q_SLOTS:
    /**
     * @brief notify
     *      Status messages for the user interface
     */
    void notify(const std::string &message, const Core::MessageType type);



  private:
    NotificationUi m_ui;
    Core::BioTrackerApp &m_facade;

    void initConnects();

  private Q_SLOTS:
};

}
}
