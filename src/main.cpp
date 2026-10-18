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

struct Flags {
	bool despegue 		= false; // Solo puede haber una celda de despegue por tabla.
	bool aterrizaje 	= false;
	bool kamikaze 		= false;
	bool granada1 		= false;
	bool granada2 		= false;
	bool posiciones_transitadas[TABLA_TAMANO_X][TABLA_TAMANO_Y] = { false };
};


void clonarTabla(
	Orden original[TABLA_TAMANO_X][TABLA_TAMANO_Y],
	Orden copia[TABLA_TAMANO_X][TABLA_TAMANO_Y]
) {
	for (int y = 0; y < TABLA_TAMANO_Y; y++) { for (int x = 0; x < TABLA_TAMANO_X; x++) {
		copia[x][y] = original[x][y];
	} }
	return;
}


bool tablaCargada(bool flag_tabla_cargada) {
	if (!flag_tabla_cargada) {
		// MSG_ERROR
		cout << "Error: No hay una tabla de órdenes cargada. ";
		cout << "Cargue o cree una tabla primero para poder visualizarla." << endl;
		return true;
	}
	return false;
}


void archivoATabla(
	OrdenArchivo lista_archivo[TABLA_TAMANO_X * TABLA_TAMANO_Y],
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]
) {
	unsigned int tabla_x = -1, tabla_y = -1;
	for (int i = 0; i < TABLA_TAMANO_X * TABLA_TAMANO_Y; i++) {
		tabla_x = lista_archivo[i].x;
		tabla_y = lista_archivo[i].y;

		tabla[tabla_x][tabla_y].despegue 		= lista_archivo[i].despegue;
		tabla[tabla_x][tabla_y].aterrizaje 		= lista_archivo[i].aterrizaje;
		tabla[tabla_x][tabla_y].soltar_granada1 = lista_archivo[i].soltar_granada1;
		tabla[tabla_x][tabla_y].soltar_granada2 = lista_archivo[i].soltar_granada2;
		tabla[tabla_x][tabla_y].ataque_kamikaze = lista_archivo[i].ataque_kamikaze;
		tabla[tabla_x][tabla_y].espera 			= lista_archivo[i].espera;
		tabla[tabla_x][tabla_y].siguiente_x 	= lista_archivo[i].siguiente_x;
		tabla[tabla_x][tabla_y].siguiente_y 	= lista_archivo[i].siguiente_y;
	}

	return;
}


void tablaAArchivo(
	OrdenArchivo tabla_archivo[TABLA_TAMANO_X * TABLA_TAMANO_Y],
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]
) {
	int n = 0;
	for (int y = 0; y < TABLA_TAMANO_Y; y++) { for (int x = 0; x < TABLA_TAMANO_X; x++) {
		tabla_archivo[n].x = x;
		tabla_archivo[n].y = y;

		tabla_archivo[n].despegue 			= tabla[x][y].despegue;
		tabla_archivo[n].aterrizaje 		= tabla[x][y].aterrizaje;
		tabla_archivo[n].soltar_granada1 	= tabla[x][y].soltar_granada1;
		tabla_archivo[n].soltar_granada2 	= tabla[x][y].soltar_granada2;
		tabla_archivo[n].ataque_kamikaze 	= tabla[x][y].ataque_kamikaze;
		tabla_archivo[n].espera 			= tabla[x][y].espera;
		tabla_archivo[n].siguiente_x 		= tabla[x][y].siguiente_x;
		tabla_archivo[n].siguiente_y 		= tabla[x][y].siguiente_y;
		n++;
	} }

	return;
}


