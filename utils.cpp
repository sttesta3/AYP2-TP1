#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

#include "utils.h"

using namespace std;

int iniciar_equipo_vacio(Equipo* equipo){
    equipo = new Equipo;
    if (equipo == nullptr){
        cerr << "ERROR AL CREAR NUEVO EQUIPO" << endl;
        return 1;
    }
    equipo->nombre = '\0';
    equipo->grupo = '\0';
    equipo->fase_final = 0;

    for (int i = 0; i < 6; i++)
        equipo->puntos[i] = 0;

    return 0;
}

void iniciar_equipo(char grupo, string nombre, Equipo* equipo){
    equipo->grupo = grupo;
    // DEBUGGING: cout << "GRUPO ASIGNADO" << endl;
    equipo->nombre = nombre;
}

int estirar_vector(Mundial* mundial){
    int n = mundial->cant_equipos;
    Equipo* nuevo = new Equipo [n + 10];  // Alocar Array de Equipos
    if (nuevo == nullptr){
        cout << "Error al agrandar vector" << endl;
        return 1;
    }

    for (int i=0; i < n ; i++)              // ITEM [i] de NUEVO apunta a ITEM [i] de vector viejo
        nuevo[i] = mundial->equipos[i];
    for (int i=n; i < n + 10 ; i++){        // PONER VACIO en ESPACIOS NUEVOS
        iniciar_equipo_vacio(&nuevo[i]);
    }

    delete [] mundial->equipos;             // Borrar Array viejo (no se pierden elementos porque nuevo apunta a ellos)

    mundial->equipos = nuevo;             // Equipos Mundial -> Nuevo vector

    return 0;
}

int cargar_equipo_en_memoria(string linea, Mundial* mundial){
    // La carga es random (como vienen), y luego se ordenará por insertion sort
    if((mundial->cant_equipos)%10 == 0)                                    // PRIMER ITERACION O SIN MAS MEMORIA (estirar 10 equipos mas)
        estirar_vector(mundial);

    int n = mundial->cant_equipos;
    Equipo* actual = &(mundial->equipos[n]);
    actual->grupo = linea[len_string(linea)-1];
    actual->nombre = to_lower(split_string(linea));

    // DEBUGGING: cout << "GUARDADOS LOS VALORES" << endl;
//    iniciar_equipo(linea[len_string(linea)-1],split_string(linea),equipo);  // Ponerle nombre y grupo a la caja
    mundial->cant_equipos += 1;                                               // Un equipo mas

    return 0;
}

int recursion_swap(Mundial* mundial, int i){
    if (i != 0){
        /*  DEBUGGING
        cout << "i: " << i << endl;
        cout << "eq[i]: " << mundial->equipos[i].nombre << endl;
        cout << "eq[i-1]: " << mundial->equipos[i - 1].nombre << endl;
        cout << "res: " << comparar_alfabeticamente( mundial->equipos[i].nombre, mundial->equipos[i -1].nombre) << endl;
        */
        if (comparar_alfabeticamente( mundial->equipos[i].nombre, mundial->equipos[i -1].nombre) == -1) {
            // cout << "SWAP" << endl;

            Equipo* swap_equipo = new Equipo;                // equipo[i] apunta a direcc de equipos
            *swap_equipo = mundial->equipos[i];
            mundial->equipos[i] = mundial->equipos[i - 1];
            mundial->equipos[i-1] = *swap_equipo;

            delete swap_equipo;
            recursion_swap(mundial,i-1);
        }
    }

    return 0;
}

int recursion_swap_pp(Mundial* mundial, int i){
    if (i != 0){
        if ( mundial->equipos_pp[i].puntos[0] < mundial->equipos_pp[i -1].puntos[0]) {
            // cout << "SWAP" << endl;

            Equipo* swap_equipo_pp = new Equipo;                // equipo[i] apunta a direcc de equipos
            *swap_equipo_pp = mundial->equipos_pp[i];
            mundial->equipos_pp[i] = mundial->equipos_pp[i - 1];
            mundial->equipos_pp[i-1] = *swap_equipo_pp;

            delete swap_equipo_pp;
            recursion_swap_pp(mundial,i-1);
        }
    }

    return 0;
}

int comparar_alfabeticamente(string a, string b){
    int resultado = 0;
    int i = 0;

    int largo_a = len_string(a);
    int largo_b = len_string(b);

    int ac = int(a[i]);
    int bc = int(b[i]);

    while (ac == bc && ((i < largo_a)&&(i < largo_b))){
        i++;
        ac = int(a[i]);
        bc = int(b[i]);
    }

    if (ac > bc)
        return 1;
    else if (ac < bc)
        return -1;
    else
        return 0;
}

