#include <iostream>
#include <stdio.h>
#include <string.h> // STRING
#include <limits> // Línea 30
#define CURSOR "-> " // Cursor indicativo de interfaz. Puramente estético.
#define RUTA_MAX 256 // Tamaño máximo en caracteres posible para ruta de un archivo.
#define TABLA_TAMANO_X 5 // Tamaño horizontal de la tabla del órdenes.
#define TABLA_TAMANO_Y 5 // Tamaño vertical de la tabla del órdenes.
#define ANSI_ITALICS "\033[3m" // Código ANSI para Italics.
#define ANSI_CLEAR "\033[0m"
#define ANSI_LINEA_ANTERIOR "\033[F" // Mover el cursor de la terminal al inicio de la linea anterior.
#define ERROR "\033[1mError\033[0m: " // "Error:" con el código ANSI de negrita.
#define WARN "\033[1mAlerta\033[0m: " // Esto solo se usa una sola vez.
using std::cin, std::cout, std::endl;

// Comprueba que se se haya ingresado un valor válido. En caso de que
// "cin" devuelva error, limpia el estado de error y devuelve "true".
bool cinInvalido() {
	// "cin <<" 		-> int.
	// "cin.getline()" 	-> char[].
	// "cin.get()" 		-> char.
	// "cin.ignore()" 	-> Leading whitespace.
	if (!cin) {
		cout << ERROR; // MSG_ERROR
		cout << "Respuesta no valida. Vuelva a intentarlo." << endl;
		cin.clear();
		// Limpio el buffer de entrada completo, evitando que si se ingresaron
		// multiples caracteres, se ignoren todos y no solo el primero.
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		return true;
	}

	return false;
}

// ──────────── Prototipos de órdenes ───────────────

struct Flags {
	bool despegue 		= false;
	bool aterrizaje 	= false;
	bool kamikaze 		= false;
	bool granada1 		= false;
	bool granada2 		= false;
	bool posiciones_transitadas[TABLA_TAMANO_X][TABLA_TAMANO_Y] = { false };
	int despegue_x 		= -1;
	int despegue_y 		= -1;
};

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


// Copia el contenido de una tabla de órdenes "Original" a otra "Copia".
void clonarTabla(
	Orden original[TABLA_TAMANO_X][TABLA_TAMANO_Y],
	Orden copia[TABLA_TAMANO_X][TABLA_TAMANO_Y]
) {
	// Podría usar un solo bucle For y aritmética de punteros para transitar
	// las tablas, pero en pro de la legibilidad, prefiero recorrerlo en
	// ambos ejes por separado con dos bucles.
	for (int y = 0; y < TABLA_TAMANO_Y; y++) {
		for (int x = 0; x < TABLA_TAMANO_X; x++) {
			copia[x][y] = original[x][y];
		}
	}

	return;
}


// Copia el contenido de una lista de prototipo "OrdenArchivo"
// a una tabla en memoria de prototipo "Orden".
void listaATabla(
	OrdenArchivo lista[TABLA_TAMANO_X * TABLA_TAMANO_Y],
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]
) {
	int x, y;
	for (int i = 0; i < TABLA_TAMANO_X * TABLA_TAMANO_Y; i++) {
		x = lista[i].x;
		y = lista[i].y;

		tabla[x][y].despegue 		= lista[i].despegue;
		tabla[x][y].aterrizaje 		= lista[i].aterrizaje;
		tabla[x][y].soltar_granada1 = lista[i].soltar_granada1;
		tabla[x][y].soltar_granada2 = lista[i].soltar_granada2;
		tabla[x][y].ataque_kamikaze = lista[i].ataque_kamikaze;
		tabla[x][y].espera 			= lista[i].espera;
		tabla[x][y].siguiente_x 	= lista[i].siguiente_x;
		tabla[x][y].siguiente_y 	= lista[i].siguiente_y;
	}

	return;
}


// Copia el contenido de una tabla en memoria de prototipo "Orden"
// a una lista de prototipo "OrdenArchivo".
void tablaALista(
	OrdenArchivo lista[TABLA_TAMANO_X * TABLA_TAMANO_Y],
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]
) {
	int i = 0;
	for (int y = 0; y < TABLA_TAMANO_Y; y++) {
		for (int x = 0; x < TABLA_TAMANO_X; x++) {
			lista[i].x = x;
			lista[i].y = y;

			lista[i].despegue 			= tabla[x][y].despegue;
			lista[i].aterrizaje 		= tabla[x][y].aterrizaje;
			lista[i].soltar_granada1 	= tabla[x][y].soltar_granada1;
			lista[i].soltar_granada2 	= tabla[x][y].soltar_granada2;
			lista[i].ataque_kamikaze 	= tabla[x][y].ataque_kamikaze;
			lista[i].espera 			= tabla[x][y].espera;
			lista[i].siguiente_x 		= tabla[x][y].siguiente_x;
			lista[i].siguiente_y 		= tabla[x][y].siguiente_y;
			i++;
		}
	}

	return;
}

// ──────────────────────────────────────────────────


// ───────────── Registros y tablas ─────────────────

