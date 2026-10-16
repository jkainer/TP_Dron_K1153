// PascalCase para estructuras.
// camelCase procedimientos.
// snake_case para variables.
// MACRO_CASE para macros.

#include <iostream>
#include <stdio.h>
#define CURSOR "-> " // Cursor indicativo de interfaz. Puramente estético.
#define RUTA_MAX 256 // Tamaño máximo en caracteres posible para ruta de un archivo.
#define TABLA_TAMANO_X 5 // Tamaño horizontal de la tabla del órdenes.
#define TABLA_TAMANO_Y 5 // Tamaño vertical de la tabla del órdenes.
using std::cin, std::cout, std::endl;

// TODO: Crear protocolo de conversión de Orden a OrdenArchivo y viseversa.
struct OrdenArchivo {
	int x 					= 0;
	int y 					= 0;
	unsigned int espera 	= 0;
	bool soltar_granada1 	= false;
	bool soltar_granada2 	= false;
	bool ataque_kamikaze 	= false;
	bool aterrizaje 		= false;
	bool despegue 			= false;
	int siguiente_x 		= 0;
	int siguiente_y 		= 0;
};	

struct Orden {
	bool despegue 			= false;
	bool aterrizaje 		= false;
	bool soltar_granada1 	= false;
	bool soltar_granada2 	= false;
	bool ataque_kamikaze 	= false;
	unsigned int espera 	= 0;
	int siguiente_x 		= 0;
	int siguiente_y 		= 0;
};


bool recorridoTabla(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y], int despegue_x, int despegue_y) {
	int x = despegue_x, y = despegue_y;
	int siguiente_x = -1, siguiente_y = -1;
	unsigned int celdas_recorridas = 0;

	while (true) {
		celdas_recorridas++;

		// DEBUG ----
		// cout << "Celda ";
		// cout << x + 1 << " ✕ " << y + 1;
		// if (tabla[x][y].despegue) cout << " (Celda de despeje)";
		// if (tabla[x][y].aterrizaje) cout << " (Celda de aterrizaje)";
		// if (tabla[x][y].ataque_kamikaze) cout << " (Celda kamikaze)";
		// if (tabla[x][y].soltar_granada1) cout << " (Granada 1)";
		// if (tabla[x][y].soltar_granada2) cout << " (Granada 2)";
		// cout << endl;
		// ----------

		if (tabla[x][y].aterrizaje || tabla[x][y].ataque_kamikaze) break; // Fin del recorrido.

		siguiente_x = tabla[x][y].siguiente_x;
		siguiente_y = tabla[x][y].siguiente_y;
		x = siguiente_x;
		y = siguiente_y;
	}

	if (celdas_recorridas != TABLA_TAMANO_X * TABLA_TAMANO_Y) {
		// MSG_ERROR
		cout << "Error: La tabla de órdenes no recorre su totalidad ";
		cout << "(";
		cout << celdas_recorridas << "/";
		cout << TABLA_TAMANO_X * TABLA_TAMANO_Y << " celdas recorridas).";
		cout << endl;
		return false;
	}

	return true;
}


bool validacionTabla(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]) {
	int despegue_x = -1, despegue_y = -1;

	for (int y = 0; y < TABLA_TAMANO_Y; y++) { for (int x = 0; x < TABLA_TAMANO_X; x++) {
		if (tabla[x][y].despegue) {
			despegue_x = x;
			despegue_y = y;

			// TODO: Agregar el resto de validaciones.
		}
	} }
	if (!recorridoTabla(tabla, despegue_x, despegue_y)) return false;
	return true;
}


