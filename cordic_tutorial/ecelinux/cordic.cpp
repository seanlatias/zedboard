//=========================================================================
// cordic.cpp
//=========================================================================
// @brief : A CORDIC implementation of sine and cosine functions.

#include <hls_stream.h>
#include <iostream>
#include "cordic.h"

//-----------------------------------
// dut function (top module)
//-----------------------------------
// @param[in]  : strm_in - input stream
// @param[out] : strm_out - output stream 
void dut (
    hls::stream<bit32_t> &strm_in,
    hls::stream<bit32_t> &strm_out
)
{
  theta_type theta;
  cos_sin_type c, s;

  // ------------------------------------------------------
  // Input processing
  // ------------------------------------------------------
  // Read the two input 32-bit words (low word first)
  bit32_t input_lo = strm_in.read();
  bit32_t input_hi = strm_in.read();

  // Convert input raw bits to fixed-point representation via bit slicing
  theta(31, 0) = input_lo;
  theta(theta.length()-1, 32) = input_hi;

  // ------------------------------------------------------
  // Call CORDIC 
  // ------------------------------------------------------
  cordic( theta, s, c );

  // ------------------------------------------------------
  // Output processing
  // ------------------------------------------------------
  // Write out the cos value (low word first)
  strm_out.write( c(31, 0) );
  strm_out.write( c(c.length()-1, 32) );
  
  // Write out the sin value (low word first)
  strm_out.write( s(31, 0) );
  strm_out.write( s(s.length()-1, 32) );
}


//-----------------------------------
// cordic function
//-----------------------------------
// @param[in]  : theta - input angle
// @param[out] : s - sine output
// @param[out] : c - cosine output
void cordic(theta_type theta, cos_sin_type &s, cos_sin_type &c)
{
  // Normalization constant    
  const double K_const = 0.6072529350088812561694;

  c = K_const;
  s = 0;

  // Compute the cos and sin using fixed point 
  for (unsigned i = 0; i < 20; ++i) {
    if (theta > 0) {
      theta -= cordic_ctab[i];
      const cos_sin_type c_old = c;
      c = c - (s >> i);
      s = (c_old >> i) + s;
    }
    else {
      theta += cordic_ctab[i];
      const cos_sin_type c_old = c;
      c = c + (s >> i);
      s = -(c_old >> i) + s;
    }
  }
}
