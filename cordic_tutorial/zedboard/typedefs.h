//===========================================================================
// typedefs.h
//===========================================================================
// @brief: This header defines the shorthand of several ap_uint data types.

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include "ap_int.h"

typedef ap_uint<32> bit32_t;
typedef ap_uint<64> bit64_t;

// define the types used for the input angle
// and the output cos and sin results
#define BITWIDTH 40
typedef ap_fixed<BITWIDTH,2> theta_type;
typedef ap_fixed<BITWIDTH,2> cos_sin_type;


#endif