void cargarAtaque(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y], bool &flag_tabla_cargada) {
	FILE *archivo_ataque;
	char direccion_archivo[RUTA_MAX];
	int tamaño_archivo = 0;

	cout << "Por favor, indique ruta del archivo de ataque a cargar." << endl;
	// Acá estoy usando un While de una manera particular. Generalmente le pone una condición de
	// salida y se comprueba cada loop, y se utilizan sentencias selectivas para lograr
	// dicha condición de salida. En este caso ocurre lo contrario. No hay condición de salida
	// y las sentencias selctivas son las que repiten el bucle, pero al llegar al final del bucle
	// se consigue salir por la sentencia Break. Se eligió esta forma por la sencillez y
	// claridad visual de su implementación, ya que permite secuenciar multiples condiciones de
	// salida en sentencias If de manera clara.
	while (true) {
		cout << CURSOR;

		// Se utiliza getline en vez de cin para poder incluir espacios
		// en la ruta del archivo. Se usan 2 para evitar un error de leading whitespace.
		cin.getline(direccion_archivo, RUTA_MAX);
		cin.getline(direccion_archivo, RUTA_MAX);
		//cin >> direccion_archivo;

		if (!(archivo_ataque = fopen(direccion_archivo, "r+b"))) {
			// MSG_ERROR
			cout << "Error: La dirección del archivo ingresada es inválida o el archivo no existe. ";
			cout << "Por favor, vuelva a intentarlo o ingrese una ruta válida." << endl;
			continue; // Vuelve a empezar el loop while.
		}	

		cout << "Dirección de archivo válida." << endl;
		fseek(archivo_ataque, 0, SEEK_END);
		// FIXME: 16/10 Cambiar tamaño de Orden a OrdenArchivo.
		tamaño_archivo = ftell(archivo_ataque) / sizeof(Orden); // Tamaño del archivo en casilleros.
		if (tamaño_archivo != TABLA_TAMANO_X * TABLA_TAMANO_Y) {
			// MSG_ERROR
			cout << "Error: El archivo cargado tiene un tamaño invalido. ";
			cout << "Por favor, ingrese un archivo de ataque válido." << endl;
			continue;
		}	
		
		cout << "Cargando archivo de ataque a memoria..." << endl;

		// FIXME: 16/10 Cambiar tipo de prototipo de variable a guardar de Orden a OrdenArchivo.
		// e integrar sistema correspondiente de adaptación.
		fseek(archivo_ataque, 0, SEEK_SET);
		fread(
			tabla,
			sizeof(Orden),
			TABLA_TAMANO_X * TABLA_TAMANO_Y,
			archivo_ataque
		);
		if (!validacionTabla(tabla)) {
			// MSG_ERROR
			cout << "Error: El archivo ingresado contiene una o más instrucciones inválidas." << endl;
			continue;
		}

		break;
	};

	flag_tabla_cargada = true;
	fclose(archivo_ataque);
	return;
}


