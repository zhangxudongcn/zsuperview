#include "zseismicdata.h"

class ZSeismicDataPrivate {
public:
    ZSeismicDataPrivate() {
    }
    ~ZSeismicDataPrivate() {
    }
};
ZSeismicData::ZSeismicData() : _p_data( new ZSeismicDataPrivate )
{
}

ZSeismicData::~ZSeismicData()
{
    delete _p_data;
}