// Recorre la tabla desde la celda de despegue hasta una celda final
// (celda de aterrizaje o de ataque kamikaze), o hasta encontrar un
// registro inválido, retornando "false" en ese caso junto a un mensaje
// de error.
bool recorrerTabla(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y], Flags flags) {
	Orden registro;
	int x = flags.despegue_x, y = flags.despegue_y;
	unsigned int celdas_recorridas = 0;

	while (true) {
		registro = tabla[x][y];

		if (flags.posiciones_transitadas[x][y]) {
			cout << ERROR; // MSG_ERROR
			cout << "La celda ";
			cout << "(" << x + 1 << ";" << y + 1 << ")";
			cout << " es recorrida más de 1 vez." << endl;
			return false;
		}
		flags.posiciones_transitadas[x][y] = true;
		celdas_recorridas++;

		if (registro.aterrizaje || registro.ataque_kamikaze) break; // Final del recorrido.

		if (
			registro.siguiente_x >= TABLA_TAMANO_X 	||
			registro.siguiente_y >= TABLA_TAMANO_Y 	||
			registro.siguiente_x < 0 				||
			registro.siguiente_y < 0
		) {
			cout << ERROR; // MSG_ERROR
			cout << "La tabla intenta recorrer una o más celdas fuera de los límites de la tabla." << endl;
			return false;
		}
		if (
			!(registro.siguiente_x == x 	&& registro.siguiente_y == y - 1) 	&&	// Norte.
			!(registro.siguiente_x == x + 1 && registro.siguiente_y == y) 		&&	// Este.
			!(registro.siguiente_x == x 	&& registro.siguiente_y == y + 1) 	&&	// Sur.
			!(registro.siguiente_x == x - 1 && registro.siguiente_y == y)			// Oeste.
		) {
			cout << ERROR; // MSG_ERROR
			cout << "La celda ";
			cout << "(" << x + 1 << ";" << y + 1 << ")";
			cout << " intenta recorrer una celda no adyacente." << endl;
			return false;
		}

		x = registro.siguiente_x;
		y = registro.siguiente_y;
	}

	if (celdas_recorridas != TABLA_TAMANO_X * TABLA_TAMANO_Y) {
		cout << ERROR; // MSG_ERROR
		cout << "La tabla de órdenes no recorre su totalidad ";
		cout << "(";
		cout << celdas_recorridas << "/";
		cout << TABLA_TAMANO_X * TABLA_TAMANO_Y << " celdas recorridas).";
		cout << endl;
		return false;
	}

	return true;
}


bool indexarTabla(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y], Flags &flags) {
	Orden registro;

	for (int y = 0; y < TABLA_TAMANO_Y; y++) {
		for (int x = 0; x < TABLA_TAMANO_X; x++) {
			registro = tabla[x][y];
			if (registro.despegue) {
				if (flags.despegue) {
					cout << ERROR; // MSG_ERROR
					cout << "La tabla contiene más de 1 celda de despegue." << endl;
					return false;
				}
				flags.despegue = true;
				if (
					registro.aterrizaje 		||
					registro.ataque_kamikaze 	||
					registro.soltar_granada1 	||
					registro.soltar_granada2
				) {
					cout << ERROR; // MSG_ERROR
					cout << "La celda de despegue contiene multiples instrucciones." << endl;
					return false;
				}
				flags.despegue_x = x;
				flags.despegue_y = y;
			}

			if (registro.aterrizaje) {
				if (flags.aterrizaje || flags.kamikaze) {
					cout << ERROR; // MSG_ERROR
					cout << "La tabla contiene más de 1 celda de aterrizaje o de ataque kamikaze." << endl;
					return false;
				}
				flags.aterrizaje = true;
			}
			if (registro.ataque_kamikaze) {
				if (flags.aterrizaje || flags.kamikaze) {
					cout << ERROR; // MSG_ERROR
					cout << "La tabla contiene más de 1 celda de aterrizaje o de ataque kamikaze." << endl;
					return false;
				}
				flags.kamikaze = true;
			}

			if (registro.soltar_granada1) {
				if (flags.granada1) {
					cout << ERROR; // MSG_ERROR
					cout << "La tabla contiene más de 1 celda de granada nº1." << endl;
					return false;
				}
				flags.granada1 = true;
			}
			if (registro.soltar_granada2) {
				if (flags.granada2) {
					cout << ERROR; // MSG_ERROR
					cout << "La tabla contiene más de 1 celda de granada nº2." << endl;
					return false;
				}
				flags.granada1 = true;
			}
		}
	}

	return true;
}


// Valida una tabla dada una serie de reglas:
// * Tener exactamente 1 celda de despegue.
// * Una celda de despegue no puede contener ninguna otra instrucción.
// * Tener exactamente 1 celda de finalización (Aterrizaje o kamikaze).
// * Ser recorrida en su totalidad desde la celda de despegue hasta la de finalización.
// * Recorrer cada celda exactamente 1 vez.
// * No recorrer celdas fuera de los limites.
// * No recorrer celdas no adyacentes.
// * Tener no más de 1 celda de granada 1 y 2.
// En caso de fallar alguna de estas validaciones, retorna "false" junto un mensaje de error.
bool validarTabla(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]) {
	// unsigned int despegue_x = -1, despegue_y = -1;
	// bool flag_despegue = false;
	// bool flag_final = false;
	// bool flag_granada1 = false;
	// bool flag_granada2 = false;
	Flags flags;

	if (!indexarTabla(tabla, flags)) return false;

	if (!flags.despegue) {
		cout << ERROR; // MSG_ERROR
		cout << "La tabla no contiene celda de despegue." << endl;
		return false;
	}
	if (!(flags.aterrizaje || flags.kamikaze)) {
		cout << ERROR; // MSG_ERROR
		cout << "La tabla no contiene celda de aterrizaje o de ataque kamikaze." << endl;
		return false;
	}

	if (!recorrerTabla(tabla, flags)) return false;

	return true;
}