void ordenar_equipos(Mundial* mundial){
    int i = 0;
    while( (mundial->equipos[i]).grupo != '\0'){
        recursion_swap( mundial , i);
        i++;
    }
}

void ordenar_equipos_pp(Mundial* mundial){
    // COPIAMOS EQUIPOS TAL CUAL ESTA
    mundial->equipos_pp = new Equipo [mundial->cant_equipos];
    for (int i = 0; i < mundial->cant_equipos; i++)
        mundial->equipos_pp[i] = mundial->equipos[i];

    // ORDENAMOS POR NUMERO (por insercion)
    int i = 0;
    while ((mundial->equipos_pp[i]).grupo != '\0'){
        recursion_swap_pp( mundial , i);
        i++;
    }
}

bool divisor_de_fase(string input){
    return ( cmp_string(input,"grupos") || cmp_string(input,"octavos") || cmp_string(input,"cuartos") || cmp_string(input,"semifinales") || cmp_string(input,"final") || cmp_string(input,"tercer puesto"));
}

int cargar_partidos(string linea, Mundial* mundial, string fase){
// PROCESAMIENTO DEL TEXTO
    // DEBUGGING cout << "NUEVA ITERACION" << endl;
    // menu_listar_equipos(mundial);    DEBUGGING
    Partido* partido = new Partido;

    int i = 0;
    int cant_palabras = 1;
    while (linea[i] != '\0'){
        if (linea[i] == ',')
            cant_palabras++;
        i++;
    }

    string* lista = new string [cant_palabras];

    i = 0;
    int comas_contadas = 0;
    int largo_acumulado = 0;
    while (linea[i] != '\0'){
        if (linea[i] == ','){
            // cout << "COMA ENCONTRADA" << endl;
            lista[comas_contadas] += '\0';
            comas_contadas++;
        }
        else
            lista[comas_contadas] += tolower(linea[i]);
        i++;
    }

    //string* lista = separar_csv(linea);   DEBUG
    // cout << "POR BUSCAR EQUIPOS " << lista[0] << "Y " << lista[2] << endl;
    Equipo* encontrado1;
    Equipo* encontrado2;
    if (cmp_string(fase,"grupos")){
        encontrado1 = buscar_equipo(mundial, lista[0]);
        encontrado2 = buscar_equipo(mundial, lista[2]);
    }
    else {
        encontrado1 = buscar_equipo(mundial, lista[0]);
        encontrado2 = buscar_equipo(mundial, lista[3]);
    }

    /*  DEBUGGING
    if (encontrado1 != nullptr)
        cout << "ENCONTRADO 1 " << encontrado1->nombre << endl;
    else
        cout << "NO ENCONTRE A 1" << endl;


        if (encontrado1 != nullptr)
        cout << "ENCONTRADO 2 " << encontrado2->nombre << endl;
    else
        cout << "NO ENCONTRE A 2" << endl;

    */
    if ((encontrado1 != nullptr) && (encontrado2 != nullptr)){
        partido->equipo1 = encontrado1;
        partido->equipo2 = encontrado2;

        if (cmp_string(fase,"grupos")){
            partido->goles1 = int(lista[1][0]);
            partido->penales1 = -1;
            partido->goles2 = int(lista[3][0]);
            partido->penales2 = -1;
        }
        else {
            partido->goles1 = int(lista[1][0]);
            partido->penales1 = int(lista[2][0]);
            partido->goles2 = int(lista[4][0]);
            partido->penales2 = int(lista[5][0]);
        }
    }
    else {
        cerr << "ERROR: Equipo no encontrado" << endl;
        return 1;
    }


// EDITAR LOS EQUIPOS
//    Equipo* equipo1 = buscar_equipo(mundial,partido->equipo1->nombre);
//    Equipo* equipo2 = buscar_equipo(mundial,partido->equipo2->nombre);

// MODIFICAR ETAPA FINAL
    int fase_numerica = fase_a_numero(fase);
//    cout << "FASE NUMERICA: " << fase_numerica << endl;
// ASIGNACION DE PUNTOS

    int goles = partido->goles1 - partido->goles2;
    int penales = partido->penales1 - partido->penales2;
    int ganador = goles + penales;

    // DEBUGGING
    //cout << "GOLES: " << goles << endl;
    //cout << "PENALES: " << penales << endl;
    //cout << "GANADOR: " << ganador << endl;
    //cout << "PRE PRINT PUNTOS 1" << endl;
    //for (int m = 0; m < 6; m++)
    //     cout << partido->equipo1->puntos[m] << endl;



    // SUMA DE PUNTOS
    if (goles < 0)
        partido->equipo2->puntos[fase_numerica] += 3;
    else if (goles > 0)
        partido->equipo1->puntos[fase_numerica] += 3;
    else {
        if (penales < 0){
            partido->equipo2->puntos[fase_numerica] += 2;
            partido->equipo1->puntos[fase_numerica] += 1;
        }
        else if (penales > 0){
            partido->equipo2->puntos[fase_numerica] += 1;
            partido->equipo1->puntos[fase_numerica] += 2;
        }
        else {
            partido->equipo2->puntos[fase_numerica] += 1;
            partido->equipo1->puntos[fase_numerica] += 1;
        }
    }

// PREMIOS
    partido->equipo1->fase_final = fase_numerica;
    partido->equipo2->fase_final = fase_numerica;

    switch(fase_numerica){
        case 5:
        {
            if (ganador < 0){
                mundial->primero = partido->equipo2;
                mundial->segundo = partido->equipo1;
            }
            else if (ganador > 0){
                mundial->primero = partido->equipo1;
                mundial->segundo = partido->equipo2;

            }
        }; break;
        case 4:
        {
            if (ganador < 0){
                mundial->tercero = partido->equipo2;
                mundial->cuarto = partido->equipo1;
            }
            else if (ganador > 0){
                mundial->tercero = partido->equipo1;
                mundial->cuarto = partido->equipo2;

            }
        }; break;
        case 3:
        {
            mundial->semifinal_pp[mundial->agregados_a_semifinal] = *partido->equipo1;
            mundial->agregados_a_semifinal += 1;
            mundial->semifinal_pp[mundial->agregados_a_semifinal] = *partido->equipo2;
            mundial->agregados_a_semifinal += 1;
        }; break;
        case 2:
        {
            mundial->cuartos_pp[mundial->agregados_a_cuartos] = *partido->equipo1;
            mundial->agregados_a_cuartos += 1;
            mundial->cuartos_pp[mundial->agregados_a_cuartos] = *partido->equipo2;
            mundial->agregados_a_cuartos += 1;
        }; break;
        case 1:
        {
            mundial->octavos_pp[mundial->agregados_a_octavos] = *partido->equipo1;
            mundial->agregados_a_octavos += 1;
            mundial->octavos_pp[mundial->agregados_a_octavos] = *partido->equipo2;
            mundial->agregados_a_octavos += 1;
        }; break;
    }

    delete [] lista;
    delete partido;

    return 0;
}

