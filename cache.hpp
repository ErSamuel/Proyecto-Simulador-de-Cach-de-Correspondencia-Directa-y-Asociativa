#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
using namespace std;

class Cache {
protected:
    long long accesos = 0, aciertos = 0, fallos = 0; // contadores de estadisticas
    int numBloques, tamBloque;//numBloques = cantidad de bloques que posee la cache, tamBloque = tamaño del bloque en palabras

public:
    //Metodo virtual: para implementarse en las clases hijas
    //Constructor
    Cache(int nBloques, int tBloque) :numBloques(nBloques),tamBloque(tBloque){}
    //Destructor
    virtual ~Cache() {}
    
    virtual void accederDireccion(const string &direccionBin) = 0;  //Precimiento abstracta utilizada por las clases hijas
    int getEtiqueta(const string &direccion);   //Retorna la etiqueta de la direccion pasada por parametros

    //Procedimiento para imprimir estadisticas
    virtual void estadisticas() const {
        cout << "Accesos: " << accesos << endl;
        cout << "Aciertos: " << aciertos << endl;
        cout << "Fallos: " << fallos << endl;
        cout << fixed << setprecision(3) << "Tasa de aciertos: " << (accesos ? (100.0 * aciertos / accesos) : 0.0) << "%"<<endl;
        cout << fixed << setprecision(3) << "Tasa de Fallos: " << (accesos ? (100.0 * fallos / accesos) : 0.0) << "%"<<endl;
    }

    //geters
    int getNumBloques(){return numBloques;}
    int getTamBloque(){return tamBloque;}
    //setters
    void setTamBloque(int tBloque){ tamBloque=tBloque;}
};

int Cache::getEtiqueta(const string &direccionBin){
    int direccion = stoi(direccionBin, NULL, 2);    //Convertir direccion binaria -> entero
    return direccion / tamBloque;      //Calcula la etiqueta del bloque de memoria correspondiente
}

// Clase CacheDirecta (Un bloque se relaciona a una unica posicion)
class CacheDirecta : public Cache {
private:
    vector<int> etiquetas;   // vector de bloques cargados en cache (es representado con la etiqueta)

public:
    CacheDirecta(int nBloques, int tBloque) : Cache(nBloques,tBloque), etiquetas(nBloques, -1) {}

    void accederDireccion(const string &direccionBin)override;  //Mapeo de cache de direccionamiento directo
    //getters
    vector<int> getEtiquetas(){return etiquetas;}

    //setters
    void setNunmBloques(int nBloques);
};

void CacheDirecta::setNunmBloques(int nBloques){
    numBloques = nBloques;      //El atributo numBloques toma el valor de nBloques
    etiquetas.assign(nBloques,-1); //Al ser modificado el numero de bloques, el tamaño de la cache tambien se redimensiona
}

void CacheDirecta::accederDireccion(const string &direccionBin) {
    accesos++;

    // Calcular bloque de memoria con desplazamiento
    int etiqueta = getEtiqueta(direccionBin);

    // Indice de cache donde deberea ir el bloque
    int indice = etiqueta % numBloques;

    // Revisar si el bloque ya estaba cargado o si hay fallo
    if (etiquetas[indice] == etiqueta) {
        aciertos++;
    } else {
        fallos++;
        etiquetas[indice] = etiqueta; // Reemplaza el bloque 
    }
}

// Clase Cache Asociativa por n vias
class CacheAsociativa : public Cache {
private:
    int numVias, numConjuntos;      //numVias = numero de vias de la cache, numConjuntos = numero de conjuntos de la cache
    vector<vector<int>> etiquetas;  // etiquetas[via][conjunto] = bloque almacenado
    vector<vector<int>> tiempoLRU;  // matriz paralela a etiquetas, almacena los ultimos accesos a la cache

    void redimensionar(int vias,int nBloques){
        numConjuntos = nBloques / vias;
        etiquetas.assign(vias, vector<int>(numConjuntos, -1));
        tiempoLRU.assign(vias, vector<int>(numConjuntos, 0));
    }

public:
    CacheAsociativa(int vias, int nBloques, int tBloque): numVias(vias), Cache(nBloques,tBloque) {
        redimensionar(numVias,numBloques);
    }

    void accederDireccion(const string &direccionBin)override;
    //getters
    vector<vector<int>> getEtiquetas(){return etiquetas;}   //Retorna la matriz con las etiquetas guardadas hasta ese momento
    vector<vector<int>> getLRU(){return tiempoLRU;}         //Retorna la matriz que contiene los accesos

    //setters
    void setNumVias(int vias){
        numVias = vias;
        redimensionar(numVias,numBloques);
    }
    void setNumBloques(int nBloques){
        numBloques = nBloques;
        redimensionar(numVias,numBloques);
    }
};

void CacheAsociativa::accederDireccion(const string &direccionBin) {
    accesos++;
    int etiqueta = getEtiqueta(direccionBin);      //Calcula la etiqueta del bloque de memoria correspondiente
    int indice = etiqueta % numConjuntos;      //Calcula el indice
    int viaUsada = -1;

    // Buscar si el bloque ya esta cargado en alguna via
    for (int v = 0; v < numVias && viaUsada == -1 ; v++) {
        if (etiquetas[v][indice] == etiqueta)
            viaUsada = v;
    }

    if (viaUsada != -1) {
        aciertos++;
        tiempoLRU[viaUsada][indice] = accesos; // actualizar tiempo de acceso
    } else {
        fallos++;
        // Reemplazar bloque siguiendo politica LRU
        int accesoMasAntiguo = 0, tiempo = tiempoLRU[0][indice];
        for (int v = 1; v < numVias; v++) {
            if (tiempoLRU[v][indice] < tiempo) {
                accesoMasAntiguo = v;
                tiempo = tiempoLRU[v][indice];
            }
        }
        etiquetas[accesoMasAntiguo][indice] = etiqueta; //se sustituye el bloque
        tiempoLRU[accesoMasAntiguo][indice] = accesos;  //se guarda el ultimo acceso
    }
}
