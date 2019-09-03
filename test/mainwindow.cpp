#include "chip.h"
#include "mainwindow.h"
#include "view.h"
#include <zbasescene.h>

#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    populateScene();
    qDebug( " child num = %d", scene->children().size());

    View *view = new View("Top left view");
    view->view()->setScene( scene );
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(view);
    setLayout(layout);
    setWindowTitle(tr("Chip Example"));
}

void MainWindow::populateScene()
{
    scene = new ZBaseScene();
    scene->setDoubleBuffering( true );

    QImage image(":/qt4logo.png");

    // Populate scene
    int xx = 0;
    int nitems = 0;
    for (int i = -11000; i < 11000; i += 110) {
        ++xx;
        int yy = 0;
        for (int j = -7000; j < 7000; j += 70) {
            ++yy;
            qreal x = (i + 11000) / 22000.0;
            qreal y = (j + 7000) / 14000.0;

            QColor color(image.pixel(int(image.width() * x), int(image.height() * y)));
            QGraphicsItem *item = new Chip(color, xx, yy);
            item->setPos(QPointF(i, j));
            scene->addItem(item);

            ++nitems;
        }
    }
    qDebug( "x0: %f, y0: %f, width = %f, height = %f", scene->sceneRect().x(), scene->sceneRect().y(), scene->sceneRect().width(), scene->sceneRect().height() );
    scene->setBackgroundBrush( Qt::white );

    //return;
    int item_num = 2;
    QRectF scene_rect = scene->sceneRect();
    top_scene = new ZBaseScene();
    top_scene->setDoubleBuffering(true);
    for ( int i = 0; i < item_num; i++ ) {
        top_scene->addLine( random() % int( scene_rect.width() ) - scene_rect.width()/2, random() % int( scene_rect.height() ) - scene_rect.height()/2, 
                            random() % int( scene_rect.width() )- scene_rect.width()/2, random() % int( scene_rect.height() ) - scene_rect.height()/2, QPen(Qt::red, 0));
    }
    scene->addChild(  top_scene );
    tmp_scene = new ZBaseScene();
    tmp_scene->setDoubleBuffering(true );
    double step = scene_rect.width() / item_num;
    for ( int i = 0; i < item_num; i++ ) {
        //tmp_scene->addLine( random() % int( scene_rect.width() ) - scene_rect.width()/2, random() % int( scene_rect.height() ) - scene_rect.height()/2, 
          //                  random() % int( scene_rect.width() )- scene_rect.width()/2, random() % int( scene_rect.height() ) - scene_rect.height()/2, QPen(Qt::blue, 10));
        
        tmp_scene->addLine( scene_rect.left() + i * step, scene_rect.top(), scene_rect.left() + i * step, scene_rect.bottom(), QPen(Qt::blue, 0));

    }
    top_scene->addChild(  tmp_scene );
}
