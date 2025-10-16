#include <iostream>
#include "cache.hpp"
using namespace std;

int main() {

    int vias, numBloques, tamBloque;
    cin >> vias >> numBloques >> tamBloque; //lee la configuracion de la cache

    if (vias < 1 || numBloques < 1 || tamBloque < 1)
    {
        cout << "Error! Los valores no pueden ser menores a 1" << endl;
        return 1;
    }
    
    if (vias > numBloques)
    {
        cout << "Error! El numero de vias supera el numero de Bloques" << endl;
        return 1;
    }
    

    string direccionBin;

    if (vias == 1) {
        // Cache directo
        CacheDirecta cache(numBloques,tamBloque);

        // Leer todas las direcciones 
        while (cin >> direccionBin) {
            cache.accederDireccion(direccionBin);
        }

        cache.estadisticas();
    } else {
        // Cache asociativa por vias
        CacheAsociativa cache(vias, numBloques, tamBloque);

        while (cin >> direccionBin) {
            cache.accederDireccion(direccionBin);
        }

        cache.estadisticas();
    }

    return 0;
}
