#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<math.h>
#include<assert.h>

#include<iostream>
#include<fstream>

#include "host.h"
#include "timer.h"

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
// main function
//--------------------------------------
int main(int argc, char** argv)
{
  // Open channels to the FPGA board.
  // These channels appear as files to the Linux OS
  int fdr = open("/dev/xillybus_read_32", O_RDONLY);
  int fdw = open("/dev/xillybus_write_32", O_WRONLY);

  // Check that the channels are correctly opened
  if ((fdr < 0) || (fdw < 0)) {
    fprintf (stderr, "Failed to open Xillybus device channels\n");
    exit(-1);
  }

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

  int nbytes;
  Timer timer("CORDIC fpga (batch)");

  timer.start();

  //----------------------------------------------------------------------
  // Send all values to the module
  //----------------------------------------------------------------------
  for (int i = 1; i < NUM_DEGREE; ++i) {
    radian = i * M_PI / 180;

    // Convert double value to fixed-point representation
    theta_type theta(radian);

    // Convert fixed-point to int64
    bit64_t theta_i;
    theta_i(theta.length()-1,0) = theta(theta.length()-1,0);
    int64_t input = theta_i;

    // Send bytes through the write channel
    // and assert that the right number of bytes were sent
    nbytes = write (fdw, (void*)&input, sizeof(input));
    assert (nbytes == sizeof(input));
  }

  //----------------------------------------------------------------------
  // Read all results
  //----------------------------------------------------------------------
  for (int i = 1; i < NUM_DEGREE; ++i) {
    // Receive bytes through the read channel
    // and assert that the right number of bytes were recieved
    int64_t c_out, s_out;
    nbytes = read (fdr, (void*)&c_out, sizeof(c_out));
    assert (nbytes == sizeof(c_out));
    nbytes = read (fdr, (void*)&s_out, sizeof(s_out));
    assert (nbytes == sizeof(s_out));
    
    // convert int64 to fixed point
    bit64_t c_i = c_out;
    bit64_t s_i = s_out;
    c(c.length()-1,0) = c_i(c.length()-1,0);
    s(s.length()-1,0) = s_i(s.length()-1,0);
    
    // Store to array
    c_array[i] = c;
    s_array[i] = s;
  }

  timer.stop();
  
  //------------------------------------------------------------ 
  // Check results
  //------------------------------------------------------------ 
  for (int i = 1; i < NUM_DEGREE; ++i) {
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

  // Clean up
  close(fdr);
  close(fdw);

  return 0;
}
