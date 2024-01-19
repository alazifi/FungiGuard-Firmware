#ifndef DataRemote_H
#define DataRemote_H

#include <Arduino.h>

class DataRemote
{
public:
    DataRemote();
    ~DataRemote();
    void dataInput(String modeAC);
    const char* getProntoData();
    String uppercaseInput(const String &input);
private:
    String _modeAC;
};

#endif
