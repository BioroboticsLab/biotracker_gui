#include <clocale>

#include <QApplication>
#include <QMessageBox>

#include "biotracker/settings/Messages.h"
#include "biotracker/settings/Settings.h"
#include "biotracker/settings/SystemCompatibilityCheck.h"
#include "../Gui.h"
#include "biotracker/util/stdext.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // workaround for cereal issue 144
    // see: https://github.com/USCiLab/cereal/issues/144
    std::setlocale(LC_NUMERIC, "C");

    // meta types
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<BioTracker::Core::Messages::MessageType>("BioTracker::Core::Messages::MessageType");
    qRegisterMetaType<BioTracker::Core::Messages::MessageType>("MessageType");
    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<std::size_t>("std::size_t");
    qRegisterMetaType<size_t>("size_t");
    qRegisterMetaType<BioTracker::Core::TrackerType>("TrackerType");

    if (SystemCompatibilityCheck::checkOpenGLSupport()) {
        app.setOrganizationName("Biorobotics Lab / FU Berlin");
        app.setApplicationName("BioTracker");

        BioTracker::Gui::Gui w;
        return app.exec();
    } else {
        static const std::string title = BioTracker::Core::Messages::System::APPLICATION_CANNOT_START;
        static const std::string msg   = BioTracker::Core::Messages::System::NO_OPENGL;
        QMessageBox::critical(nullptr,
                              QString::fromStdString(title),
                              QString::fromStdString(msg));
    }
    return EXIT_FAILURE;
}
