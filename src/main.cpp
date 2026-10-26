#include <iostream>
#include <stdio.h>
#define CURSOR "-> " // Cursor indicativo de interfaz. Puramente estético.
#define RUTA_MAX 256 // Tamaño máximo en caracteres posible para ruta de un archivo.
#define TABLA_TAMANO_X 5 // Tamaño horizontal de la tabla del órdenes.
#define TABLA_TAMANO_Y 5 // Tamaño vertical de la tabla del órdenes.
using std::cin, std::cout, std::endl;

// ──────────── Prototipos de órdenes ───────────────

struct Flags {
	bool despegue 		= false; // Solo puede haber una celda de despegue por tabla.
	bool aterrizaje 	= false;
	bool kamikaze 		= false;
	bool granada1 		= false;
	bool granada2 		= false;
	bool posiciones_transitadas[TABLA_TAMANO_X][TABLA_TAMANO_Y] = { false };
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
void archivoATabla(
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
void tablaAArchivo(
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
bool recorrerTabla(
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y],
	int despegue_x, int despegue_y
) {
	Orden registro;
	int x = despegue_x, y = despegue_y;
	bool flag_posiciones_transitadas[TABLA_TAMANO_X][TABLA_TAMANO_Y] = { false };
	unsigned int celdas_recorridas = 0;

	while (true) {
		registro = tabla[x][y];

		// DEBUG ----
		// cout << " # Celda ";
		// cout << "(" << x + 1 << ";" << y + 1 << ")";
		// if (registro.despegue) 			cout << " (Celda de despegue)";
		// if (registro.aterrizaje) 		cout << " (Celda de aterrizaje)";
		// if (registro.ataque_kamikaze) 	cout << " (Celda kamikaze)";
		// if (registro.soltar_granada1) 	cout << " (Granada 1)";
		// if (registro.soltar_granada2) 	cout << " (Granada 2)";
		// if (!registro.aterrizaje && !registro.ataque_kamikaze) {
		// 	cout << " → ";
		// 	cout << "Celda ";
		// 	cout << "(" << registro.siguiente_x + 1 << ";" << registro.siguiente_y + 1 << ")";
		// }
		// cout << endl;
		// ----------

		if (flag_posiciones_transitadas[x][y]) {
			// MSG_ERROR
			cout << "Error: La celda ";
			cout << "(" << x + 1 << ";" << y + 1 << ")";
			cout << " es recorrida más de 1 vez." << endl;
			return false;
		}
		flag_posiciones_transitadas[x][y] = true;
		celdas_recorridas++;

		if (registro.aterrizaje || registro.ataque_kamikaze) break; // Final del recorrido.

		if (
			registro.siguiente_x >= TABLA_TAMANO_X 	||
			registro.siguiente_y >= TABLA_TAMANO_Y 	||
			registro.siguiente_x < 0 				||
			registro.siguiente_y < 0
		) {
			// MSG_ERROR
			cout << "Error: La tabla intenta recorrer una o más celdas fuera de los límites de la tabla." << endl;
			return false;
		}
		if (
			!(registro.siguiente_x == x 	&& registro.siguiente_y == y - 1) 	&&	// Norte.
			!(registro.siguiente_x == x + 1 && registro.siguiente_y == y) 		&&	// Este.
			!(registro.siguiente_x == x 	&& registro.siguiente_y == y + 1) 	&&	// Sur.
			!(registro.siguiente_x == x - 1 && registro.siguiente_y == y)			// Oeste.
		) {
			// MSG_ERROR
			cout << "Error: La celda ";
			cout << "(" << x + 1 << ";" << y + 1 << ")";
			cout << " intenta recorrer una celda no adyacente." << endl;
			return false;
		}

		x = registro.siguiente_x;
		y = registro.siguiente_y;
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
	unsigned int despegue_x = -1, despegue_y = -1;
	bool flag_despegue = false;
	bool flag_final = false;
	bool flag_granada1 = false;
	bool flag_granada2 = false;
	Orden registro;

	for (int y = 0; y < TABLA_TAMANO_Y; y++) {
		for (int x = 0; x < TABLA_TAMANO_X; x++) {
			registro = tabla[x][y];
			if (registro.despegue) {
				if (flag_despegue) {
					// MSG_ERROR
					cout << "Error: La tabla contiene más de 1 celda de despegue." << endl;
					return false;
				}
				flag_despegue = true;
				if (
					registro.aterrizaje 		||
					registro.ataque_kamikaze 	||
					registro.soltar_granada1 	||
					registro.soltar_granada2
				) {
					// MSG_ERROR
					cout << "Error: La celda de despegue contiene multiples instrucciones." << endl;
					return false;
				}
				despegue_x = x;
				despegue_y = y;
			}

			if (registro.aterrizaje) {
				if (flag_final) {
					// MSG_ERROR
					cout << "Error: La tabla contiene más de 1 celda de aterrizaje o de ataque kamikaze." << endl;
					return false;
				}
				flag_final = true;
			}
			if (registro.ataque_kamikaze) {
				if (flag_final) {
					// MSG_ERROR
					cout << "Error: La tabla contiene más de 1 celda de aterrizaje o de ataque kamikaze." << endl;
					return false;
				}
				flag_final = true;
			}

			if (registro.soltar_granada1) {
				if (flag_granada1) {
					// MSG_ERROR
					cout << "Error: La tabla contiene más de 1 celda de granada nº1." << endl;
					return false;
				}
				flag_granada1 = true;
			}
			if (registro.soltar_granada2) {
				if (flag_granada2) {
					// MSG_ERROR
					cout << "Error: La tabla contiene más de 1 celda de granada nº2." << endl;
					return false;
				}
				flag_granada1 = true;
			}
		}
	}

	if (!flag_despegue) {
		// MSG_ERROR
		cout << "Error: La tabla no contiene celda de despegue." << endl;
		return false;
	}
	if (!flag_final) {
		// MSG_ERROR
		cout << "Error: La tabla no contiene celda de aterrizaje o de ataque kamikaze." << endl;
		return false;
	}
	if (!recorrerTabla(tabla, despegue_x, despegue_y)) return false;

	return true;
}


// Solicita valores al usuario para poblar un registro.
void poblarRegistro(Orden &registro, int x, int y, Flags &flags) {
	char respuesta = '\0';
	unsigned int respuesta_tiempo = 0;
	unsigned int respuesta_x = 0, respuesta_y = 0;

	cout << "Posición (" << x + 1 << ";" << y + 1 << "):" << endl;

	while (!flags.despegue) { // Despegue.
		cout << "  ¿Despegue? [s/N]" << endl;
		cout << "  " << CURSOR;
		cin.get(respuesta);

		switch (respuesta)
		{
			default:
				// MSG_ERROR
				cout << "Error: Respuesta no valida. Vuelva a intentarlo." << endl;
				cin.ignore(); // Leading whitespace.
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

		switch (respuesta)
		{
			default:
				// MSG_ERROR
				cout << "Error: Respuesta no valida. Vuelva a intentarlo." << endl;
				cin.ignore(); // Leading whitespace.
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

		switch (respuesta)
		{
			default:
				// MSG_ERROR
				cout << "Error: Respuesta no valida. Vuelva a intentarlo." << endl;
				cin.ignore(); // Leading whitespace.
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

		switch (respuesta)
		{
			default:
				// MSG_ERROR
				cout << "Error: Respuesta no valida. Vuelva a intentarlo." << endl;
				cin.ignore(); // Leading whitespace.
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

		switch (respuesta)
		{
			default:
				// MSG_ERROR
				cout << "Error: Respuesta no valida. Vuelva a intentarlo." << endl;
				cin.ignore(); // Leading whitespace.
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
		if (!cin) { // En caso de ingresar un caracter en vez de un número.
			cin.clear();
			cin.ignore();
			// MSG_ERROR
			cout << "Error: Respuesta no valida. Vuelva a intentarlo." << endl;
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
		cout << "  X: ";
		cin >> respuesta_x;
		if (!cin) { // En caso de ingresar un caracter en vez de un número.
			cin.clear();
			cin.ignore();
			// MSG_ERROR
			cout << "Error: Respuesta no valida. Vuelva a intentarlo." << endl;
			continue;
		}
		cout << "  Y: ";
		cin >> respuesta_y;
		if (!cin) { // En caso de ingresar un caracter en vez de un número.
			cin.clear();
			cin.ignore();
			// MSG_ERROR
			cout << "Error: Respuesta no valida. Vuelva a intentarlo." << endl;
			continue;
		}
		respuesta_x--;
		respuesta_y--;
		// (Se interpreta que la tabla comienza en 1x1 y no en 0x0).
	
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
			respuesta_y >= TABLA_TAMANO_Y
		) {
			// MSG_ERROR
			cout << "Error: Celda fuera de límites. Elija otra posición." << endl;
			continue;
		}
	
		if (flags.posiciones_transitadas[respuesta_x][respuesta_y]) {
			// MSG_ERROR
			cout << "Error: Celda ya transitada. Elija otra posición." << endl;
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
			// MSG_ERROR
			cout << "Error: La tabla de órdenes creada es inválida. Vuelva a crearla." << endl; 
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
	// Un poquito de diseño de tui.
	cout << "┌─────────────────────────────────────┐" << endl;

	cout << "│ " << "Posición actual (X;Y):         ";
	cout << "(" << x + 1 << ";" << y + 1 << ")" << endl;

	cout << "│ " << "Despegue:                  ";
	cout << (registro.despegue ? "Veradero" : "   Falso") << endl;
	// Operador ternario para legibilidad.

	cout << "│ " << "Aterrizaje:                ";
	cout << (registro.aterrizaje ? "Veradero" : "   Falso") << endl;

	cout << "│ " << "Soltar granada nº1:        ";
	cout << (registro.soltar_granada1  ? "Veradero" : "   Falso") << endl;

	cout << "│ " << "Soltar granada nº2:        ";
	cout << (registro.soltar_granada2 ? "Veradero" : "   Falso") << endl;

	cout << "│ " << "Ataque kamikaze:           ";
	cout << (registro.ataque_kamikaze ? "Veradero" : "   Falso") << endl;

	cout << "│ " <<  "Espera [Segundos]:              "; // TODO
	cout << registro.espera << " s" << endl;

	cout << "│ " << "Posición siguiente (X;Y):     ";
	if (registro.aterrizaje || registro.ataque_kamikaze) cout << "Final";
	else cout << "(" << registro.siguiente_x + 1 << ";" << registro.siguiente_y + 1 << ")";
	cout << endl;
	// Se le suma 1 a las coordenadas para volverlas amistosas para el usuario.
	// (Se interpreta que la tabla comienza en 1x1 y no en 0x0).

	cout << "└─ Presione Enter para continuar... ──┘ ";

	cin.ignore(); // Presione Enter para continuar.
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

// TODO: Mejorar sistemas de interacción con ficheros
void cargarArchivo(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y], bool &flag_tabla_cargada) {
	char direccion_archivo[RUTA_MAX];
	FILE *archivo_ataque;
	int tamaño_archivo = 0;
	OrdenArchivo lista_archivo[TABLA_TAMANO_X * TABLA_TAMANO_Y];

	flag_tabla_cargada = false;

	cout << "Por favor, indique ruta del archivo de ataque." << endl;
	// Acá estoy usando un While de una manera particular. Generalmente le pone una condición de
	// salida y se comprueba cada loop, y se utilizan sentencias selectivas para lograr
	// dicha condición de salida. En este caso ocurre lo contrario. No hay condición de salida
	// y las sentencias selctivas son las que repiten el bucle, pero al llegar al final del bucle
	// se consigue salir por la sentencia Break. Se eligió esta forma por la sencillez y
	// claridad visual de su implementación, ya que permite secuenciar multiples condiciones de
	// salida en sentencias If de manera clara.
	while (true) {
		cout << CURSOR;
		cin.getline(direccion_archivo, RUTA_MAX);
		//cin >> direccion_archivo;

		if (!(archivo_ataque = fopen(direccion_archivo, "r+b"))) {
			// MSG_ERROR
			cout << "Error: La dirección del archivo ingresada es inválida o el archivo no existe. ";
			cout << "Por favor, vuelva a intentarlo o ingrese una ruta válida." << endl;
			continue; // Vuelve a empezar el loop while.
		}

		fseek(archivo_ataque, 0, SEEK_END);
		tamaño_archivo = ftell(archivo_ataque) / sizeof(OrdenArchivo); // Tamaño del archivo en celdas.
		if (tamaño_archivo != TABLA_TAMANO_X * TABLA_TAMANO_Y) {
			// MSG_ERROR
			cout << "Error: El archivo cargado tiene un tamaño invalido. ";
			cout << "Por favor, ingrese un archivo de ataque válido." << endl;
			fclose(archivo_ataque);
			continue;
		}

		cout << "Cargando archivo de ataque..." << endl;

		fseek(archivo_ataque, 0, SEEK_SET);
		fread(lista_archivo, sizeof(OrdenArchivo), tamaño_archivo, archivo_ataque);
		archivoATabla(lista_archivo, tabla);

		if (!validarTabla(tabla)) {
			// MSG_ERROR
			cout << "Error: El archivo ingresado contiene una o más instrucciones inválidas." << endl;
			fclose(archivo_ataque);
			continue;
		}

		cout << "¡Archivo cargado exitosamente!" << endl;
		break;
	}
	fclose(archivo_ataque);
	flag_tabla_cargada = true;

	return;
}


void escribirArchivo(OrdenArchivo lista[TABLA_TAMANO_X * TABLA_TAMANO_Y]) {
	char direccion_archivo[RUTA_MAX];
	FILE *archivo_ataque = NULL;
	char respuesta = '\0';

	while (true) {
		cout << "Por favor, indique ruta de guardado del ataque." << endl;
		cout << CURSOR;
		cin.getline(direccion_archivo, RUTA_MAX);
		//cin >> direccion_archivo;


		// Primero intento encontrar la dirección indicada en modo "r",
		// de esta manera, si el archivo no existe, "archivo_ataque" queda
		// como un puntero NULL, accediendo nuevamente pero en modo "w+b".
		// Pero en caso de que exista, para evitar una sobreescritura
		// accidental, se pregunta al usuario como desea continuar.
		archivo_ataque = fopen(direccion_archivo, "r");
		if (archivo_ataque != NULL) { // Existe el archivo.
			fclose(archivo_ataque);
			archivo_ataque = NULL;
			// MSG_ERROR
			cout << "  Error: El archivo ya existe. ¿Desea sobreescribirlo? [s/N]" << endl;
			while (true)
			{
				cout << CURSOR;
				cin.get(respuesta);
				if (
					respuesta == 's' ||
					respuesta == 'S'
				) {
					if (!(archivo_ataque = fopen(direccion_archivo, "w+b"))) {
						// MSG_ERROR
						cout << "  Error: No se pudo sobreescribir el archivo." << endl;
						archivo_ataque = NULL;
						break; // "archivo_ataque" == NULL.
					}
					break; // "archivo_ataque" != NULL.
				} else if (
					respuesta == 'n' ||
					respuesta == 'N' ||
					respuesta == '\n'
				) {
					break; // "archivo_ataque" == NULL.
				} else {
					// MSG_ERROR
					cout << "  Error: Respuesta no valida. Vuelva a intentarlo." << endl;
					cin.ignore(); // Leading whitespace.
					continue;
				}
			}
			if (archivo_ataque == NULL) continue;
		} else {
			archivo_ataque = fopen(direccion_archivo, "w+b");
		}
		break;
	}

	cout << "Guardando tabla de órdenes en ";
	cout << "\"" << direccion_archivo << "\"";
	cout << "..." << endl;

	fseek(archivo_ataque, 0, SEEK_SET);
	if (!fwrite(lista, sizeof(OrdenArchivo), TABLA_TAMANO_X * TABLA_TAMANO_Y, archivo_ataque)) {
		// MSG_ERROR
		cout << "Error: Error al guardar la tabla en ";
		cout << "\"" << direccion_archivo << "\"" << endl;
		cout << ". Puede que el archivo se haya movido o borrado. Vuelva a intentarlo." << endl;
	} else {
		cout << "¡Archivo guardado exitosamente!" << endl;
	}
	fclose(archivo_ataque);

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
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y];
	OrdenArchivo lista_archivo[TABLA_TAMANO_X * TABLA_TAMANO_Y];

	// TODO: Cambiar workflow:
	// Pedir ubicación del archivo -> Poblar tabla -> Guardar tabla en el archivo.

	poblarTabla(tabla);
	tablaAArchivo(lista_archivo, tabla);
	escribirArchivo(lista_archivo);

	return;
}

// Almacena la tabla de memoria que luego almacena en un archivo nuevo.
void guardarAtaque(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y], bool &flag_tabla_cargada) {
	OrdenArchivo lista_archivo[TABLA_TAMANO_X * TABLA_TAMANO_Y];

	// TODO: Cambiar workflow:
	// Pedir ubicación del archivo -> Guardar tabla en el archivo.

	tablaAArchivo(lista_archivo, tabla);
	escribirArchivo(lista_archivo);
	flag_tabla_cargada = false;

	return;
}


// Solicita uno o más registros de una tabla dada para modificar.
void modificarRegistroDeTabla(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]) {
	char respuesta = '\0';
	int respuesta_x = 0, respuesta_y = 0;
	Orden tabla_nueva[TABLA_TAMANO_X][TABLA_TAMANO_Y];
	Flags flags; // FIXME: ???

	// Para poder anular los cambios en caso de que sean inválidos.
	clonarTabla(tabla, tabla_nueva);

	do {
		while (true) { // Qué registro.
			cout << "¿Qué registro desea modificar? (X;Y)" << endl;
			cout << "  X: ";
			cin >> respuesta_x;
			if (!cin) { // En caso de ingresar un caracter en vez de un número.
				cin.clear();
				cin.ignore();
				// MSG_ERROR
				cout << "Error: Respuesta no valida. Vuelva a intentarlo." << endl;
				continue;
			}
			cout << "  Y: ";
			cin >> respuesta_y;
			if (!cin) { // En caso de ingresar un caracter en vez de un número.
				cin.clear();
				cin.ignore();
				// MSG_ERROR
				cout << "Error: Respuesta no valida. Vuelva a intentarlo." << endl;
				continue;
			}
			respuesta_x--;
			respuesta_y--;
			// (Se interpreta que la tabla comienza en 1x1 y no en 0x0).
	
			if (
				respuesta_x >= TABLA_TAMANO_X ||
				respuesta_y >= TABLA_TAMANO_Y
			) {
				// MSG_ERROR
				cout << "Error: Celda fuera de límites. Elija otra posición." << endl;
				continue;
			}
			cin.ignore(); // Leading whitespace.

			break;
		}

		cout << "Registro original:" << endl;
		mostrarRegistro(tabla_nueva[respuesta_x][respuesta_y], respuesta_x, respuesta_y);
		poblarRegistro(tabla_nueva[respuesta_x][respuesta_y], respuesta_x, respuesta_y, flags);
		cout << endl << "Resultado:" << endl;
		mostrarRegistro(tabla_nueva[respuesta_x][respuesta_y], respuesta_x, respuesta_y);

		cout << "¿Cambiar otro registro? [s/N]" << endl;
		cout << CURSOR;
		cin.get(respuesta);

		while (true) {
			switch (respuesta)
			{
				default:
					// MSG_ERROR
					cout << "Error: Respuesta no valida. Vuelva a intentarlo." << endl;
					cin.ignore(); // Leading whitespace.
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

		switch (respuesta)
		{
			default:
				// MSG_ERROR
				cout << "Error: Respuesta no valida. Vuelva a intentarlo." << endl;
				cin.ignore(); // Leading whitespace.
				continue;
			case 's':
			case 'S':
			case '\n':
				cout << "Validando cambios..." << endl;
				if (!validarTabla(tabla_nueva)) {
					// MSG_ERROR
					cout << "Error: Las modificaciones hechas a la tabla son inválidas.";
					cout << " Deshaciendo los cambios." << endl;
					clonarTabla(tabla, tabla_nueva);
					break;
				}
				cout << "Cambios validados. Todo en orden." << endl;
				clonarTabla(tabla_nueva, tabla);
				break;
			case 'n':
			case 'N':
				clonarTabla(tabla, tabla_nueva);
				cout << "Se han deshecho los cambios." << endl;
				break;
		}
		if (respuesta != '\n') cin.ignore(); // Leading whitespace.
		break;
	}

	return;
}


// Solicita uno o más registros de una tabla solicitada de un archivo para modificar.
void modificarRegistroDeArchivo() { // FIXME: ayuda
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y];
	OrdenArchivo lista_archivo[TABLA_TAMANO_X * TABLA_TAMANO_Y];

	//cargarArchivo(tabla);
	
	char direccion_archivo[RUTA_MAX];
	FILE *archivo_ataque;
	int tamaño_archivo = 0;

	cout << "Por favor, indique ruta del archivo de ataque." << endl;
	while (true) { // Cargar archivo.
		cout << CURSOR;
		cin.getline(direccion_archivo, RUTA_MAX);
		//cin >> direccion_archivo;

		if (!(archivo_ataque = fopen(direccion_archivo, "r+b"))) {
			// MSG_ERROR
			cout << "Error: La dirección del archivo ingresada es inválida o el archivo no existe. ";
			cout << "Por favor, vuelva a intentarlo o ingrese una ruta válida." << endl;
			continue; // Vuelve a empezar el loop while.
		}

		fseek(archivo_ataque, 0, SEEK_END);
		tamaño_archivo = ftell(archivo_ataque) / sizeof(OrdenArchivo); // Tamaño del archivo en celdas.
		if (tamaño_archivo != TABLA_TAMANO_X * TABLA_TAMANO_Y) {
			// MSG_ERROR
			cout << "Error: El archivo cargado tiene un tamaño invalido. ";
			cout << "Por favor, ingrese un archivo de ataque válido." << endl;
			fclose(archivo_ataque);
			continue;
		}

		fseek(archivo_ataque, 0, SEEK_SET);
		fread(lista_archivo, sizeof(OrdenArchivo), tamaño_archivo, archivo_ataque);
		archivoATabla(lista_archivo, tabla);

		if (!validarTabla(tabla)) {
			// MSG_ERROR
			cout << "Error: El archivo ingresado contiene una o más instrucciones inválidas." << endl;
			fclose(archivo_ataque);
			continue;
		}

		break;
	}
	modificarRegistroDeTabla(tabla);
	tablaAArchivo(lista_archivo, tabla);

	fseek(archivo_ataque, 0, SEEK_SET);
	if (!fwrite(lista_archivo, sizeof(OrdenArchivo), TABLA_TAMANO_X * TABLA_TAMANO_Y, archivo_ataque)) {
		// MSG_ERROR
		cout << "Error: Error al guardar la tabla en ";
		cout << "\"" << direccion_archivo << "\"" << endl;
		cout << ". Puede que el archivo se haya movido o borrado. Vuelva a intentarlo." << endl;
	} else {
		cout << "¡Archivo guardado exitosamente!" << endl;
	}
	fclose(archivo_ataque);

	// Archivo -> Memoria (Lista) -> Memoria (Tabla) -> Se edita -> Memoria (Tabla) -> Memoria (Lista) -> Archivo.
	// Efectivamente ineficiente.

	return;
}

// ──────────────────────────────────────────────────


// ───────────────────── HTML ───────────────────────

void crearHTML(
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y],
	int despegue_x, int despegue_y
) {
	FILE *archivo;
	archivo = fopen("index.htm", "w");

	int x = despegue_x, y = despegue_y;
	Orden registro, registro_siguiente;
	bool norte = false, sur = false, este = false, oeste = false;
	bool norte_siguiente = false, sur_siguiente = false, este_siguiente = false, oeste_siguiente = false;
	const char* char_registro[TABLA_TAMANO_X][TABLA_TAMANO_Y] = { "\0" }; // Wide char para almacenar unicode.

	fprintf(archivo, R"(
<html>
	<head>
	<title>Nombre del archivo - TP Dron</title>
	</head>
	<body>
	<div class="main">
		<header width="100%">
		<h1>Vista previa de la tabla de órdenes del dron.</h1>
		<h2>Dirección del archivo: /direccion/del/archivo.file</h2>
		</header>
		<hr width="100%"/>
		<div style="display: flex; gap: 10px;">
			<div class="border">
			<div class="scrollable">
			<table class="table">
				<thead>
				<th><div class="content"></div></th>)");
	for (int x = 0; x < TABLA_TAMANO_X; x++) {
		fprintf(archivo, "\n\t\t\t<th><div class=\"content\">%d</div></th>", x + 1); // Columna
	}
	fprintf(archivo, "\n\t\t\t</thead>");

	while (true) {
		registro = tabla[x][y];
		registro_siguiente = tabla[registro.siguiente_x][registro.siguiente_y];

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

	for (int k = 0; k < TABLA_TAMANO_Y; k++) {
		fprintf(archivo, "\n\t\t\t<tr>\n\t\t\t<td><div class=\"content\">%d</div></td>", k + 1); // Fila.
		for (int j = 0; j < TABLA_TAMANO_X; j++) {
			fprintf(archivo, "\n\t\t\t<td title=\"Celda %dx%d", j + 1, k + 1); // Numero de celda.
			if (tabla[j][k].despegue) 			fprintf(archivo, "&#10;Celda de despegue");
			if (tabla[j][k].aterrizaje) 		fprintf(archivo, "&#10;Celda de aterrizaje");
			if (tabla[j][k].ataque_kamikaze) 	fprintf(archivo, "&#10;Ataque kamikaze");
			if (tabla[j][k].soltar_granada1) 	fprintf(archivo, "&#10;Ataque granada nº1");
			if (tabla[j][k].soltar_granada2) 	fprintf(archivo, "&#10;Ataque granada nº2");
			fprintf(archivo, "&#10;Tiempo de espera: %d s\"><div class=\"content\">%s</div></td>",
				tabla[j][k].espera,
				char_registro[j][k]
			);
		}
		fprintf(archivo, "\n\t\t\t</tr>");
	}
	fprintf(archivo, R"(
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
			</div>
		</section>
	</div>
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
</html>
<style>
	body {
	display: flex;
	flex-direction: column;
	margin: 0;
	height: 100%;
	}
	.main {
	display: inline-flex;
	flex-direction: column;
	gap: 5px;
	align-items: center;
	align-self: center;
	height: 100%;
	}
	.border {
	border: 2px solid black;
	border-radius: 8px;
	height: min-content;
	overflow: hidden;
	}
	.scrollable {
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
	border: 2px solid black;
	border-radius: 8px;
	overflow: hidden;
	display: flex;
	flex-direction: column;
	gap: 5px;
	height: fit-content;
	}
	.referencia table{
	border-top: 2px solid black;
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
</style>)");

	fclose(archivo);
	cout << "index.htm guardado correctamente." << endl;
	return;
}

// ──────────────────────────────────────────────────


// ──────────────── Menú principal ──────────────────

// Imprime un error y retorna "false" cuando no hay una tabla de
// órdenes cargada en memoria. Retorna "true" en caso contrario.
bool comprobarTablaCargada(bool flag) {
	if (!flag) {
		// MSG_ERROR
		cout << "Error: No hay una tabla de órdenes cargada. ";
		cout << "Cargue o cree una tabla primero para poder visualizarla." << endl;
		return false;
	}

	return true;
}


// Menú principal.
void bucleMenu(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y], bool &flag_tabla_cargada) {
	int eleccion = -1;

	do {
		// Con esta comparación se evita reimprimir el menú si
		// la opción ingresada es incorrecta, pero sí si se
		// acaba de salir de una opción real luego de utilizarla.
		if (eleccion < 8) {
			cout << endl;
			cout << "Menú de opciones de control de memoria del dron." << endl;
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
		if (!cin) { // En caso de ingresar un caracter en vez de un número.
			cin.clear();
			cin.ignore();
			eleccion = 8;
		}

		// Se utiliza cin.ignore() para prevenir error de leading whitespace al presionar Enter.
		// Este comportamiento se puede obtener utilizando cin.get(), pero se decidió por
		// usar cin.ignore() ya que el valor obtenido no va a ser almacenado.
		cin.ignore();
		switch (eleccion) {
			default:
				// MSG_ERROR
				cout << "Error: Opción no valida. Vuelva a intentarlo." << endl;
				break;
			case 0:
				cout << "Terminando ejecución." << endl;
				break;
			case 1: // Cargar archivo de ataque.
				cargarArchivo(tabla, flag_tabla_cargada);
				break;
			case 2: // Mostrar archivo cargado.
				if (!comprobarTablaCargada(flag_tabla_cargada)) break;
				mostrarTabla(tabla);
				break;
			case 3: // Crear archivo de ataque nuevo y poblarlo.
				crearAtaque();
				break;
			case 4: // Modificar tabla en archivo.
				modificarRegistroDeArchivo(); // Terminar funcionamiento antes de implementar.
				break;
			case 5: // Modificar tabla en memoria.
				if (!comprobarTablaCargada(flag_tabla_cargada)) break;
				modificarRegistroDeTabla(tabla);
				break;
			case 6: // Guardar archivo de ataque.
				if (!comprobarTablaCargada(flag_tabla_cargada)) break;
				guardarAtaque(tabla, flag_tabla_cargada);
				break;
			case 7: // Crear página HTML.
				if (!comprobarTablaCargada(flag_tabla_cargada)) break;
				crearHTML(tabla, 3, 1);
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

// ──────────────────────────────────────────────────