// Solicita valores al usuario para  un registro.
void poblarRegistro(Orden &registro, int x, int y, Flags &flags) {
	char respuesta = '\0';
	unsigned int respuesta_tiempo = 0;
	unsigned int respuesta_x = 0, respuesta_y = 0;

	cout << "Posición (" << x + 1 << ";" << y + 1 << "):" << endl;

	while (!flags.despegue) { // Despegue.
		cout << "  ¿Despegue? [s/N]" << endl;
		cout << "  " << CURSOR;
		cin.get(respuesta);

		switch (respuesta) {
			default:
				cout << ERROR; // MSG_ERROR
				cout << "Respuesta no valida. Vuelva a intentarlo." << endl;
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Leading whitespace.
				continue;
			case 's':
			case 'S':
				registro.despegue = true;
				registro.aterrizaje = false;
				registro.ataque_kamikaze = false;
				registro.soltar_granada1 = false;
				registro.soltar_granada2 = false;
				flags.despegue = true;
				flags.posiciones_transitadas[x][y] = true;
				break;
			case 'n':
			case 'N':
			case '\n':
				registro.despegue = false;
				break;
		}
		if (respuesta != '\n') cin.ignore(); // Leading whitespace.
		break;
	}

	// Aterrizaje.
	while (
		!flags.aterrizaje 	&&
		!flags.kamikaze 	&&
		!registro.despegue
	) {
		cout << "  ¿Aterrizaje? [s/N]" << endl;
		cout << "  " << CURSOR;
		cin.get(respuesta);

		switch (respuesta) {
			default:
				cout << ERROR; // MSG_ERROR
				cout << "Respuesta no valida. Vuelva a intentarlo." << endl;
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Leading whitespace.
				continue;
			case 's':
			case 'S':
				registro.despegue = false;
				registro.aterrizaje = true;
				registro.ataque_kamikaze = false;
				flags.aterrizaje = true;
				break;
			case 'n':
			case 'N':
			case '\n':
				registro.aterrizaje = false;
				break;
		}
		if (respuesta != '\n') cin.ignore(); // Leading whitespace.
		break;
	}

	// Kamikaze.
	while (
		!flags.aterrizaje 	&&
		!flags.kamikaze 	&&
		!registro.despegue
	) {
		cout << "  ¿Ataque kamikaze? [s/N]" << endl;
		cout << "  " << CURSOR;
		cin.get(respuesta);

		switch (respuesta) {
			default:
				cout << ERROR; // MSG_ERROR
				cout << "Respuesta no valida. Vuelva a intentarlo." << endl;
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Leading whitespace.
				continue;
			case 's':
			case 'S':
				registro.despegue = false;
				registro.aterrizaje = false;
				registro.ataque_kamikaze = true;
				flags.kamikaze = true;
				break;
			case 'n':
			case 'N':
			case '\n':
				registro.ataque_kamikaze = false;
				break;
		}
		if (respuesta != '\n') cin.ignore(); // Leading whitespace.
		break;
	}

	// Granada nº1.
	while (
		!flags.granada1 &&
		!registro.despegue
	) {
		cout << "  ¿Soltar granada nº1? [s/N]" << endl;
		cout << "  " << CURSOR;
		cin.get(respuesta);

		switch (respuesta) {
			default:
				cout << ERROR; // MSG_ERROR
				cout << "Respuesta no valida. Vuelva a intentarlo." << endl;
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Leading whitespace.
				continue;
			case 's':
			case 'S':
				registro.despegue = false;
				registro.soltar_granada1 = true;
				flags.granada1 = true;
				break;
			case 'n':
			case 'N':
			case '\n':
				registro.soltar_granada1 = false;
				break;
		}
		if (respuesta != '\n') cin.ignore(); // Leading whitespace.
		break;
	}

	// Granada nº2.
	while (
		!flags.granada2 &&
		!registro.despegue
	) {
		cout << "  ¿Soltar granada nº2? [s/N]" << endl;
		cout << "  " << CURSOR;
		cin.get(respuesta);

		switch (respuesta) {
			default:
				cout << ERROR; // MSG_ERROR
				cout << "Respuesta no valida. Vuelva a intentarlo." << endl;
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Leading whitespace.
				continue;
			case 's':
			case 'S':
				registro.despegue = false;
				registro.soltar_granada2 = true;
				flags.granada2 = true;
				break;
			case 'n':
			case 'N':
			case '\n':
				registro.soltar_granada2 = false;
				break;
		}
		if (respuesta != '\n') cin.ignore(); // Leading whitespace.
		break;
	}

	// Tiempo de espera.
	while (true) {
		cout << "  Tiempo de espera (Segundos)." << endl;
		cout << "  " << CURSOR;
		cin >> respuesta_tiempo;
		if (cinInvalido()) continue;
		if (respuesta_tiempo == 0) {
			cout << ERROR; // MSG_ERROR
			cout << "Ingrese un valor de tiempo de espera diferente a 0." << endl;
			continue;
		}
		registro.espera = respuesta_tiempo;
		break;
	}

	// Siguiente posición.
	while (
		!registro.aterrizaje &&
		!registro.ataque_kamikaze
	) {
		cout << "  Posición siguiente (X;Y)." << endl;
		cout << "  " << CURSOR << "X: ";
		cin >> respuesta_x;
		if (cinInvalido()) continue;
		cout << "  " << CURSOR << "Y: ";
		cin >> respuesta_y;
		if (cinInvalido()) continue;
		respuesta_x--;
		respuesta_y--;
		// (Se interpreta que la tabla comienza en 1x1 y no en 0x0).
	
		if (
			!(respuesta_x == x 		&& respuesta_y == y - 1) 	&&	// Norte.
			!(respuesta_x == x + 1 	&& respuesta_y == y) 		&&	// Este.
			!(respuesta_x == x 		&& respuesta_y == y + 1) 	&&	// Sur.
			!(respuesta_x == x - 1 	&& respuesta_y == y)			// Oeste.
		) {
			cout << ERROR; // MSG_ERROR
			cout << "Seleccione una celda adyasente." << endl;
			continue;
		}
	
		if (
			respuesta_x >= TABLA_TAMANO_X ||
			respuesta_y >= TABLA_TAMANO_Y
		) {
			cout << ERROR; // MSG_ERROR
			cout << "Celda fuera de límites. Elija otra posición." << endl;
			continue;
		}
	
		if (flags.posiciones_transitadas[respuesta_x][respuesta_y]) {
			cout << ERROR; // MSG_ERROR
			cout << "Celda ya transitada. Elija otra posición." << endl;
			continue;
		}
	
		registro.siguiente_x = respuesta_x;
		registro.siguiente_y = respuesta_y;
		flags.posiciones_transitadas[respuesta_x][respuesta_y] = true;
		break;
	}

	cin.ignore(); // Leading whitespace.

	return;
}


// Solicita valores al usuario para poblar una tabla, registro a registro.
void poblarTabla(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]) {
	Flags flags;

	while (true) {
		for (int y = 0; y < TABLA_TAMANO_Y; y++) {
			for (int x = 0; x < TABLA_TAMANO_X; x++) {
				poblarRegistro(tabla[x][y], x, y, flags);
			}
		}

		if (!validarTabla(tabla)) {
			cout << ERROR; // MSG_ERROR
			cout << "La tabla de órdenes creada es inválida. Vuelva a crearla." << endl;
			// Este mensaje después de introducir 40 mil registros
			// debe ser horrible, por decirlo de manera no bulgar.
			continue;
		}
		break;
	}

	return;
}


