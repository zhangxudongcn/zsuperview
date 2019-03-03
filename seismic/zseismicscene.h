#ifndef ZSEISMICSCENE_H_
#define ZSEISMICSCENE_H_

#include <zbasescene.h>
class ZSeismicScenePrivate;
class ZSeismicScene : public ZBaseScene {
    Q_OBJECT
public:
    ZSeismicScene();
    ~ZSeismicScene();
    void setSeismicData( const ZSeismicData *seismic_data );
private:
    ZSeismicScenePrivate *_p_data;
};
#endif /* ZSEISMICSCENE_H_ */
