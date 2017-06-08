#include "sparse_formats.h"

triplet* triplet_new_array(const size_t N) {
	//dispatch on location
	return (triplet*)malloc(N * sizeof(triplet));
}

int triplet_comparator(const void *v1, const void *v2)
{
	const triplet* t1 = (triplet*)v1;
	const triplet* t2 = (triplet*)v2;

	if (t1->i < t2->i)
		return -1;
	else if (t1->i > t2->i)
		return +1;
	else if (t1->j < t2->j)
		return -1;
	else if (t1->j > t2->j)
		return +1;
	else
		return 0;
}

int unsigned_int_comparator(const void* v1, const void* v2)
{
	const unsigned int int1 = *((unsigned int*)v1);
	const unsigned int int2 = *((unsigned int*)v2);

	if (int1 < int2)
		return -1;
	else if (int1 > int2)
		return +1;
	else
		return 0;
}

void write_csr(const csr_matrix* csr, const unsigned int num_csr, const char* file_path)
{
	FILE* fp;
	int i, j;
	fp = fopen(file_path, "w");
	if(fp != NULL) printf("sparse_formats.write_csr() - Cannot Open File \n");
	fprintf(fp, "%u\n\n", num_csr);

	for (j = 0; j<num_csr; j++)
	{
		fprintf(fp, "%u\n%u\n%u\n%u\n%lf\n%lf\n%lf\n", csr[j].num_rows, csr[j].num_cols, csr[j].num_nonzeros, csr[j].density_ppm, csr[j].density_perc, csr[j].nz_per_row, csr[j].stddev);

		for (i = 0; i <= csr[j].num_rows; i++)
			fprintf(fp, "%u ", csr[j].Ap[i]);
		fprintf(fp, "\n");

		for (i = 0; i<csr[j].num_nonzeros; i++)
			fprintf(fp, "%u ", csr[j].Aj[i]);
		fprintf(fp, "\n");

		for (i = 0; i<csr[j].num_nonzeros; i++)
			fprintf(fp, "%f ", csr[j].Ax[i]);
		fprintf(fp, "\n\n");
	}

	fclose(fp);
}

csr_matrix* read_csr(const unsigned int* num_csr, const char* file_path)
{
	FILE* fp;
	int i, j, read_count;
	csr_matrix* csr;
	
	if(num_csr == NULL) printf("sparse_formats.read_csr() - ptr to num_csr is NULL! \n");
	
	printf(file_path);
	fp = fopen(file_path, "r");
	if (fp == NULL) perror("error");//printf("sparse_formats.read_csr() - Cannot Open Input File \n");
	
	read_count = fscanf(fp, "%u\n\n", num_csr);
	
	if(read_count != 1) printf("sparse_formats.read_csr() - Input File Corrupted! Read count for num_csr differs from 1 \n");
	csr = (csr_matrix* )malloc(sizeof(struct csr_matrix)*(*num_csr));
	
	for (j = 0; j<*num_csr; j++)
	{
		read_count = fscanf(fp, "%u\n%u\n%u\n%u\n%lf\n%lf\n%lf\n", &(csr[j].num_rows), &(csr[j].num_cols), &(csr[j].num_nonzeros), &(csr[j].density_ppm), &(csr[j].density_perc), &(csr[j].nz_per_row), &(csr[j].stddev));
		if(read_count != 7) printf("sparse_formats.read_csr() - Input File Corrupted! Read count for header info differs from 7 \n");
		
		read_count = 0;
		csr[j].Ap = (unsigned int*)int_new_array(csr[j].num_rows + 1, "sparse_formats.read_csr() - Heap Overflow! Cannot allocate space for csr.Ap \n");
		for (i = 0; i <= csr[j].num_rows; i++)
			read_count += fscanf(fp, "%u ", csr[j].Ap + i);
		if(read_count != (csr[j].num_rows + 1)) printf("sparse_formats.read_csr() - Input File Corrupted! Read count for Ap differs from csr[j].num_rows+1 \n");

		read_count = 0;
		csr[j].Aj = (unsigned int*)int_new_array(csr[j].num_nonzeros, "sparse_formats.read_csr() - Heap Overflow! Cannot allocate space for csr.Aj \n");
		for (i = 0; i<csr[j].num_nonzeros; i++)
			read_count += fscanf(fp, "%u ", csr[j].Aj + i);
		if(read_count != (csr[j].num_nonzeros)) printf("sparse_formats.read_csr() - Input File Corrupted! Read count for Aj differs from csr[j].num_nonzeros \n");

		read_count = 0;
		csr[j].Ax = (float*)float_new_array(csr[j].num_nonzeros, "sparse_formats.read_csr() - Heap Overflow! Cannot allocate space for csr.Ax \n");
		for (i = 0; i<csr[j].num_nonzeros; i++)
			read_count += fscanf(fp, "%f ", csr[j].Ax + i);
		if(read_count != (csr[j].num_nonzeros)) printf("sparse_formats.read_csr() - Input File Corrupted! Read count for Ax differs from csr[j].num_nonzeros \n");
	}

	fclose(fp);
	return csr;
}

