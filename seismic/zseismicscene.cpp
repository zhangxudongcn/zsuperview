#include "zseismicscene.h"
#include "zseismicdata.h"
class ZSeismicScenePrivate {
public:
    ZSeismicScenePrivate() {
    }
    ~ZSeismicScenePrivate() {
    }
    ZSeismicData *_seismic_data;
};

ZSeismicScene::ZSeismicScene() : _p_data( new ZSeismicScenePrivate() )
{
}

ZSeismicScene::~ZSeismicScene()
{
    delete _p_data;
}

void ZSeismicScene::setSeismicData( const ZSeismicData *seismic_data )
{
    if ( _p_data->_seismic_data != nullptr && ) {
    }
    _p_data->_seismic_data = seismic_data;
}
