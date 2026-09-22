// PascalCase para estructuras.
// camelCase procedimientos.
// snake_case para variables.
// MACRO_CASE para macros.

#include <iostream>
#include <stdio.h>
#define CURSOR "-> " // Cursor indicativo de interfaz. Puramente estético.
#define RUTA_MAX 256 // Tamaño máximo en caracteres posible para ruta de un archivo.
#define TABLA_TAMAÑO_X 5 // Tamaño horizontal de la tabla del órdenes.
#define TABLA_TAMAÑO_Y 5 // Tamaño vertical de la tabla del órdenes.
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
void cargarAtaque(Orden tabla[TABLA_TAMAÑO_Y][TABLA_TAMAÑO_Y]) {
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
		if (tamaño_archivo != TABLA_TAMAÑO_X * TABLA_TAMAÑO_Y) {
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


void guardarAtaque(Orden tabla[TABLA_TAMAÑO_X][TABLA_TAMAÑO_Y]) {
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
	fwrite(tabla, sizeof(Orden), TABLA_TAMAÑO_X * TABLA_TAMAÑO_Y, archivo_ataque);
	cout << "Archivo guardado." << endl;
	fclose(archivo_ataque);

	return;
}

// TODO: Mostrar tabla de n * m tamaño.
void mostrarAtaque() {
	return;
}


void bucleMenu(Orden tabla[TABLA_TAMAÑO_X][TABLA_TAMAÑO_Y]) {
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
					cout << "Opción 2." << endl;
					break;
				case 3:
					cout << "Opción 3." << endl;
					break;
				case 4:
					cout << "Opción 4." << endl;
					break;
				case 5:
					cout << "Opción 5." << endl;
					break;
				case 6:
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
	Orden tabla[TABLA_TAMAÑO_X][TABLA_TAMAÑO_Y]; // Tabla de órdenes.

	bucleMenu(tabla);

	return 0;
}