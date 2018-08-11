#ifndef ZBASESCENE_H_
#define ZBASESCENE_H_

#include <QGraphicsScene>
class ZBaseScenePrivate;
class ZBaseScene : public QGraphicsScene {
    friend class ZBaseView;
    Q_OBJECT
public:
    ZBaseScene();
    ~ZBaseScene();
    ZBaseScene* parentScene() const;
    QList<ZBaseScene*> children() const;
    bool isDoubleBufferEnabled() const;
    void setDoubleBufferEnabled( bool flag );
    bool isDoubleBufferPainted() const;
    ZBaseScene* rootScene();
    void addChild( ZBaseScene *child_scene );
    void removeFromParent();
    void removeChild( ZBaseScene *child_scene);
    void removeChildren();
    QList<ZBaseScene*> renderList() const;
protected:
    bool event(QEvent *event) override;

    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;
    void drawScene( QPainter *painter, const QRect &viewport_exposed_rect, const QRectF &scene_exposed_rect);

    void renderDoubleBuffer( const QPainter &painter, const QRect &viewport_exposed_rect, const QRectF &scene_exposed_rect );
    void renderDoubleBuffer( const QPainter &painter, const QVector<QRect> v_exposed_rects, const QVector<QRectF> &s_exposed_rects );
    void translateDoubleBuffer( int dx, int dy, const QRect &valid_rect );

    ZBaseScene* prevSceneInRenderList() const;
    ZBaseScene* nextSceneInRenderList() const;
    ZBaseScene* topSceneInRenderList() const;
    ZBaseScene* bottomSceneInRenderList() const;

private:
    ZBaseScenePrivate *_p_data;
};
#endif /* ZBASESCENE_H_ */
