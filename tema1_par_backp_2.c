/*
 * APD - Tema 1
 * Octombrie 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

char *in_filename_julia;
char *in_filename_mandelbrot;
char *out_filename_julia;
char *out_filename_mandelbrot;
int P; // numarul de thread-uri

pthread_barrier_t barrier;



// structura pentru un numar complex

typedef struct _complex {
	double a;
	double b;
} complex;

// structura pentru parametrii unei rulari
typedef struct _params {
	int is_julia, iterations;
	double x_min, x_max, y_min, y_max, resolution;
	complex c_julia;
} params;

// definit structura pentru datele necesare unui thread
/**
typedef struct _data {
	int id;
	params par;
	int width;
	int height;

} data;
*/
params par;
int width, height;
int **result;




// citeste argumentele programului
void get_args(int argc, char **argv)
{
	if (argc < 6) {
		printf("Numar insuficient de parametri:\n\t"
				"./tema1 fisier_intrare_julia fisier_iesire_julia "
				"fisier_intrare_mandelbrot fisier_iesire_mandelbrot P\n");
		exit(1);
	}

	in_filename_julia = argv[1];
	out_filename_julia = argv[2];
	in_filename_mandelbrot = argv[3];
	out_filename_mandelbrot = argv[4];
	P = atoi(argv[5]);
}

// citeste fisierul de intrare
// aici as vrea sa combin read ul si write ul cumva
void read_input_file(char *in_filename, params* par)
{
	FILE *file = fopen(in_filename, "r");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de intrare!\n");
		exit(1);
	}

	fscanf(file, "%d", &par->is_julia);
	fscanf(file, "%lf %lf %lf %lf",
			&par->x_min, &par->x_max, &par->y_min, &par->y_max);
	fscanf(file, "%lf", &par->resolution);
	fscanf(file, "%d", &par->iterations);

	if (par->is_julia) {
		fscanf(file, "%lf %lf", &par->c_julia.a, &par->c_julia.b);
	}

	fclose(file);
}

// scrie rezultatul in fisierul de iesire
void write_output_file(char *out_filename, int **result, int width, int height)
{
	int i, j;

	FILE *file = fopen(out_filename, "w");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de iesire!\n");
		return;
	}

	fprintf(file, "P2\n%d %d\n255\n", width, height);
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			fprintf(file, "%d ", result[i][j]);
		}
		fprintf(file, "\n");
	}


	fclose(file);
}

// aloca memorie pentru rezultat
int **allocate_memory(int width, int height)
{
	int **result;
	int i;

	result = malloc(height * sizeof(int*));

	if (result == NULL) {
		printf("Eroare la malloc!\n");
		exit(1);
	}

	for (i = 0; i < height; i++) {
		result[i] = malloc(width * sizeof(int));
		if (result[i] == NULL) {
			printf("Eroare la malloc!\n");
			exit(1);
		}
	}

	return result;
}

void allocate_memory_p(int id)
{
	//int **result;
	int i;
	int start = id * (double)height / P;
	int end = fmin((id + 1) * (double) height / P, height);

	for (i = start; i < end; i++) {
		result[i] = malloc(width * sizeof(int));
		if (result[i] == NULL) {
			printf("Eroare la malloc! paralel\n");
			exit(1);
		}
	}
	
}






// elibereaza memoria alocata
void free_memory(int **result, int height)
{
	int i;

	for (i = 0; i < height; i++) {
		free(result[i]);
	}
	free(result);
}

void free_memory_p(int id)
{
	int i;

	int start = id * (double)height / P;
	int end = fmin((id + 1) * (double) height / P, height);

	for (i = start; i < end; i++) {
		free(result[i]);
	}
	
}



