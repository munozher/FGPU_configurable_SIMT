#include "kernel_descriptor.hpp"

#include "image.h"
#include "filtersOnCode.h"
#include "biases_convolution_OnCode.h"
#include "biasesOnCode.h"
#include "weightsOnCode.h"
#include "biasesOnCode2.h"
#include "weightsOnCode2.h"

extern unsigned *code; // binary storde in code.c as an array
extern unsigned *code_hard_float; // binary storde in code_hard_float.c as an array

extern float img[];
extern float filters[];
extern float biases_conv[];
extern float biases[];
extern float weights[];
extern float biases_2[];
extern float weights_2[];

int width = 28;
int height = 28;

//Convolution Layer 1 parameters
#define FILTER_WIDTH 5
#define NUM_FILTERS 16
#define NUM_IMAGES 1
//Maxpooling layer 1 parameters
#define WINDOW_POOL_DIM 2
#define WINDOW_STRIDE 2
//Fully Connected layer 1 parameters
#define INPUT_NODES_1 3136
#define OUTPUT_NODES_1 128
//Fully Connected layer 2 parameters
#define INPUT_NODES_2 OUTPUT_NODES_1
#define OUTPUT_NODES_2 10

//Layer selector:
//0 Convolutional layer
//1 Maxpooling layer
//2 Fully Connected layer

