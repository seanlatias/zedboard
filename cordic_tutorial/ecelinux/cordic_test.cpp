//=========================================================================
// cordic_test.cpp
//=========================================================================
// @brief: A C++ test bench (TB) for validating the sine & cosine functions
//         implemented using CORDIC.
/* @desc: 
   1. This TB generates angles between [1, NUM_DEGREE).
   2. It calculates the difference (or errors) between the results from
      CORDIC-based sin/cos with those from standard math library in <math.h>.
   3. All results are logged in the file out.dat for debugging purposes.
   4. The final cumulative errors are printed.
*/

#include <math.h>
#include <hls_stream.h>
#include <iostream>
#include <fstream>

#include "cordic.h"

//--------------------------------------
// Compute absolute value of a double
//--------------------------------------
double abs_double(double var)
{
    return ( var < 0 ) ? -var : var;
}

//--------------------------------------
// Compute absolute value of a double
//--------------------------------------
double RMSE(double total_error)
{
    return sqrt( total_error / (NUM_DEGREE - 1) );
}

//--------------------------------------
// main function of TB
//--------------------------------------
int main(int argc, char** argv)
{
  // sin output
  cos_sin_type s = 0;
  // cos output
  cos_sin_type c = 0;
  // radian input
  double radian; 
  // sin & cos calculated by math.h
  double m_s = 0.0, m_c = 0.0;

  // Error terms
  double err_ratio_sin = 0.0;
  double err_ratio_cos = 0.0;
  double accum_err_sin = 0.0;
  double accum_err_cos = 0.0;

  // arrays to store the output
  double c_array[NUM_DEGREE];
  double s_array[NUM_DEGREE];

  // HLS streams for communicating with the cordic block
  hls::stream<bit32_t> cordic_in;
  hls::stream<bit32_t> cordic_out;

  //------------------------------------------------------------ 
  // Send data to CORDIC sim
  //------------------------------------------------------------ 
  for (int i = 1; i < NUM_DEGREE; i++) {

    radian = i * M_PI / 180;

    // Convert double value to fixed-point representation
    theta_type theta(radian);

    // Split the theta value into lo and hi 32-bit words
    bit32_t input_lo = theta(31, 0);
    bit32_t input_hi = theta(theta.length()-1, 32);

    // Send both words to the HLS module
    cordic_in.write( input_lo );
    cordic_in.write( input_hi );
  }

  //------------------------------------------------------------ 
  // Execute CORDIC sim and store results
  //------------------------------------------------------------ 
  for (int i = 1; i < NUM_DEGREE; i++) {
    // Run the HLS function 
    dut( cordic_in, cordic_out );

    // Read the two 32-bit cosine output words, low word first
    c(31, 0)            = cordic_out.read();
    c(c.length()-1, 32) = cordic_out.read();
    
    // Read the two 32-bit cosine output words
    s(31, 0)            = cordic_out.read();
    s(s.length()-1, 32) = cordic_out.read();
    
    // Store to array
    c_array[i] = c;
    s_array[i] = s;
  }

  //------------------------------------------------------------ 
  // Check results
  //------------------------------------------------------------ 
  for (int i = 1; i < NUM_DEGREE; i++) {
    // Load the stored result
    c = c_array[i];
    s = s_array[i];

    // Call math lib
    radian = i * M_PI / 180;
    m_s = sin( radian );
    m_c = cos( radian );
    
    // Calculate normalized error
    err_ratio_sin = ( abs_double( (double)s - m_s) / (m_s) ) * 100.0;
    err_ratio_cos = ( abs_double( (double)c - m_c) / (m_c) ) * 100.0;
    
    // Accumulate error ratios
    accum_err_sin += err_ratio_sin * err_ratio_sin;
    accum_err_cos += err_ratio_cos * err_ratio_cos;
  }

  //------------------------------------------------------------ 
  // Write out root mean squared error (RMSE) of error ratios
  //------------------------------------------------------------ 
  // Print to screen
  std::cout << "#------------------------------------------------\n"
            << "Overall_Error_Sin = " << RMSE(accum_err_sin) << "\n"
            << "Overall_Error_Cos = " << RMSE(accum_err_cos) << "\n"
            << "#------------------------------------------------\n";

  return 0;
}
