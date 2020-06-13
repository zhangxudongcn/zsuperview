#include <zbasescene.h>
#include <QList>
#include <QImage>
#include <QDesktopWidget>
#include <QApplication>
#include <QPainter>
#include <QScreen>
#include <omp.h>

class ZBaseScenePrivate {
public:
    ZBaseScenePrivate() : _parent_scene( nullptr ), _is_visible( true ), 
        _is_double_buffering( false ), _double_buffer_render_flag(false ),
        _double_buffer(nullptr), _is_changed(false)
    {
        setDoubleBuffer();
    }
    ~ZBaseScenePrivate() {
        setDoubleBuffer();
    }
    void eraseDoubleBuffer( const QRect &rect )
    {
        QRect r = rect & _double_buffer->rect();
        if ( r.isEmpty() == false ) {
            // erase buffer
            for (int dst_y_index = r.top(); dst_y_index <= r.bottom(); dst_y_index++) {
                memset(_double_buffer->scanLine(dst_y_index) + r.left() * sizeof(QRgb), 0, sizeof(QRgb) * r.width());
            }
        }
    }
    void setDoubleBuffer()
    {
        if ( _is_double_buffering ) {
            int width = 0;
            int height = 0;
            for ( auto screen : qApp->primaryScreen()->virtualSiblings() ) {
                width += screen->size().width();
                height += screen->size().height();
            }
            _double_buffer = new QImage(width, height, QImage::Format_ARGB32_Premultiplied);
            _double_buffer->fill(0);
            _double_buffer_render_flag = true;
        } else {
            delete _double_buffer;
            _double_buffer = nullptr;
            _is_double_buffering = false;
            _double_buffer_render_flag = false;
        }
    }

    ZBaseScene *_parent_scene;
    bool _is_visible;
    QList<ZBaseScene*> _children;
    bool _is_double_buffering;
    bool _double_buffer_render_flag; // double buffer render flag, true: need render in renderDoubleBuffer
    QImage *_double_buffer;
    bool _is_changed;
    QTransform _transform;
    QPointF _scene_point;
    QRect _viewport_rect;
};

ZBaseScene::ZBaseScene() : QGraphicsScene( 0 ), _p_data( new ZBaseScenePrivate() )
{
    connect( this, &QGraphicsScene::changed, this, &ZBaseScene::selfChangedSlot );
    _p_data->setDoubleBuffer();
}

ZBaseScene::~ZBaseScene()
{
    removeFromParent();
    for ( auto &child : children() ) {
        delete child;
    }
    delete _p_data;
}

ZBaseScene* ZBaseScene::parentScene() const
{
    return _p_data->_parent_scene;
}

bool ZBaseScene::isSceneVisible() const
{
    return _p_data->_is_visible;
}

void ZBaseScene::setSceneVisible( bool flag )
{
    if ( flag != isSceneVisible() ) {
        _p_data->_is_visible = flag;
        if ( _p_data->_is_visible == false ) {
            _p_data->_double_buffer->fill(0); 
        }
        emit rootScene()->changed(QList<QRectF>({ sceneRect()}));
    }
}

QList<ZBaseScene*> ZBaseScene::children() const
{
    return _p_data->_children;
}

ZBaseScene* ZBaseScene::rootScene()
{
    ZBaseScene *root_scene = this;
    while ( root_scene->parentScene() != nullptr ) {
        root_scene = root_scene->parentScene();
    }
    return root_scene;
}

void ZBaseScene::addChild( ZBaseScene *child_scene )
{
    if ( child_scene != nullptr && child_scene != this ) {
        child_scene->removeFromParent();
        child_scene->setSceneRect(sceneRect()); // must call setSceneRect here, because parent scene is nullptr here
        child_scene->_p_data->_parent_scene = this;
        _p_data->_children.append( child_scene );
        // emit signal
        if ( child_scene->isSceneVisible() ) {
            emit rootScene()->changed(QList<QRectF>({ sceneRect()}));
        }
    }
}

