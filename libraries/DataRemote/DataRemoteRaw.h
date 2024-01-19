#ifndef DataRemoteRaw_H
#define DataRemoteRaw_H

#include <Arduino.h>

class DataRemoteRaw
{
public:
    DataRemoteRaw();
    ~DataRemoteRaw();

    void dataInput(String modeAC);
    uint16_t* getRawData();

private:
    String _modeAC;
};

#endif