// Paso 1: Crear archivo.
// Paso 2: Poblar tabla registro por registro.
// Paso 3: Guardar tabla en archivo creado.
void poblarArchivo(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]) {
	char respuesta = ' ';
	bool flag_despegue 		= false; // Solo puede haber una celda de despegue por tabla.
	bool flag_aterrizaje 	= false;
	bool flag_kamikaze 		= false;
	bool flag_granada1 		= false;
	bool flag_granada2 		= false;
	bool flag_posiciones_transitadas[TABLA_TAMANO_X][TABLA_TAMANO_Y] = {false};
	unsigned int respuesta_tiempo = 0;
	unsigned int respuesta_x = 0, respuesta_y = 0;

	// DEBUG ----
	cout << "Tabla de " << TABLA_TAMANO_X << " ✕ " << TABLA_TAMANO_Y;
	cout << "." << endl;
	// ----------
	while (true) {
		for (int y = 0; y < TABLA_TAMANO_Y; y++) { for (int x = 0; x < TABLA_TAMANO_X; x++) {
			cout << "Posición " << x + 1 << " ✕ " << y + 1 << ":" << endl;

			// Un While por valor a introducir es una implementación un poco
			// exagerada, pero como expliqué anteriormente, me da la flexibilidad
			// de permitirle al usuario introducir un valor no válido y corregirlo
			// antes de continuar con la ejecución.
			// Cabe destacar también que en este caso se utiliza una disposición
			// donde la función If es la que otorga la condición de salida del bucle,
			// a diferencia de casos posteriores donde otorga la condición de permanencia.
			// Esto es porque en este caso particular, la cantidad de valores válidos son
			// limitados y determinables, donde los inválidos son indeterminables; a
			// diferencia de casos posteriores donde los valores válidos son indeterminados
			// mientras que los inválidos son determinados. Una manera de adaptar esta
			// implementación es la de usar condiciones negativas (Ej: cuando la respuesta
			// no sea 's' ni 'n'), pero se decidió de esta manera para evitar agregar ruido
			// visual con Ifs de varias condiciones, que más tarde serían
			// comparadas igualmente.

			// Despegue
			while (!flag_despegue) {
				// TODO: Crear respuesta predeterminada
				// (Ej: Presionar Enter sin llenar la entrada y que se asuma una opción válida).
				cout << "  ¿Despegue? [s/n]" << endl;
				cout << "  " << CURSOR;
				cin >> respuesta;

				if (respuesta == 's') {
					tabla[x][y].despegue = true;
					flag_despegue = true;
					flag_posiciones_transitadas[x][y] = true;
					break;
				} else if (respuesta == 'n') {
					tabla[x][y].despegue = false;
					break;
				}
				// MSG_ERROR
				cout << "Error: Respuesta no valida. Vuelva a intentarlo." << endl;
			}

			// Aterrizaje
			while (
				!flag_aterrizaje &&
				!flag_kamikaze &&
				!tabla[x][y].despegue
			) {
				cout << "  ¿Aterrizaje? [s/n]" << endl;
				cout << "  " << CURSOR;
				cin >> respuesta;
				if (respuesta == 's') {
					tabla[x][y].aterrizaje = true;
					flag_aterrizaje = true;
					break;
				} else if (respuesta == 'n') {
					tabla[x][y].aterrizaje = false;
					break;
				}
				// MSG_ERROR
				cout << "Error: Respuesta no valida. Vuelva a intentarlo." << endl;
			}

			// Kamikaze
			while (
				!flag_aterrizaje &&
				!flag_kamikaze &&
				!tabla[x][y].despegue
			) {
				cout << "  ¿Ataque kamikaze? [s/n]" << endl;
				cout << "  " << CURSOR;
				cin >> respuesta;
				if (respuesta == 's') {
					tabla[x][y].ataque_kamikaze = true;
					flag_aterrizaje = true;
					break;
				} else if (respuesta == 'n') {
					tabla[x][y].ataque_kamikaze = false;
					break;
				}
				// MSG_ERROR
				cout << "Error: Respuesta no valida. Vuelva a intentarlo." << endl;
			}

			// Granada 1
			while (
				!flag_granada1 &&
				!tabla[x][y].despegue
			) {
				cout << "  ¿Soltar granada nº1? [s/n]" << endl;
				cout << "  " << CURSOR;
				cin >> respuesta;
				if (respuesta == 's') {
					tabla[x][y].soltar_granada1 = true;
					flag_granada1 = true;
					break;
				} else if (respuesta == 'n') {
					tabla[x][y].soltar_granada1 = false;
					break;
				}
				// MSG_ERROR
				cout << "Error: Respuesta no valida. Vuelva a intentarlo." << endl;
			}

			// Granada 2
			while (
				!flag_granada2 &&
				!tabla[x][y].despegue
			) {
				cout << "  ¿Soltar granada nº2? [s/n]" << endl;
				cout << "  " << CURSOR;
				cin >> respuesta;
				if (respuesta == 's') {
					tabla[x][y].soltar_granada2 = true;
					flag_granada2 = true;
					break;
				} else if (respuesta == 'n') {
					tabla[x][y].soltar_granada2 = false;
					break;
				}
				// MSG_ERROR
				cout << "Error: Respuesta no valida. Vuelva a intentarlo." << endl;
			}

			// Tiempo de espera
			while (true) {
				cout << "  Tiempo de espera (Segundos)." << endl;
				cout << "  " << CURSOR;
				cin >> respuesta_tiempo;
				// TODO: Crear condiciones de invalidez.
				tabla[x][y].espera = respuesta_tiempo;
				break;
			}

			// Este es el caso de implementación alterna del que hablo más arriba.

			// Siguiente posición
			while (
				!tabla[x][y].aterrizaje &&
				!tabla[x][y].ataque_kamikaze
			) {
				cout << "  Posición siguiente [X ✕ Y]." << endl;
				cout << "  " << CURSOR;
				cin >> respuesta_x;
				cout << "  " << CURSOR;
				cin >> respuesta_y;
				// (Se interpreta que la tabla comienza en 1x1 y no en 0x0).
				respuesta_x--;
				respuesta_y--;

				// Esta condición es innecesaria, ya que la comprobación
				// siguiente impide este caso también, pero tomé la decisión
				// de contemplarlo igualmente para devolver un mensaje de
				// alerta más adecuado.
				if (
					respuesta_x == x &&
					respuesta_y == y
				) {
					// MSG_ERROR
					cout << "Error: Ingrese una celda distinta de en la que se encuentra.";
					cout << endl;
					continue;
				}

				if (
					!(respuesta_x == x 		&& respuesta_y == y - 1) 	&&	// Norte.
					!(respuesta_x == x + 1 	&& respuesta_y == y) 		&&	// Este.
					!(respuesta_x == x 		&& respuesta_y == y + 1) 	&&	// Sur.
					!(respuesta_x == x - 1 	&& respuesta_y == y)			// Oeste.
				) {
					// MSG_ERROR
					cout << "Error: Seleccione una celda adyasente." << endl;
					continue;
				}

				if (
					respuesta_x >= TABLA_TAMANO_X ||
					respuesta_y >= TABLA_TAMANO_Y ||
					respuesta_x < 0 ||
					respuesta_y < 0
				) {
					// MSG_ERROR
					cout << "Error: Celda fuera de límites. Elija otra posición." << endl;
					continue;
				}

				if (flag_posiciones_transitadas[respuesta_x][respuesta_y]) {
					// MSG_ERROR
					cout << "Error: Celda ya transitada. Elija otra posición." << endl;
					continue;
				}

				tabla[x][y].siguiente_x = respuesta_x;
				tabla[x][y].siguiente_y = respuesta_y;
				flag_posiciones_transitadas[respuesta_x][respuesta_y] = true;
				break;
			}
		} }

		if (!validacionTabla(tabla)) {
			// MSG_ERROR
			cout << "Error: La tabla de órdenes creada es inválida. Vuelva a crearla." << endl;
			continue;
		}
		break;
	}

	return;
}