Equipo* buscar_equipo(Mundial* mundial, string nombre){
    int resultado = busqueda_binaria(mundial, nombre, round((mundial->cant_equipos)/2), 0);

    if (resultado == -1)
        return nullptr;
    else
        return &(mundial->equipos[resultado]);
}


int busqueda_binaria(Mundial* mundial, string nombre, int n, int cant_iteraciones){
    // cout << "NOMBRE" << nombre << endl;
    // cout << "ENCONTRE" << mundial->equipos[n].nombre << endl;

    int a = comparar_alfabeticamente(mundial->equipos[n].nombre,to_lower(nombre));

    cant_iteraciones += 1;
    float desplazamiento = mundial->cant_equipos/pow(2, cant_iteraciones + 1);

    if (desplazamiento < 0.5)
        desplazamiento = 1;
    else
        desplazamiento = round(desplazamiento);

    int retraso = n - desplazamiento;
    int adelanto = n + desplazamiento;
    if (retraso < 0)
        retraso = 0;

    if (adelanto > mundial->cant_equipos -1)
        adelanto = mundial->cant_equipos - 1;

    if (cant_iteraciones - 1 > mundial->MAXIMO_ITERACIONES)
        return -1;
    else if (a == 1)
        return busqueda_binaria(mundial, nombre, retraso, cant_iteraciones);
    else if (a == -1)
        return busqueda_binaria(mundial, nombre, adelanto, cant_iteraciones);
    else if (a == 0)
        return n;
}

int fase_a_numero(string fase){
    int resultado;
    if (cmp_string(fase,"grupos"))
        resultado = 0;
    else if (cmp_string(fase,"octavos"))
        resultado = 1;
    else if (cmp_string(fase,"cuartos"))
        resultado = 2;
    else if (cmp_string(fase,"semifinales"))
        resultado = 3;
    else if (cmp_string(fase,"tercer puesto"))
        resultado = 4;
    else if (cmp_string(fase,"final"))
        resultado = 5;

    return resultado;
}