template<typename T>
kernel<T>::kernel(unsigned maxDim, bool vector_types, bool hard_float, int layer){
  param1 =  new T[maxDim*maxDim];// new T[width*height];
  param2 = new T[maxDim*maxDim];//new T[width*height*NUM_FILTERS];
  param_filter = new T[maxDim*maxDim]; //new T[FILTER_WIDTH*FILTER_WIDTH*NUM_FILTERS];
  target_arm = new T[maxDim*maxDim];
  use_vector_types = vector_types;
  use_hard_float = hard_float;
  layer_select = layer;
}
template<typename T>
kernel<T>::kernel(unsigned maxDim, bool vector_types, bool hard_float, int layer, float *input){
  param1 = new T[maxDim*maxDim];
  param2 = new T[maxDim*maxDim];
  param_filter = new T[maxDim*maxDim];
  target_arm = new T[maxDim*maxDim];
  use_vector_types = vector_types;
  use_hard_float = hard_float;
  layer_select = layer;
  param1=(T*)input;
}
template<typename T>
kernel<T>::~kernel(){
  delete[] param1;
  delete[] param2;
  delete[] param_filter;
  delete[] target_arm;
}
template<typename T>
void kernel<T>::download_code(){
  volatile unsigned *cram_ptr = (unsigned *)(FGPU_BASEADDR+ 0x4000);
  unsigned int size = CNN_LEN;
  unsigned *code_ptr = code;

  switch(layer_select){
  case convolutional_layer:
	  start_addr = CONVOLUTION_POS;
	  break;
  case maxpooling_layer:
	  start_addr = MAXPOOL_POS;
	  break;
  default:
	  start_addr = FULLYCONNECTED_POS;
	  break;
  }


  for(unsigned i = 0; i < size; i++){
    cram_ptr[i] = code_ptr[i];
  }
}
template<typename T>
void kernel<T>::download_descriptor(){
	int i;
	volatile unsigned* lram_ptr = (unsigned*)FGPU_BASEADDR;
	for(i = 0; i < 32; i++){
		lram_ptr[i] = 0;
	}
	lram_ptr[0] = ((nWF_WG-1) << 28) | (0 << 14) | (start_addr);
	lram_ptr[1] = size0;
	lram_ptr[2] = size1;
	lram_ptr[3] = size2;
	lram_ptr[4] = offset0;
	lram_ptr[5] = offset1;
	lram_ptr[6] = offset2;
	lram_ptr[7] = ((nDim-1) << 30) | (wg_size2 << 20) | (wg_size1 << 10) | (wg_size0);
	lram_ptr[8] = n_wg0-1;
	lram_ptr[9] = n_wg1-1;
	lram_ptr[10] = n_wg2-1;
	lram_ptr[11] = (nParams << 28) | wg_size;

	switch(layer_select){
	case convolutional_layer:
		lram_ptr[16] = (unsigned) param1;	//input image
		lram_ptr[17] = (unsigned) param2;	//output image
		lram_ptr[18] = (unsigned) param_filter;	//filter
//		lram_ptr[19] = (unsigned) width;
//		lram_ptr[20] = (unsigned) FILTER_WIDTH;
		break;
	case maxpooling_layer:
		lram_ptr[16] = (unsigned) param1;	//input image
		lram_ptr[17] = (unsigned) param2;	//output image
		lram_ptr[18] = (unsigned) WINDOW_POOL_DIM;
		lram_ptr[19] = (unsigned) WINDOW_STRIDE;
		lram_ptr[20] = (unsigned) width;
		break;
	case fully_connected_layer_1:
		lram_ptr[16] = (unsigned) param1;	//input image
		lram_ptr[17] = (unsigned) param2;	//output image
		lram_ptr[18] = (unsigned) param_weights;
		lram_ptr[19] = (unsigned) param_biases;
		lram_ptr[20] = (unsigned) INPUT_NODES_1;
		break;
	case fully_connected_layer_2:
		lram_ptr[16] = (unsigned) param1;	//input image
		lram_ptr[17] = (unsigned) param2;	//output image
		lram_ptr[18] = (unsigned) param_weights_2;
		lram_ptr[19] = (unsigned) param_biases_2;
		lram_ptr[20] = (unsigned) INPUT_NODES_2;
		break;
	default:
	  break;
  }

}
template<typename T>
void kernel<T>::prepare_descriptor(unsigned int Size){
	switch(layer_select){
	case convolutional_layer:
		wg_size0 = 16; //16
		wg_size1 = 8; //8
		wg_size2 = 1;
		problemSize = width*height*NUM_FILTERS*NUM_IMAGES; //param 2 length
		offset0 = 0;
		offset1 = 0;
		offset2 = 0;
		nDim = 3;
		size0 = width*height;
		size1 = NUM_FILTERS;
		size2 = NUM_IMAGES;
		nParams = 3; // number of parameters
		break;
	case maxpooling_layer:
		wg_size0 = 7;
		wg_size1 = 7;
		wg_size2 = 4;
		problemSize = (int)((width*height*NUM_FILTERS)/4);
		offset0 = 0;
		offset1 = 0;
		offset2 = 0;
		nDim = 3;
		size0 = (int)(width/2);
		size1 = (int)(height/2);
		size2 = NUM_FILTERS;
		nParams = 5; // number of parameters
		break;
	case fully_connected_layer_1:
		wg_size0 = 8;//128
		problemSize = OUTPUT_NODES_1;
		offset0 = 0;
		nDim = 1;
		size0 = OUTPUT_NODES_1;
		nParams = 5; // number of parameters
		break;
	case fully_connected_layer_2:
		wg_size0 = 5;
		problemSize = OUTPUT_NODES_2;
		offset0 = 0;
		nDim = 1;
		size0 = OUTPUT_NODES_2;
		nParams = 5; // number of parameters
		break;
	default:
		wg_size0 = 8;
		wg_size1 = 8;
		problemSize = width*height*NUM_FILTERS;
		offset0 = 0;
		offset1 = 0;
		nDim = 2;
		size0 = width*height;
		size1 = NUM_FILTERS;
		nParams = 2; // number of parameters
		break;
	}


  dataSize = sizeof(T)*problemSize;

  if(size0 < 8)
    wg_size0 = size0;
  if(size1 < 8)
    wg_size1 = size1;
  if(size2 < 8)
      wg_size2 = size2;

  compute_descriptor();

  offset0 = offset1 = offset2 = 0;

}
template<typename T>
void kernel<T>::reLU(int size){
	for(int i=0; i<size;i++){
		if((param2[i]<0)||(param2[i]>20)){
			param2[i]=0;
		}
	}
}
template<typename T>
void kernel<T>::add_biases(int size){
	for(int i=0; i<size;i++){
		param2[i]+=biases_conv[i];
	}
}
template<typename T>
void kernel<T>::flatten(int size){
	float *hOutputFlatten =  (float *)malloc((INPUT_NODES_1* sizeof(float)));

	    for(int i=0; i<size; i++){
	        hOutputFlatten[i]=param2[((i%NUM_FILTERS)*(INPUT_NODES_1/NUM_FILTERS))+(int)(i/NUM_FILTERS)];
	    }
	    for(int i=0; i<size; i++){
	    	param2[i]=hOutputFlatten[i];
		}

}
template<typename T>
void kernel<T>::softmax(int size){
    //getting sum of all elements
    float sum=0.0;
    for(int i=0; i<size; i++){
        sum += expf(param2[i]);
    }

    //the actual softmax function
    printf("\n\rIndex   Value\n\r");
    for(int i=0; i<size; i++){
       param2[i]=expf(param2[i]) / sum;

       //print the values after the softmax
       //printf("%d.......%f\n\r", i, (float)hOutputNodes2[i]);
    }

}
template<typename T>
void kernel<T>::initialize_memory()
{
  unsigned i;
//  T *param1_ptr = (T*) param1;
//  T *param2_ptr = (T*) param2;
//  T *target_ptr = (T*) param_filter;
	switch(layer_select){
	case convolutional_layer:
		param_filter = (T*)filters;
		param_biases_conv = (T*)biases_conv;
		param1 = (T*) img;
		break;
	case maxpooling_layer:
		break;
	case fully_connected_layer_1:
		param_biases = (T*)biases;
		param_weights = (T*)weights;
		break;
	case fully_connected_layer_2:
		param_biases_2 = (T*)biases_2;
		param_weights_2 = (T*)weights_2;
		break;
	default:
		param1 = (T*) img;
	  break;
	}
  for(i = 0; i < problemSize; i++) {
    param2[i] = 0;
  }
  Xil_DCacheFlush(); // flush data to global memory
}