void print_timestamp(FILE* stream)
{
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	fprintf(stream, "Current time: %s", asctime(timeinfo));
}

unsigned long gen_rand(const long LB, const long HB)
{
	int range = HB - LB + 1;
	if((HB >= 0 && LB >= 0 && range > 0)) printf("sparse_formats.gen_rand() - Invalid Bound(s). Exiting...");
	return (rand() % range) + LB;
}



int bin_search(const triplet* data, int size, const triplet* key)
{
	triplet* mid_triplet;
	int lo, hi, m;
	lo = 0;
	hi = size - 1;
	while (lo <= hi) //binary search to determine if element exists and, if not, what is the proper index for insertion
	{
		m = lo + ((hi - lo) / 2);
		if (triplet_comparator(key, &(data[m])) > 0)
			lo = m + 1;
		else if (triplet_comparator(key, &(data[m])) < 0)
			hi = m - 1;
		else
			return m;
	}
	return (-1 * lo - 1);
}

coo_matrix rand_coo(const unsigned int N, const unsigned long density, FILE* log)
{
	coo_matrix coo;
	triplet tmp, *current_triplet, *mid_triplet;

	unsigned int ind;
	int m;

	coo.num_rows = N;
	coo.num_cols = N;
	coo.density_ppm = density;
	coo.num_nonzeros = (((double)(N*density)) / 1000000.0)*N;
	printf("NUM_nonzeros: %d\n", coo.num_nonzeros);

	coo.non_zero = triplet_new_array(coo.num_nonzeros);
	if(coo.non_zero != NULL) printf("sparse_formats.rand_coo_bin_insertion(): Heap Overflow - Cannot allocate memory for coo.non_zero\n");
	print_timestamp(log);
	fprintf(log, "Memory Allocated. Generating Data...\n");

	current_triplet = &(coo.non_zero[0]); //Generate random first element
	(current_triplet->i) = gen_rand(0, N - 1);
	(current_triplet->j) = gen_rand(0, N - 1);

	for (ind = 1; ind<coo.num_nonzeros; ind++)
	{
		current_triplet = &(coo.non_zero[ind]); //element to be inserted
		(current_triplet->i) = gen_rand(0, N - 1);
		(current_triplet->j) = gen_rand(0, N - 1);

		m = bin_search(coo.non_zero, ind, current_triplet);
		if (m < 0)
		{
			m = -1 * m - 1;
		}
		else
		{
			ind--;
			continue;
		}

		if (m < ind)
		{
			tmp = *current_triplet;
			memmove(coo.non_zero + m + 1, coo.non_zero + m, sizeof(triplet)*(ind - m));
			coo.non_zero[m] = tmp;
		}
	}

	for (ind = 0; ind<coo.num_nonzeros; ind++)
	{
		current_triplet = &(coo.non_zero[ind]);
		(current_triplet->v) = 1.0 - 2.0 * (rand() / (2147483647 + 1.0));
		while ((current_triplet->v) == 0.0)
			(current_triplet->v) = 1.0 - 2.0 * (rand() / (2147483647 + 1.0));
	}

	print_timestamp(log);
	fprintf(log, "Matrix Completed. Returning...\n");

	return coo;
}