/*
void formar_grupos(Mundial* mundial){
    if((mundial->cant_grupos)%10 == 0){                         // PRIMER ITERACION O SIN MAS MEMORIA (estirar 10 equipos mas)
        estirar_vector_grupos(mundial);

    int i = 0;
    bool equipo_existente = false;
    while (mundial->equipos[i].grupo != '\0'){              // Itera todos los equipos
        for (int ii = 0; ii < mundial->cant_grupos; ii++){
            if (mundial->equipos[i].grupo == mundial->grupos[ii][0]->grupo){

            }
        }

        if (equipo_existente == false){
            Equipo* nuevo_grupo = new Equipo[10];
            nuevo_grupo[0] = mundial->equipos[i];
            mundial->cant_grupos++;
        }
    }
}

int estirar_vector_grupos(Mundial* mundial){
    int n = mundial->cant_grupos;
    Equipo* nuevo = new Equipo [n + 10];  // Alocar Array de Equipos
    if (nuevo == nullptr){
        cout << "Error al agrandar vector" << endl;
        return 1;
    }

    for (int i=0; i < n ; i++)              // ITEM [i] de NUEVO apunta a ITEM [i] de vector viejo
        nuevo[i] = mundial->grupos[i];
    for (int i=n; i < n + 10 ; i++)        // PONER VACIO en ESPACIOS NUEVOS
        nuevo[i] = nullptr;


    delete [] mundial->grupos;             // Borrar Array viejo (no se pierden elementos porque nuevo apunta a ellos)

    mundial->grupos = nuevo;             // Equipos Mundial -> Nuevo vector

    return 0;
}
*/
Mundial* iniciar_mundial(void){
    Mundial* mundial = new Mundial;

    mundial->cant_equipos = 0;
    mundial->MAXIMO_ITERACIONES;

    mundial->equipos = nullptr;
    mundial->equipos_pp = nullptr;

    mundial->cuarto = nullptr;
    mundial->tercero = nullptr;
    mundial->segundo = nullptr;
    mundial->primero = nullptr;

    mundial->octavos_pp = new Equipo [16];
    mundial->agregados_a_octavos = 0;
    mundial->cuartos_pp = new Equipo [8];
    mundial->agregados_a_cuartos = 0;
    mundial->semifinal_pp = new Equipo [4];
    mundial->agregados_a_semifinal = 0;

    for (int i = 0; i < 4; i++)
        mundial->semifinal_pp[i].nombre = '\0';
    for (int i = 0; i < 8; i++)
        mundial->cuartos_pp[i].nombre = '\0';
    for (int i = 0; i < 16; i++)
        mundial->octavos_pp[i].nombre = '\0';

    return mundial;
}

int descargar_mundial(Mundial* mundial){
    delete [] mundial->equipos_pp;
    delete [] mundial->octavos_pp;
    delete [] mundial->cuartos_pp;
    delete [] mundial->semifinal_pp;

    int i = 0;
    if (mundial->equipos != nullptr){
        while (mundial->equipos[i].nombre[0] != '\0'){     // BORRAR ELEMNTOS
            delete &(mundial->equipos[i]);
            i++;
        }
    }


    if (mundial->equipos != nullptr)            //BORRAR VECTOR
        delete [] mundial->equipos;

    delete mundial;                             // BORRAR MUNDIAL
    return 0;
}

bool validar_equipo(string linea){
    return true;
}

bool validar_partido(string linea){
    return true;
}

int suma_puntos(Equipo* equipo){
    int suma = 0;
    for (int i = 0; i < 6; i++)
        suma += equipo->puntos[i];

    return suma;
}


// FUNCIONES DE STRING
int len_string(string a){
    int i = 0;
    while (a[i] != '\0')
        i += 1;
    return i;
}

string split_string(string a){
    string output;
    int largo = len_string(a);

    for (int i = 0; i < largo - 2; i++)
        output += a[i];
    a[largo - 2] = '\0';

    return output;
}

bool cmp_string(string a, string b){
    // Devuelve True si son iguales, falso si no
    bool resultado = false;
    int largo_a = len_string(a);
    int largo_b = len_string(b);

    if (largo_a == largo_b){
        int i = 0;
        while ((a[i] == b[i]) && i < largo_a)
            i++;

        if (i == largo_a)
            resultado = true;
    }

    return resultado;
}

string to_lower(string a){
    int largo = len_string(a);

    for (int i=0; i < largo;i++)
        a[i] = tolower(a[i]);

    return a;
}