// Muestra el contenido de un registro por pantalla.
void mostrarRegistro(Orden registro, int x, int y) {
	int digitos = 0;

	// Un poquito de diseño de tui.
	cout << "┌──────────────────────────────────────┐" << endl;

	cout << "│ " << "Posición actual (X;Y): ";
	for (int i = x + 1; i != 0; i /= 10) digitos++;
	for (int i = y + 1; i != 0; i /= 10) digitos++;
	digitos += 3; // "(", ";" y ")".
	for (int j = 0; j < 14 - digitos - 1; j++) cout << " ";
	digitos = 0;
	cout << "(" << x + 1 << ";" << y + 1 << ")" << endl;
	// Se le suma 1 a las coordenadas para volverlas amistosas para el usuario.
	// (Se interpreta que la tabla comienza en 1x1 y no en 0x0).

	cout << "│ " << "Despegue:                   ";
	cout << (registro.despegue ? "Veradero" : "   Falso") << endl;
	// Operador ternario para legibilidad.

	cout << "│ " << "Aterrizaje:                 ";
	cout << (registro.aterrizaje ? "Veradero" : "   Falso") << endl;

	cout << "│ " << "Soltar granada nº1:         ";
	cout << (registro.soltar_granada1  ? "Veradero" : "   Falso") << endl;

	cout << "│ " << "Soltar granada nº2:         ";
	cout << (registro.soltar_granada2 ? "Veradero" : "   Falso") << endl;

	cout << "│ " << "Ataque kamikaze:            ";
	cout << (registro.ataque_kamikaze ? "Veradero" : "   Falso") << endl;

	cout << "│ " <<  "Espera [Segundos]: ";
	for (int i = registro.espera; i != 0; i /= 10) digitos++; 	// Para alinear el texto a la derecha
	for (int j = 0; j < 16 - digitos - 1; j++) cout << " "; 	// sin importar la cantidad de tiempo.
	digitos = 0;
	cout << registro.espera << " s" << endl;

	cout << "│ " << "Posición siguiente (X;Y): ";
	if (registro.aterrizaje || registro.ataque_kamikaze) cout << "     Final";
	else {
		for (int i = registro.siguiente_x + 1; i != 0; i /= 10) digitos++;
		for (int i = registro.siguiente_y + 1; i != 0; i /= 10) digitos++;
		digitos += 3; // "(", ";" y ")".
		for (int j = 0; j < 11 - digitos - 1; j++) cout << " ";
		digitos = 0;
		cout << "(" << registro.siguiente_x + 1 << ";" << registro.siguiente_y + 1 << ")";
	}
	cout << endl;

	cout << "└─ Presione Enter para continuar… ─────┘ ";
	cin.ignore(); // Presione Enter para continuar.
	cout << ANSI_LINEA_ANTERIOR;
	cout << "└──────────────────────────────────────┘ " << endl;

	return;
};


// Muestra el contenido de una tabla de órdenes por pantalla.
void mostrarTabla(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]) {
	for (int y = 0; y < TABLA_TAMANO_Y; y++) {
		for (int x = 0; x < TABLA_TAMANO_X; x++) {
			mostrarRegistro(tabla[x][y], x, y);
		}
	}

	return;
}

// ──────────────────────────────────────────────────


// ─────────────────── Archivos ─────────────────────

// Comprueba la existencia de un archivo en una dirección dada.
// Retorna "false" si no existe.
bool buscarArchivo(char direccion_archivo[RUTA_MAX], char modo) {
	FILE *archivo;
	int tamaño_archivo = 0;
	char respuesta = '\0';

	cout << CURSOR;
	cin.getline(direccion_archivo, RUTA_MAX);
	//cin >> direccion_archivo;

	switch (modo) {
		case 'r':
			if (!(archivo = fopen(direccion_archivo, "r"))) {
				cout << ERROR; // MSG_ERROR
				cout << "La dirección del archivo ingresada es inválida o el archivo no existe. ";
				cout << "Por favor, vuelva a intentarlo o ingrese una ruta válida." << endl;
				return false;
			}

			fseek(archivo, 0, SEEK_END);
			tamaño_archivo = ftell(archivo) / sizeof(OrdenArchivo); // Tamaño del archivo en celdas.
			if (tamaño_archivo != TABLA_TAMANO_X * TABLA_TAMANO_Y) {
				cout << ERROR; // MSG_ERROR
				cout << "El archivo cargado tiene un tamaño invalido. ";
				cout << "Por favor, ingrese un archivo de ataque válido." << endl;
				fclose(archivo);
				return false;
			}
			fclose(archivo);
			break;
		case 'w':
			archivo = fopen(direccion_archivo, "r");
			if (archivo == NULL) {
				archivo = fopen(direccion_archivo, "w");
				if (archivo == NULL) { // No existe el directorio, no se puede crear
					cout << ERROR; // MSG_ERROR
					cout << "La dirección del archivo ingresada es inválida. ";
					cout << "Por favor, vuelva a intentarlo o ingrese una ruta válida." << endl;
					return false;
				}
				break; // No existe el archivo, se puede crear.
			}

			fclose(archivo); // Si existe.
			archivo = NULL;
			cout << WARN; // MSG_WARN
			cout << "El archivo ya existe. ¿Desea sobreescribirlo? [s/N]" << endl;
			do {
				cout << CURSOR;
				cin.get(respuesta);
				switch (respuesta) {
					default:
						cout << ERROR; // MSG_ERROR
						cout << "Respuesta no valida. Vuelva a intentarlo." << endl;
						cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Leading whitespace.
						continue;
					case 's':
					case 'S':
						if (!(archivo = fopen(direccion_archivo, "w"))) {
							cout << ERROR; // MSG_ERROR
							cout << "No se pudo sobreescribir el archivo." << endl;
							archivo = NULL;
							break; // "archivo_ataque" == NULL.
						}
						fclose(archivo);
						break; // "archivo_ataque" != NULL.
					case 'n':
					case 'N':
					case '\n':
						cout << "Ingrese una ruta diferente." << endl;
						return false;
				}
				if (respuesta != '\n') cin.ignore(); // Leading whitespace.
			} while (archivo == NULL);
	}

	return true;
}


