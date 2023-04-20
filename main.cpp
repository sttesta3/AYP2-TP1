#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

#include "utils.h"

using namespace std;

int main(int argc, char* argv[]){
    /*
        Leer archivo de equipos, cargar equipos en memoria (vector dinamico).
        Leer archivo de partidos, cargar puntos y fase final en equipos.
        Formar grupos y fases finales
        Ofrecer menu, while usuario quiera seguir utilizando app
        Alternar entre opciones del menu
        Exit (y descarga memoria dinamica)
    */

    if (argc != 1){
        cerr << "Uso: ./"<<argv[0]<<" file1 file2"<<endl;
        return 1;
    }

    Mundial* mundial = iniciar_mundial();
    if (mundial == nullptr){
        cerr << "ERROR AL CREAR MUNDIAL" << endl;
        return 1;
    }

    // Cargar equipos en memoria
    ifstream entrada_equipos("equipos.txt");
    if (!entrada_equipos){
        cerr << "ERROR APERTURA file1" << endl;
        descargar_mundial(mundial);
        return 1;
    }

    string linea;
    while (getline(entrada_equipos, linea)){    // Se utiliza string debido a error de compilación
        linea = to_lower(linea);
        if(validar_equipo(linea)){
            if (cargar_equipo_en_memoria(linea, mundial) == 1){
                entrada_equipos.close();
                descargar_mundial(mundial);
            }
        }
        else{
            cerr << "Formato de equipo invalido" << endl;
            entrada_equipos.close();
            descargar_mundial(mundial);
            return 1;
        }
    }
    entrada_equipos.close();
    ordenar_equipos(mundial);                                               // Ordenar alfabeticamente
    mundial->MAXIMO_ITERACIONES = 2*log(mundial->cant_equipos)/(log(2));    // MAXIMO ITERACIONES(el doble del minimo calculado)

    // Cargar partidos en memoria (asignar puntos y guardar puestos)
    ifstream entrada_partidos("resultados2.csv");
    if (!entrada_partidos){
        cerr << "ERROR APERTURA file2" << endl;
        descargar_mundial(mundial);
        return 1;
    }

    //COMENTARIO PARA TEST SIN CARGAR PARTIDOS
    string fase;
    while (getline(entrada_partidos, linea)){
        linea = to_lower(linea);
        if (divisor_de_fase(linea) == false){
            Partido* partido = validar_partido(mundial, linea);
            if (partido != nullptr)
                cargar_partidos(partido, mundial, fase);
            else {
                cerr << "Formato de partido invalido, linea: '" << linea << "'" << endl;
                entrada_partidos.close();
                descargar_mundial(mundial);
                return 1;
            }
        }
        else
            fase = linea;
    }
    entrada_partidos.close();
    formar_grupos(mundial);     // SE PODRIA ORDENAR GRUPOS ALFABETICAMENTE PARA QUE SE MUESTREN ALFABETICAMENTE, POR FALTA DE TIEMPO NO SE IMPLEMENTA
    ordenar_fases(mundial);

    // Menu
    cout << "Bienvenido" << endl;
    bool mostrar_menu = true;
    while (mostrar_menu)
        mostrar_menu = menu(mundial);

    descargar_mundial(mundial);

    return 0;
}

