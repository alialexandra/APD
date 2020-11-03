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
int N; // size-ul matricii
int chunk_size;
//params par;
//int width, height;
int **result;




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

// daca nu merge, o sa ma gandesc daca sa adaug height sau width
typedef struct _data {
	int id;
	params par;
	int width;
	int height;

} data;



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

// elibereaza memoria alocata
void free_memory(int **result, int height)
{
	int i;

	for (i = 0; i < height; i++) {
		free(result[i]);
	}
	free(result);
}


void *julia(void *arg) {


	//data* thread_data = *(data *)arg;

	data* tdata = (data *)arg;

	// limitele pentru o impartire egala

	int id = tdata->id;
	int width = tdata->width;
	int height = tdata->height;
	N = width * height;
	chunk_size = N / P;


	
	int start = id * chunk_size;
	int end = fmin((id + 1) * chunk_size, N);

	int w, h, i;

	for (int k = start; i < end; ++k)
	{
		
	for (w = 0; w < width; w++) {
		for (h = 0; h < height; h++) {
			int step = 0;
			complex z = { .a = w * tdata->par.resolution + tdata->par.x_min,
							.b = h * tdata->par.resolution + tdata->par.y_min };

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < tdata->par.iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2) - pow(z_aux.b, 2) + tdata->par.c_julia.a;
				z.b = 2 * z_aux.a * z_aux.b + tdata->par.c_julia.b;

				step++;
			}

			result[h][w] = step % 256;
		}
	}

	// transforma rezultatul din coordonate matematice in coordonate ecran
	// de paralelizat
	// cred ca trb pusa o bariera

	for (i = 0; i < height / 2; i++) {
		int *aux = result[i];
		result[i] = result[height - i - 1];
		result[height - i - 1] = aux;
	}

	}

	pthread_exit(NULL);

}


// ruleaza algoritmul Julia 
// de paralelizat

void run_julia(params *par, int **result, int width, int height)
{
	int w, h, i;

	for (w = 0; w < width; w++) {
		for (h = 0; h < height; h++) {
			int step = 0;
			complex z = { .a = w * par->resolution + par->x_min,
							.b = h * par->resolution + par->y_min };

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < par->iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2) - pow(z_aux.b, 2) + par->c_julia.a;
				z.b = 2 * z_aux.a * z_aux.b + par->c_julia.b;

				step++;
			}

			result[h][w] = step % 256;
		}
	}

	// transforma rezultatul din coordonate matematice in coordonate ecran
	// de paralelizat
	// cred ca trb pusa o bariera
	for (i = 0; i < height / 2; i++) {
		int *aux = result[i];
		result[i] = result[height - i - 1];
		result[height - i - 1] = aux;
	}
}

// ruleaza algoritmul Mandelbrot
//TODO: de paralelizat



void *mandelbrot(void *arg) {



	data* tdata = (data *)arg;

	// limitele pentru o impartire egala

	int id = tdata->id;
	int width = tdata->width;
	int height = tdata->height;
	N = width * height;
	chunk_size = N / P;



	int start = id * chunk_size;
	int end = fmin((id + 1) * chunk_size, N);

	int w, h, i;


	for(int k = start; k < end; k++){

	for (w = 0; w < width; w++) {
		for (h = 0; h < height; h++) {
			complex c = { .a = w * tdata->par.resolution + tdata->par.x_min,
							.b = h * tdata->par.resolution + tdata->par.y_min };
			complex z = { .a = 0, .b = 0 };
			int step = 0;

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < tdata->par.iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2.0) - pow(z_aux.b, 2.0) + c.a;
				z.b = 2.0 * z_aux.a * z_aux.b + c.b;

				step++;
			}

			result[h][w] = step % 256;
		}
	}
	}

	// transforma rezultatul din coordonate matematice in coordonate ecran
	// TODO: de paralelizat
	for (i = 0; i < height / 2; i++) {
		int *aux = result[i];
		result[i] = result[height - i - 1];
		result[height - i - 1] = aux;
	}
	pthread_exit(NULL);
}



void run_mandelbrot(params *par, int **result, int width, int height)
{
	int w, h, i;

	for (w = 0; w < width; w++) {
		for (h = 0; h < height; h++) {
			complex c = { .a = w * par->resolution + par->x_min,
							.b = h * par->resolution + par->y_min };
			complex z = { .a = 0, .b = 0 };
			int step = 0;

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < par->iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2.0) - pow(z_aux.b, 2.0) + c.a;
				z.b = 2.0 * z_aux.a * z_aux.b + c.b;

				step++;
			}

			result[h][w] = step % 256;
		}
	}

	// transforma rezultatul din coordonate matematice in coordonate ecran
	// TODO: de paralelizat
	for (i = 0; i < height / 2; i++) {
		int *aux = result[i];
		result[i] = result[height - i - 1];
		result[height - i - 1] = aux;
	}
}



int main(int argc, char *argv[])
{

	/* facem gloabale
	params par;
	int **result;
	*/

	int width, height;

	params par;

	// declar threadurile si param necesari
	// aici nu mi dau seama daca e suficient doar un array de threads???


	// se citesc argumentele programului
	get_args(argc, argv);

	pthread_t threads[P];
	//int tid[P];

	data *thread_data = (data* ) malloc(sizeof (data) * P);

	int r;
	//void *status;

	// Julia:
	// - se citesc parametrii de intrare
	// - se aloca tabloul cu rezultatul
	// - se ruleaza algoritmul
	// - se scrie rezultatul in fisierul de iesire
	// - se elibereaza memoria alocata
	read_input_file(in_filename_julia, &par);

	width = (par.x_max - par.x_min) / par.resolution;
	height = (par.y_max - par.y_min) / par.resolution;
	thread_data->par = par;
	thread_data->height = height;
	thread_data->width = width;

	result = allocate_memory(width, height);
	// aici treb sa am threadurile create
	//run_julia(&par, result, width, height);
	//aici ar trebui sa se intample paralelizarea pt julia
	
	for (int i = 0; i < P; i++) {

		thread_data[i].id = i;
		r = pthread_create(&threads[i], NULL, julia, &(thread_data[i]));

		if (r) {
	  		printf("Eroare la crearea thread-ului %d\n", i);
	  		exit(-1);
		}
  	}



  	// se asteapta thread-urile
	
	for (int i = 0; i < P; i++) {
		pthread_join(threads[i], NULL);
	}



	write_output_file(out_filename_julia, result, width, height);
	free_memory(result, height);

	// Mandelbrot:
	// - se citesc parametrii de intrare
	// - se aloca tabloul cu rezultatul
	// - se ruleaza algoritmul
	// - se scrie rezultatul in fisierul de iesire
	// - se elibereaza memoria alocata
	read_input_file(in_filename_mandelbrot, &par);

	width = (par.x_max - par.x_min) / par.resolution;
	height = (par.y_max - par.y_min) / par.resolution;


	result = allocate_memory(width, height);

	thread_data->par = par;
	thread_data->height = height;
	thread_data->width = width;
	//run_mandelbrot(&par, result, width, height);
	
	for (int i = 0; i < P; i++) {

		thread_data[i].id = i;
		r = pthread_create(&threads[i], NULL, mandelbrot, &(thread_data[i]));

		if (r) {
	  		printf("Eroare la crearea thread-ului %d\n", i);
	  		exit(-1);
		}
  	}


  	// se asteapta thread-urile
	
	for (int i = 0; i < P; i++) {
		pthread_join(threads[i], NULL);
	}

	

	write_output_file(out_filename_mandelbrot, result, width, height);
	free_memory(result, height);

	return 0;
}