string to_upper(string a){
    string output = "";
    int largo = len_string(a);

    if (largo > 0){
        output += char(toupper(a[0]));

        for (int i=1; i < largo;i++){
            if (a[i] == '_'){
                output += ' ';
                output += char(toupper(a[i+1]));
                i += 1;
            }
            else
                output += a[i];
        }

        return output;
    }
    else
        return a;
}

int mod (float a){
    if (a < 0)
        a *= -1;

    return a;
}

// FUNCIONES DE PROGRAMA PRINCIPAL
bool menu(Mundial* mundial){
    bool mostrar_menu = true;

    cout << "Opciones (ingresar numero de opcion)" << endl;
    cout << "1. Listar equipos" << endl;
    cout << "2. Mostrar los equipos en primer segundo y tercer lugar" << endl;
    cout << "3. Buscar equipo por nombre" << endl;
    cout << "4. Mostrar por fase los países ordenados por puntaje" << endl;
    cout << "5. Salir" << endl;

    char input; cin >> input;
    if (int(input) < 49 || int(input) > 57)
        input = '0';

    switch (int(input)%48){
    case 1:
        {
            menu_listar_equipos(mundial); break;
        }
    case 2:
        {
            menu_podio(mundial); break;
        }
    case 3:
        {
            cout << "Ingrese nombre del equipo:" << endl; string busqueda; cin >> busqueda; menu_buscar_equipo(busqueda, mundial); break;
        }
    case 4:
        {
            menu_puntos(mundial); break;
        }
    case 5:
        {
            cout << "¡Hasta la proxima!" << endl; mostrar_menu = false; break;
        }
    default:
        {
            cout << "Opción invalida, favor reingresar" << endl; break;
        }
    }
    return mostrar_menu;
}

void menu_listar_equipos(Mundial* mundial){
    // ASUMIMOS QUE FUE ORDENADO ALFABETICAMENTE
    int i = 0;
    while(mundial->equipos[i].grupo != '\0'){
//        cout << "Equipo: " << to_upper(mundial->equipos[i].nombre) << "Grupo: " << char(toupper(mundial->equipos[i].grupo)) << endl;
        cout << " Grupo: " << char(toupper(mundial->equipos[i].grupo)) << " Equipo: " << to_upper(mundial->equipos[i].nombre) << endl;
        i++;
    }
}


void menu_podio(Mundial* mundial){
    cout << "1er:" << to_upper(mundial->primero->nombre) << " Total puntos:" << suma_puntos(mundial->primero) << endl;
    cout << "2do:" << to_upper(mundial->segundo->nombre) << " Total puntos:" << suma_puntos(mundial->segundo) << endl;
    cout << "3ro:" << to_upper(mundial->tercero->nombre) << " Total puntos:" << suma_puntos(mundial->tercero) << endl;
    cout << "4to:" << to_upper(mundial->cuarto->nombre) << " Total puntos:" << suma_puntos(mundial->cuarto) << endl;
}

void menu_buscar_equipo(string busqueda, Mundial* mundial){
    // Asumiendo que esta ordenado alfabeticamente

    Equipo* resultado = buscar_equipo(mundial, busqueda);

    if (resultado == nullptr)
        cout << "No hay coincidencias" << endl;
    else {
        cout << "Equipo:" << to_upper(resultado->nombre) << endl;
        cout << "Grupo:" << char(toupper(resultado->grupo)) << endl;

        switch(resultado->fase_final){
            case 0:
                cout << "Etapa final: Grupos" << endl; break;
            case 1:
                cout << "Etapa final: Octavos" << endl; break;
            case 2:
                cout << "Etapa final: Cuartos" << endl; break;
            case 3:
                cout << "Etapa final: Semifinales" << endl; break;
            case 4:
                cout << "Etapa final: Tercer puesto" << endl; break;
            case 5:
                cout << "Etapa final: Final" << endl; break;
        }
        if (resultado->fase_final == 4){
            if (cmp_string(resultado->nombre,mundial->cuarto->nombre))
                cout << "Cuarto" << endl;
            else if (cmp_string(resultado->nombre,mundial->tercero->nombre))
                cout << "Tercero" << endl;
        }
        else if (resultado->fase_final == 5){
            if (cmp_string(resultado->nombre,mundial->segundo->nombre))
                cout << "Segundo" << endl;
            else if (cmp_string(resultado->nombre,mundial->primero->nombre))
                cout << "Primero" << endl;
            }
    }
}

void menu_puntos(Mundial* mundial){
    int i = 0;
    while(mundial->equipos[i].nombre[0] != '\0'){
        cout << "Equipo: " << mundial->equipos[i].nombre << "Puntos: " << mundial->equipos[i].puntos[0] << endl;
        i++;
    }

}

