#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <strings.h>




int main(int argc, char* argv[])
{

	FILE* w1, * w2, * wyC, * tmul, * tall;
	int nth = 1;
	int i, j, k;
	int ILEWIERSZY = 1000;
	int ILEKOLUMN = 1000;
	double start, end, Start, End;
	char* A_filename = "A_matrix";
	char* B_filename = "B_matrix";
	char* C_filename = "C_outmatrix";
	char czas_mnozenia[30] = "czas_mnozenia_liczbawatkow-";
	char czas_calkowity[30] = "czas_calkowity_liczbawatkow-";
	char suf[5];
	int s = sizeof(double);
	long int elem = ILEWIERSZY * ILEKOLUMN;
	double** A, ** B, ** C;
	if (argc >= 1)
		nth = atoi(argv[1]);
	sprintf(suf, "%d", nth);
	strcat(czas_mnozenia, suf);
	strcat(czas_calkowity, suf);
	A = (double**)malloc(ILEWIERSZY * sizeof(double*));
	B = (double**)malloc(ILEWIERSZY * sizeof(double*));
	C = (double**)malloc(ILEWIERSZY * sizeof(double*));
	for (i = 0; i < ILEWIERSZY; ++i) {
		A[i] = (double*)malloc(ILEKOLUMN * sizeof(double));
		B[i] = (double*)malloc(ILEKOLUMN * sizeof(double));
		C[i] = (double*)malloc(ILEKOLUMN * sizeof(double));
	}

	w1 = fopen(A_filename, "rb+");
	w2 = fopen(B_filename, "rb+");
	wyC = fopen(C_filename, "wb+");
	tmul = fopen(czas_mnozenia, "a+");
	tall = fopen(czas_calkowity, "a+");
	/* generowanie tablic z danymi
			for(j = 0; j < ILEWIERSZY; j++)
			for(k = 0; k < ILEKOLUMN; k++)
				{
				   A[j][k] = rand()%96;
				   B[j][k] = rand()%89;
				   C[j][k] = 0;
				 }
	for(j = 0; j < ILEWIERSZY; j++)
	 {
	  fwrite(A[j] , sizeof(double),  ILEKOLUMN, w1 );
	  fwrite(B[j] , sizeof(double),  ILEKOLUMN, w2 );
	 }
	*/
	/*odczytuję tablice z plików*/
	Start = omp_get_wtime();
	for (j = 0; j < ILEWIERSZY; j++)
	{
		fread(A[j], sizeof(double), ILEKOLUMN, w1);
		fread(B[j], sizeof(double), ILEKOLUMN, w2);
	}
	fclose(w1);
	fclose(w2);


	omp_set_nested(1);
	start = omp_get_wtime();
	/*	#pragma omp parallel num_threads(3)
		{
		   //id = omp_get_thread_num();
		#pragma omp parallel for  num_threads(3) private(i,j,k)*/
#pragma omp parallel default(none) num_threads(nth) private(i,j,k) shared(A,B,C) firstprivate(ILEWIERSZY,ILEKOLUMN)
	{
#pragma omp for  schedule(static)
		for (i = 0; i < ILEWIERSZY; i++)
			for (j = 0; j < ILEWIERSZY; j++)
				for (k = 0; k < ILEKOLUMN; k++)
					C[i][j] = C[i][j] + A[i][k] * B[k][j];
	}
	end = omp_get_wtime();

	printf("Mnozenie zajelo %lf sec. time.\n", end - start);
	for (j = 0; j < ILEWIERSZY; j++)
		fwrite(C[j], sizeof(double), ILEKOLUMN, wyC);
	End = omp_get_wtime();
	fprintf(tmul, "%lf\n", end - start);
	fprintf(tall, "%lf\n", End - Start);
	printf("Czas calkowity %lf sec. time.\n", End - Start);
	printf("Ilosc watkow %d.\n", nth);
	fclose(wyC);
	fclose(tmul);
	fclose(tall);

	/*zwolnienie pamięci*/
	for (i = 0; i < ILEWIERSZY; ++i) {
		free(A[i]);
		free(B[i]);
		free(C[i]);
	}
	free(A);
	free(B);
	free(C);
	return 0;
}
