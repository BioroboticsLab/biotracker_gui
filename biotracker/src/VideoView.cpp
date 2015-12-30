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

    /*QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(sizePolicy);

    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setSamples(94);
    this->setFormat(format);
    */
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

void VideoView::resizeGL(int w, int h) {

    //m_projection.setToIdentity();
    //m_projection.perspective(45.0f, w / float(h), 0.01f, 100.0f);


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
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLdouble posX, posY, posZ;
    QPoint imageCoord;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluUnProject(mouseCoords.x(), viewport[3] - mouseCoords.y(), 0, modelview,
                 projection, viewport, &posX, &posY, &posZ);
    imageCoord.setX(static_cast<int>((m_texture.getImage().cols / 2) - posX *
                                     (m_texture.getImage().cols / 2)));
    imageCoord.setY(static_cast<int>((m_texture.getImage().rows / 2) - posY *
                                     (m_texture.getImage().rows / 2)));

    return imageCoord;
}

QPoint VideoView::projectPicturePos(QPoint imageCoords) {
    //variables required to map picture coordinates to window coordinates
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLdouble posX, posY, posZ;
    QPoint windowCoord;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluProject(imageCoords.x(), imageCoords.y() , 0, modelview, projection,
               viewport, &posX, &posY, &posZ);
    windowCoord.setX(static_cast<int>(posX));
    windowCoord.setY(-(static_cast<int>(posY - viewport[3])));

    return windowCoord;
}

void VideoView::keyPressEvent(QKeyEvent *e) {
    e->accept();
    QKeyEvent event(e->type(), e->key(), e->modifiers(), e->text());
    m_biotracker.keyboardEvent(&event);
}

void VideoView::mouseMoveEvent(QMouseEvent *e) {

    switch (m_currentMode) {
        case Mode::PANZOOM: {

            QPoint stuff = Core::ScreenHelper::screenToImageCoords(
                m_panZoomState,
                m_texture.getImage().cols,
                m_texture.getImage().rows,
                width(),
                height(),
                QPoint(0,0));

            if (m_panZoomState.panState) {
                const QPointF delta = e->localPos() - (*m_panZoomState.panState).lastPos;
                (*m_panZoomState.panState).lastPos = e->localPos();

                m_panZoomState.isChanged = true;
                m_panZoomState.panX -= static_cast<float>(delta.x());
                m_panZoomState.panY -= static_cast<float>(delta.y());
                update();
            } else {
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
    // The maximum zoom is defined such that one image pixel can never become bigger than the widget size. The zoom
    // step size is calculated based on the ratio of widget size and image size and decays exponentially when zooming
    // in.
    //float step = 0.0005f * m_screenPicRatio;
    float step = 1;
    if ((m_panZoomState.zoomFactor / m_screenPicRatio) < 0.f) {
        //step *= 1.f + (m_panZoomState.zoomFactor / m_screenPicRatio);
    }

    switch (m_currentMode) {
        case Mode::PANZOOM: {
            if (e->orientation() == Qt::Vertical) {
                const QPointF pos = e->posF();
                const int numDegrees  = e->delta();
                const float deltaZoom = step * numDegrees;

                const float oldZoomFactor = 1 + m_panZoomState.zoomFactor;

                QPoint imPos = BioTracker::Core::ScreenHelper::screenToImageCoords(
                    m_panZoomState,
                    m_texture.getImage().cols, m_texture.getImage().rows,
                    this->width(), this->height(),
                    e->pos()
                );

                m_panZoomState.zoomFactor -= deltaZoom/1200;
                m_panZoomState.isChanged = true;

                const float delta_zoom = oldZoomFactor - m_panZoomState.zoomFactor;


                float propX = -30 * delta_zoom;
                float propY = 0;

                float zoom = 1 + m_panZoomState.zoomFactor;

                // reset to non-zoomed coordinates
                const float oldPanX = m_panZoomState.panX / oldZoomFactor;
                const float oldPanY = m_panZoomState.panY / oldZoomFactor;

                m_panZoomState.panX = (oldPanX - propX) * zoom;
                m_panZoomState.panY = (oldPanY - propY) * zoom;

                resizeGL(this->width(), this->height());
                update();
                e->accept();
            }
            break;
        }
        case Mode::INTERACTION: {
            e->accept();
            QPoint p  = unprojectScreenPos(e->pos());
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
