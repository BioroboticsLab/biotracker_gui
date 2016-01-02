#include "../VideoView.h"

// TODO
// check if m_texture is nullptr

// OS X puts the headers in a different location in the include path than
// Windows and Linux, so we need to distinguish between OS X and the other
// systems.
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <biotracker/util/ScreenHelper.h>

namespace BioTracker {
namespace Gui {

VideoView::VideoView(QWidget *parent, Core::BioTrackerApp &biotracker)
    : QOpenGLWidget(parent)
    , m_openGLLogger(this)
    , m_currentMode(Mode::INTERACTION)
    , m_screenPicRatio(0)
    , m_texture(this)
    , m_biotracker(biotracker)
    , m_view(TrackingAlgorithm::OriginalView)
    , m_painter()
    , m_firstAttempt(true) {

}

void VideoView::setMode(const VideoView::Mode mode) {
    m_currentMode = mode;
    switch (mode) {
    case Mode::PANZOOM:
        setCursor(Qt::OpenHandCursor);
        break;
    case Mode::INTERACTION:
        setCursor(Qt::ArrowCursor);
        break;
    default:
        assert(false);
        break;
    }
}

void VideoView::fitToWindow() {
    makeCurrent();
    // reset PanZoomState
    m_panZoomState = BioTracker::Core::PanZoomState();
    update();
}

/**
  This attempts to fix a nasty bug that the very first image is not set
  correctly.
  TODO: fix this bug CORRECTLY!
 * @brief VideoView::initialPaint
 */
void VideoView::initialPaint() {
    QTimer::singleShot(20, this, SLOT(firstPaint()));
}

void VideoView::handleLoggedMessage(const QOpenGLDebugMessage &debugMessage) {
    std::cout << debugMessage.message().toStdString() << std::endl;
}

void VideoView::initializeGL() {
    m_biotracker.initializeOpenGL(context(), this->getTexture());
}

void VideoView::paintGL()
{
    const size_t width = this->width();
    const size_t height = this->height();

    const int imageCols = m_texture.getImage().cols;
    const int imageRows = m_texture.getImage().rows;

    // calculate ratio of screen to displayed image
    const float imgRatio    = static_cast<float>(imageCols) / imageRows;
    const float windowRatio = static_cast<float>(width) / height;

    if (windowRatio < imgRatio) {
        m_screenPicRatio = imageRows / (width / imgRatio);
    } else {
        m_screenPicRatio = imageCols / (height * imgRatio);
    }

    QPainter painter(this);
    m_biotracker.paint(width, height, painter, m_panZoomState, m_view);

    painter.setPen(QColor(255, 0, 0));
    painter.drawRect(QRect(10, 20, 50, 60));

}

void VideoView::firstPaint() {

    if (m_firstAttempt) {
        m_firstAttempt = false;
        this->resize(this->width() + 1, this->height());
        QTimer::singleShot(20, this, SLOT(firstPaint()));
    } else {
        this->resize(this->width() - 1, this->height());
    }
}

QPoint VideoView::unprojectScreenPos(QPoint mouseCoords) {
    // variables required to map window coordinates to picture coordinates
    return BioTracker::Core::ScreenHelper::screenToImageCoords(
        m_panZoomState,
        m_texture.getImage().cols, m_texture.getImage().rows,
        width(), height(),
        mouseCoords
    );
}

void VideoView::keyPressEvent(QKeyEvent *e) {
    e->accept();
    QKeyEvent event(e->type(), e->key(), e->modifiers(), e->text());
    m_biotracker.keyboardEvent(&event);
}

void VideoView::mouseMoveEvent(QMouseEvent *e) {

    switch (m_currentMode) {
        case Mode::PANZOOM: {
            if (m_panZoomState.panState) {
                const QPointF delta = e->localPos() - (*m_panZoomState.panState).lastPos;
                (*m_panZoomState.panState).lastPos = e->localPos();
                m_panZoomState.isChanged = true;
                m_panZoomState.panX -= static_cast<float>(delta.x());
                m_panZoomState.panY -= static_cast<float>(delta.y());
                update();
            }
            break;
        }
        case Mode::INTERACTION: {
            e->accept();
            QPoint p  = unprojectScreenPos(e->pos());
            const QPointF localPos(p);
            QMouseEvent modifiedEvent(e->type(),localPos,e->screenPos(),e->button(),e->buttons(),e->modifiers());
            m_biotracker.mouseEvent(&modifiedEvent);
            break;
        }
        default: {
            assert(false);
            break;
        }
    }

}

void VideoView::mousePressEvent(QMouseEvent *e) {
    switch (m_currentMode) {
        case Mode::PANZOOM: {
            if (QApplication::keyboardModifiers() == Qt::NoModifier) {
                m_panZoomState.isChanged = true;
                m_panZoomState.panState = BioTracker::Core::CurrentPanState(e->localPos());
                setCursor(Qt::ClosedHandCursor);
            }
            if (e->button() == Qt::LeftButton && e->type() == QEvent::MouseButtonDblClick) {
                fitToWindow();
            }
            break;
        }
        case Mode::INTERACTION: {
            e->accept();
            QPoint p  = unprojectScreenPos(e->pos());
            const QPointF localPos(p);
            QMouseEvent modifiedEvent(e->type(),localPos,e->screenPos(),e->button(),e->buttons(),e->modifiers());
            m_biotracker.mouseEvent(&modifiedEvent);
            break;
        }
        default: {
            assert(false);
            break;
        }
    }
}

void VideoView::mouseReleaseEvent(QMouseEvent *e) {
    switch (m_currentMode) {
        case Mode::PANZOOM: {
            setCursor(Qt::OpenHandCursor);
            m_panZoomState.panState.reset();
            break;
        }
        case Mode::INTERACTION: {
            e->accept();
            QPoint p  = unprojectScreenPos(e->pos());
            const QPointF localPos(p);
            QMouseEvent modifiedEvent(e->type(),localPos,e->screenPos(),e->button(),e->buttons(),e->modifiers());
            m_biotracker.mouseEvent(&modifiedEvent);
            break;
        }
        default: {
            assert(false);
            break;
        }
    }
}

void VideoView::wheelEvent(QWheelEvent *e) {
    switch (m_currentMode) {
        case Mode::PANZOOM: {
            if (e->orientation() == Qt::Vertical) {
                const int numDegrees  = e->delta();
                const float deltaZoom = numDegrees;
                m_panZoomState = BioTracker::Core::ScreenHelper::zoomTo(
                    m_panZoomState,
                    m_texture.getImage().cols, m_texture.getImage().rows,
                    this->width(), this->height(),
                    deltaZoom,
                    e->pos()
                );
                update();
                e->accept();
            }
            break;
        }
        case Mode::INTERACTION: {
            e->accept();
            const QPoint p  = unprojectScreenPos(e->pos());
            const QPointF localPos(p);
            QWheelEvent modifiedEvent(e->pos(),localPos,e->pixelDelta(),e->angleDelta(),e->delta(),e->orientation(),e->buttons(),e->modifiers());
            QCoreApplication::sendEvent(QApplication::activeWindow(), &modifiedEvent);
            m_biotracker.mouseWheelEvent(&modifiedEvent);
            break;
        }
        default: {
            assert(false);
            break;
        }
    }
}

bool VideoView::isZoomed() {
    return m_panZoomState.isChanged;
}

}
}
