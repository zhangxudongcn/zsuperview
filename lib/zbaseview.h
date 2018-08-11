#ifndef ZBASEVIEW_H_
#define ZBASEVIEW_H_
#include <QGraphicsView>
class ZBaseViewPrivate;
class ZBaseScene;
class ZBaseView : public QGraphicsView
{
    Q_OBJECT
public:
    ZBaseView( QWidget *parent = nullptr );
    ~ZBaseView();
    virtual void setScene(QGraphicsScene *scene );
protected:
    void paintEvent( QPaintEvent *event ) override;
    void scrollContentsBy(int dx, int dy) override;

private:
    ZBaseViewPrivate *_p_data;
};
#endif /*ZBASEVIEW_H_*/

