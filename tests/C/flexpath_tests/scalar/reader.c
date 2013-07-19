/**
 * reader.c
 *
 * ADIOS is freely available under the terms of the BSD license described
 * in the COPYING file in the top level directory of this source distribution.
 *
 * Copyright (c) 2008 - 2009.  UT-BATTELLE, LLC. All rights reserved.
 *
 * Created on: Jul 1, 2013
 * Author: Magda Slawinska aka Magic Magg magg dot gatech at gmail.com
 *
 * The process reads an integer scalar. It should be equal to its rank.
 * Excessive processes (if rank > the number of writers) quit. It is recommended,
 * however, to run as many readers as writers.
 */

#include "mpi.h"
#include "adios.h"
#include "adios_read.h"

#include "misc.h"
#include "utils.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>


// sorry just to be sure that nothing bad is with macros in macros
#ifdef FLEXPATH_METHOD
#define CLOSE_ADIOS \
	do { 														\
		adios_read_close(adios_handle);  						\
		adios_read_finalize_method(ADIOS_READ_METHOD_FLEXPATH);	\
		MPI_Finalize(); \
	} while (0)
#else
#define CLOSE_ADIOS \
	do { 														\
		adios_read_close(adios_handle);  						\
		adios_read_finalize_method(ADIOS_READ_METHOD_BP);		\
		MPI_Finalize(); \
	} while (0)
#endif

#define JUST_CLEAN \
	do {								\
		adios_selection_delete(sel);	\
		sel = NULL;						\
	} while (0)

#define CLEAN_ON_ERROR_AND_CLOSE_ADIOS 	\
	do {								\
		JUST_CLEAN;						\
		CLOSE_ADIOS;					\
	} while (0)


int main (int argc, char **argv){
	char filename[256];
	int rank =0, size =0;
	int my_scalar = -1;         // this will hold what I got from the writer
	MPI_Comm comm = MPI_COMM_WORLD;
	int diag = 0;  // to store the diagnostic information; 0 is OK, -1 error
	int test_passed = TEST_PASSED;   // TEST_PASSED if test passed; TEST_FAILED if test failed

	if (1 < argc){
		usage(argv[0], "Runs readers. It is recommended to run as many readers as writers.");
		return 0;
	}
	// adios read initialization
	MPI_Init( &argc, &argv);
	MPI_Comm_rank (comm, &rank);

	// choose the right method depending on the method
#ifdef FLEXPATH_METHOD
	if( (diag = adios_read_init_method( ADIOS_READ_METHOD_FLEXPATH, comm, ADIOS_OPTIONS)) != 0){
#else
	if( (diag = adios_read_init_method( ADIOS_READ_METHOD_BP, comm, ADIOS_OPTIONS)) != 0){
#endif
		printf("ERROR: Quitting ... Issues with initialization adios_read: (%d) %s\n", adios_errno, adios_errmsg());
		return -1;
	}

	// I will be working with streams so the lock mode is necessary,
	// return immediately if the stream unavailable
	ADIOS_FILE *adios_handle = adios_read_open(FILE_NAME,
#ifdef FLEXPATH_METHOD
			ADIOS_READ_METHOD_FLEXPATH,
#else
			ADIOS_READ_METHOD_BP,
#endif
			comm, ADIOS_LOCKMODE_NONE, 0.0);
	if ( !adios_handle){
		printf("ERROR: Quitting ... (%d) %s\n", adios_errno, adios_errmsg());
		return -1;
	}

	// define portions of data how they will be read
	ADIOS_SELECTION *sel = NULL;
	ADIOS_VARINFO *avi = NULL;

	// read how many processors wrote that array
	avi = adios_inq_var (adios_handle, "size");
	if (!avi){
		printf("ERROR: rank %d: Quitting ... (%d) %s\n", rank, adios_errno, adios_errmsg());
		CLOSE_ADIOS;
		return -1;
	}
	size = *((int*)avi->value);
	adios_free_varinfo(avi);
	avi = NULL;

	// if I run more readers than writers; just release
	// the excessive readers
	if (rank >= size){
		printf("INFO: rank %d: I am an excessive rank. Nothing to read ...\n", rank);
		CLOSE_ADIOS;
		return 0;
	}

	// this is the index of the written block
	sel = adios_selection_writeblock(rank);
	if( !sel ){
		printf("ERROR: rank %d: Quitting ... (%d) %s\n", rank, adios_errno, adios_errmsg());
		CLEAN_ON_ERROR_AND_CLOSE_ADIOS;
		return -1;
	}

	// TODO as of 2013-07-08, I was told that err_end_of_stream doesn't work
	// as it supposed to work
	//while(adios_errno != err_end_of_stream){

		if (adios_schedule_read(adios_handle, sel, "lucky_scalar",0,1,&my_scalar) != 0){
			printf("ERROR: rank %d: Quitting ...(%d) %s\n", rank, adios_errno, adios_errmsg());
			CLEAN_ON_ERROR_AND_CLOSE_ADIOS;
			return -1;
		}

		// not sure if this assumption is correct; difficult to find in the ADIOS sources
		if (adios_perform_reads(adios_handle, 1) != 0){
			printf("ERROR: rank %d: Quitting ...(%d) %s\n", rank, adios_errno, adios_errmsg());
			CLEAN_ON_ERROR_AND_CLOSE_ADIOS;
			return -1;
		}

		if( rank == my_scalar){
			printf("\nRank %d: my_scalar=%d. TEST PASSED (rank == my_scalar)\n", rank,  my_scalar);
			test_passed = TEST_PASSED;
		} else {
			printf("\nRank %d: my_scalar=%d. TEST FAILED (rank != my_scalar)\n", rank,  my_scalar);
			test_passed = TEST_FAILED;
		}
	//}

	// clean everything
	JUST_CLEAN;
	CLOSE_ADIOS;

	printf("\n");
	return test_passed;
}