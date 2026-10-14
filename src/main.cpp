// PascalCase para estructuras.
// camelCase procedimientos.
// snake_case para variables.
// MACRO_CASE para macros.

#include <iostream>
#include <stdio.h>
#define CURSOR "-> " // Cursor indicativo de interfaz. Puramente estético.
#define RUTA_MAX 256 // Tamaño máximo en caracteres posible para ruta de un archivo.
#define TABLA_TAMANO_X 100 // Tamaño horizontal de la tabla del órdenes.
#define TABLA_TAMANO_Y 100 // Tamaño vertical de la tabla del órdenes.
using std::cin, std::cout, std::endl;

struct OrdenArchivo {
	int x 					= 0;
	int y 					= 0;
	bool despegue 			= false;
	bool aterrizaje 		= false;
	bool soltar_granada1 	= false;
	bool soltar_granada2 	= false;
	bool ataque_kamikaze 	= false;
	unsigned int espera 	= 0;
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


// TODO: Cargar y validar registros del archivo.
void cargarAtaque(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]) {
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
			cout << "La dirección del archivo ingresada es inválida o el archivo no existe. ";
			cout << "Por favor, vuelva a intentarlo o ingrese una ruta válida." << endl;
			continue; // Vuelve a empezar el loop while.
		}

		cout << "Dirección de archivo válida." << endl;
		fseek(archivo_ataque, 0, SEEK_END);
		tamaño_archivo = ftell(archivo_ataque) / sizeof(OrdenArchivo); // Tamaño del archivo en casilleros.
		if (tamaño_archivo != TABLA_TAMANO_X * TABLA_TAMANO_Y) {
			cout << "El archivo cargado tiene un tamaño invalido. ";
			cout << "Por favor, ingrese un archivo de ataque válido." << endl;
			continue;
		}
		
		cout << "Cargando archivo de ataque a memoria..." << endl;

		// TODO: Cargar archivo a tabla según 'x' e 'y'.
		// TODO: Comprobar condiciones de validación.

		break;
	};


	fclose(archivo_ataque);
	return;
}


void guardarAtaque(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]) {
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
			cout << "La dirección del archivo ingresada es inválida o el archivo ya existe. ";
			cout << "Por favor, vuelva a intentarlo o ingrese una ruta válida." << endl;
			continue; // Vuelve a empezar el loop while.
		}

		break;
	};

	cout << "Guardando ataque..." << endl;
	// FIXME: Cargar OrdenArchivo, no Orden.
	//fwrite(tabla, sizeof(Orden), TABLA_TAMANO_X * TABLA_TAMANO_Y, archivo_ataque);
	cout << "Archivo guardado." << endl;
	fclose(archivo_ataque);

	return;
}

// Paso 1: Crear archivo
// Paso 2: Llenar tabla registro por registro.
// Paso 3: Guardar tabla en archivo creado.
void poblarArchivo(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]) {
	char respuesta = ' ';
	bool flag_despegue = false; // Solo puede haber una casilla de despegue por tabla.
	unsigned flag_despegue_x = 0;
	unsigned flag_despegue_y = 0;
	bool flag_aterrizaje = false;
	bool flag_kamikaze = false;
	bool flag_granada1 = false;
	bool flag_granada2 = false;
	bool flag_posiciones_transitadas[TABLA_TAMANO_X][TABLA_TAMANO_Y] = {false};
	unsigned int respuesta_tiempo = 0;
	unsigned int respuesta_x = 0;
	unsigned int respuesta_y = 0;

	for (int y = 0; y < TABLA_TAMANO_Y; y++) {
		for (int x = 0; x < TABLA_TAMANO_X; x++) {
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
			while (!flag_despegue) {
				// TODO: Crear respuesta predeterminada (Ej: Presionar Enter sin llenar
				// la entrada y que se asuma una opción válida).
				cout << "  ¿Despegue? [s/n]" << endl;
				cout << "  " << CURSOR;
				cin >> respuesta;
				if (respuesta == 's') {
					tabla[x][y].despegue = true;
					flag_despegue = true;
					flag_despegue_x = x;
					flag_despegue_y = y;
					break;
				} else if (respuesta == 'n') {
					tabla[x][y].despegue = false;
					break;
				}
				cout << "Respuesta no valida. Vuelva a intentarlo." << endl;
			}
			
			while (!flag_aterrizaje && !flag_kamikaze && !tabla[x][y].despegue) {
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
				cout << "Respuesta no valida. Vuelva a intentarlo." << endl;
			}
			
			while (!flag_aterrizaje && !flag_kamikaze && !tabla[x][y].despegue) {
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
				cout << "Respuesta no valida. Vuelva a intentarlo." << endl;
			}
			
			while (!flag_granada1 && !tabla[x][y].despegue) {
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
				cout << "Respuesta no valida. Vuelva a intentarlo." << endl;
			}
			
			while (!flag_granada2 && !tabla[x][y].despegue) {
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
				cout << "Respuesta no valida. Vuelva a intentarlo." << endl;
			}

			while (true) {
				cout << "  Tiempo de espera (Segundos)." << endl;
				cout << "  " << CURSOR;
				cin >> respuesta_tiempo;
				// TODO: Comprobar condición de invalidez.
				break;
			}

			// Este es el caso de implementación alterna del que hablo más arriba.
			while (!tabla[x][y].aterrizaje) {
				cout << "  Posición siguiente [X ✕ Y]." << endl;
				cout << "  " << CURSOR;
				cin >> respuesta_x;
				cout << "  " << CURSOR;
				cin >> respuesta_y;
				respuesta_x--;
				respuesta_y--;
				if (respuesta_x > TABLA_TAMANO_X || respuesta_y > TABLA_TAMANO_Y) {
					cout << "Posición no válida. Vuelva a intentarlo." << endl;
					continue;
				}
				if (flag_posiciones_transitadas[respuesta_x][respuesta_y]) {
					cout << "Posición ya transitada. Elija otra posición." << endl;
					continue;
				}
				tabla[x][y].siguiente_x = respuesta_x;
				tabla[x][y].siguiente_y = respuesta_y;
				flag_posiciones_transitadas[respuesta_x][respuesta_y] = true;
				break;
			}

			// TODO: Agregar procedimiento para recorrer tabla.
		}
	}

	return;
}

