//==========================================================================
//digitrec.cpp
//==========================================================================
// @brief: A k-nearest-neighbor implementation for digit recognition (k=1)

#include "digitrec.h"

//----------------------------------------------------------
// Top function
//----------------------------------------------------------

void dut(
    hls::stream<bit32_t> &strm_in,
    hls::stream<bit32_t> &strm_out
)
{
  digit input;

  bit32_t input_lo = strm_in.read();
  bit32_t input_hi = strm_in.read();

  input(31, 0) = input_lo;
  input(input.length()-1, 32) = input_hi;

  bit4_t result = digitrec(input);
  
  strm_out.write( result );

}

//----------------------------------------------------------
// Digitrec
//----------------------------------------------------------
// @param[in] : input - the testing instance
// @return : the recognized digit (0~9)

bit4_t digitrec( digit input ) 
{
  #include "training_data.h"

  // This array stores K minimum distances per training set
  bit6_t knn_set[10][K_CONST];

  // Initialize the knn set
  for ( int i = 0; i < 10; ++i )
    for ( int k = 0; k < K_CONST; ++k )
      // Note that the max distance is 49
      knn_set[i][k] = 50; 

  L1800: for ( int i = 0; i < TRAINING_SIZE; ++i ) {
    L10: for ( int j = 0; j < 10; j++ ) {
      // Read a new instance from the training set
      digit training_instance = training_data[j][i];
      // Update the KNN set
      update_knn( input, training_instance, knn_set[j] );
    }
  } 

  // Compute the final output
  return knn_vote( knn_set ); 
}


//-----------------------------------------------------------------------
// update_knn function
//-----------------------------------------------------------------------
// Given the test instance and a (new) training instance, this
// function maintains/updates an array of K minimum
// distances per training set.

// @param[in] : test_inst - the testing instance
// @param[in] : train_inst - the training instance
// @param[in/out] : min_distances[K_CONST] - the array that stores the current
//                  K_CONST minimum distance values per training set

void update_knn( digit test_inst, digit train_inst, bit6_t min_distances[K_CONST] )
{
  // Compute the difference using XOR
  digit diff = test_inst ^ train_inst;

  bit6_t dist = 0;
  for ( int i = 0; i < 49; ++i ) { 
    dist += diff[i];
  }

  if (dist < min_distances[K_CONST-1])
    min_distances[K_CONST-1] = dist;

  bit6_t swap;
  for (int i = K_CONST-1; i > 0; i--) {
    if ( min_distances[i] < min_distances[i-1] ) {
      swap = min_distances[i];
      min_distances[i] = min_distances[i-1];
      min_distances[i-1] = swap;
    }
  }

}


//-----------------------------------------------------------------------
// knn_vote function
//-----------------------------------------------------------------------
// Given 10xK minimum distance values, this function 
// finds the actual K nearest neighbors and determines the
// final output based on the most common digit represented by 
// these nearest neighbors (i.e., a vote among KNNs). 
//
// @param[in] : knn_set - 10xK_CONST min distance values
// @return : the recognized digit
// 

bit4_t knn_vote( bit6_t knn_set[10][K_CONST] )
{
  bit4_t k_smallest_index[10];
  bit4_t k_smallest_vote[10];

  bit6_t smallest;
  bit4_t index;
  bit4_t digit;


  for (int i = 0; i < 10; i++) {
    k_smallest_index[i] = 0;
    k_smallest_vote[i] = 0;
  }

  for (int i = 0; i < K_CONST; i++) {
    smallest = knn_set[0][k_smallest_index[0]];
    digit = 0;
    for (int j = 1; j < 10; j++) {
      index = k_smallest_index[j];
      if (knn_set[j][index] < smallest) {
        smallest = knn_set[j][index];
        digit = j;
      }
    }
    k_smallest_index[digit] += 1;
    k_smallest_vote[digit] += K_CONST-i;
  }

  bit4_t vote_num = k_smallest_vote[0];
  bit4_t vote_idx = k_smallest_index[0];
  bit4_t vote_id = 0;
  for (int i = 1; i < 10; i++) {
    if (k_smallest_index[i] > vote_idx) {
      vote_idx = k_smallest_index[i];
      vote_num = k_smallest_vote[i];
      vote_id = i;
    }
    else if (k_smallest_index[i] == vote_idx) {
      if (k_smallest_vote[i] > vote_idx) {
        vote_idx = k_smallest_index[i];
        vote_num = k_smallest_vote[i];
        vote_id = i;
      }
    }
  }

  return vote_id;
}

