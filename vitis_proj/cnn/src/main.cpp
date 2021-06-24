#include "aux_functions.hpp"

using namespace std;

#define TYPE  float
// #define TYPE  int 
// #define TYPE  short
// #define TYPE  char


int main(){
  // The correctness of all results will be checked at the end of each execution round
  const unsigned check_results = 0;
  // The kernel will be executed for problem sizes of 64, 64*2, ... , 64*2^(test_vec_len-1)
  const unsigned test_vec_len = 1;
  // Executions & time measurements will be repeated nruns times 
  const unsigned nruns = 1;
  // use vector types:ushort2 instead of ushort OR uchar4 instead of byte
  const bool use_vector_types = false;
  // use hard floating point units
  const bool use_hard_float = 1;
  
  if(check_results)
    xil_printf("\n\r---Entering main (checking FGPU results is" ANSI_COLOR_GREEN" active" ANSI_COLOR_RESET ") ---\n\r");
  else
    xil_printf("\n\r---Entering main (checking FGPU results is" ANSI_COLOR_RED" inactive" ANSI_COLOR_RESET ") ---\n\r");
  


  unsigned i, size_index;

  unsigned *timer_val_fgpu = new unsigned[test_vec_len]();
  unsigned *timer_val_arm = new unsigned[test_vec_len]();

  // enable ARM caches
  Xil_ICacheEnable();
  Xil_DCacheEnable();

// create kernels

//CONVOLUTION
  	kernel<TYPE> convolutional_kernel(sqrt(12544), use_vector_types, use_hard_float, convolutional_layer);
	convolutional_kernel.download_code();
	convolutional_kernel.prepare_descriptor(8 << 6);
	fflush(stdout);
	timer_val_fgpu[0] = convolutional_kernel.compute_on_FGPU(nruns, check_results);
	xil_printf("Convolutional layer execution time: %d us \n\r", timer_val_fgpu[0]);

	float *output = convolutional_kernel.getOutput();

//MAXPOOLING
	kernel<TYPE> maxpooling_kernel(sqrt(12544), use_vector_types, use_hard_float, maxpooling_layer, output);
	maxpooling_kernel.download_code();
	maxpooling_kernel.prepare_descriptor(8 << 6);
	fflush(stdout);
	timer_val_fgpu[0] = maxpooling_kernel.compute_on_FGPU(nruns, check_results);
	xil_printf("Maxpooling layer execution time: %d us \n\r", timer_val_fgpu[0]);

	output = maxpooling_kernel.getOutput();
//FC1
	kernel<TYPE> fully_connected1_kernel(128, use_vector_types, use_hard_float, fully_connected_layer_1, output);
	fully_connected1_kernel.download_code();
	fully_connected1_kernel.prepare_descriptor(8 << 6);
	fflush(stdout);
	timer_val_fgpu[0] = fully_connected1_kernel.compute_on_FGPU(nruns, check_results);
	xil_printf("FC1 layer execution time: %d us \n\r", timer_val_fgpu[0]);

	output = fully_connected1_kernel.getOutput();
//FC2
	kernel<TYPE> fully_connected2_kernel(10, use_vector_types, use_hard_float, fully_connected_layer_2, output);
	fully_connected2_kernel.download_code();
	fully_connected2_kernel.prepare_descriptor(8 << 6);
	fflush(stdout);
	timer_val_fgpu[0] = fully_connected2_kernel.compute_on_FGPU(nruns, check_results);
	xil_printf("FC2 layer execution time: %d us \n\r", timer_val_fgpu[0]);

//  // print execution times
//  cout<<endl<<left<<setw(20)<<"Problem Size"<<setw(25)<<"Execution Time (us)"<<setw(25)<<           "Execution Time (us)"<<setw(25)<<"Speedup"<< endl;
//  cout<<setw(32)<<ANSI_COLOR_GREEN  <<                  "FGPU"<<setw(27)<<ANSI_COLOR_RED<<setw(10)<<"ARM"<< ANSI_COLOR_RESET <<endl;
//  for(i = 0; i < size_index; i++)
//    cout<<setw(28) << (8<<i)*(8<<i) <<
//      setw(25) << timer_val_fgpu[i] <<
//      setw(18) << timer_val_arm[i] <<
//      setw(20)<< fixed << setprecision(2) << ((float)timer_val_arm[i]/(float)timer_val_fgpu[i])<<endl;


  xil_printf("---Exiting main---\n\r");
  fflush(stdout);

  delete[] timer_val_fgpu;
  delete[] timer_val_arm;

  return 0;
}
