/*
 ============================================================================
 Name        : univerga.c
 Author      : 
 Version     :
 Copyright   : a veces siento que
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <stdint.h>

#ifdef CACA_COMUN_LOG
#include <execinfo.h>
#endif

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#else
#include <time.h>
#include <stdarg.h>
#endif

#ifndef LONG_LONG_MAX
#define LONG_LONG_MAX LONG_MAX
#endif

#define CACA_COMUN_TAM_MAX_LINEA (16*200000)
#define CACA_LOG_MAX_TAM_CADENA 2000

#define CACA_COMUN_BUF_STATICO (char[1000] ) { '\0' }

#define BITCH_VECTOR_NUM_BITS (sizeof(bitch_vector) * 8)

#define CACA_COMUN_ASSERT_DUROTE 0
#define CACA_COMUN_ASSERT_SUAVECITO 1
#define CACA_COMUN_ASSERT_NIMADRES 2

#define UNIVERGA_VALOR_INVALIDO -1LL
#define UNIVERGA_IDX_INVALIDO ((natural)UNIVERGA_VALOR_INVALIDO)

typedef unsigned int natural;
typedef int tipo_dato;

typedef long long entero_largo;
typedef unsigned long long entero_largo_sin_signo;
typedef long long bitch_vector;

typedef enum BOOLEANOS {
	falso = 0, verdadero
} bool;

/*
 #define CACA_COMUN_TIPO_ASSERT CACA_COMUN_ASSERT_SUAVECITO
 */
#define CACA_COMUN_TIPO_ASSERT CACA_COMUN_ASSERT_DUROTE

#define assert_timeout_dummy(condition) 0;

#if CACA_COMUN_TIPO_ASSERT == CACA_COMUN_ASSERT_DUROTE
#define assert_timeout(condition) assert(condition);
#endif
#if CACA_COMUN_TIPO_ASSERT == CACA_COMUN_ASSERT_SUAVECITO
#define assert_timeout(condition) if(!(condition)){sleep(10);abort();}
#endif
#if CACA_COMUN_TIPO_ASSERT == CACA_COMUN_ASSERT_NIMADRES
#define assert_timeout(condition) 0
#endif

