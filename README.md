# COL788 Course Project

The Project is split into two parts. We worked on two different boards based on the same microprocessor - cortex-m4.

## Part 1: STEVAL-STLKT01V1
The goal for this part was to detect car horns by training a machine learning model, quantizing it and then deploying it on the board. 

1. The code for this part is present in ```./part_1/```
1. The training and quantization was done by taking scripts from [stm32ai-modelzoo](https://github.com/STMicroelectronics/stm32ai-modelzoo/tree/main/audio_event_detection/scripts/training).
2. Our first attempt involved using the [tflite-micro](https://github.com/tensorflow/tflite-micro/tree/main) library to deploy the quantized model on our micro controller.
    1. The folder ```./part_1/first_attempt/``` contains the relevant files that converts  ```.tflite``` model to a c-model and provides functions to load and run inference on the model.
    2. The code is then packaged into a static library and included in the audioloop code provided earlier.
3. We faced several errors with our first attempt:
    1. Even after quantization, the model was not able to fit on the ram. We had to train a smaller model to fix this.
    2. Various other bugs due to manual compilation of the tflite-micro library for the cortex-m4 chip.
4. We then used STM32CUBEAI to generate the c-model from the ```.tflite``` directly. CUBEAI was found to be much more optimized for the board and the generated binary ran without errors on the board. We were able to run inference on the original model for a dummy input and blink an LED based on the output.

## Part 2: STEVAL-MKSBOX1V1
This board also has a Machine Learning Core which can be used to run a decision tree without using the MCU. The Goal of this part was to Use this feature and run a decision tree to detect yoga poses.
1.  The code for this part is present in ```part_2```.
2.  We first used the datalog given in this [repository](https://github.com/STMicroelectronics/STMems_Machine_Learning_Core/tree/master/configuration_examples/example_1_sensortilebox_stble_unico) to train a decision tree using UNICO software to generate the unico configuration file.
3.  Then we deployed the ```.ucf``` file using the STBLESenson app. 
