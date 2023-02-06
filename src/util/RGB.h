#ifndef SRC_UTIL_RGB_H_
#define SRC_UTIL_RGB_H_

#include <WiFiNINA.h>

#define RED     26
#define GREEN   25
#define BLUE    27

class RGB {
public:
    void begin();
    void clear();

    void redOn();
    void greenOn();
    void blueOn();

    void redOff();
    void greenOff();
    void blueOff();
};


#endif /* SRC_UTIL_RGB_H_ */
