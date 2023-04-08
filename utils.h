#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

// ESTRUCTURAS DE DATOS
struct Equipo {
    string nombre;
    char grupo;

    int puntos[6];        // Vector dinamico de puntos[fase]
    int fase_final;
};



struct Mundial {
    int cant_equipos;
    int MAXIMO_ITERACIONES;
    //Vectores dinamicos de equipos (ordenados alfabeticamente)
    Equipo* equipos;

    // Vectores dinamicos de equipos (ordenados por puntaje)
    Equipo* equipos_pp;
    Equipo* octavos_pp;
    int agregados_a_octavos;
    Equipo* cuartos_pp;
    int agregados_a_cuartos;
    Equipo* semifinal_pp;
    int agregados_a_semifinal;

    // Ganadores
    Equipo* cuarto;
    Equipo* tercero;
    Equipo* segundo;
    Equipo* primero;

};

struct Partido {        // Estructura no guardada en mem. dinamica, solo para facilitar trabajo
    Equipo* equipo1;
    Equipo* equipo2;

    int goles1;
    int goles2;
    int penales1;
    int penales2;
};

// FUNCIONES DE MEMORIA
int iniciar_equipo_vacio(Equipo* equipo);
int estirar_vector(Mundial* mundial);
void iniciar_equipo(char grupo, string nombre, Equipo* equipo);
int cargar_equipo_en_memoria(string linea, Mundial* mundial);
int recursion_swap(Mundial* mundial, int i);
int comparar_alfabeticamente(string a, string b);
void ordenar_equipos(Mundial* mundial);
void ordenar_equipos_pp(Mundial* mundial);

Mundial* iniciar_mundial(void);

//void formar_grupos(Mundial* mundial);
int cargar_partidos(string linea, Mundial* mundial, string fase);
int fase_a_numero(string fase);

Equipo* buscar_equipo(Mundial* mundial, string nombre);
int busqueda_binaria(Mundial* mundial, string nombre, int n, int cant_iteraciones);

int descargar_mundial(Mundial* mundial);

bool divisor_de_fase(string input);
bool validar_equipo(string linea);
bool validar_partido(string linea);

// FUNCIONES DE STRING
int len_string(string a);
string to_lower(string a);
string to_upper(string a);
string split_string(string a);
bool cmp_string(string a, string b);
string* separar_csv(string a);
int mod (float a);

bool menu(Mundial* mundial);
void menu_listar_equipos(Mundial* mundial);
void menu_podio(Mundial* mundial);
void menu_buscar_equipo(string busqueda, Mundial* mundial);
void menu_puntos(Mundial* mundial);

#endif // UTILS_H_INCLUDED