// "crearArchivo()" no pide como parametro la tabla global, ya que esta no
// se utiliza fuera de su propio procedimiento. Está contenida dentro de
// la creación del archivo en donde se almacenará. Para interactuar con
// el resto de funciones, como "mostrarAtaque()", por ejemplo, se debe
// cargar como cualquier otro archivo.
void crearArchivo() {
	char direccion_archivo[RUTA_MAX];
	FILE *archivo_ataque;
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y];
	//OrdenArchivo tabla_archivo[TABLA_TAMANO_X][TABLA_TAMANO_Y];

	cout << "Por favor, indique ruta de guardado del ataque." << endl;
	while (true) {
		cout << CURSOR;
		// Se utiliza getline en vez de cin para poder incluir espacios
		// en la ruta del archivo. Se usan 2 para evitar un error de leading whitespace.
		cin.getline(direccion_archivo, RUTA_MAX);
		cin.getline(direccion_archivo, RUTA_MAX);
		//cin >> direccion_archivo;

		// Flag 'x' al modo para evitar la sobreescritura.
		// TODO: Opción de sobreescribir.
		if ( !(archivo_ataque = fopen(direccion_archivo, "w+bx")) ) {
			// MSG_ERROR
			cout << "Error: La dirección del archivo ingresada es inválida o el archivo ya existe. ";
			cout << "Por favor, vuelva a intentarlo o ingrese una ruta válida." << endl;
			continue; // Vuelve a empezar el loop while.
		}
		break;
	};

	poblarArchivo(tabla);
	cout << "Guardando tabla de órdenes en ";
	cout << "\"" << direccion_archivo << "\"";
	cout << "..." << endl;

	// FIXME: 16/10 Cambiar a OrdenArchivo, no Orden.
	if (
		!fwrite(
			tabla,
			sizeof(Orden),
			TABLA_TAMANO_X * TABLA_TAMANO_Y,
			archivo_ataque
		)
	) {
		// MSG_ERROR
		cout << "Error: Error al guardar la tabla en ";
		cout << "\"" << direccion_archivo << "\"" << endl;
		cout << ". Puede que el archivo se haya movido o borrado. Vuelva a intentarlo." << endl;
	} else {
		cout << "¡Guardado de archivo exitoso!" << endl;
	}

	fclose(archivo_ataque);

	return;
}

