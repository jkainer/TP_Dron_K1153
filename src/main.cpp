#include <iostream>
#include <stdio.h>
#define CURSOR "-> " // Cursor indicativo de interfaz. Puramente estético.
#define RUTA_MAX 100 // Tamaño máximo en caracteres posible para ruta de un archivo.
using std::cin, std::cout, std::endl;

// PascalCase para estructuras.
// camelCase procedimientos.
// snake_case para variables.
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
void cargarAtaque() {
	FILE *archivo_ataque;
	char direccion_archivo[RUTA_MAX];

	cout << "Por favor, indique ruta del archivo de ataque a cargar." << endl;
	while (true) {
		cout << CURSOR;
		// Se utiliza getline en vez de cin para poder incluir espacios
		// en la ruta del archivo. Se usan 2 para evitar un error de leading whitespace.
		cin.getline(direccion_archivo, RUTA_MAX);
		cin.getline(direccion_archivo, RUTA_MAX);
		//cin >> direccion_archivo;

		if (archivo_ataque = fopen(direccion_archivo, "r+b")) break;

		cout << "La dirección del archivo ingresada es inválida o el archivo no existe. ";
		cout << "Por favor, vuelva a intentarlo o ingrese una ruta válida." << endl;
	};

	cout << "Archivo válido." << endl;
	fclose(archivo_ataque);
	return;
}


void bucleMenu() {
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
				case 1:
					cargarAtaque();
					break;
				case 2:
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
					cout << "Opción 6." << endl;
					break;
				case 7:
					cout << "Opción 7." << endl;
					break;
			}
	} while (eleccion != 0);
	return;
}


int main() {
	bucleMenu();

	return 0;
}