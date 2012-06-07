/* 
 * ADIOS is freely available under the terms of the BSD license described
 * in the COPYING file in the top level directory of this source distribution.
 *
 * Copyright (c) 2008 - 2009.  UT-BATTELLE, LLC. All rights reserved.
 */

#ifndef __ADIOS_ERROR_H_
#define __ADIOS_ERROR_H_

enum ADIOS_ERRCODES {
    err_no_error                        = 0,
    err_no_memory                       = -1,
    err_MPI_open_error                  = -2,
    err_file_not_found                  = -3,
    err_invalid_file_pointer            = -4,
    err_invalid_group                   = -5,
    err_invalid_group_struct            = -6,
    err_invalid_varid                   = -7,
    err_invalid_varname                 = -8,
    err_corrupted_variable              = -9,

    err_invalid_attrid                  = -10,
    err_invalid_attrname                = -11,
    err_corrupted_attribute             = -12,
    err_invalid_attribute_reference     = -13,
    err_invalid_timestep                = -14,
    err_no_data_at_timestep             = -15,
    err_time_at_wrong_dimension         = -16,
    err_invalid_read_method             = -17,
    err_connection_failed               = -18,
    err_out_of_bound                    = -19,

    err_operation_not_supported         = -20,
    err_end_of_stream                   = -21,    // stream: reached end of stream, returned by adios_read_open() or
                          //         by adios_advance_step()
    err_step_notready                   = -22,    // stream: tried to advance the step, but no new step has arrived yet
    err_too_many_files                  = -23,   // some staging methods allow for using only a fixed number of different filenames

    err_unspecified                     = -24
};

void adios_error (enum ADIOS_ERRCODES errno, char *fmt, ...);
void adios_error_at_line (enum ADIOS_ERRCODES errno, const char* filename, unsigned int linenum, char *fmt, ...);

const char* adios_get_last_errmsg (void);

#endif