// TODO: Deindividualizar y proceduralizar función compartida de carga de archivo con "cargarArchivo()"
void guardarAtaque(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y], bool &flag_tabla_cargada) {
	if (!flag_tabla_cargada) {
		// MSG_ERROR
		cout << "Error: No hay una tabla de órdenes cargada. ";
		cout << "Cargue o cree una tabla primero para poder guardarla." << endl;
		return;
	}

	char direccion_archivo[RUTA_MAX];
	FILE *archivo_ataque;

	cout << "Por favor, indique ruta de guardado del ataque." << endl;
	while (true) {
		cout << CURSOR;
		// Se utiliza getline en vez de cin para poder incluir espacios
		// en la ruta del archivo. Se usan 2 para evitar un error de leading whitespace.
		cin.getline(direccion_archivo, RUTA_MAX);
		cin.getline(direccion_archivo, RUTA_MAX);
		//cin >> direccion_archivo;

		// Flag 'x' al modo para evitar la sobreescritura.
		// TODO: Opción de sobreescribir.
		if (!(archivo_ataque = fopen(direccion_archivo, "w+bx"))) {
			// MSG_ERROR
			cout << "Error: La dirección del archivo ingresada es inválida o el archivo ya existe. ";
			cout << "Por favor, vuelva a intentarlo o ingrese una ruta válida." << endl;
			continue; // Vuelve a empezar el loop while.
		}	

		break;
	};	

	cout << "Guardando ataque..." << endl;
	// FIXME: 16/10 Cargar a OrdenArchivo, no Orden.
	fwrite(
		tabla,
		sizeof(Orden),
		TABLA_TAMANO_X * TABLA_TAMANO_Y,
		archivo_ataque
	);
	cout << "Archivo guardado." << endl;
	fclose(archivo_ataque);

	return;
}