template<typename T>
void kernel<T>::check_FGPU_results()
{
  unsigned i, nErrors = 0;
  
  for (i = 0; i < problemSize; i++)
    if(target_arm[i] != param_filter[i])
    {
      #if PRINT_ERRORS
      if( typeid(T) == typeid(int) ) {
          xil_printf("res[%d]=%6.2f (must be %6.2f)\n\r", i, (float)param_filter[i], (float) target_arm[i]);
      } else {
          xil_printf("res[0x%x]=0x%x (must be 0x%x)\n\r", i, (unsigned)param_filter[i], (unsigned) target_arm[i]);
      }
      #endif
      nErrors++;
    }
  if(nErrors != 0) {
    xil_printf("Memory check failed (nErrors = %d)!\n\r", nErrors);
      if( typeid(T) != typeid(int) )
        xil_printf("WARNING: Overflow may cause some mismatch between ARM and FGPU results\n");
    }
}
template<typename T>
unsigned kernel<T>::compute_on_FGPU(unsigned n_runs, bool check_results){

  unsigned runs = 0;
  XTime tStart, tEnd;
  unsigned exec_time = 0;

	float outputImage[10];
	for(int i=0;i<10;i++){
		outputImage[i]=param2[i];
	}

  while(runs < n_runs){
    initialize_memory();
    download_descriptor();
    REG_WRITE(INITIATE_REG_ADDR, 0xFFFF); // initiate FGPU when execution starts
    REG_WRITE(CLEAN_CACHE_REG_ADDR, 0xFFFF); // clean FGPU cache at end of execution
    
    XTime_GetTime(&tStart);
    REG_WRITE(START_REG_ADDR, 1);
    while(REG_READ(STATUS_REG_ADDR)==0);
    XTime_GetTime(&tEnd);
    exec_time += elapsed_time_us(tStart, tEnd);
    
    if(check_results)
      check_FGPU_results();

    xil_printf(ANSI_COLOR_GREEN "." ANSI_COLOR_RESET);
    fflush(stdout);
    runs++;

    if(exec_time > 1000000*MAX_MES_TIME_S)// do not execute all required runs if it took too long
      break;
  }
	//data post processing
	switch(layer_select){
	case convolutional_layer:
		reLU(width*height*NUM_FILTERS);
		add_biases(NUM_FILTERS);
		break;
	case maxpooling_layer:
		flatten(INPUT_NODES_1);
		break;
	case fully_connected_layer_1:
		reLU(OUTPUT_NODES_1);
		break;
	case fully_connected_layer_2:
	{
		softmax(OUTPUT_NODES_2);
		float temp=param2[0];
		int index=0;
		for(int i=0;i<OUTPUT_NODES_2;i++){
			printf("%d.......%f\n\r", i, (float)param2[i]);
			if(param2[i]>temp){
				temp = param2[i];
				index = i;
			}
		}
		xil_printf("resulted class: %d\n\r", index);

		//float outputImage[25];
		float inputImage[10];
		for(int i=0;i<10;i++){
		  //printf(": %f\n ", resulted_convolution[i]);
		  outputImage[i]=param2[i];
		  inputImage[i]=param1[i];

		}
		break;
	}
	default:
		xil_printf("This case was not defined \n\r");
		break;

	}



  return exec_time/runs;
}
template<typename T>
float * kernel<T>::getOutput(){
	return (float *)param2;
}
template<typename T>
void kernel<T>::print_name()
{
  if( typeid(T) == typeid(int) )
    xil_printf("\n\r" ANSI_COLOR_YELLOW "Kernel is matrix_multiply word\n\r" ANSI_COLOR_RESET);
  else if (typeid(T) == typeid(short))
    xil_printf("\n\r" ANSI_COLOR_YELLOW "Kernel is matrix_multiply half word\n\r" ANSI_COLOR_RESET);
  else if (typeid(T) == typeid(char))
    xil_printf("\n\r" ANSI_COLOR_YELLOW "Kernel is matrix_multiply byte\n\r" ANSI_COLOR_RESET);
  else if (typeid(T) == typeid(float)) {
    xil_printf("\n\r" ANSI_COLOR_YELLOW "Kernel is matrix_multiply float");
    if(use_hard_float)
      xil_printf(" (hard)\n\r" ANSI_COLOR_RESET);
    else
      xil_printf(" (soft)\n\r" ANSI_COLOR_RESET);
  }
}
template<typename T>
unsigned kernel<T>::get_problemSize(){
  return problemSize;
}
template<typename T>
void kernel<T>::compute_descriptor(){
  assert(wg_size0 > 0 && wg_size0 <= 512);
  assert(size0 % wg_size0 == 0);
  size = size0;
  wg_size = wg_size0;
  n_wg0 = size0 / wg_size0;
  if(nDim > 1)
  {
    assert(wg_size1 > 0 && wg_size1 <= 512);
    assert(size1 % wg_size1 == 0);
    size = size0 * size1;
    wg_size = wg_size0 * wg_size1;
    n_wg1 = size1 / wg_size1;
  }
  else
  {
    wg_size1 = n_wg1 = size1 = 0;
  }
  if(nDim > 2)
  {
    assert(wg_size2 > 0 && wg_size2 <= 512);
    assert(size2 % wg_size2 == 0);
    size = size0 * size1 * size2;
    wg_size = wg_size0 * wg_size1 * wg_size2;
    n_wg2 = size2 / wg_size2;
  }
  else
  {
    wg_size2 = n_wg2 = size2 = 0;
  }
  assert(wg_size <= 512);
  nWF_WG = wg_size / 64;
  if(wg_size % 64 != 0)
    nWF_WG++;
}

template class kernel<float>;
template class kernel<int>;
template class kernel<short>;
template class kernel<char>;
