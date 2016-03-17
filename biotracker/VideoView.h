#pragma once

#include <memory>

#include <boost/optional.hpp>

#include <opencv2/opencv.hpp>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>

#include "biotracker/TextureObject.h"
#include "biotracker/BioTrackerApp.h"

#include "biotracker/util/stdext.h"

class QOpenGLContext;

namespace BioTracker {
namespace Gui {

class VideoView : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
  public:
    enum class Mode : uint8_t {
        INTERACTION = 0,
        PANZOOM
    };

    VideoView(QWidget *parent, Core::BioTrackerApp &facade);

    Mode getMode() const {
        return m_currentMode;
    }

    bool isZoomed();

    void setView(Core::TrackingAlgorithm::View v) {
        m_view = v;
        update();
    }

public Q_SLOTS:
    void setMode(const Mode mode);
    void fitToWindow();

private Q_SLOTS:
    void handleLoggedMessage(const QOpenGLDebugMessage &debugMessage);

  private:

    QOpenGLDebugLogger m_openGLLogger;

    /**
     * @brief Current State of the VideoWidget. In Interaction mode, mouse and keyboard event will be forwarded to
     * the currently active tracking algorithm.
     */
    Mode m_currentMode;

    /**
     * @brief Pan/Zoom state variables.
     */
    BioTracker::Core::PanZoomState m_panZoomState;

    /**
     * @brief Ratio of widget size and image size
     */
    float m_screenPicRatio;

    Core::BioTrackerApp &m_biotracker;

    Core::TrackingAlgorithm::View m_view;

    /**
     * @brief m_painter
     */
    QPainter m_painter;
    bool m_firstAttempt;

    void paintGL() override;

    /**
    * @brief unprojectScreenPos
    * @param mouseCoords coordinates relative to window
    * @return coordinates relative to image
    */
    QPoint unprojectScreenPos(QPoint mouseCoords);

    void keyPressEvent(QKeyEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
};
}
}