void run_julia(int id)
{
	int w, h, i;
	int start = id * (double)height / P;
	int end = fmin((id + 1) * (double) height / P, height);

	for (w = 0; w < width; w++) {
		for (h = start; h < end; h++) {

			int step = 0;
			complex z = { .a = w * par.resolution + par.x_min,
							.b = h * par.resolution + par.y_min };

			// asta sta pe loc
			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < par.iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2) - pow(z_aux.b, 2) + par.c_julia.a;
				z.b = 2 * z_aux.a * z_aux.b + par.c_julia.b;

				step++;
			}

			result[h][w] = step % 256;
		}
	}
	
	pthread_barrier_wait(&barrier);

	start = id * (double)height / 2 / P;
	end = fmin((id + 1) * (double) height/ 2 / P, height / 2);


	// exista cumva o metoda prin care pot scrie coord 
	// direct bine fara sa le mai transform?, in write_file sau ceva
	
	for (i = start; i < end; i++) {
		int *aux = result[i];
		result[i] = result[height - i - 1];
		result[height - i - 1] = aux;
	}

	pthread_barrier_wait(&barrier);

}



void run_mandelbrot(int id)
{
	int w, h, i;

	int start = id * (double)height / P;
	int end = fmin((id + 1) * (double) height / P, height);

	for (w = 0; w < width; w++) {
		for (h = start; h < end; h++) {
			complex c = { .a = w * par.resolution + par.x_min,
							.b = h * par.resolution + par.y_min };
			complex z = { .a = 0, .b = 0 };
			int step = 0;

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < par.iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2.0) - pow(z_aux.b, 2.0) + c.a;
				z.b = 2.0 * z_aux.a * z_aux.b + c.b;

				step++;
			}

			result[h][w] = step % 256;
		}
	}



	pthread_barrier_wait(&barrier);
	
	start = id * (double)height / 2 / P;
	end = fmin((id + 1) * (double) height/ 2 / P, height / 2);

	
	for (i = start; i < end; i++) {
		int *aux = result[i];
		result[i] = result[height - i - 1];
		result[height - i - 1] = aux;
	}

	pthread_barrier_wait(&barrier);

}

void *thread_function(void *arg) {

	int id = *(int *)arg;

	run_julia(id);


	pthread_barrier_wait(&barrier);

	if (id == 0) {
		
	write_output_file(out_filename_julia, result, width, height);
	}
	
	//bariera

	pthread_barrier_wait(&barrier);
	free_memory_p(id); 
	pthread_barrier_wait(&barrier);

	if (id == 0) {
	free(result);
	read_input_file(in_filename_mandelbrot, &par);

	width = (par.x_max - par.x_min) / par.resolution;
	height = (par.y_max - par.y_min) / par.resolution;
	//result = allocate_memory(width, height);
	result = malloc(height * sizeof(int*));
	if (result == NULL) {
		printf("Eroare la malloc!in thread\n");
		exit(1);
	}

    }
    
    pthread_barrier_wait(&barrier);
    allocate_memory_p(id);
	
    pthread_barrier_wait(&barrier);

	run_mandelbrot(id);


	pthread_exit(NULL);

}


int main(int argc, char *argv[])
{



	// declar threadurile si param necesari

	// se citesc argumentele programului
	get_args(argc, argv);

	pthread_t threads[P];
	int tid[P];	
	pthread_barrier_init(&barrier, NULL, P);


	int r;
	read_input_file(in_filename_julia, &par);

	width = (par.x_max - par.x_min) / par.resolution;
	height = (par.y_max - par.y_min) / par.resolution;

	result = allocate_memory(width, height);

	// se creaza thread-urile
	
	for (int i = 0; i < P; i++) {

		tid[i] = i;
		r = pthread_create(&threads[i], NULL, thread_function, &(tid[i]));

		if (r) {
	  		printf("Eroare la crearea thread-ului %d\n", i);
	  		exit(-1);
		}
  	}

  	// se asteapta thread-urile
	
	for (int i = 0; i < P; i++) {
		pthread_join(threads[i], NULL);
	}

	

	pthread_barrier_destroy(&barrier);

	write_output_file(out_filename_mandelbrot, result, width, height);
	
	free_memory(result, height);


	return 0;
}
