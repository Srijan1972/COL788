#ifdef __cplusplus
extern "C"
{
#endif

int test_input();
void process_and_fill_input(float* features);
int LoadQuantModel();

#ifdef __cplusplus
}
#endif

extern const unsigned char quantized_model_tflite[];