void ZBaseScene::removeFromParent()
{
    if ( parentScene() ) {
        parentScene()->_p_data->_children.removeOne( this );
        _p_data->_parent_scene = nullptr;
        if ( isSceneVisible() ) {
            emit rootScene()->changed(QList<QRectF>({ sceneRect()}));
        }
    }
}

void ZBaseScene::removeChild( ZBaseScene *child_scene )
{
    if ( child_scene->parentScene() == this ) {
        child_scene->removeFromParent();
    }
}

void ZBaseScene::removeChildren()
{
    for ( auto &scene : children() ) {
        scene->removeFromParent();
    }
}

void ZBaseScene::swapChild( ZBaseScene *child_1, ZBaseScene *child_2 )
{
    if ( child_1->parentScene() == this && child_2->parentScene() == this && child_1 != child_2 ) {
        int child_1_index = _p_data->_children.indexOf( child_1 );
        int child_2_index = _p_data->_children.indexOf( child_2 );
        _p_data->_children.swapItemsAt( child_1_index, child_2_index );
        if ( child_1->isSceneVisible() || child_2->isSceneVisible() ) {
            emit rootScene()->changed(QList<QRectF>({ sceneRect()}));
        }
    }
}

void ZBaseScene::moveChildUp( ZBaseScene *child_scene )
{
    if ( child_scene->parentScene() == this ) {
        int child_index = _p_data->_children.indexOf(child_scene );
        if ( child_index + 1 < _p_data->_children.count() ) {
            swapChild(child_scene, _p_data->_children[child_index+1] );
        }
    }
}

void ZBaseScene::moveChildDown( ZBaseScene *child_scene )
{
    if ( child_scene->parentScene() == this ) {
        int child_index = _p_data->_children.indexOf(child_scene );
        if ( child_index - 1 >= 0) {
            swapChild(child_scene, _p_data->_children[child_index-1] );
        }
    }
}

void ZBaseScene::moveChildTop( ZBaseScene *child_scene )
{
    if ( child_scene->parentScene() == this ) {
        if ( child_scene != _p_data->_children.last() ) {
            int child_index = _p_data->_children.indexOf(child_scene);
            _p_data->_children.takeAt(child_index );
            _p_data->_children.push_back(child_scene );
            if ( child_scene->isSceneVisible() ) {
                emit rootScene()->changed(QList<QRectF>({ sceneRect()}));
            }
        }
    }
}

void ZBaseScene::moveChildBottom( ZBaseScene *child_scene )
{
    if ( child_scene->parentScene() == this ) {
        if ( child_scene != _p_data->_children.first() ) {
            int child_index = _p_data->_children.indexOf(child_scene);
            _p_data->_children.takeAt(child_index );
            _p_data->_children.push_front(child_scene );
            if ( child_scene->isSceneVisible() ) {
                emit rootScene()->changed(QList<QRectF>({ sceneRect()}));
            }
        }
    }
}


bool ZBaseScene::isDoubleBuffering() const
{
    return _p_data->_is_double_buffering;
}

void ZBaseScene::setDoubleBuffering( bool flag )
{
    if ( flag != isDoubleBuffering() ) {
        _p_data->_is_double_buffering = flag;
        _p_data->setDoubleBuffer();
        //render double buffer
    }
}

// make follow base class function to virtual function
QList<ZBaseScene*> ZBaseScene::renderList() const
{
    QList<ZBaseScene*> render_list;
    for ( auto &child : children() ) {
        render_list += child->renderList();
    }
    render_list.append( const_cast<ZBaseScene*>(this) );
    return render_list;
}

void ZBaseScene::setSceneRect(const QRectF &rect)
{
    if( parentScene() == nullptr ){
        QList<ZBaseScene*>  render_list = renderList();
        for ( auto &scene : render_list ) {
            scene->QGraphicsScene::setSceneRect( rect );
        }
    } else {
        qDebug( "You cann't set scene rect in child scene" );
    }
}