// Lee a memoria un archivo dada una dirección.
bool leerArchivo(
	char direccion_archivo[RUTA_MAX],
	OrdenArchivo lista[TABLA_TAMANO_X * TABLA_TAMANO_Y]
) {
	FILE *archivo;

	cout << "Cargando archivo de ataque ";
	cout << "\"" << direccion_archivo << "\"";
	cout << "…" << endl;

	archivo = fopen(direccion_archivo, "r+b");
	if (!fread(lista, sizeof(OrdenArchivo), TABLA_TAMANO_X * TABLA_TAMANO_Y, archivo)) {
		cout << ERROR; // MSG_ERROR
		cout << "Error al leer ";
		cout << "\"" << direccion_archivo << "\"" << endl;
		cout << ". Puede que el archivo se haya movido o borrado. Vuelva a intentarlo." << endl;
		return false;
	}
	//cout << "Archivo cargado correctamente." << endl;
	//cout << "Presione Enter para continuar… "; // MSG_SUCCESS
	//cin.ignore();
	fclose(archivo);

	return true;
}


bool escribirArchivo(
	char direccion_archivo[RUTA_MAX],
	OrdenArchivo lista[TABLA_TAMANO_X * TABLA_TAMANO_Y]
) {
	FILE *archivo;

	cout << "Guardando tabla de órdenes en ";
	cout << "\"" << direccion_archivo << "\"";
	cout << "…" << endl;

	archivo = fopen(direccion_archivo, "w+b");
	if (!fwrite(lista, sizeof(OrdenArchivo), TABLA_TAMANO_X * TABLA_TAMANO_Y, archivo)) {
		cout << ERROR; // MSG_ERROR
		cout << "Error al guardar la tabla en ";
		cout << "\"" << direccion_archivo << "\"" << endl;
		cout << ". Puede que el archivo se haya movido o borrado. Vuelva a intentarlo." << endl;
		return false;
	}
	cout << "Archivo guardado correctamente." << endl;
	cout << "Presione Enter para continuar… "; // MSG_SUCCESS
	cin.ignore();
	fclose(archivo);

	return true;
}


void cargarArchivo(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y], char direccion_archivo[RUTA_MAX]){
	OrdenArchivo lista_archivo[TABLA_TAMANO_X * TABLA_TAMANO_Y];

	// Acá estoy usando un While de una manera particular. Generalmente se le pone una
	// condición de salida y se comprueba cada loop, utilizando sentencias
	// selectivas para lograr dicha condición de salida.
	// En este caso ocurre lo contrario, no hay condición de salida y las sentencias
	// selctivas son las que repiten el bucle, pero al llegar al final del bucle
	// se consigue salir por la sentencia Break. Se eligió esta forma por la sencillez y
	// claridad visual de su implementación, ya que permite secuenciar multiples
	// condiciones de salida en sentencias If de manera clara.
	cout << "Por favor, ingrese una ruta de un archivo a cargar." << endl;
	while (true) {
		if (!buscarArchivo(direccion_archivo, 'r')) continue; // Vuelve a empezar el loop while.
		if (!leerArchivo(direccion_archivo, lista_archivo)) continue;
		listaATabla(lista_archivo, tabla);
		if (!validarTabla(tabla)) continue;
		// El mensaje de "archivo guardardo" está acá y no en "leerArchivo()" porque
		// se debe mostrar posterior a la comprovación de validez.
		cout << "Archivo cargado correctamente." << endl;
		cout << "Presione Enter para continuar… "; // MSG_SUCCESS
		cin.ignore();
		break;
	}

	return;
}


// Solicita valores al usuario para crear una tabla que luego almacena en
// un archivo nuevo.
void crearAtaque() {
	// "crearAtaque()" no pide como parametro la tabla de órdenes de la memoria,
	// ya que esta no se utiliza fuera de su propio procedimiento.
	// Está contenida dentro de la creación del archivo en donde se almacenará.
	// Para interactuar con el resto de funciones, como "mostrarTabla()",
	// por ejemplo, se debe cargar como cualquier otro archivo.
	char direccion_archivo[RUTA_MAX] = { '\0' };
	OrdenArchivo lista_archivo[TABLA_TAMANO_X * TABLA_TAMANO_Y];
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y];

	poblarTabla(tabla);
	tablaALista(lista_archivo, tabla);

	cout << "Por favor, ingrese la ruta donde guardar el archivo." << endl;
	while (true) {
		if (!buscarArchivo(direccion_archivo, 'w')) continue;
		if (!escribirArchivo(direccion_archivo, lista_archivo)) continue;
		break;
	}

	return;
}


// Almacena la tabla de memoria que luego almacena en un archivo nuevo.
void guardarAtaque(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y], char direccion_archivo[RUTA_MAX]) {
	OrdenArchivo lista_archivo[TABLA_TAMANO_X * TABLA_TAMANO_Y];

	cout << "Por favor, ingrese la ruta donde guardar el archivo." << endl;
	while(true) {
		if (!buscarArchivo(direccion_archivo, 'w')) continue;
		tablaALista(lista_archivo, tabla);
		if (!escribirArchivo(direccion_archivo, lista_archivo)) continue;
		break;
	}

	return;
}


