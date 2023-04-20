# AYP2-TP1
TP1 / La mision de Laeb

# Funcionamiento del programa
 - En main se deben editar los argumentos de ifstream entrada_equipos() y ifstream entrada_partidos() tal que coincidan con los archivos a utilizar.
 - Por falta de tiempo, se programo tal que solo se acepten nombres de equipo separados por guión bajo, tanto en el file de equipos como en el de partidos.
 - Las funciones de carga de datos son, en general, indiferentes a si la palabra esta escrita coherentemente con minusculas o mayúsculas. EJ: ArGeNTINa es un nombre valido.
 - La elección de método de busqueda es busqueda binaria
 
# Estructuras de datos utilizadas
 - Se utilizó una estructura Mundial como contenedor de todos los datos
 - Se generó un vector dinámico de equipos que se va estirando de 10 en 10 (considere la idea de implementarlo por medio de duplicar/dividir a la mitad el largo del mismo, pero como se trata de un evento deportivo con una cantidad reducida de equipos, no creí que justificará tratar exponencialmente al vector).
 - Por último, tambien se generó un vector dinámico de fases que se estira de 10 en 10. Donde las fases, a su vez, tienen un vector de equipos que componen la fase

# Funciones del menu
 - Listar: Itera los equipos hasta no encontrar mas elementos.
 - Podio: Accede a los equipos por medio de punteros desde las fases (los punteros a equipos se encuentran ordenados por puntos)
 - Buscar equipo: misma función de búsqueda binaria.
 - Puntaje por fases: Itera las fases desde el final hacia el principio, pasando por los equipos de cada fase.
 - Salida: Libera vectores de equipos de las fases, libera las fases, libera los equipos (tanto memoria de los equipos en si, como el vector) y libera el mundial
 
# Notas 
 - Los indices del vector de fases coinciden con lo siguiente (0 = Final, 1 = Tercer Puesto, 2 = Semifinal, 3 = Cuartos, 4 = Octavos, 4 > grupos).
 - Las fases finales se encuentran con largo hardcodeado.
 - Por falta de tiempo, no se ordenaron las fases alfabeticamente para que sean mostradas de forma estética. 
 - Por falta de tiempo, el vector de equipos de las fases de grupos se estiró solo una vez (con las funciones existentes podría estirarse las veces que sea necesario)

# DISCLAIMER por primer entrega
Por motivos personales no pude finalizar a tiempo el trabajo práctico. En caso de que lo requieran, abro este repositorio para subir el codigo
