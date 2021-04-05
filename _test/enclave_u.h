#ifndef ENCLAVE_U_H__
#define ENCLAVE_U_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "sgx_edger8r.h" /* for sgx_status_t etc. */


#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PRINT_INT_DEFINED__
#define PRINT_INT_DEFINED__
void SGX_UBRIDGE(SGX_NOCONVENTION, print_int, (int i_));
#endif

sgx_status_t test(sgx_enclave_id_t eid, int* retval, int test_input_);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
