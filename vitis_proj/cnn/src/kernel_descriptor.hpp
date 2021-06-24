/*
 * kernel_descriptor.h
 *
 *  Created on: 14 Jun 2016
 *      Author: muhammed
 */

#ifndef KERNEL_DESCRIPTOR_H_
#define KERNEL_DESCRIPTOR_H_

#include "CNN.h"
#include "aux_functions.hpp"
#include <typeinfo>

enum layer_selector{convolutional_layer, maxpooling_layer, fully_connected_layer_1, fully_connected_layer_2};


template<typename T>
class kernel{
  // basic parameters
  unsigned size0, size1, size2;
  unsigned offset0, offset1, offset2;
  unsigned wg_size0, wg_size1, wg_size2;
  unsigned nParams, nDim;
  //calculated parameters
  unsigned size, n_wg0, n_wg1, n_wg2;
  unsigned wg_size;
  unsigned nWF_WG;
  unsigned start_addr;
  //extra info
  unsigned problemSize, dataSize;
  T *param1, *param2, *param_filter, *target_arm, *param_biases_conv, *param_biases, *param_weights, *param_biases_2, *param_weights_2;

  void compute_descriptor();
  void reLU(int size);
  void softmax(int size);
  void add_biases(int size);
  void flatten(int size);
  bool use_vector_types, use_hard_float;
  int layer_select;
public:
  kernel(unsigned max_size, bool vector_types, bool hard_float, int layer);
  kernel(unsigned max_size, bool vector_types, bool hard_float, int layer, float *input);
  ~kernel();
  void download_code();
  void download_descriptor();
  void prepare_descriptor(unsigned int Size);
  unsigned get_problemSize();
  unsigned compute_on_ARM(unsigned int n_runs);
  void initialize_memory();
  unsigned compute_on_FGPU(unsigned n_runs, bool check_results);
  void check_FGPU_results();
  void print_name();
  float *getOutput();
};




#endif /* KERNEL_DESCRIPTOR_H_ */