bool recorridoTabla(
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y],
	unsigned int despegue_x, unsigned int despegue_y
) {
	unsigned x = despegue_x, y = despegue_y;
	unsigned int celdas_recorridas = 0;
	bool flag_posiciones_transitadas[TABLA_TAMANO_X][TABLA_TAMANO_Y] = { false };
	Orden pagina_tabla;

	while (true) {
		pagina_tabla = tabla[x][y];

		// DEBUG ----
		// cout << " # Celda ";
		// cout << "(" << x + 1 << ";" << y + 1 << ")";
		// if (pagina_tabla.despegue) 			cout << " (Celda de despegue)";
		// if (pagina_tabla.aterrizaje) 		cout << " (Celda de aterrizaje)";
		// if (pagina_tabla.ataque_kamikaze) 	cout << " (Celda kamikaze)";
		// if (pagina_tabla.soltar_granada1) 	cout << " (Granada 1)";
		// if (pagina_tabla.soltar_granada2) 	cout << " (Granada 2)";
		// if (!pagina_tabla.aterrizaje && !pagina_tabla.ataque_kamikaze) {
		// 	cout << " → ";
		// 	cout << "Celda ";
		// 	cout << "(" << pagina_tabla.siguiente_x + 1 << ";" << pagina_tabla.siguiente_y + 1 << ")";
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

		if (pagina_tabla.aterrizaje || pagina_tabla.ataque_kamikaze) break; // Final del recorrido.

		if (
			pagina_tabla.siguiente_x >= TABLA_TAMANO_X 	||
			pagina_tabla.siguiente_y >= TABLA_TAMANO_Y 	||
			pagina_tabla.siguiente_x < 0 				||
			pagina_tabla.siguiente_y < 0
		) {
			// MSG_ERROR
			cout << "Error: La tabla intenta recorrer una o más celdas fuera de los límites de la tabla." << endl;
			return false;
		}
		if (
			!(pagina_tabla.siguiente_x == x 	&& pagina_tabla.siguiente_y == y - 1) 	&&	// Norte.
			!(pagina_tabla.siguiente_x == x + 1 && pagina_tabla.siguiente_y == y) 		&&	// Este.
			!(pagina_tabla.siguiente_x == x 	&& pagina_tabla.siguiente_y == y + 1) 	&&	// Sur.
			!(pagina_tabla.siguiente_x == x - 1 && pagina_tabla.siguiente_y == y)			// Oeste.
		) {
			// MSG_ERROR
			cout << "Error: La celda ";
			cout << "(" << x + 1 << ";" << y + 1 << ")";
			cout << " intenta recorrer una celda no adyacente." << endl;
			return false;
		}

		x = pagina_tabla.siguiente_x;
		y = pagina_tabla.siguiente_y;
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
	unsigned int despegue_x = -1, despegue_y = -1;
	bool flag_despegue = false;
	bool flag_final = false;
	bool flag_granada1 = false;
	bool flag_granada2 = false;
	Orden pagina_tabla;

	// Reglas de validación:
	// ✓ Tener exactamente 1 celda de despegue.
	// ✓ Una celda de despegue no puede contener ninguna otra instrucción.
	// ✓ Tener exactamente 1 celda de finalización (Aterrizaje o kamikaze).
	// ✓ Ser recorrida en su totalidad desde la celda de despegue hasta la de finalización.
	// ✓ Recorrer cada celda exactamente 1 vez.
	// ✓ No recorrer celdas fuera de los limites.
	// ✓ No recorrer celdas no adyacentes.
	// ✓ Tener no más de 1 celda de granada 1 y 2.
	for (int y = 0; y < TABLA_TAMANO_Y; y++) { for (int x = 0; x < TABLA_TAMANO_X; x++) {
		pagina_tabla = tabla[x][y];
		if (pagina_tabla.despegue) {
			if (flag_despegue) {
				// MSG_ERROR
				cout << "Error: La tabla contiene más de 1 celda de despegue." << endl;
				return false;
			}
			if (
				pagina_tabla.aterrizaje 		||
				pagina_tabla.ataque_kamikaze 	||
				pagina_tabla.soltar_granada1 	||
				pagina_tabla.soltar_granada2
			) {
				// MSG_ERROR
				cout << "Error: La celda de despegue contiene multiples instrucciones." << endl;
				return false;
			}
			flag_despegue = true;
			despegue_x = x;
			despegue_y = y;
		}

		if (pagina_tabla.aterrizaje) {
			if (flag_final) {
				// MSG_ERROR
				cout << "Error: La tabla contiene más de 1 celda de aterrizaje o de ataque kamikaze." << endl;
				return false;
			}
			flag_final = true;
		}
		if (pagina_tabla.ataque_kamikaze) {
			if (flag_final) {
				// MSG_ERROR
				cout << "Error: La tabla contiene más de 1 celda de aterrizaje o de ataque kamikaze." << endl;
				return false;
			}
			flag_final = true;
		}

		if (pagina_tabla.soltar_granada1) {
			if (flag_granada1) {
				// MSG_ERROR
				cout << "Error: La tabla contiene más de 1 celda de granada nº1." << endl;
				return false;
			}
			flag_granada1 = true;
		}
		if (pagina_tabla.soltar_granada2) {
			if (flag_granada2) {
				// MSG_ERROR
				cout << "Error: La tabla contiene más de 1 celda de granada nº2." << endl;
				return false;
			}
			flag_granada1 = true;
		}
	} }

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
	if (!recorridoTabla(tabla, despegue_x, despegue_y)) return false;

	return true;
}


void poblarRegistro(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y], int x, int y, Flags &flags) {
	char respuesta = '\0';
	//
	unsigned int respuesta_tiempo = 0;
	unsigned int respuesta_x = 0, respuesta_y = 0;

	// TODO: En caso de que 2 acciones sean incomptabiles, anularlas
	// al momento de definir la contradictoria. Esto para que en caso
	// de modificar un registro existente, no dejar valores
	// contradictorios.
	// Ej: Tengo una tabla con el aterrizaje en (2;1), y deseo
	// posicionar ahí ahora el despegue, si yo modifico ese registro
	// solo, sin este checkeo, podría darse la situación de dejar
	// (2;1) con un despegue y aterrizaje en la misma celda.

	// Despegue.
	while (!flags.despegue) {
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
					tabla[x][y].despegue = true;
					flags.despegue = true;
					tabla[x][y].aterrizaje = false;
					tabla[x][y].ataque_kamikaze = false;
					tabla[x][y].soltar_granada1 = false;
					tabla[x][y].soltar_granada2 = false;
					flags.posiciones_transitadas[x][y] = true;
				break;
			case 'n':
			case 'N':
			case '\n':
					tabla[x][y].despegue = false;
				break;
		}
		if (respuesta != '\n') cin.ignore(); // Leading whitespace.
		break;
	}

	// Aterrizaje.
	while (
		!flags.aterrizaje &&
		!flags.kamikaze &&
		!tabla[x][y].despegue
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
				tabla[x][y].despegue = false;
				tabla[x][y].aterrizaje = true;
				tabla[x][y].ataque_kamikaze = false;
				flags.aterrizaje = true;
				break;
			case 'n':
			case 'N':
			case '\n':
				tabla[x][y].aterrizaje = false;
				break;
		}
		if (respuesta != '\n') cin.ignore(); // Leading whitespace.
		break;
	}

	// Kamikaze.
	while (
		!flags.aterrizaje &&
		!flags.kamikaze &&
		!tabla[x][y].despegue
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
				tabla[x][y].despegue = false;
				tabla[x][y].aterrizaje = false;
				tabla[x][y].ataque_kamikaze = true;
				flags.kamikaze = true;
				break;
			case 'n':
			case 'N':
			case '\n':
				tabla[x][y].ataque_kamikaze = false;
				break;
		}
		if (respuesta != '\n') cin.ignore(); // Leading whitespace.
		break;
	}

	// Granada nº1.
	while (
		!flags.granada1 &&
		!tabla[x][y].despegue
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
				tabla[x][y].despegue = false;
				tabla[x][y].soltar_granada1 = true;
				flags.granada1 = true;
				break;
			case 'n':
			case 'N':
			case '\n':
				tabla[x][y].soltar_granada1 = false;
				break;
		}
		if (respuesta != '\n') cin.ignore(); // Leading whitespace.
		break;
	}

	// Granada nº2.
	while (
		!flags.granada2 &&
		!tabla[x][y].despegue
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
				tabla[x][y].despegue = false;
				tabla[x][y].soltar_granada2 = true;
				flags.granada2 = true;
				break;
			case 'n':
			case 'N':
			case '\n':
				tabla[x][y].soltar_granada2 = false;
				break;
		}
		if (respuesta != '\n') cin.ignore(); // Leading whitespace.
		break;
	}

	// Tiempo de espera.
	while (true) {
		cout << "  Tiempo de espera (Segundos)." << endl;
		cout << "  " << CURSOR;
		// FIXME: 16/10 Failsafe caracter en vez de número.
		cin >> respuesta_tiempo;
		// TODO: Consultar condición de invalidez de tiempo de espera.
		// (Tiempo mínimo, máximo, etc).
		tabla[x][y].espera = respuesta_tiempo;
		break;
	}

	// Siguiente posición.
	while (
		!tabla[x][y].aterrizaje &&
		!tabla[x][y].ataque_kamikaze
	) {
		cout << "  Posición siguiente (X;Y)." << endl;
		cout << "  " << CURSOR;
		// FIXME: 16/10 Failsafe caracter en vez de número.
		cin >> respuesta_x;
		cout << "  " << CURSOR;
		// FIXME: 16/10 Failsafe caracter en vez de número.
		cin >> respuesta_y;
		// (Se interpreta que la tabla comienza en 1x1 y no en 0x0).
		respuesta_x--;
		respuesta_y--;
	
		// Esta condición es innecesaria, ya que la comprobación
		// siguiente impide este caso también, pero tomé la decisión
		// de contemplarlo igualmente para devolver un mensaje de
		// alerta con mayor granuladidad.
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
	
		tabla[x][y].siguiente_x = respuesta_x;
		tabla[x][y].siguiente_y = respuesta_y;
		flags.posiciones_transitadas[respuesta_x][respuesta_y] = true;
		break;
	}

	cin.ignore(); // Leading whitespace.

	return;
}