// Solicita uno o más registros de una tabla dada para modificar.
bool modificarRegistro(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]) {
	char respuesta = '\0';
	unsigned int respuesta_x = 0, respuesta_y = 0;
	Orden tabla_nueva[TABLA_TAMANO_X][TABLA_TAMANO_Y];
	Flags *flags;

	// Para poder anular los cambios en caso de que sean inválidos.
	clonarTabla(tabla, tabla_nueva);

	do {
		while (true) { // Qué registro.
			cout << "¿Qué registro desea modificar? (X;Y)" << endl;
			cout << CURSOR << "X: ";
			cin >> respuesta_x;
			if (cinInvalido()) continue;
			cout << CURSOR << "Y: ";
			cin >> respuesta_y;
			if (cinInvalido()) continue;
			respuesta_x--; // (Se interpreta que la tabla comienza en 1x1 y no en 0x0,
			respuesta_y--; // así que se decrementan ambos).
	
			if (
				respuesta_x >= TABLA_TAMANO_X ||
				respuesta_y >= TABLA_TAMANO_Y
			) {
				cout << ERROR; // MSG_ERROR
				cout << "Celda fuera de límites. Elija otra posición." << endl;
				continue;
			}
			cin.ignore(); // Leading whitespace.

			break;
		}

		cout << "Registro original:" << endl;
		mostrarRegistro(tabla_nueva[respuesta_x][respuesta_y], respuesta_x, respuesta_y);

		flags = new Flags();
		// ~He construído un castillo desde afuera hacia adentro, y ahora que yazco en la última de las cámaras,
		// sin puertas ni ventanas, solo me resta una única salida: cavar un agujero~
		poblarRegistro(tabla_nueva[respuesta_x][respuesta_y], respuesta_x, respuesta_y, *flags);
		delete flags;

		cout << endl << "Resultado:" << endl;
		mostrarRegistro(tabla_nueva[respuesta_x][respuesta_y], respuesta_x, respuesta_y);

		while (true) {
			cout << "¿Cambiar otro registro? [s/N]" << endl;
			cout << CURSOR;
			cin.get(respuesta);
			switch (respuesta) {
				default:
					cout << ERROR; // MSG_ERROR
					cout << "Respuesta no valida. Vuelva a intentarlo." << endl;
					cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Leading whitespace.
					continue;
				case 's':
				case 'S':
				case 'n':
				case 'N':
				case '\n':
					break;
			}
			if (respuesta != '\n') cin.ignore(); // Leading whitespace.
			break;
		}
	} while (respuesta == 's' || respuesta == 'S');

	while (true) { // Guardar cambios.
		cout << "¿Guardar cambios? [S/n]" << endl;
		cout << CURSOR;
		cin.get(respuesta);

		switch (respuesta) {
			default:
				cout << ERROR; // MSG_ERROR
				cout << "Respuesta no valida. Vuelva a intentarlo." << endl;
				cin.ignore(); // Leading whitespace.
				continue;
			case 's':
			case 'S':
			case '\n':
				cout << "Validando cambios…" << endl;
				if (!validarTabla(tabla_nueva)) {
					cout << ERROR; // MSG_ERROR
					cout << "Las modificaciones hechas a la tabla son inválidas.";
					cout << " Deshaciendo los cambios." << endl;
					clonarTabla(tabla, tabla_nueva);
					return false;
				}
				cout << "Cambios validados. Todo en orden." << endl;
				clonarTabla(tabla_nueva, tabla);
				cout << "Presione Enter para continuar… "; // MSG_SUCCESS
				cin.ignore();
				return true;
			case 'n':
			case 'N':
				clonarTabla(tabla, tabla_nueva);
				cout << "Se han deshecho los cambios." << endl;
				return false;
		}
	}
}


// Solicita uno o más registros de una tabla solicitada de un archivo para modificar.
void modificarRegistroDeArchivo() {
	char direccion_archivo[RUTA_MAX] = { '\0' };
	OrdenArchivo lista_archivo[TABLA_TAMANO_X * TABLA_TAMANO_Y];
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y];

	cout << "Por favor, ingrese una ruta de un archivo a cargar." << endl;
	while (true) {
		if (!buscarArchivo(direccion_archivo, 'r')) continue;
		if (!leerArchivo(direccion_archivo, lista_archivo)) continue;
		listaATabla(lista_archivo, tabla);
		if (!validarTabla(tabla)) continue;
		if (!modificarRegistro(tabla)) return;
		tablaALista(lista_archivo, tabla);
		if (!escribirArchivo(direccion_archivo, lista_archivo)) continue;
		break;
	}
	// Archivo -> Memoria (Lista) -> Memoria (Tabla) -> Se edita -> Memoria (Tabla) -> Memoria (Lista) -> Archivo.
	// Efectivamente ineficiente.

	return;
}

// ──────────────────────────────────────────────────


// ───────────────────── HTML ───────────────────────

