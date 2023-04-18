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
    string entrada = "equipos.txt";
    ifstream entrada_equipos(entrada);
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
            cout << "Formato de equipo invalido" << endl;
            entrada_equipos.close();
            descargar_mundial(mundial);
            return 1;
        }
    }
    entrada_equipos.close();
    ordenar_equipos(mundial);                                               // Ordenar alfabeticamente
    mundial->MAXIMO_ITERACIONES = 2*log(mundial->cant_equipos)/(log(2));    // MAXIMO ITERACIONES(el doble del minimo calculado)

    /*
    cout << "CANT FASES: " << mundial->cant_fases << endl;
    for (int i = 0; i < mundial->cant_fases; i++)
        cout << "FASE: " << mundial->fases[i].fase << endl;
    */


    /* // DEBUG
    menu_listar_equipos(mundial);
    menu_puntos(mundial);
    */

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

    menu_listar_equipos(mundial);
    formar_grupos(mundial);
    ordenar_fases(mundial);

     /* // DEBUG /*
    for (int i=0;i < mundial->cant_equipos; i++){
        cout << "EQUIPO: " << mundial->equipos[i].nombre << " Puntos Gr: " << mundial->equipos[i].puntos[5] << " FASe FINAL: " << mundial->equipos[i].fase_final << endl;
        cout << "EQUIPO: " << mundial->equipos[i].nombre << " Puntos Oc: " << mundial->equipos[i].puntos[4] << " FASe FINAL: " << mundial->equipos[i].fase_final << endl;
        cout << "EQUIPO: " << mundial->equipos[i].nombre << " Puntos 4t: " << mundial->equipos[i].puntos[3] << " FASe FINAL: " << mundial->equipos[i].fase_final << endl;
        cout << "EQUIPO: " << mundial->equipos[i].nombre << " Puntos Sf: " << mundial->equipos[i].puntos[2] << " FASe FINAL: " << mundial->equipos[i].fase_final << endl;
        cout << "EQUIPO: " << mundial->equipos[i].nombre << " Puntos Tp: " << mundial->equipos[i].puntos[1] << " FASe FINAL: " << mundial->equipos[i].fase_final << endl;
        cout << "EQUIPO: " << mundial->equipos[i].nombre << " Puntos FF: " << mundial->equipos[i].puntos[0] << " FASe FINAL: " << mundial->equipos[i].fase_final << endl;
    }

    cout << "GANADOR " << mundial->fases[0].equipos[0].nombre << endl;
    cout << "PG: " << mundial->fases[0].equipos[0].puntos[5] << endl;
    cout << "Tercer " << mundial->fases[1].equipos[0].nombre << endl;
    cout << "PG: " << mundial->fases[1].equipos[0].puntos[5] << endl;
    cout << "Siete " << mundial->fases[4].equipos[1].nombre << endl;
    cout << "PG: " << mundial->fases[4].equipos[1].puntos[2] << endl;
    */

    // mundial->fases[posicion].equipos[mundial->fases[posicion].cant_equipos].puntos[5];
    for (int i=0; i < 4; i++){
        cout << "GRUPO: " << mundial->fases[i].fase << endl;
        for (int ii = 0; ii < mundial->fases[i].cant_equipos; ii++){
            cout << "EQUIPO: " << mundial->fases[i].equipos[ii].nombre << endl;
            cout << "PUNTOS G: " << mundial->fases[i].equipos[ii].puntos[5] << endl;
            cout << "OCTAVOS : " << mundial->fases[i].equipos[ii].puntos[4] << endl;
            cout << "CUARTOS : " << mundial->fases[i].equipos[ii].puntos[3] << endl;
            cout << "SEMIFINA: " << mundial->fases[i].equipos[ii].puntos[2] << endl;
            cout << "FINAL   : " << mundial->fases[i].equipos[ii].puntos[0] << endl;

            Equipo* encontrado = buscar_equipo(mundial, mundial->fases[i].equipos[ii].nombre);
            cout << "ENCONTRE A: " << encontrado->nombre << " PUNTOS: " << encontrado->puntos[5] <<endl;
        }
    }

    // Menu
    cout << "Bienvenido" << endl;
    bool mostrar_menu = true;
    while (mostrar_menu)
        mostrar_menu = menu(mundial);

    descargar_mundial(mundial);

    return 0;
}

