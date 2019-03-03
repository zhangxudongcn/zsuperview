#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
class ZBaseScene;
class MainWindow : public QWidget
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);

private:
    void setupMatrix();
    void populateScene();

    ZBaseScene *scene;
    ZBaseScene *top_scene;
    ZBaseScene *tmp_scene;
};

#endif // MAINWINDOW_H