void poblarArchivo(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]) {
	Flags flags;
	cout << "# Tabla de " << TABLA_TAMANO_X << " ✕ " << TABLA_TAMANO_Y << "." << endl; // DEBUG ----
	while (true) {
		for (int y = 0; y < TABLA_TAMANO_Y; y++) { for (int x = 0; x < TABLA_TAMANO_X; x++) {
			cout << "Posición (" << x + 1 << ";" << y + 1 << "):" << endl;
			poblarRegistro(tabla, x, y, flags);
		} }

		if (!validacionTabla(tabla)) {
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
				cout << "  " << CURSOR;
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


void cargarAtaque(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y], bool &flag_tabla_cargada) {
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
		// FIXME: 16/10 Detección de tecla Esc para salir.
		// FIXME: 16/10 Detección flechas para moverse al introducir texto.
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

		if (!validacionTabla(tabla)) {
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


// "crearArchivo()" no pide como parametro la tabla global, ya que esta no
// se utiliza fuera de su propio procedimiento. Está contenida dentro de
// la creación del archivo en donde se almacenará. Para interactuar con
// el resto de funciones, como "mostrarAtaque()", por ejemplo, se debe
// cargar como cualquier otro archivo.
void crearAtaque() {
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y];
	OrdenArchivo lista_archivo[TABLA_TAMANO_X * TABLA_TAMANO_Y];

	poblarArchivo(tabla);
	tablaAArchivo(lista_archivo, tabla);
	escribirArchivo(lista_archivo);

	return;
}


void guardarAtaque(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y], bool &flag_tabla_cargada) {
	OrdenArchivo lista_archivo[TABLA_TAMANO_X * TABLA_TAMANO_Y];

	tablaAArchivo(lista_archivo, tabla);
	escribirArchivo(lista_archivo);
	flag_tabla_cargada = false;

	return;
}


void mostrarRegistro(Orden registro, int x, int y) {
	// Un poquito de diseño de tui.
	cout << "┌─────────────────────────────────────┐" << endl;

	cout << "│ " << "Posición actual (X;Y):        ";
	cout << "(" << x + 1 << ";" << y + 1 << ")" << endl;

	cout << "│ " << "Despegue:                  ";
	cout << (registro.despegue ? "Veradero" : "   Falso") << endl;
	// Operador ternario para redibilidad

	cout << "│ " << "Aterrizaje:                ";
	cout << (registro.aterrizaje ? "Veradero" : "   Falso") << endl;

	cout << "│ " << "Soltar granada nº1:        ";
	cout << (registro.soltar_granada1  ? "Veradero" : "   Falso") << endl;

	cout << "│ " << "Soltar granada nº2:        ";
	cout << (registro.soltar_granada2 ? "Veradero" : "   Falso") << endl;

	cout << "│ " << "Ataque kamikaze:           ";
	cout << (registro.ataque_kamikaze ? "Veradero" : "   Falso") << endl;

	cout << "│ " <<  "Espera [Segundos]:              ";
	cout << registro.espera << " s" << endl;

	cout << "│ " << "Posición siguiente (X;Y):     ";
	if (registro.aterrizaje || registro.ataque_kamikaze) cout << "Final";
	else cout << "(" << registro.siguiente_x + 1 << ";" << registro.siguiente_y + 1 << ")";
	cout << endl;
	// Se le suma 1 a las coordenadas para volverlas amistosas para el usuario.
	// (Se interpreta que la tabla comienza en 1x1 y no en 0x0).

	cout << "└─ Presione Enter para continuar... ──┘" << endl;

	cin.ignore(); // Presione Enter para continuar.
};


void mostrarAtaque(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]) {
	for (int y = 0; y < TABLA_TAMANO_Y; y++) { for (int x = 0; x < TABLA_TAMANO_X; x++) {
		mostrarRegistro(tabla[x][y], x, y);
	} }

	return;
}


void modificarRegistro(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y]) {
	int respuesta_x = 0, respuesta_y = 0;
	char respuesta = '\0';
	Orden tabla_nueva[TABLA_TAMANO_X][TABLA_TAMANO_Y];
	Flags flags;

	clonarTabla(tabla, tabla_nueva);

	while (true) {
		while (true) {
			cout << "¿Qué registro desea modificar? (X;Y)" << endl;
			cout << "  " << CURSOR;
			// FIXME: 16/10 Failsafe caracter en vez de número.
			cin >> respuesta_x;
			cout << "  " << CURSOR;
			// FIXME: 16/10 Failsafe caracter en vez de número.
			cin >> respuesta_y;
			// (Se interpreta que la tabla comienza en 1x1 y no en 0x0).
			respuesta_x--;
			respuesta_y--;
	
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
		// validarTabla y obtener flags correctas
		poblarRegistro(tabla_nueva, respuesta_x, respuesta_y, flags);
		cout << endl << "Resultado:" << endl;
		mostrarRegistro(tabla_nueva[respuesta_x][respuesta_y], respuesta_x, respuesta_y);

		while (true) {
			cout << "¿Guardar cambios? [S/n]" << endl;
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
				case '\n':
					// TODO: Validar cambios.
					cout << "Validando cambios..." << endl; // DEBUG ----
					if (!validacionTabla(tabla_nueva)) {
						// MSG_ERROR
						cout << "Error: Las modificaciones hechas a la tabla son inválidas.";
						cout << " Deshaciendo los cambios." << endl;
						clonarTabla(tabla, tabla_nueva);
						break;
					}
					cout << "Cambios validados. Todo en orden." << endl; // DEBUG ----
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
		cout << "¿Hacer más cambios? [s/N]" << endl;
		cout << "  " << CURSOR;
		cin.get(respuesta);

		// TODO: revisar esto
		switch (respuesta)
		{
			default:
				// MSG_ERROR
				//cout << "Error: Respuesta no valida. Vuelva a intentarlo." << endl;
				//cin.ignore(); // Leading whitespace.
				//continue;
			case 's':
			case 'S':
				continue; // idk
			case 'n':
			case 'N':
			case '\n':
				break;
		}
		if (respuesta != '\n') cin.ignore(); // Leading whitespace.
		break;
	}

	return;
}


void modificarArchivo() {
	Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y];
	OrdenArchivo lista_archivo[TABLA_TAMANO_X * TABLA_TAMANO_Y];

	// cargarArchivo
	archivoATabla(lista_archivo, tabla);
	modificarRegistro(tabla);
	tablaAArchivo(lista_archivo, tabla);
	escribirArchivo(lista_archivo); // "escribirArchivo()" no funciona así. Hay que corregirlo.

	return;
}


void bucleMenu(Orden tabla[TABLA_TAMANO_X][TABLA_TAMANO_Y], bool &flag_tabla_cargada) {
	int eleccion = -1;

	do {
		// Con esta comparación se evita reimprimir el menú si
		// la opción ingresada es incorrecta, pero sí si se
		// acaba de salir de una opción real luego de utilizarla.
		if (eleccion < 7) {
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
		// FIXME: 16/10 Failsafe para caracter en vez de número.
		cin >> eleccion;

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
				cargarAtaque(tabla, flag_tabla_cargada);
				break;
			case 2: // Mostrar archivo cargado.
				if (tablaCargada(flag_tabla_cargada)) break;
				mostrarAtaque(tabla);
				break;
			case 3: // Crear archivo de ataque nuevo y poblarlo.
				crearAtaque();
				break;
			case 4: // Modificar tabla en archivo.
				//modificarArchivo(); // Terminar funcionamiento antes de implementar.
				break;
			case 5: // Modificar tabla en memoria.
				if (tablaCargada(flag_tabla_cargada)) break;
				modificarRegistro(tabla);
				break;
			case 6: // Guardar archivo de ataque.
				if (tablaCargada(flag_tabla_cargada)) break;
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