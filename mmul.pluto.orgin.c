#include <omp.h>
#include <math.h>
#define ceild(n,d)  ceil(((double)(n))/((double)(d)))
#define floord(n,d) floor(((double)(n))/((double)(d)))
#define max(x,y)    ((x) > (y)? (x) : (y))
#define min(x,y)    ((x) < (y)? (x) : (y))

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
	char czas_mnozenia[30] = "czas_mnozenia_kafelki";
	char czas_calkowity[30] = "czas_calkowity_kafelki";
	char suf[5];
	int s = sizeof(double);
	long int elem = ILEWIERSZY * ILEKOLUMN;
	double** A, ** B, ** C;
	if (argc >= 1)
		nth = atoi(argv[1]);
	sprintf(suf, "%d", nth);

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
	/*odczytujê tablice z plików*/
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

	int t1, t2, t3, t4, t5, t6;

	int lb, ub, lbp, ubp, lb2, ub2;
	register int lbv, ubv;

	/* Start of CLooG code */
	if ((ILEKOLUMN >= 1) && (ILEWIERSZY >= 1)) {
		lbp = 0;
		ubp = floord(ILEWIERSZY - 1, nth);
#pragma omp parallel for private(lbv,ubv)
		for (t1 = lbp; t1 <= ubp; t1++) {
			for (t2 = 0; t2 <= floord(ILEWIERSZY - 1, nth); t2++) {
				for (t3 = 0; t3 <= floord(ILEKOLUMN - 1, nth); t3++) {
					for (t4 = nth * t1; t4 <= min(ILEWIERSZY - 1, nth * t1 + nth); t4++) {
						for (t5 = nth * t3; t5 <= min(ILEKOLUMN - 1, nth * t3 + nth); t5++) {
							lbv = nth * t2;
							ubv = min(ILEWIERSZY - 1, nth * t2 + nth);
#pragma ivdep
#pragma vector always
							for (t6 = lbv; t6 <= ubv; t6++) {
								C[t4][t6] = C[t4][t6] + A[t4][t5] * B[t5][t6];;
							}
						}
					}
				}
			}
		}
	}
	/* End of CLooG code */
	end = omp_get_wtime();

	printf("Mnozenie zajelo %lf sec. time.\n", end - start);
	for (j = 0; j < ILEWIERSZY; j++)
		fwrite(C[j], sizeof(double), ILEKOLUMN, wyC);
	End = omp_get_wtime();
	fprintf(tmul, "%lf\n", end - start);
	fprintf(tall, "%lf\n", End - Start);
	printf("Czas calkowity %lf sec. time.\n", End - Start);
	printf("Ilosc kafelkow %d.\n", nth);
	fclose(wyC);
	fclose(tmul);
	fclose(tall);

	/*zwolnienie pamiêci*/
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