void ZBaseScene::render(QPainter *painter, const QRectF &target, const QRectF &source, Qt::AspectRatioMode aspectRatioMode)
{
    QGraphicsScene::render(painter,target,source,aspectRatioMode);
}

void ZBaseScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    if ( this == rootScene() && rootScene()->isSceneVisible()) {
        QGraphicsScene::drawBackground(painter,rect);
    }
    // draw nothing
}

void ZBaseScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    if ( this == rootScene() && rootScene()->isSceneVisible() ) {
        QGraphicsScene::drawForeground(painter,rect);
    }
}

void ZBaseScene::drawScene( QPainter *painter, const QRect &viewport_exposed_rect, const QRectF & /*scene_exposed_rect */)
{
    if ( isSceneVisible() ) {
        painter->drawImage( viewport_exposed_rect, *_p_data->_double_buffer, viewport_exposed_rect, Qt::NoFormatConversion );
    }
}

bool ZBaseScene::event(QEvent *event)
{
    if ( this == rootScene() ) {
        QList<ZBaseScene*> render_list = renderList();
        render_list.removeLast(); // remove root scene for avoiding loop forever
        for ( auto &layer : render_list ) {
            if( layer->event( event ) == true ) {
                return true;
            }
        }
        return rootScene()->QGraphicsScene::event( event );
    } else {
        return QGraphicsScene::event( event );
    }
    return false;
}

void ZBaseScene::renderDoubleBuffer( const QPainter &painter, 
                                     const QList<QRect> &viewport_exposed_rects, 
                                     const QList<QRectF> &scene_exposed_rects,
                                     const QTransform &transform, 
                                     const QPointF &scene_point, const QRect &viewport_rect )
{
    if ( _p_data->_is_changed || _p_data->_transform != transform || 
         _p_data->_scene_point != scene_point || 
         _p_data->_viewport_rect.contains( viewport_rect ) == false  )  {
        QPainter buffer_painter(_p_data->_double_buffer);
        buffer_painter.setRenderHints(buffer_painter.renderHints(), false);
        buffer_painter.setRenderHints(painter.renderHints(), true);

        for (int r_index = 0; r_index < viewport_exposed_rects.size(); r_index++) {
            QRect v_rect = viewport_exposed_rects[r_index];
            QRectF s_rect = scene_exposed_rects[r_index];

            // erase buffer
            _p_data->eraseDoubleBuffer( v_rect );

            // render scene
            render(&buffer_painter, v_rect, s_rect);
        }
        _p_data->_is_changed = false;
        _p_data->_transform = transform;
        _p_data->_scene_point = scene_point;
        _p_data->_viewport_rect = viewport_rect;
    }
}

void ZBaseScene::translateDoubleBuffer( int dx, int dy, const QRect &viewport_rect )
{
    if ( abs(dx) < viewport_rect.width() && abs(dy) < viewport_rect.height()) {
        int x = dx < 0 ? -dx : 0;
        int y = dy < 0 ? -dy : 0;
        int width = viewport_rect.width() - abs( dx );
        int height = viewport_rect.height() - abs( dy );
        QImage img = _p_data->_double_buffer->copy( x, y, width, height );
        // erase double buffer, should optimize following code
        _p_data->_double_buffer->fill(0);
        QPainter painter(_p_data->_double_buffer);
        painter.drawImage(QPoint(dx < 0 ? 0 : dx, dy < 0 ? 0 : dy ), img, img.rect() );
    }
}

void ZBaseScene::selfChangedSlot( const QList<QRectF> &region )
{
    _p_data->_is_changed = true;
    if ( this != rootScene() ) {
        emit rootScene()->changed(region);
    }
}

bool ZBaseScene::doubleBufferRenderFlag() const
{
    return _p_data->_double_buffer_render_flag;
}