void crearArchivo() {
	char direccion_archivo[RUTA_MAX];
	FILE *archivo_ataque;
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y];
	OrdenArchivo tabla_archivo[TABLA_TAMANO_X][TABLA_TAMANO_Y];

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
			cout << "La dirección del archivo ingresada es inválida o el archivo ya existe. ";
			cout << "Por favor, vuelva a intentarlo o ingrese una ruta válida." << endl;
			continue; // Vuelve a empezar el loop while.
		}
		break;
	};



	return;
}


void mostrarAtaque(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]) {
	Orden pagina_tabla;
	
	// Procedimiento para evitar leading whitespace al seleccionar esta opción.
	cin.get();
	
	for (int y = 0; y < TABLA_TAMANO_Y; y++) {
		for (int x = 0; x < TABLA_TAMANO_X; x++) {
			pagina_tabla = tabla[x][y];

			// Un poquito de diseño de tui.
			cout << "┌─────────────────────────────────────┐" << endl;

			cout << "│ " << "Despegue:                     ";
			cout << (pagina_tabla.despegue ? "Veradero" : "Falso") << endl;

			cout << "│ " << "Aterrizaje:                   ";
			cout << (pagina_tabla.aterrizaje ? "Veradero" : "Falso") << endl;

			cout << "│ " << "Soltar granada nº1:           ";
			cout << (pagina_tabla.soltar_granada1  ? "Veradero" : "Falso") << endl;

			cout << "│ " << "Soltar granada nº2:           ";
			cout << (pagina_tabla.soltar_granada2 ? "Veradero" : "Falso") << endl;

			cout << "│ " << "Ataque kamikaze:              ";
			cout << (pagina_tabla.ataque_kamikaze ? "Veradero" : "Falso") << endl;

			cout << "│ " <<  "Espera [Segundos]:           ";
			cout << pagina_tabla.espera << " s" << endl;

			cout << "│ " << "Posición siguiente [X ✕ Y]: ";
			cout << pagina_tabla.siguiente_x + 1 << " ✕ " << pagina_tabla.siguiente_y + 1 << endl;
			// Se le suma 1 a las coordenadas para volverlas amistosas para el usuario.
			// (Se interpreta que la tabla comienza en 1x1 y no en 0x0).

			cout << "│ " << "Posición actual [X ✕ Y]:  ";
			cout << x + 1 << " ✕ " << y + 1 << endl;
			cout << "└─ Presione Enter para continuar... ──┘" << endl;

			cin.get();
		}
	}
	return;
}


void bucleMenu(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]) {
	int eleccion = 0;

	do {
		// Con esta comparación se evita reimprimir el menú si
		// la opción ingresada es incorrecta, pero sí si se
		// acaba de salir de una opción real luego de utilizarla.
		if (eleccion < 7) {
			cout << "Menú de opciones de control de memoria del dron." << endl;
			cout << endl;
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
				cout << "Opción no valida. Vuelva a intentarlo." << endl;
				break;
				case 0:
					cout << "Terminando ejecución." << endl;
					break;
				case 1: // Cargar archivo de ataque.
					cargarAtaque(tabla);
					break;
				case 2: // Mostrar archivo cargado.
					mostrarAtaque(tabla);
					break;
				case 3: // Crear archivo de ataque nuevo y poblarlo
					poblarArchivo(tabla);
					break;
				case 4:
					cout << "Opción 4." << endl;
					break;
				case 5:
					cout << "Opción 5." << endl;
					break;
				case 6: // Guardar archivo de ataque.
					guardarAtaque(tabla);
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
	bucleMenu(tabla);

	return 0;
}