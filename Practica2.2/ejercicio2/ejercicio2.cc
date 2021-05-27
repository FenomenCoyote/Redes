#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:

    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    void to_bin(){

        alloc_data(JUGADOR_SIZE);

        memset(_data, 0, JUGADOR_SIZE);

        char* pointer = _data;
        
        memcpy(pointer, name, MAX_NAME);
        pointer += MAX_NAME;

        memcpy(pointer, &x, sizeof(int16_t));
        pointer += sizeof(int16_t);
        
        memcpy(pointer, &y, sizeof(int16_t));
    }

    int from_bin(char* data) {

        alloc_data(JUGADOR_SIZE);

        memcpy(static_cast<void *>(_data), data, JUGADOR_SIZE);

        char* pointer = _data;

        memcpy(name, pointer, MAX_NAME);
        pointer += MAX_NAME;

        memcpy(&x, pointer, sizeof(int16_t));
        pointer += sizeof(int16_t);
        
        memcpy(&y, pointer, sizeof(int16_t));

        return 0;
    }


public:
    static const size_t MAX_NAME = 20;
    static const size_t JUGADOR_SIZE = MAX_NAME + 2*sizeof(int16_t);
    char name[MAX_NAME];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    one_w.to_bin();

    int f = creat("file.txt", 00666);
    write(f, one_w.data(), one_w.size());
    close(f);

    return 0;
}