void crearHTML(
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y],
	char direccion_archivo[RUTA_MAX]
) {
	const char *nombre_archivo[RUTA_MAX] = { "\0" };
	char direccion_pagina[RUTA_MAX] = { '\0' };
	Orden registro, registro_siguiente;
	FILE *archivo_pagina;
	Flags flags;
	unsigned int x = 0, y = 0;
	bool norte = false, sur = false, este = false, oeste = false;
	const char *char_registro[TABLA_TAMANO_X][TABLA_TAMANO_Y] = { "\0" };

	*nombre_archivo = strrchr(direccion_archivo, '/'); // Si no hay un '/' en la dirección
	if (nombre_archivo[0] == NULL) *nombre_archivo = direccion_archivo; // se asigna completa.
	else nombre_archivo[0] = *nombre_archivo + 1;
	strcpy(direccion_pagina, direccion_archivo); // STRING
	strcat(direccion_pagina, ".html"); // En un mundo ideal donde todos usamos strings. // STRING

	indexarTabla(tabla, flags); // Obtener "despegue_x" y "despegue_y".
	x = flags.despegue_x;
	y = flags.despegue_y;
	archivo_pagina = fopen(direccion_pagina, "w");

	while (true) {
		registro = tabla[x][y];
		registro_siguiente = tabla[registro.siguiente_x][registro.siguiente_y];
		if (registro.aterrizaje || registro.ataque_kamikaze) registro_siguiente = registro;

		if (
			registro.siguiente_x == x 									&&
			registro.siguiente_y == y - 1 								||
			registro_siguiente.siguiente_x == registro.siguiente_x 		&&
			registro_siguiente.siguiente_y == registro.siguiente_y + 1
		) {
			sur = true;
		}
		if (
			registro.siguiente_x == x + 1 								&&
			registro.siguiente_y == y									||
			registro_siguiente.siguiente_x == registro.siguiente_x - 1 	&&
			registro_siguiente.siguiente_y == registro.siguiente_y
		) {
			oeste = true;
		}
		if (
			registro.siguiente_x == x 									&&
			registro.siguiente_y == y + 1								||
			registro_siguiente.siguiente_x == registro.siguiente_x 		&&
			registro_siguiente.siguiente_y == registro.siguiente_y - 1
		) {
			norte = true;
		}
		if (
			registro.siguiente_x == x - 1 								&&
			registro.siguiente_y == y									||
			registro_siguiente.siguiente_x == registro.siguiente_x + 1 	&&
			registro_siguiente.siguiente_y == registro.siguiente_y
		) {
			este = true;
		}

		if 	(norte && sur) 		char_registro[registro.siguiente_x][registro.siguiente_y] = "│";
		if 	(sur && este) 		char_registro[registro.siguiente_x][registro.siguiente_y] = " ┌─";
		if 	(norte && este) 	char_registro[registro.siguiente_x][registro.siguiente_y] = " └─";
		if 	(sur && oeste) 		char_registro[registro.siguiente_x][registro.siguiente_y] = "─┐ ";
		if 	(norte && oeste) 	char_registro[registro.siguiente_x][registro.siguiente_y] = "─┘ ";
		if 	(oeste && este) 	char_registro[registro.siguiente_x][registro.siguiente_y] = "───";

		if 		(registro.despegue) 		char_registro[x][y] = "●";
		else if (registro.aterrizaje) 		char_registro[x][y] = "⯀";
		else if (registro.ataque_kamikaze) 	char_registro[x][y] = "⯁";
		else if (registro.soltar_granada1) 	char_registro[x][y] = "⯌";
		else if (registro.soltar_granada2) 	char_registro[x][y] = "⯍";

		if (registro.aterrizaje || registro.ataque_kamikaze) break; // Final del recorrido.

		norte 	= false;
		sur 	= false;
		este 	= false;
		oeste 	= false;
		x = registro.siguiente_x;
		y = registro.siguiente_y;
	}

	fprintf(archivo_pagina, "<html>\n\t<head>\n");
	fprintf(archivo_pagina, "\t<title>%s - TP Dron</title>", *nombre_archivo);
	fprintf(archivo_pagina, R"(
	</head>
	<body>
	<header style="padding: 5px; width: 100%;">
		<h1>Vista previa de la tabla de órdenes del dron.</h1>)");
	fprintf(archivo_pagina, "\n\t\t<h2>Tabla \"%s\"</h2>", *nombre_archivo);
	fprintf(archivo_pagina, R"(
		<hr width="100%"/>
	</header>
	<section style="display: flex; gap: 10px;">
		<div class="borde">
		<div class="scrolleable">
		<table class="table">
			<thead>
			<th><div class="content"></div></th>)");
	for (int x = 0; x < TABLA_TAMANO_X; x++) {
		fprintf(archivo_pagina, "\n\t\t\t<th><div class=\"content\">%d</div></th>", x + 1); // Columna
	}
	fprintf(archivo_pagina, "\n\t\t\t</thead>");
	for (int k = 0; k < TABLA_TAMANO_Y; k++) {
		fprintf(archivo_pagina, "\n\t\t\t<tr>\n\t\t\t<td><div class=\"content\">%d</div></td>", k + 1); // Fila.
		for (int j = 0; j < TABLA_TAMANO_X; j++) {
			fprintf(archivo_pagina, "\n\t\t\t<td title=\"Celda %d×%d", j + 1, k + 1); // Numero de celda.
			if (!tabla[j][k].aterrizaje && !tabla[j][k].ataque_kamikaze) {
				fprintf(archivo_pagina, "&#10;Siguiente %d×%d",
					tabla[j][k].siguiente_x + 1, tabla[j][k].siguiente_y + 1
				);
			}
			if (tabla[j][k].despegue) 			fprintf(archivo_pagina, "&#10;Celda de despegue");
			if (tabla[j][k].aterrizaje) 		fprintf(archivo_pagina, "&#10;Celda de aterrizaje");
			if (tabla[j][k].ataque_kamikaze) 	fprintf(archivo_pagina, "&#10;Ataque kamikaze");
			if (tabla[j][k].soltar_granada1) 	fprintf(archivo_pagina, "&#10;Ataque granada nº1");
			if (tabla[j][k].soltar_granada2) 	fprintf(archivo_pagina, "&#10;Ataque granada nº2");
			fprintf(archivo_pagina, "&#10;Tiempo de espera: %d s\"><div class=\"content\">%s</div></td>",
				tabla[j][k].espera,
				char_registro[j][k]
			);
		}
		fprintf(archivo_pagina, "\n\t\t\t</tr>");
	}
	fprintf(archivo_pagina, R"(
		</table>
		</div>
		</div>
		<div class="referencia">
		<h3 style="margin: 10px;">Referencia.</h3>
		<table>
		<tr><td><div class="content">●</div></td><td><p>Despegue</p></td></tr>
		<tr><td><div class="content">⯀</div></td><td><p>Aterrizaje</p></td></tr>
		<tr><td><div class="content">⯁</div></td><td><p>Ataque kamikaze</p></td></tr>
		<tr><td><div class="content">⯌</div></td><td><p>Granada 1</p></td></tr>
		<tr><td><div class="content">⯍</div></td><td><p>Granada 2</p></td></tr>
		</table>
	</section>
	<footer style="padding: 5px; margin-top: auto; width: 100%;">
		<hr width="100%"/>
		<table width="100%">
		<tr>
		<td><p><small>Trabajo práctico AyED, K1153. <a href="https://github.com/jkainer/TP_Dron_K1153">Repositorio de GitHub.</a></small></p></td>
		<td><p align="right"><small>Joaquín Kainer, Legajo 233.968-7</small></p></td>
		</tr>
		</table>
	</footer>
	</body>
	<style>
		html {
		display: flex;
		justify-content: center;
		margin: 0;
		height: 100%;
		}
		body {
		display: inline-flex;
		flex-direction: column;
		gap: 5px;
		align-items: center;
		align-self: center;
		height: 100%;
		}
		.borde {
		border: 1px solid black;
		border-radius: 1px;
		height: min-content;
		overflow: hidden;
		}
		.scrolleable {
		max-height: 610px;
		max-width: 629px;
		overflow: auto;
		}
		.table {
		border-collapse: collapse;
		background-color: white;
		height: min-content;
		}
		.table td {
		border-top: 1px dashed gray;
		border-left: 1px dashed gray;
		border-bottom: 0px dashed gray;
		border-right: 0px dashed gray;
		padding: 0;
		}
		.content {
		width: 28px;
		user-select: none;
		font-family: monospace;
		font-size: 28px;
		white-space: pre;
		aspect-ratio: 1 / 1 ;
		overflow: hidden;
		display: flex;
		justify-content: center;
		align-items: center;
		}
		.referencia {
		border: 1px solid black;
		border-radius: 1px;
		overflow: hidden;
		display: flex;
		flex-direction: column;
		gap: 5px;
		height: fit-content;
		}
		.referencia table{
		border-top: 1px solid black;
		border-bottom: 1px solid white;
		border-collapse: collapse;
		background-color: white;
		}
		.referencia td {padding: 5px;border-bottom: 1px dashed gray;}
		.referencia td:nth-child(1) {border-right: 1px dashed gray;}
		.table th .content, .table td:nth-child(1) .content{font-size: 12px;}
		.table td:nth-child(1), .table th {
		background-color: #d9d9d9;
		position: sticky;
		border: 0px dashed gray;
		font-weight: 400;
		}
		.table td:nth-child(1) {left: 0;z-index: 2;}
		.table th {top: 0;z-index: 3;}
		.table th:nth-child(1) {top: 0;left: 0;z-index: 4;}
	</style>
</html>)");

	fclose(archivo_pagina);
	cout << "El archivo ";
	cout << "\"" << direccion_pagina << "\"";
	cout << " se guardó correctamente." << endl;
	cout << "Presione Enter para continuar… "; // MSG_SUCCESS
	cin.ignore();

	return;
}

