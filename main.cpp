#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

#include "utils.h"

using namespace std;

int main(int argc, char* argv[]){
    /*
        Leer archivos y cargar equipos-partidos a memoria
        Ofrecer menu, while usuario quiera seguir utilizando app
        Alternar entre opciones del menu
        Exit (y descarga de datos de memoria)
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
    string entrada = "equipos.txt";
    ifstream entrada_equipos(entrada);
    if (!entrada_equipos){
        cerr << "ERROR APERTURA file1" << endl;
        descargar_mundial(mundial);
        return 1;
    }

    string linea;
    while (getline(entrada_equipos, linea)){    // Se utiliza string debido a error de compilación
        if(validar_equipo(linea)){
            //Equipo* equipo;
            //iniciar_equipo_vacio(equipo);
            cargar_equipo_en_memoria(to_lower(linea), mundial);
        }
        else{
            cout << "Formato de equipo invalido" << endl;
            entrada_equipos.close();
            descargar_mundial(mundial);
            return 1;
        }
    }
    entrada_equipos.close();
    ordenar_equipos(mundial);     // Ordenar alfabeticamente
    //formar_grupos(mundial);

    mundial->MAXIMO_ITERACIONES = 2*log(mundial->cant_equipos)/(log(2));
    menu_listar_equipos(mundial);
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
        if (validar_partido(linea)){
            if (divisor_de_fase(linea))
                fase = linea;
            else{
                if (cargar_partidos(linea, mundial, fase) == 1){
                    entrada_partidos.close();
                    descargar_mundial(mundial);
                }
            }
        }
        else {
            cout << "Formato de partido invalido" << endl;
            cout << linea << endl;
            entrada_partidos.close();
            descargar_mundial(mundial);
            return 1;
        }
    }
    entrada_partidos.close();
    ordenar_equipos_pp(mundial);


    cout << "EQUIPOS DE OCTAVOS" << endl;
    for (int m = 0; m < mundial->agregados_a_octavos; m++)
        if (mundial->octavos_pp[m].nombre != "")
            cout << "EQUIPO: " << mundial->octavos_pp[m].nombre << " PUNTOS: " << mundial->octavos_pp[m].puntos[1] << endl;

    cout << "EQUIPOS DE CUARTOS" << endl;
    for (int m = 0; m < mundial->agregados_a_cuartos; m++)
        if (mundial->cuartos_pp[m].nombre != "")
            cout << "EQUIPO: " << mundial->cuartos_pp[m].nombre << " PUNTOS: " << mundial->cuartos_pp[m].puntos[1] << endl;

    cout << "EQUIPOS DE SEMIS" << endl;
    for (int m = 0; m < mundial->agregados_a_semifinal; m++)
        if (mundial->semifinal_pp[m].nombre != "")
            cout << "EQUIPO: " << mundial->semifinal_pp[m].nombre << " PUNTOS: " << mundial->semifinal_pp[m].puntos[3] << endl;


    // Ordenar por puntos
    /*
    ordenar_equipos_pp(mundial->equipos_pp,"grupos");
    ordenar_equipos_pp(mundial->octavos_pp,"octavos");
    ordenar_equipos_pp(mundial->cuartos_pp,"cuartos");
    ordenar_equipos_pp(mundial->semifinal_pp,"semifinales");
    */
    int i = 0;
    while(mundial->equipos_pp[i].grupo != '\0'){
        cout << "Puntos Grupos: " << mundial->equipos_pp[i].puntos[0] << " Equipo: " << to_upper(mundial->equipos_pp[i].nombre) << endl;
        i++;
    }

    // COMENTARIO PARA TEST SIN CARGAR PARTIDOS

    // Menu
    cout << "Bienvenido" << endl;
    bool mostrar_menu = true;
    while (mostrar_menu)
        mostrar_menu = menu(mundial);

    descargar_mundial(mundial);

    return 0;
}

