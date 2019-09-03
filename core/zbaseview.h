#ifndef ZBASEVIEW_H_
#define ZBASEVIEW_H_
#include <QGraphicsView>
class ZBaseScene;
class ZBaseViewPrivate;
class ZBaseView : public QGraphicsView
{
    Q_OBJECT
public:
    ZBaseView( QWidget *parent = nullptr );
    ~ZBaseView();
    
    // make follow base class function to virtual function
    virtual void setScene(QGraphicsScene *scene );
    bool isDoubleBuffering() const;
    virtual void setDoubleBuffering( bool flag );
protected:
    void paintEvent( QPaintEvent *event ) override;
    void scrollContentsBy(int dx, int dy) override;
private:
    ZBaseViewPrivate *_p_data;
};
#endif /*ZBASEVIEW_H_*/

