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
    equipo->fase_final = 5;

    for (int i = 0; i < 6; i++)
        equipo->puntos[i] = 0;

    return 0;
}

int iniciar_fase_vacia(Fase* fase){
    fase = new Fase;
    if (fase == nullptr){
        cerr << "ERROR AL CREAR NUEVA FASE" << endl;
        return 1;
    }
    /*
    fase->fase = '\0';
    fase->cant_equipos = 0;
    fase->equipos = nullptr;
    */
    return 0;
}

void iniciar_equipo(char grupo, string nombre, Equipo* equipo){
    equipo->grupo = grupo;
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

int estirar_vector_fases(Mundial* mundial){
    int n = mundial->cant_fases;

    Fase* nueva = new Fase [n + 10];
    if (nueva == nullptr){
        cerr << "Error al agrandar vector de fases" << endl;
        return 1;
    }

    for (int i=0; i < n; i++)
        nueva[i] = mundial->fases[i];
    for (int i=n; i < n+10; i++){
        if (iniciar_fase_vacia(&nueva[i]) == 1)
            return 1;
    }

    delete [] mundial->fases;
    mundial->fases = nueva;

    for (int i = n; i < n+10; i++){     // AGREGADO POR DEBUG BRUTO
        mundial->fases[i].cant_equipos = 0;
        mundial->fases[i].fase = '\0';
        mundial->fases[i].equipos = nullptr;
    }

    return 0;
}

int estirar_vector_grupo(Mundial* mundial, int num_fase){
    int n = mundial->fases[num_fase].cant_equipos;

    Equipo* nuevo = new Equipo [n + 10];
    if (nuevo == nullptr){
        cerr << "Error al agrandar vector de grupo" << endl;
        return 1;
    }

    for (int i=0; i < n; i++)
        nuevo[i] = mundial->fases[num_fase].equipos[i];
    for (int i=n; i < n+10; i++)
        nuevo[i].nombre = '\0';

    delete [] mundial->fases[num_fase].equipos;
    mundial->fases[num_fase].equipos = nuevo;


    return 0;
}

int cargar_equipo_en_memoria(string linea, Mundial* mundial){
// Carga de equipos (aleatoria, ordenado post cierre de file con inserción
    //cout << "CANT EQUIPOS: " << mundial->cant_equipos << endl;
    //cout << "CANT FASES: " << mundial->cant_fases << endl;

    if((mundial->cant_equipos)%10 == 0){
        if (estirar_vector(mundial) == 1)
            return 1;
    }

    Equipo* actual = &mundial->equipos[mundial->cant_equipos];
    actual->grupo = linea[len_string(linea)-1];
    actual->nombre = to_lower(split_string(linea));
    mundial->cant_equipos += 1;

    return 0;
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
            lista[comas_contadas] += "\0";
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

    // Descartar
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

// CARGAR PARTIDO
    int fase_numerica = fase_a_numero(fase);

    // ASIGNACION DE PUNTOS
    int goles = partido->goles1 - partido->goles2;
    int penales = partido->penales1 - partido->penales2;
    int ganador = goles + penales;

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

    /* No es mas necesario (se mantiene por las dudas)
    if (fase_numerica < 5){
        mundial->fases[fase_numerica].equipos[mundial->fases[fase_numerica].cant_equipos] = *partido->equipo2;
        mundial->fases[fase_numerica].cant_equipos += 1;
        mundial->fases[fase_numerica].equipos[mundial->fases[fase_numerica].cant_equipos] = *partido->equipo1;
        mundial->fases[fase_numerica].cant_equipos += 1;
    }
    */

    delete [] lista;
    delete partido;

    return 0;
}

int formar_grupos(Mundial* mundial){
// Carga de fases

    // cout << "PARA EMPEZAR, 10 fases" << endl;
    if (estirar_vector_fases(mundial) == 1)
        return 1;

    // Inicializar etapas conocidas
    mundial->fases[0].fase = "final";
    mundial->fases[0].equipos = new Equipo [2];
    mundial->fases[1].fase = "tercer puesto";
    mundial->fases[1].equipos = new Equipo [2];
    mundial->fases[2].fase = "semifinal";
    mundial->fases[2].equipos = new Equipo [4];
    mundial->fases[3].fase = "cuartos";
    mundial->fases[3].equipos = new Equipo [8];
    mundial->fases[4].fase = "octavos";
    mundial->fases[4].equipos = new Equipo [16];
    mundial->cant_fases = 5;

    for (int i=0;i<5;i++)
        mundial->fases[i].cant_equipos = 0;

    // ITERAR EQUIPOS, FORMANDO FASES
    int posicion;
    for (int i = 0; i < mundial->cant_equipos; i++){

        // AÑADIR EQUIPO A GRUPO
        posicion = iterar_fases(mundial,mundial->equipos[i].grupo);
        // cout << "POSICION: " << posicion << endl;
        // Si la fase esta vacia, guardar nuevo grupo
        if (mundial->fases[posicion].cant_equipos == 0){
            mundial->fases[posicion].fase = mundial->equipos[i].grupo;
            mundial->cant_fases += 1;

            // ESTIRAR SI LLEGAMOS A MULTIPLO DE 10
            if (mundial->cant_fases%10 == 0){
                // cout << "TE DOY 10 FASES MAS" << endl;
                if (estirar_vector_fases(mundial) == 1)
                    return 1;
            }

            // Se estira grupo solo una vez, podría estirarse mas
            if (estirar_vector_grupo(mundial, posicion) == 1)
                return 1;
        }

        // Agregar equipo a grupo
        int n = mundial->fases[posicion].cant_equipos;
        mundial->fases[posicion].equipos[n] = mundial->equipos[i];
        mundial->fases[posicion].cant_equipos += 1;


        // cout << "EQUIPO AGREGADO A GRUPO" << endl;
        // AGREGAR EQUIPO A FASES FINALES (si llega a fase n, se lo agrega a todas las fases anteriores)
        int m = mundial->equipos[i].fase_final;
        if (m < 5){
            // ESTA AGREGANDO A TERCER PUESTO
            for (int j = 4; j >= m && j >= 2; j--){
                mundial->fases[j].equipos[mundial->fases[j].cant_equipos] = mundial->equipos[i];
                mundial->fases[j].cant_equipos += 1;
            }

            if (m == 0 || m == 1){
                mundial->fases[m].equipos[mundial->fases[m].cant_equipos] = mundial->equipos[i];
                mundial->fases[m].cant_equipos += 1;
            }
        }
    }

    return 0;
}

int iterar_fases(Mundial* mundial, char grupo){
    int i = 5;
    bool grupo_encontrado = false;
    while (grupo_encontrado == false && i < mundial->cant_fases){
        if (mundial->fases[i].fase[0] == grupo){
            grupo_encontrado = true;
            // cout << "GRUPO EXISTENTE: " << grupo << endl;
        }
        else
            i++;
    }

    return i;
}

int recursion_swap(Mundial* mundial, int i){
    if (i != 0){
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

int recursion_swap_pp(Mundial* mundial, int index_fase, int index_equipo, int fase_numerica){
    if (index_equipo != 0){
        if ( mundial->fases[index_fase].equipos[index_equipo].puntos[fase_numerica] > mundial->fases[index_fase].equipos[index_equipo - 1].puntos[fase_numerica]) {
            //cout << "PREVIO/ i-1: " << fase->equipos[i - 1].puntos[fase_numerica] << " i: " << fase->equipos[i].puntos[fase_numerica] << endl;
            Equipo* swap_equipo_pp = new Equipo;                // equipo[i] apunta a direcc de equipos
            *swap_equipo_pp = mundial->fases[index_fase].equipos[index_equipo];
            mundial->fases[index_fase].equipos[index_equipo] = mundial->fases[index_fase].equipos[index_equipo-1];
            mundial->fases[index_fase].equipos[index_equipo-1] = *swap_equipo_pp;
            // cout << "POST/ i-1: " << fase->equipos[i - 1].puntos[fase_numerica] << " i: " << fase->equipos[i].puntos[fase_numerica] << endl;

            delete swap_equipo_pp;
            recursion_swap_pp(mundial,index_fase,index_equipo - 1,fase_numerica);
        }
    }

    return 0;
}

void ordenar_equipos(Mundial* mundial){
    int i = 0;
    while( (mundial->equipos[i]).grupo != '\0' ){
        recursion_swap( mundial , i);
        i++;
    }
}

void ordenar_fases(Mundial* mundial){
    // ORDENAMOS POR NUMERO (por insercion)
    for (int i = 0; i < mundial->cant_fases; i++){
        for (int ii = 0; ii < mundial->fases[i].cant_equipos; ii++){
            if (i < 5)
                recursion_swap_pp(mundial,i, ii, i);
            else
                recursion_swap_pp(mundial,i, ii, 5);
        }
    }
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

bool divisor_de_fase(string input){
    return ( cmp_string(input,"grupos") || cmp_string(input,"octavos") || cmp_string(input,"cuartos") || cmp_string(input,"semifinales") || cmp_string(input,"final") || cmp_string(input,"tercer puesto"));
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
        resultado = 5;
    else if (cmp_string(fase,"octavos"))
        resultado = 4;
    else if (cmp_string(fase,"cuartos"))
        resultado = 3;
    else if (cmp_string(fase,"semifinales"))
        resultado = 2;
    else if (cmp_string(fase,"tercer puesto"))
        resultado = 1;
    else if (cmp_string(fase,"final"))
        resultado = 0;

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
    mundial->cant_fases = 0;
    mundial->MAXIMO_ITERACIONES;

    mundial->equipos = nullptr;
    mundial->fases = nullptr;

    /*
    // mundial->fases = new Fase[10];        // Total de fases
    estirar_vector_fases(mundial);
    mundial->fases[0].equipos = new Fase[2];      // Final
    mundial->fases[1]. = new Fase[2];      // Tercer puesto
    mundial->fases[2] = new Fase[4];      // Semifinal
    mundial->fases[3] = new Fase[8];      // Cuartos
    mundial->fases[4] = new Fase[16];     // Octavos

    mundial->cant_fases = 5;
    for (int i = 0; i < mundial->cant_fases; i++)
        mundial->fases[i]->cant_equipos = 0;
    */

    return mundial;
}

int descargar_mundial(Mundial* mundial){
    // BORRAR vector dinamico DE EQUIPOS EN FASES y DE FASES
    for (int i = 0; i < mundial->cant_fases; i++)
        delete [] mundial->fases[i].equipos;
    delete [] mundial->fases;

    cout << "FASES BORRADAS" << endl;
    /* BORRAR EQUIPOS
    int i = 0;
    if (mundial->equipos != nullptr){
        while (mundial->equipos[i].nombre[0] != '\0'){
            delete &(mundial->equipos[i]);
            i++;
        }
    }
    */
    for (int i = 0; i < mundial->cant_equipos; i++){
        cout << "I: " << i << endl;
        delete &(mundial->equipos[i]);
    }

    cout << "EQUIPOS BORRADOS" << endl;

    delete [] mundial->equipos;                 // BORRAR VECTOR
    delete mundial;                             // BORRAR MUNDIAL

    cout << "MUNDIAL ELIMINADO" << endl;
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
    cout << "1er:" << to_upper(mundial->fases[0].equipos[0].nombre) << " Total puntos:" << suma_puntos(&mundial->fases[0].equipos[0]) << endl;
    cout << "2do:" << to_upper(mundial->fases[0].equipos[1].nombre) << " Total puntos:" << suma_puntos(&mundial->fases[0].equipos[1]) << endl;
    cout << "3ro:" << to_upper(mundial->fases[1].equipos[0].nombre) << " Total puntos:" << suma_puntos(&mundial->fases[1].equipos[0]) << endl;
    cout << "4to:" << to_upper(mundial->fases[1].equipos[1].nombre) << " Total puntos:" << suma_puntos(&mundial->fases[1].equipos[1]) << endl;
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
            case 5:
                cout << "Etapa final: Grupos" << endl; break;
            case 4:
                cout << "Etapa final: Octavos" << endl; break;
            case 3:
                cout << "Etapa final: Cuartos" << endl; break;
            case 2:
                cout << "Etapa final: Semifinales" << endl; break;
            case 1:
                cout << "Etapa final: Tercer puesto" << endl; break;
            case 0:
                cout << "Etapa final: Final" << endl; break;
        }
        if (resultado->fase_final == 4){
            if (cmp_string(resultado->nombre,mundial->fases[1].equipos[0].nombre))
                cout << "Cuarto" << endl;
            else if (cmp_string(resultado->nombre,mundial->fases[1].equipos[1].nombre))
                cout << "Tercero" << endl;
        }
        else if (resultado->fase_final == 5){
            if (cmp_string(resultado->nombre,mundial->fases[0].equipos[1].nombre))
                cout << "Segundo" << endl;
            else if (cmp_string(resultado->nombre,mundial->fases[0].equipos[0].nombre))
                cout << "Primero" << endl;
            }
    }
}

void menu_puntos(Mundial* mundial){
    cout << "FASE DE GRUPOS" << endl;
    for (int i = mundial->cant_fases -1; i >= 0; i--){
        if (i > 4)
            cout << "GRUPO " << to_upper(mundial->fases[i].fase) << endl;
        else if (i == 4)
            cout << "OCTAVOS" << endl;
        else if (i == 3)
            cout << "CUARTOS" << endl;
        else if (i == 2)
            cout << "SEMIFINAL" << endl;
        else if (i == 1)
            cout << "TERCER PUESTO" << endl;
        else if (i == 0)
            cout << "FINAL" << endl;

        for (int ii = 0; ii < mundial->fases[i].cant_equipos; ii++){
                if (i > 4)
                    cout << "Equipo: " << to_upper(mundial->fases[i].equipos[ii].nombre) << " Puntos: " << mundial->fases[i].equipos[ii].puntos[5] << endl;
                else
                    cout << "Equipo: " << to_upper(mundial->fases[i].equipos[ii].nombre) << " Puntos: " << mundial->fases[i].equipos[ii].puntos[i] << endl;
            }
    }
}

