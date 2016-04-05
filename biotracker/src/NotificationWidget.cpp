#include "../NotificationWidget.h"

namespace BioTracker {
namespace Gui {

NotificationWidget::NotificationWidget(QWidget *parent,
                                       Core::BioTrackerApp &facade)
    : QWidget(parent)
    , m_ui(parent)
    , m_facade(facade) {

}

void NotificationWidget::initConnects() {

}

void NotificationWidget::notify(const std::string &message,
                                const Core::MessageType type) {
    std::stringstream ss;
    switch (type) {
    case Core::MessageType::FILE_OPEN:
        ss << "<span style='color:#4c4cc9;'>" << message << "</span>";
        break;
    case Core::MessageType::NOTIFICATION:
        ss << "<span>" << message << "</span>";
        break;
    case Core::MessageType::FAIL:
        ss << "<span style='color:red; font-weight:bold;'>" << message << "</span>";
        break;
    default:
        assert(false);
    }
    ss << "<br>";
    m_ui.edit_notification->insertHtml(QString(ss.str().c_str()));

}

}
}
