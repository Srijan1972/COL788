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
#include "aed_custom_cmodel.h"

namespace {
  using AudioDetectionOpResolver = tflite::MicroMutableOpResolver<1>;
  TfLiteStatus RegisterOps(AudioDetectionOpResolver& op_resolver) {
    TF_LITE_ENSURE_STATUS(op_resolver.AddFullyConnected());
    return kTfLiteOk;
  }

  /* const tflite::Model* model = nullptr; */
  /* tflite::MicroInterpreter* interpreter = nullptr; */
  /* TfLiteTensor* model_input = nullptr; */

  /* // Arena size just a round number. The exact arena usage can be determined */
  /* // using the RecordingMicroInterpreter. */
  /* int8_t* model_input_buffer = nullptr; */
  /* constexpr int kTensorArenaSize = 40 * 1024, minval = -128, maxval = 127; */
  /* uint8_t tensor_arena[kTensorArenaSize]; */
}  // namespace

/* int LoadQuantModel() { */
/*   // Map the model into a usable data structure. This doesn't involve any */
/*   // copying or parsing, it's a very lightweight operation. */
/*   tflite::InitializeTarget(); */
/*   model = tflite::GetModel(hello_world_int8_tflite); */
/*   TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION); */

/*   AudioDetectionOpResolver op_resolver; */
/*   TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver)); */

/*   static tflite::MicroInterpreter static_interpreter(model, op_resolver, tensor_arena, */
/*       kTensorArenaSize); */
/*   interpreter = &static_interpreter; */

/*   TF_LITE_ENSURE_STATUS(interpreter->AllocateTensors()); */

/*   model_input = interpreter->input(0); */
/*   TFLITE_CHECK_NE(model_input, nullptr); */
/*   // check dimensions */
/*   //MicroPrintf("dims size = %d, %d, %d, %d, %d, %d\n", model_input->dims->size, model_input->dims->data[0], model_input->dims->data[1], model_input->dims->data[2], model_input->dims->data[3], model_input->bytes); */

/*   model_input_buffer = model_input->data.int8; */
/*   input_features = (float*) malloc(sizeof(float) * features_cnt); */

/*   return 0; */
/* } */

int LoadQuantModelAndPerformInference() {
  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  const tflite::Model* model =
      ::tflite::GetModel(hello_world_int8_tflite);
  TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);

  AudioDetectionOpResolver op_resolver;
  TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

  // Arena size just a round number. The exact arena usage can be determined
  // using the RecordingMicroInterpreter.
  constexpr int kTensorArenaSize = 3000;
  uint8_t tensor_arena[kTensorArenaSize];

  tflite::MicroInterpreter interpreter(model, op_resolver, tensor_arena,
                                       kTensorArenaSize);

  TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());

  TfLiteTensor* input = interpreter.input(0);
  TFLITE_CHECK_NE(input, nullptr);

  std::cout<<"Dims Size:"<<input->dims->size<<std::endl;

  TfLiteTensor* output = interpreter.output(0);
  TFLITE_CHECK_NE(output, nullptr);

  float output_scale = output->params.scale;
  int output_zero_point = output->params.zero_point;

  // Check if the predicted output is within a small range of the
  // expected output
  float epsilon = 0.05;

  constexpr int kNumTestValues = 4;
  float golden_inputs_float[kNumTestValues] = {0.77, 1.57, 2.3, 3.14};

  // The int8 values are calculated using the following formula
  // (golden_inputs_float[i] / input->params.scale + input->params.scale)
  int8_t golden_inputs_int8[kNumTestValues] = {-96, -63, -34, 0};

  for (int i = 0; i < kNumTestValues; ++i) {
    input->data.int8[0] = golden_inputs_int8[i];
    TF_LITE_ENSURE_STATUS(interpreter.Invoke());
    float y_pred = (output->data.int8[0] - output_zero_point) * output_scale;
    TFLITE_CHECK_LE(abs(sin(golden_inputs_float[i]) - y_pred), epsilon);
  }

  return 0;
}

/* int test_input(){ */
/*     model_input_buffer[0] = 0; */

/* 	  TF_LITE_ENSURE_STATUS(interpreter->Invoke()); */

/* 	  /1* TfLiteTensor* output = interpreter->output(0); *1/ */
/*     //MicroPrintf("%f ", output->data.int8[0]); */
/* 	  return 0; */
/* } */
