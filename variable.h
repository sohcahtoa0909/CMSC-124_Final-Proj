#ifndef VARIABLE_H
#define VARIABLE_H

struct Variable {
    enum Type { INT_TYPE, FLOAT_TYPE };
    Type type;
    union {
        int intValue;
        float floatValue;
    };
};

#endif
