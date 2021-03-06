#include <stdint.h>

const int8_t arr_input[100] = {
    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28};
const int32_t in_zp = -5;
const float in_scale = 0.03019111417233944;

const int8_t arr_filter[90] = {
    -4,  68,   -86,  -36,  34,   -50,  116,  -55,  -102, -23,  -127, 62,   116,
    -58, -122, 56,   51,   -96,  -97,  3,    -111, -127, 115,  117,  5,    119,
    -56, -5,   67,   -115, 79,   -111, -115, -127, 55,   -94,  37,   -74,  10,
    -23, -10,  112,  -68,  -118, 88,   127,  66,   59,   127,  127,  -126, -5,
    -24, -18,  -106, 101,  25,   -39,  90,   -52,  126,  -93,  -77,  10,   106,
    21,  -31,  6,    -120, -24,  81,   -86,  127,  58,   -114, -105, -127, -127,
    -98, 43,   37,   5,    -14,  -50,  -127, -2,   66,   53,   0,    38};
const int32_t filter_zps[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float filter_scales[10] = {0.00183519, 0.00169519, 0.00124887, 0.00184465,
                                 0.00170925, 0.0019304,  0.00190223, 0.00194009,
                                 0.00153537, 0.00192699};

const int32_t arr_bias[10] = {90, 98, 131, 89, 96, 86, 86, 85, 106, 85};
const int32_t bias_zps[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float bias_scales[10] = {
    5.5406552e-05, 5.1179526e-05, 3.7704816e-05, 5.5691969e-05, 5.1604115e-05,
    5.8280824e-05, 5.7430512e-05, 5.8573467e-05, 4.6354406e-05, 5.8178121e-05};

const int8_t ref_output[250] = {
    -128, -128, -128, -128, -128, -78,  -86,  -128, -128, -128, -128, -128,
    -128, -128, -128, -78,  -86,  -128, -128, -128, -128, -128, -128, -128,
    -128, -78,  -86,  -128, -128, -128, -128, -128, -128, -128, -128, -78,
    -86,  -128, -128, -128, -88,  -128, -128, -128, -115, -128, -96,  -128,
    -128, -98,  -128, -128, -128, -128, -128, -78,  -86,  -128, -128, -128,
    -128, -128, -128, -128, -128, -78,  -86,  -128, -128, -128, -128, -128,
    -128, -128, -128, -78,  -86,  -128, -128, -128, -128, -128, -128, -128,
    -128, -78,  -86,  -128, -128, -128, -88,  -128, -128, -128, -115, -128,
    -96,  -128, -128, -98,  -128, -128, -128, -128, -128, -78,  -86,  -128,
    -128, -128, -128, -128, -128, -128, -128, -78,  -86,  -128, -128, -128,
    -128, -128, -128, -128, -128, -78,  -86,  -128, -128, -128, -128, -128,
    -128, -128, -128, -78,  -86,  -128, -128, -128, -88,  -128, -128, -128,
    -115, -128, -96,  -128, -128, -98,  -128, -128, -128, -128, -128, -78,
    -86,  -128, -128, -128, -128, -128, -128, -128, -128, -78,  -86,  -128,
    -128, -128, -128, -128, -128, -128, -128, -78,  -86,  -128, -128, -128,
    -128, -128, -128, -128, -128, -78,  -86,  -128, -128, -128, -88,  -128,
    -128, -128, -115, -128, -96,  -128, -128, -98,  -128, -128, -128, -128,
    -128, -54,  -60,  -128, -106, -128, -128, -128, -128, -128, -128, -54,
    -60,  -128, -106, -128, -128, -128, -128, -128, -128, -54,  -60,  -128,
    -106, -128, -128, -128, -128, -128, -128, -54,  -60,  -128, -106, -128,
    -128, -95,  -128, -128, -113, -116, -52,  -128, -128, -103};
const int32_t out_zp = -128;
const float out_scale = 0.006759230513125658;
