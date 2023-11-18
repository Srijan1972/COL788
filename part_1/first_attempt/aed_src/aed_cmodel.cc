/* Copyright 2023 The TensorFlow Authors. All Rights Reserved.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <math.h>
#include <iostream>

#include "tensorflow/lite/core/c/common.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_profiler.h"
#include "tensorflow/lite/micro/recording_micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "aed_cmodel.h"

namespace {
  using AudioDetectionOpResolver = tflite::MicroMutableOpResolver<7>;
  TfLiteStatus RegisterOps(AudioDetectionOpResolver& op_resolver) {
    TF_LITE_ENSURE_STATUS(op_resolver.AddFullyConnected());
    TF_LITE_ENSURE_STATUS(op_resolver.AddTranspose());
    TF_LITE_ENSURE_STATUS(op_resolver.AddConv2D());
    TF_LITE_ENSURE_STATUS(op_resolver.AddDepthwiseConv2D());
    TF_LITE_ENSURE_STATUS(op_resolver.AddMean());
    TF_LITE_ENSURE_STATUS(op_resolver.AddSoftmax());
    TF_LITE_ENSURE_STATUS(op_resolver.AddDequantize());
    return kTfLiteOk;
  }

  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* model_input = nullptr;

  // Arena size just a round number. The exact arena usage can be determined
  // using the RecordingMicroInterpreter.
  int8_t* model_input_buffer = nullptr;
  float* input_features = nullptr;
  constexpr int kTensorArenaSize = 200 * 1024, minval = -128, maxval = 127, features_cnt = 6114;
  uint8_t tensor_arena[kTensorArenaSize] ;
  float input_scale = 0.05692856386303902, input_translate = 34;
}  // namespace

void process_and_fill_input(float* features){
  int tmp;
  for(int i = 0; i < features_cnt; i++){
    tmp = (int)(features[i] / input_scale + input_translate);
    if(tmp <= minval)
      model_input_buffer[i] = minval;
    else if(tmp >= maxval)
      model_input_buffer[i] = maxval;
    else model_input_buffer[i] = tmp;
    /* MicroPrintf("%d", model_input_buffer[i]); */
  } 
}

int LoadQuantModel() {
  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  tflite::InitializeTarget();
  model = tflite::GetModel(quantized_model_tflite);
  TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);

  AudioDetectionOpResolver op_resolver;
  TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

  static tflite::MicroInterpreter static_interpreter(model, op_resolver, tensor_arena,
      kTensorArenaSize);
  interpreter = &static_interpreter;

  TF_LITE_ENSURE_STATUS(interpreter->AllocateTensors());

  model_input = interpreter->input(0);
  TFLITE_CHECK_NE(model_input, nullptr);
  // check dimensions
  MicroPrintf("dims size = %d, %d, %d, %d, %d, %d\n", model_input->dims->size, model_input->dims->data[0], model_input->dims->data[1], model_input->dims->data[2], model_input->dims->data[3], model_input->bytes);

  model_input_buffer = model_input->data.int8;
  input_features = (float*) malloc(sizeof(float) * features_cnt);

  return 0;
}

int test_input(){
	for(int i = 0; i < 6144; i++){
		input_features[i] = 0;
	  }

	  process_and_fill_input(input_features);

	  interpreter->Invoke();

	  TfLiteTensor* output = interpreter->output(0);
	  for(int i = 0; i < 10; i++){
	    MicroPrintf("%f ", (double)output->data.f[i]);
	  }
	  return 0;
}