#ifdef CACA_COMUN_LOG
#define caca_log_debug(formato, args...) \
		do \
		{ \
			size_t profundidad = 0; \
			void *array[CACA_LOG_MAX_TAM_CADENA]; \
 			profundidad = backtrace(array, CACA_LOG_MAX_TAM_CADENA); \
			caca_log_debug_func(formato,__FILE__, __func__, __LINE__,profundidad,##args); \
		} \
		while(0);
#else
#define caca_log_debug(formato, args...) 0;
#endif

#define caca_comun_max(x,y) ((x) < (y) ? (y) : (x))
#define caca_comun_min(x,y) ((x) < (y) ? (x) : (y))

void caca_log_debug_func(const char *format, ...);

void caca_comun_current_utc_time(struct timespec *ts) {

#ifdef __MACH__
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	ts->tv_sec = mts.tv_sec;
	ts->tv_nsec = mts.tv_nsec;
#else
#ifdef CACA_COMUN_LOG
	clock_gettime(CLOCK_REALTIME, ts);
#endif
#endif

}

void caca_comun_timestamp(char *stime) {
	time_t ltime;
	struct tm result;
	long ms;
#ifndef ONLINE_JUDGE
	struct timespec spec;
#endif
	char parte_milisecundos[50];

	ltime = time(NULL);

#ifndef ONLINE_JUDGE
	localtime_r(&ltime, &result);
	asctime_r(&result, stime);
#endif

	*(stime + strlen(stime) - 1) = ' ';

#ifndef ONLINE_JUDGE
	caca_comun_current_utc_time(&spec);
	ms = round(spec.tv_nsec / 1.0e3);
#endif
	sprintf(parte_milisecundos, "%ld", ms);
	strcat(stime, parte_milisecundos);
}
#ifdef CACA_COMUN_LOG
void caca_log_debug_func(const char *format, ...) {

	va_list arg;
	va_list arg2;
	const char *PEDAZO_TIMESTAMP_HEADER = "tiempo: %s; ";
	const char *HEADER =
	"archivo: %s; funcion: %s; linea %d; nivel: %zd caca 8====D ";
	char formato[CACA_LOG_MAX_TAM_CADENA + sizeof(HEADER)
	+ sizeof(PEDAZO_TIMESTAMP_HEADER)] = {'\0'};
	char pedazo_timestamp[sizeof(PEDAZO_TIMESTAMP_HEADER) + 100] = {'\0'};
	char cadena_timestamp[100] = {'\0'};

	caca_comun_timestamp(cadena_timestamp);
	sprintf(pedazo_timestamp, PEDAZO_TIMESTAMP_HEADER, cadena_timestamp);

	strcpy(formato, pedazo_timestamp);
	strcat(formato, HEADER);
	strcat(formato, format);
	strcat(formato, "\n");

	va_start(arg, format);
	va_copy(arg2, arg);
	vprintf(formato, arg2);
	va_end(arg2);
	va_end(arg);
	setbuf(stdout, NULL);
}
#endif

#ifdef CACA_COMUN_LOG
static char *caca_comun_arreglo_a_cadena(tipo_dato *arreglo, int tam_arreglo,
		char *buffer) {
	int i;
	char *ap_buffer = NULL;
	int characteres_escritos = 0;
#ifdef ONLINE_JUDGE
	return NULL;
#endif

	memset(buffer, 0, 100);
	ap_buffer = buffer;

	for (i = 0; i < tam_arreglo; i++) {
		characteres_escritos += sprintf(ap_buffer + characteres_escritos,
				"%12d", *(arreglo + i));
		if (i < tam_arreglo - 1) {
			*(ap_buffer + characteres_escritos++) = ',';
		}
	}
	*(ap_buffer + characteres_escritos) = '\0';
	return ap_buffer;
}

static char *caca_comun_arreglo_a_cadena_natural(natural *arreglo,
		natural tam_arreglo, char *buffer) {
	int i;
	char *ap_buffer = NULL;
	int characteres_escritos = 0;
#ifdef ONLINE_JUDGE
	return NULL;
#endif

	memset(buffer, 0, 100);
	ap_buffer = buffer;

	for (i = 0; i < tam_arreglo; i++) {
		characteres_escritos += sprintf(ap_buffer + characteres_escritos, "%2u",
				*(arreglo + i));
		if (i < tam_arreglo - 1) {
			*(ap_buffer + characteres_escritos++) = ',';
		}
	}
	*(ap_buffer + characteres_escritos) = '\0';
	return ap_buffer;
}
char *caca_comun_matrix_a_cadena(tipo_dato *matrix, natural filas_tam,
		natural columas_tam, char *buffer) {
	int i, j;
	natural inicio_buffer_act = 0;
	for (int i = 0; i < filas_tam; i++) {
		caca_comun_arreglo_a_cadena(matrix + i * columas_tam, columas_tam,
				buffer + inicio_buffer_act);
		inicio_buffer_act += strlen(buffer + inicio_buffer_act);
		buffer[inicio_buffer_act++] = '\n';
		/*		caca_log_debug("pero q mierda inicio buffer act %u %s",inicio_buffer_act,buffer);*/
	}
	return buffer;
}
#else
static char *caca_comun_arreglo_a_cadena(tipo_dato *arreglo, int tam_arreglo,
		char *buffer) {
	return NULL;
}
static char *caca_comun_arreglo_a_cadena_natural(natural *arreglo,
		natural tam_arreglo, char *buffer) {
	return NULL;
}
char *caca_comun_matrix_a_cadena(tipo_dato *matrix, natural filas_tam,
		natural columas_tam, char *buffer) {
	return NULL;
}
#endif
void caca_comun_strreplace(char s[], char chr, char repl_chr) {
	int i = 0;
	while (s[i] != '\0') {
		if (s[i] == chr) {
			s[i] = repl_chr;
		}
		i++;
	}
}

static int caca_comun_lee_matrix_long_stdin(tipo_dato *matrix, int *num_filas,
		int *num_columnas, int num_max_filas, int num_max_columnas) {
	int indice_filas = 0;
	int indice_columnas = 0;
	tipo_dato numero = 0;
	char *siguiente_cadena_numero = NULL;
	char *cadena_numero_actual = NULL;
	char *linea = NULL;

	linea = calloc(CACA_COMUN_TAM_MAX_LINEA, sizeof(char));

	while (indice_filas < num_max_filas
			&& fgets(linea, CACA_COMUN_TAM_MAX_LINEA, stdin)) {
		indice_columnas = 0;
		cadena_numero_actual = linea;
		caca_comun_strreplace(linea, '\n', '\0');
		if (!strlen(linea)) {
			caca_log_debug("weird, linea vacia");
			continue;
		}
		for (siguiente_cadena_numero = linea;; siguiente_cadena_numero =
				cadena_numero_actual) {
			numero = strtol(siguiente_cadena_numero, &cadena_numero_actual, 10);
			if (cadena_numero_actual == siguiente_cadena_numero) {
				break;
			}
			*(matrix + indice_filas * num_max_columnas + indice_columnas) =
					numero;
			indice_columnas++;
		}
		if (num_columnas) {
			num_columnas[indice_filas] = indice_columnas;
		}
		indice_filas++;
		caca_log_debug("las filas son %d, con clos %d", indice_filas,
				indice_columnas);
	}

	*num_filas = indice_filas;
	free(linea);
	return 0;
}

typedef struct puto_cardinal {
	union {
		struct {
			entero_largo coordenada_y_puto_cardinal;
			entero_largo coordenada_x_puto_cardinal;
		} separados_puto_cardinal;
		entero_largo coordenadas_juntas_puto_cardinal;
	} datos_puto_cardinal;
	void *extra;
} puto_cardinal;

#define coord_x datos_puto_cardinal.separados_puto_cardinal.coordenada_x_puto_cardinal
#define coord_y datos_puto_cardinal.separados_puto_cardinal.coordenada_y_puto_cardinal
#define coord_xy datos_puto_cardinal.coordenadas_juntas_puto_cardinal

#define UNIVERGA_MAX_FILAS 10
#define UNIVERGA_MAX_COLUMNAS 100
#define costo coord_x
#define fila coord_y

int matrix_chostos[UNIVERGA_MAX_FILAS][UNIVERGA_MAX_COLUMNAS] = { 0 };
entero_largo matrix_chosto_rutas[UNIVERGA_MAX_FILAS][UNIVERGA_MAX_COLUMNAS] = {
		0 };
natural matrix_fila_rutas[UNIVERGA_MAX_FILAS][UNIVERGA_MAX_COLUMNAS] = { 0 };
natural rutas_pot[UNIVERGA_MAX_FILAS][UNIVERGA_MAX_COLUMNAS] = { 0 };
natural rutas_parciales[UNIVERGA_MAX_FILAS][UNIVERGA_MAX_COLUMNAS][UNIVERGA_MAX_COLUMNAS] =
		{ 0 };

natural puta_mierda = 0;

int univerga_compara_ruta(natural *a, natural *b) {
	int resul = 0, j;
	/*
	 for (j = 0; j < UNIVERGA_MAX_COLUMNAS; j++) {
	 */
	for (j = 0; j < puta_mierda; j++) {
		if ((a)[j] != (b)[j]) {
			resul = (int) a[j] - (int) b[j];
			break;
		}
	}
	/*
	 assert_timeout(j < UNIVERGA_MAX_COLUMNAS);
	 */
	assert_timeout(j <= puta_mierda);

	return resul;
}
int univerga_compara_mierda(const void *pa, const void *pb) {
	int res = 0;
	puto_cardinal *a = (puto_cardinal *) pa;
	puto_cardinal *b = (puto_cardinal *) pb;

	if (a->costo == b->costo) {
		res = univerga_compara_ruta(a->extra, b->extra);
	} else {
		if (a->costo < b->costo) {
			res = -1;
		} else {
			if (a->costo > b->costo) {
				res = 1;
			} else {
				res = 0;
			}
		}
	}

	return res;
}

int univerga_compara_ruta_cb(const void *pa, const void *pb) {
	int resul = 0, j;
	natural (*a)[UNIVERGA_MAX_COLUMNAS] =
			(natural (*)[UNIVERGA_MAX_COLUMNAS]) pa;
	natural (*b)[UNIVERGA_MAX_COLUMNAS] =
			(natural (*)[UNIVERGA_MAX_COLUMNAS]) pb;
	resul = univerga_compara_ruta((natural *) a, (natural*) b);
	return resul;
}

static void univerga_core(tipo_dato *matrix_chostos,
		entero_largo matrix_chosto_rutas[UNIVERGA_MAX_FILAS][UNIVERGA_MAX_COLUMNAS],
		natural matrix_fila_rutas[UNIVERGA_MAX_FILAS][UNIVERGA_MAX_COLUMNAS],
		natural filas_tam, natural columnas_tam,
		natural ruta[UNIVERGA_MAX_COLUMNAS], entero_largo *chosto) {
	int i, j, k;
	int movimientos[] = { -1, 0, 1 };
	entero_largo chosto_min = LONG_LONG_MAX;
	natural chosto_min_idx = UNIVERGA_IDX_INVALIDO;
	natural chosto_min_fila_cur = UNIVERGA_IDX_INVALIDO;
	puto_cardinal rutas_ant[3] = { 0 };
	natural ruta_pot_cont = 0;
	puta_mierda = columnas_tam;

	for (i = 0; i < filas_tam; i++) {
		matrix_chosto_rutas[i][0] = *(matrix_chostos + i * columnas_tam);
		matrix_fila_rutas[i][0] = UNIVERGA_IDX_INVALIDO;
		caca_log_debug("puta mierda %d casteado %d",
				*(matrix_chostos +i*columnas_tam), matrix_chosto_rutas[i][0]);
		rutas_parciales[i][0][0] = i;
	}

	caca_log_debug("la matrix de chosto ruta\n%s",
			caca_comun_matrix_a_cadena((tipo_dato *)matrix_chosto_rutas, filas_tam, columnas_tam, CACA_COMUN_BUF_STATICO));
	for (j = 1; j < columnas_tam; j++) {
		for (i = 0; i < filas_tam; i++) {
			for (k = 0; k < 3 && k < filas_tam; k++) {
				int mov_act = movimientos[k];
				int pos_col_ant = i + mov_act;
				if (pos_col_ant == -1) {
					pos_col_ant = filas_tam - 1;
				}
				if (pos_col_ant == filas_tam) {
					pos_col_ant = 0;
				}

				assert_timeout(pos_col_ant >= 0);

				puto_cardinal *final_ruta_act = rutas_ant + k;
				final_ruta_act->costo = matrix_chosto_rutas[pos_col_ant][j - 1];
				final_ruta_act->fila = pos_col_ant;
				final_ruta_act->extra = rutas_parciales[pos_col_ant][j - 1];
				caca_log_debug(
						"considerando ruta de chosto %ld y ruta parcial %s",
						final_ruta_act->costo,
						caca_comun_arreglo_a_cadena_natural(final_ruta_act->extra, j, CACA_COMUN_BUF_STATICO));
			}
			qsort(rutas_ant, k, sizeof(puto_cardinal), univerga_compara_mierda);
			matrix_chosto_rutas[i][j] = rutas_ant->costo
					+ *(matrix_chostos + i * columnas_tam + j);
			matrix_fila_rutas[i][j] = rutas_ant->fila;
			caca_log_debug("mierda costo %u %u es %ld, suma de %ld y %d", i, j,
					matrix_chosto_rutas[i][j], rutas_ant->costo,
					*(matrix_chostos + i * columnas_tam + j));
			caca_log_debug("mierda fila %u %u es %u", i, j,
					matrix_fila_rutas[i][j]);
			caca_log_debug("mierda ruta anteior %s",
					caca_comun_arreglo_a_cadena_natural(rutas_ant->extra, j, CACA_COMUN_BUF_STATICO));
			memcpy(rutas_parciales[i][j], rutas_ant->extra,
					j * sizeof(natural));
			rutas_parciales[i][j][j] = i;
			caca_log_debug("mierda ruta parcial %s",
					caca_comun_arreglo_a_cadena_natural(rutas_parciales[i][j], j+1, CACA_COMUN_BUF_STATICO));

		}
	}
	/*
	 caca_log_debug("la matrix de chosto ruta al term\n%s",
	 caca_comun_matrix_a_cadena((tipo_dato *)matrix_chosto_rutas, filas_tam, columnas_tam, CACA_COMUN_BUF_STATICO));
	 caca_log_debug("la matrix de filas al term\n%s",
	 0	caca_comun_matrix_a_cadena((tipo_dato *)matrix_fila_rutas, filas_tam, columnas_tam, CACA_COMUN_BUF_STATICO));
	 */
	for (i = 0; i < filas_tam; i++) {
		if (matrix_chosto_rutas[i][columnas_tam - 1] < chosto_min) {
			chosto_min_idx = i;
			chosto_min = matrix_chosto_rutas[i][columnas_tam - 1];
		}
	}
	assert_timeout(chosto_min_idx != UNIVERGA_IDX_INVALIDO);
	caca_log_debug("el chosto min es %d en %u", chosto_min, chosto_min_idx);

	for (i = 0; i < filas_tam; i++) {
		j = columnas_tam - 1;
		if (matrix_chosto_rutas[i][j] == chosto_min) {
			chosto_min_fila_cur = i;
			natural *mierda = rutas_parciales[i][j];
			while (chosto_min_fila_cur != UNIVERGA_IDX_INVALIDO) {
				rutas_pot[ruta_pot_cont][j] = chosto_min_fila_cur;
				chosto_min_fila_cur = matrix_fila_rutas[chosto_min_fila_cur][j];
				j--;
			}
			assert_timeout(j == -1);
			caca_log_debug(
					"la puta ruta act %u de fila %u es %s, la ruta almac es %s",
					ruta_pot_cont, i,
					caca_comun_arreglo_a_cadena_natural(rutas_pot[ruta_pot_cont], columnas_tam, CACA_COMUN_BUF_STATICO),
					caca_comun_arreglo_a_cadena_natural(mierda,columnas_tam, CACA_COMUN_BUF_STATICO));
			assert_timeout(
					!univerga_compara_ruta(mierda, rutas_pot[ruta_pot_cont]));
			ruta_pot_cont++;
		}
	}
	qsort(rutas_pot, ruta_pot_cont, sizeof(rutas_pot[0]),
			univerga_compara_ruta_cb);
	memcpy(ruta, rutas_pot[0], columnas_tam * sizeof(natural));
	caca_log_debug("la mierda es %s",
			caca_comun_arreglo_a_cadena_natural(ruta, columnas_tam, CACA_COMUN_BUF_STATICO));
	*chosto = chosto_min;
}

static void univerga_main() {
	natural filas_tam = 0;
	natural columnas_tam = 0;
	natural ruta[UNIVERGA_MAX_COLUMNAS] = { UNIVERGA_IDX_INVALIDO };
	entero_largo chosto = 0;
	int i, j;

	/*	while (scanf("%d %d\n", &filas_tam, &columnas_tam) > 0) {*/
	while (scanf("%d %d\n", &filas_tam, &columnas_tam) != EOF) {
		caca_log_debug("d la mierda %u %u", filas_tam, columnas_tam);
		caca_comun_lee_matrix_long_stdin((tipo_dato *) matrix_chostos, &(int ) {
						0 },
				NULL, filas_tam, columnas_tam);
		caca_log_debug("matrix chostos\n%s",
				caca_comun_matrix_a_cadena((tipo_dato *)matrix_chostos, filas_tam, columnas_tam, CACA_COMUN_BUF_STATICO));
		univerga_core((tipo_dato *) matrix_chostos, matrix_chosto_rutas,
				matrix_fila_rutas, filas_tam, columnas_tam, ruta, &chosto);
		for (i = 0; i < columnas_tam - 1; i++) {
			printf("%d ", ruta[i] + 1);
		}
		printf("%d\n", ruta[i] + 1);
		printf("%lld\n", chosto);
	}
}

int main() {
	univerga_main();
	return EXIT_SUCCESS;
}