void print_coo_metadata(const coo_matrix* coo, FILE* stream) {
	fprintf(stream, "\nCOO Matrix Metadata:\n\nNRows=%d\tNCols=%d\tNNZ=%d\tDensity (ppm)=%d\tDensity (fract)=%g\n\n", coo->num_rows, coo->num_cols, coo->num_nonzeros, coo->density_ppm, (double)(coo->density_ppm / 1000000.0));
}

void print_csr_metadata(const csr_matrix* csr, FILE* stream) {
	fprintf(stream, "\nCSR Matrix Metadata:\n\nNRows=%lu\tNCols=%lu\tNNZ=%lu\tDensity=%lu ppm = %g%%\tAverage NZ/Row=%g\tStdDev NZ/Row=%g\n\n", csr->num_rows, csr->num_cols, csr->num_nonzeros, csr->density_ppm, csr->density_perc, csr->nz_per_row, csr->stddev);
}

void print_coo(const coo_matrix* coo, FILE* stream)
{
	unsigned int ind;
	fprintf(stream, "\nPrinting COO Matrix in COO Form:\n\nNRows=%d\nNCols=%d\nNNZ=%d\nDensity (ppm)=%d\nDensity (fract)=%g\n", coo->num_rows, coo->num_cols, coo->num_nonzeros, coo->density_ppm, (double)(coo->density_ppm / 1000000.0));
	for (ind = 0; ind<coo->num_nonzeros; ind++)
		fprintf(stream, "(%2d,%2d,%5.2f)\n", coo->non_zero[ind].i, coo->non_zero[ind].j, coo->non_zero[ind].v);
}

void print_coo_std(const coo_matrix* coo, FILE* stream)
{
	int ind, ind2, nz_count = 0;
	float val;

	fprintf(stream, "\nPrinting COO Matrix in Standard Form:\n\nNRows=%d\nNCols=%d\nNNZ=%d\nDensity (ppm)=%d\nDensity (fract)=%g\n", coo->num_rows, coo->num_cols, coo->num_nonzeros, coo->density_ppm, (double)(coo->density_ppm / 1000000.0));

	for (ind = 0; ind<coo->num_rows; ind++)
	{
		fprintf(stream, "[");
		for (ind2 = 0; ind2<coo->num_cols; ind2++)
		{
			if (ind == coo->non_zero[nz_count].i && ind2 == coo->non_zero[nz_count].j)
				val = coo->non_zero[nz_count++].v;
			else
				val = 0.0;
			fprintf(stream, "%6.2f", val);
		}
		fprintf(stream, "]\n");
	}
}

void print_csr_arr_std(const csr_matrix* csr, const unsigned int num_csr, FILE* stream)
{
	unsigned int k;
	for (k = 0; k<num_csr; k++)
		print_csr_std(&csr[k], stream);
}

void print_csr_std(const csr_matrix* csr, FILE* stream)
{
	int ind, ind2, nz_count = 0, row_count = 0, next_nz_row;
	float val, density;
	density = ((float)(csr->num_nonzeros)) / (((float)(csr->num_rows))*((float)(csr->num_cols)));

	print_csr_metadata(csr, stream);

	while (csr->Ap[row_count + 1] == nz_count)
		row_count++;

	for (ind = 0; ind<csr->num_rows; ind++)
	{
		fprintf(stream, "[");
		for (ind2 = 0; ind2<csr->num_cols; ind2++)
		{
			if (ind == row_count && ind2 == csr->Aj[nz_count])
			{
				val = csr->Ax[nz_count++];
				while (csr->Ap[row_count + 1] == nz_count)
					row_count++;
			}
			else
				val = 0.0;
			fprintf(stream, "%6.2f", val);
		}
		fprintf(stream, "]\n");
	}
	fprintf(stream, "\n");
}

void free_csr(csr_matrix* csr, const unsigned int num_csr)
{
	int k;
	for (k = 0; k<num_csr; k++)
	{
		free(csr[k].Ap);
		free(csr[k].Aj);
		free(csr[k].Ax);
	}
	free(csr);
}