void mostrarAtaque(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y], bool &flag_tabla_cargada) {
	if (!flag_tabla_cargada) {
		// MSG_ERROR
		cout << "Error: No hay una tabla de órdenes cargada. ";
		cout << "Cargue o cree una tabla primero para poder visualizarla." << endl;
		return;
	}

	Orden pagina_tabla;

	// Procedimiento para evitar leading whitespace al seleccionar esta opción.
	cin.get();

	for (int y = 0; y < TABLA_TAMANO_Y; y++) { for (int x = 0; x < TABLA_TAMANO_X; x++) {
		pagina_tabla = tabla[x][y];

		// Un poquito de diseño de tui.
		cout << "┌─────────────────────────────────────┐" << endl;

		cout << "│ " << "Despegue:                  ";
		cout << (pagina_tabla.despegue ? "Veradero" : "   Falso") << endl;
		// Operador ternario para redibilidad

		cout << "│ " << "Aterrizaje:                ";
		cout << (pagina_tabla.aterrizaje ? "Veradero" : "   Falso") << endl;

		cout << "│ " << "Soltar granada nº1:        ";
		cout << (pagina_tabla.soltar_granada1  ? "Veradero" : "   Falso") << endl;

		cout << "│ " << "Soltar granada nº2:        ";
		cout << (pagina_tabla.soltar_granada2 ? "Veradero" : "   Falso") << endl;

		cout << "│ " << "Ataque kamikaze:           ";
		cout << (pagina_tabla.ataque_kamikaze ? "Veradero" : "   Falso") << endl;

		cout << "│ " <<  "Espera [Segundos]:           ";
		// TODO: Agregar sistema de formato.
		// (Desplazar cierta cantidad de espacios para que quede anclado a la derecha
		// dependiendo del tamaño del numero de segundos).
		cout << pagina_tabla.espera << " s" << endl;

		cout << "│ " << "Posición siguiente [X ✕ Y]: ";
		cout << pagina_tabla.siguiente_x + 1 << " ✕ " << pagina_tabla.siguiente_y + 1 << endl;
		// Se le suma 1 a las coordenadas para volverlas amistosas para el usuario.
		// (Se interpreta que la tabla comienza en 1x1 y no en 0x0).

		cout << "│ " << "Posición actual [X ✕ Y]:    ";
		cout << x + 1 << " ✕ " << y + 1 << endl;
		cout << "└─ Presione Enter para continuar... ──┘" << endl;

		cin.get();
	} }
	return;
}


void bucleMenu(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y], bool &flag_tabla_cargada) {
	int eleccion = 0;

	do {
		// Con esta comparación se evita reimprimir el menú si
		// la opción ingresada es incorrecta, pero sí si se
		// acaba de salir de una opción real luego de utilizarla.
		if (eleccion < 7) {
			cout << "Menú de opciones de control de memoria del dron." << endl;
			//cout << endl;
			cout << "Por favor, selecciones una de las siguientes opciones: (ej: 1, 2, 3)" << endl;
			cout << "  [1]. Cargar archivo de ataque a memoria." << endl;
			cout << "  [2]. Mostrar archivo cargado." << endl;
			cout << "  [3]. Crear un archivo de ataque nuevo." << endl;
			cout << "  [4]. Corregir un registro del archivo." << endl;
			cout << "  [5]. Corregir un registro en memoria." << endl;
			cout << "  [6]. Guardar en memoria un archivo nuevo." << endl;
			cout << "  [7]. Visualizar un archivo de ataque en HTML." << endl;
			cout << "  [0]. Terminar este programa." << endl;
		}

		cout << CURSOR;
		cin >> eleccion;
		switch (eleccion) {
			default:
				// MSG_ERROR
				cout << "Error: Opción no valida. Vuelva a intentarlo." << endl;
				break;
			case 0:
				cout << "Terminando ejecución." << endl;
				break;
			case 1: // Cargar archivo de ataque.
				cargarAtaque(tabla, flag_tabla_cargada);
				break;
			case 2: // Mostrar archivo cargado.
				mostrarAtaque(tabla, flag_tabla_cargada);
				break;
			case 3: // Crear archivo de ataque nuevo y poblarlo
				crearArchivo();
				break;
			case 4:
				cout << "Opción 4." << endl;
				break;
			case 5:
				cout << "Opción 5." << endl;
				break;
			case 6: // Guardar archivo de ataque.
				guardarAtaque(tabla, flag_tabla_cargada);
				break;
			case 7:
				cout << "Opción 7." << endl;
				break;
		}
	} while (eleccion != 0);
	return;
}


int main() {
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]; // Tabla de órdenes en memoria.
	bool flag_tabla_cargada = false;
	bucleMenu(tabla, flag_tabla_cargada);

	return 0;
}