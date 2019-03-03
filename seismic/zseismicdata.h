#ifndef ZSEISMICDATA_H_
#define ZSEISMICDATA_H_
#include <QVariant>
class ZSeismicDataPrivate;
class ZSeismicData {
public:
    ZSeismicData();
    virtual ~ZSeismicData();
    virtual void traceNum() const = 0;
    virtual void headerLen() const = 0; // header len of byte
    virtual void sampleNum() const = 0;
    virtual QString headerString( int trace_index, const QString &header_name ) const = 0;
    virtual QVariant headerValue( int trace_index, const QString &header_name ) const = 0;
    virtual unsianged char *headerBuffer( int trace_index ) const = 0;
    virtual float *traceData( int trace_index ) const = 0;
    virtual double sampleInterval() const = 0; // ms for time, m for depth
    virtual double timeOfFirstSample() const = 0;
private:
    ZSeismicDataPrivate *_p_data;
};
#endif /* ZSEISMICDATA_H_ */