// ──────────────────────────────────────────────────


// ──────────────── Menú principal ──────────────────

// Imprime un error y retorna "false" cuando no hay una tabla de
// órdenes cargada en memoria. Retorna "true" en caso contrario.
bool comprobarTablaCargada(bool flag) {
	if (!flag) {
		cout << WARN; // MSG_WARN
		cout << "No hay una tabla de órdenes cargada." << endl;
		cout << "Cargue primero una tabla de órdenes a la memoria del programa para acceder a esta función." << endl;
		return false;
	}

	return true;
}


// Menú principal.
void bucleMenu(
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y],
	bool &flag_tabla_cargada, bool &flag_tabla_modificada,
	char direccion_archivo[RUTA_MAX]
) {
	int eleccion = -1;

	do {
		// Con esta comparación se evita reimprimir el menú si
		// la opción ingresada es incorrecta, pero sí si se
		// acaba de salir de una opción real luego de utilizarla.
		if (eleccion < 8) {
			//cout << endl;
			cout << "Menú de opciones de control de memoria del dron." << endl;
			if (flag_tabla_cargada) {
				cout << "Archivo cargado: ";
				if (flag_tabla_modificada) cout << ANSI_ITALICS;
				cout << "\"" << direccion_archivo << "\"";
				if (flag_tabla_modificada) cout << ANSI_CLEAR << "*";
				cout << endl;
			}
			cout << "Por favor, selecciones una de las siguientes opciones: (ej: 1, 2, 3)" << endl;

			cout << "  [1]. Cargar archivo de ataque a memoria." << endl;

			if (!flag_tabla_cargada) cout << ANSI_ITALICS;
			// La idea es que en caso de no tener cargada en memoria un archivo
			// indicar visualmente que la opción no es accesible.
			cout << "  [2]. Mostrar archivo cargado." << ANSI_CLEAR << endl;

			cout << "  [3]. Crear un archivo de ataque nuevo." << endl;

			cout << "  [4]. Corregir un registro del archivo." << endl;

			if (!flag_tabla_cargada) cout << ANSI_ITALICS;
			cout << "  [5]. Corregir un registro en memoria." << ANSI_CLEAR << endl;

			if (!flag_tabla_cargada) cout << ANSI_ITALICS;
			cout << "  [6]. Guardar tabla en memoria en un archivo nuevo." << ANSI_CLEAR << endl;

			if (!flag_tabla_cargada) cout << ANSI_ITALICS;
			cout << "  [7]. Visualizar un archivo de ataque en HTML." << ANSI_CLEAR << endl;

			cout << "  [0]. Terminar este programa." << endl;
		}

		cout << CURSOR;
		cin >> eleccion;
		if (cinInvalido()) {
			eleccion = 8;
			continue;
		}
		// Se utiliza cin.ignore() para prevenir error de leading whitespace al presionar Enter.
		// Este comportamiento se puede obtener utilizando cin.get(), pero se decidió por
		// usar cin.ignore() ya que el valor obtenido no va a ser almacenado.
		cin.ignore();
		switch (eleccion) {
			default:
				cout << ERROR; // MSG_ERROR
				cout << "Opción no valida. Vuelva a intentarlo." << endl;
				break;
			case 0:
				cout << "Terminando ejecución." << endl;
				break;
			case 1: // Cargar archivo de ataque.
				cargarArchivo(tabla, direccion_archivo);
				flag_tabla_cargada = true;
				flag_tabla_modificada = false;
				break;
			case 2: // Mostrar archivo cargado.
				if (!comprobarTablaCargada(flag_tabla_cargada)) break;
				mostrarTabla(tabla);
				break;
			case 3: // Crear archivo de ataque nuevo y poblarlo.
				crearAtaque();
				break;
			case 4: // Modificar tabla en archivo.
				modificarRegistroDeArchivo();
				break;
			case 5: // Modificar tabla en memoria.
				if (!comprobarTablaCargada(flag_tabla_cargada)) break;
				flag_tabla_modificada = modificarRegistro(tabla);
				break;
			case 6: // Guardar archivo de ataque.
				if (!comprobarTablaCargada(flag_tabla_cargada)) break;
				guardarAtaque(tabla, direccion_archivo);
				flag_tabla_modificada = false;
				break;
			case 7: // Crear página HTML.
				if (!comprobarTablaCargada(flag_tabla_cargada)) break;
				crearHTML(tabla, direccion_archivo);
				break;
		}
	} while (eleccion != 0);

	return;
}


int main() {
	char direccion_archivo[RUTA_MAX] = { '\0' };
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]; // Tabla de órdenes en memoria.
	bool flag_tabla_cargada = false;
	bool flag_tabla_modificada = false;

	bucleMenu(tabla, flag_tabla_cargada, flag_tabla_modificada, direccion_archivo);

	return 0;
}

// ──────────────────────────────────────────────────