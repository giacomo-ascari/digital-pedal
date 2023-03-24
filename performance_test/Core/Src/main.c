/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define TEST_SETS 10
#define TEST_REPETITIONS 1000000

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint32_t start_time [TEST_SETS];
uint32_t end_time [TEST_SETS];
uint32_t elapsed [TEST_SETS];
volatile float a = 2343.0;
volatile float b = 5.7001;
float c;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//https://www.researchgate.net/publication/3049422_Improving_Accuracy_in_Mitchell%27s_Logarithmic_Multiplication_Using_Operand_Decomposition#pf4

//https://www.programiz.com/python-programming/online-compiler/
/*
import math
lower, upper, samples = 0, 1, 1000
m = "const float log_table_10[" + str(samples) + "] = { "
for i in range(0+1, samples+1):
    x = i / samples * (upper - lower)
    m += str(round(math.log(x, 10), 6)) + ', '
m = m[0:len(m)-2]
m += "};"
print(m)
 */

const float log_table_10[1000] = { -3.0, -2.69897, -2.522879, -2.39794, -2.30103, -2.221849, -2.154902, -2.09691, -2.045757, -2.0, -1.958607, -1.920819, -1.886057, -1.853872, -1.823909, -1.79588, -1.769551, -1.744727, -1.721246, -1.69897, -1.677781, -1.657577, -1.638272, -1.619789, -1.60206, -1.585027, -1.568636, -1.552842, -1.537602, -1.522879, -1.508638, -1.49485, -1.481486, -1.468521, -1.455932, -1.443697, -1.431798, -1.420216, -1.408935, -1.39794, -1.387216, -1.376751, -1.366532, -1.356547, -1.346787, -1.337242, -1.327902, -1.318759, -1.309804, -1.30103, -1.29243, -1.283997, -1.275724, -1.267606, -1.259637, -1.251812, -1.244125, -1.236572, -1.229148, -1.221849, -1.21467, -1.207608, -1.200659, -1.19382, -1.187087, -1.180456, -1.173925, -1.167491, -1.161151, -1.154902, -1.148742, -1.142668, -1.136677, -1.130768, -1.124939, -1.119186, -1.113509, -1.107905, -1.102373, -1.09691, -1.091515, -1.086186, -1.080922, -1.075721, -1.070581, -1.065502, -1.060481, -1.055517, -1.05061, -1.045757, -1.040959, -1.036212, -1.031517, -1.026872, -1.022276, -1.017729, -1.013228, -1.008774, -1.004365, -1.0, -0.995679, -0.9914, -0.987163, -0.982967, -0.978811, -0.974694, -0.970616, -0.966576, -0.962574, -0.958607, -0.954677, -0.950782, -0.946922, -0.943095, -0.939302, -0.935542, -0.931814, -0.928118, -0.924453, -0.920819, -0.917215, -0.91364, -0.910095, -0.906578, -0.90309, -0.899629, -0.896196, -0.89279, -0.88941, -0.886057, -0.882729, -0.879426, -0.876148, -0.872895, -0.869666, -0.866461, -0.863279, -0.860121, -0.856985, -0.853872, -0.850781, -0.847712, -0.844664, -0.841638, -0.838632, -0.835647, -0.832683, -0.829738, -0.826814, -0.823909, -0.821023, -0.818156, -0.815309, -0.812479, -0.809668, -0.806875, -0.8041, -0.801343, -0.798603, -0.79588, -0.793174, -0.790485, -0.787812, -0.785156, -0.782516, -0.779892, -0.777284, -0.774691, -0.772113, -0.769551, -0.767004, -0.764472, -0.761954, -0.759451, -0.756962, -0.754487, -0.752027, -0.74958, -0.747147, -0.744727, -0.742321, -0.739929, -0.737549, -0.735182, -0.732828, -0.730487, -0.728158, -0.725842, -0.723538, -0.721246, -0.718967, -0.716699, -0.714443, -0.712198, -0.709965, -0.707744, -0.705534, -0.703335, -0.701147, -0.69897, -0.696804, -0.694649, -0.692504, -0.69037, -0.688246, -0.686133, -0.68403, -0.681937, -0.679854, -0.677781, -0.675718, -0.673664, -0.67162, -0.669586, -0.667562, -0.665546, -0.66354, -0.661544, -0.659556, -0.657577, -0.655608, -0.653647, -0.651695, -0.649752, -0.647817, -0.645892, -0.643974, -0.642065, -0.640165, -0.638272, -0.636388, -0.634512, -0.632644, -0.630784, -0.628932, -0.627088, -0.625252, -0.623423, -0.621602, -0.619789, -0.617983, -0.616185, -0.614394, -0.61261, -0.610834, -0.609065, -0.607303, -0.605548, -0.603801, -0.60206, -0.600326, -0.598599, -0.596879, -0.595166, -0.59346, -0.59176, -0.590067, -0.58838, -0.5867, -0.585027, -0.583359, -0.581699, -0.580044, -0.578396, -0.576754, -0.575118, -0.573489, -0.571865, -0.570248, -0.568636, -0.567031, -0.565431, -0.563837, -0.562249, -0.560667, -0.559091, -0.55752, -0.555955, -0.554396, -0.552842, -0.551294, -0.549751, -0.548214, -0.546682, -0.545155, -0.543634, -0.542118, -0.540608, -0.539102, -0.537602, -0.536107, -0.534617, -0.533132, -0.531653, -0.530178, -0.528708, -0.527244, -0.525784, -0.524329, -0.522879, -0.521434, -0.519993, -0.518557, -0.517126, -0.5157, -0.514279, -0.512862, -0.511449, -0.510042, -0.508638, -0.50724, -0.505845, -0.504456, -0.50307, -0.501689, -0.500313, -0.498941, -0.497573, -0.496209, -0.49485, -0.493495, -0.492144, -0.490797, -0.489455, -0.488117, -0.486782, -0.485452, -0.484126, -0.482804, -0.481486, -0.480172, -0.478862, -0.477556, -0.476254, -0.474955, -0.473661, -0.47237, -0.471083, -0.4698, -0.468521, -0.467246, -0.465974, -0.464706, -0.463442, -0.462181, -0.460924, -0.459671, -0.458421, -0.457175, -0.455932, -0.454693, -0.453457, -0.452225, -0.450997, -0.449772, -0.44855, -0.447332, -0.446117, -0.444906, -0.443697, -0.442493, -0.441291, -0.440093, -0.438899, -0.437707, -0.436519, -0.435334, -0.434152, -0.432974, -0.431798, -0.430626, -0.429457, -0.428291, -0.427128, -0.425969, -0.424812, -0.423659, -0.422508, -0.421361, -0.420216, -0.419075, -0.417937, -0.416801, -0.415669, -0.414539, -0.413413, -0.412289, -0.411168, -0.41005, -0.408935, -0.407823, -0.406714, -0.405607, -0.404504, -0.403403, -0.402305, -0.401209, -0.400117, -0.399027, -0.39794, -0.396856, -0.395774, -0.394695, -0.393619, -0.392545, -0.391474, -0.390406, -0.38934, -0.388277, -0.387216, -0.386158, -0.385103, -0.38405, -0.383, -0.381952, -0.380907, -0.379864, -0.378824, -0.377786, -0.376751, -0.375718, -0.374688, -0.37366, -0.372634, -0.371611, -0.37059, -0.369572, -0.368556, -0.367543, -0.366532, -0.365523, -0.364516, -0.363512, -0.36251, -0.361511, -0.360514, -0.359519, -0.358526, -0.357535, -0.356547, -0.355561, -0.354578, -0.353596, -0.352617, -0.35164, -0.350665, -0.349692, -0.348722, -0.347754, -0.346787, -0.345823, -0.344862, -0.343902, -0.342944, -0.341989, -0.341035, -0.340084, -0.339135, -0.338187, -0.337242, -0.336299, -0.335358, -0.334419, -0.333482, -0.332547, -0.331614, -0.330683, -0.329754, -0.328827, -0.327902, -0.326979, -0.326058, -0.325139, -0.324222, -0.323306, -0.322393, -0.321482, -0.320572, -0.319664, -0.318759, -0.317855, -0.316953, -0.316053, -0.315155, -0.314258, -0.313364, -0.312471, -0.31158, -0.310691, -0.309804, -0.308919, -0.308035, -0.307153, -0.306273, -0.305395, -0.304518, -0.303644, -0.302771, -0.301899, -0.30103, -0.300162, -0.299296, -0.298432, -0.297569, -0.296709, -0.295849, -0.294992, -0.294136, -0.293282, -0.29243, -0.291579, -0.29073, -0.289883, -0.289037, -0.288193, -0.28735, -0.286509, -0.28567, -0.284833, -0.283997, -0.283162, -0.282329, -0.281498, -0.280669, -0.279841, -0.279014, -0.278189, -0.277366, -0.276544, -0.275724, -0.274905, -0.274088, -0.273273, -0.272459, -0.271646, -0.270835, -0.270026, -0.269218, -0.268411, -0.267606, -0.266803, -0.266001, -0.2652, -0.264401, -0.263603, -0.262807, -0.262013, -0.261219, -0.260428, -0.259637, -0.258848, -0.258061, -0.257275, -0.25649, -0.255707, -0.254925, -0.254145, -0.253366, -0.252588, -0.251812, -0.251037, -0.250264, -0.249492, -0.248721, -0.247952, -0.247184, -0.246417, -0.245652, -0.244888, -0.244125, -0.243364, -0.242604, -0.241845, -0.241088, -0.240332, -0.239578, -0.238824, -0.238072, -0.237321, -0.236572, -0.235824, -0.235077, -0.234331, -0.233587, -0.232844, -0.232102, -0.231362, -0.230623, -0.229885, -0.229148, -0.228413, -0.227678, -0.226945, -0.226214, -0.225483, -0.224754, -0.224026, -0.223299, -0.222573, -0.221849, -0.221126, -0.220404, -0.219683, -0.218963, -0.218245, -0.217527, -0.216811, -0.216096, -0.215383, -0.21467, -0.213959, -0.213249, -0.21254, -0.211832, -0.211125, -0.210419, -0.209715, -0.209012, -0.208309, -0.207608, -0.206908, -0.20621, -0.205512, -0.204815, -0.20412, -0.203426, -0.202732, -0.20204, -0.201349, -0.200659, -0.199971, -0.199283, -0.198596, -0.197911, -0.197226, -0.196543, -0.195861, -0.195179, -0.194499, -0.19382, -0.193142, -0.192465, -0.191789, -0.191114, -0.19044, -0.189767, -0.189096, -0.188425, -0.187755, -0.187087, -0.186419, -0.185752, -0.185087, -0.184422, -0.183759, -0.183096, -0.182435, -0.181774, -0.181115, -0.180456, -0.179799, -0.179142, -0.178486, -0.177832, -0.177178, -0.176526, -0.175874, -0.175224, -0.174574, -0.173925, -0.173277, -0.172631, -0.171985, -0.17134, -0.170696, -0.170053, -0.169411, -0.16877, -0.16813, -0.167491, -0.166853, -0.166216, -0.165579, -0.164944, -0.164309, -0.163676, -0.163043, -0.162412, -0.161781, -0.161151, -0.160522, -0.159894, -0.159267, -0.158641, -0.158015, -0.157391, -0.156767, -0.156145, -0.155523, -0.154902, -0.154282, -0.153663, -0.153045, -0.152427, -0.151811, -0.151195, -0.150581, -0.149967, -0.149354, -0.148742, -0.14813, -0.14752, -0.14691, -0.146302, -0.145694, -0.145087, -0.144481, -0.143876, -0.143271, -0.142668, -0.142065, -0.141463, -0.140862, -0.140261, -0.139662, -0.139063, -0.138466, -0.137869, -0.137272, -0.136677, -0.136083, -0.135489, -0.134896, -0.134304, -0.133713, -0.133122, -0.132533, -0.131944, -0.131356, -0.130768, -0.130182, -0.129596, -0.129011, -0.128427, -0.127844, -0.127261, -0.126679, -0.126098, -0.125518, -0.124939, -0.12436, -0.123782, -0.123205, -0.122629, -0.122053, -0.121478, -0.120904, -0.120331, -0.119758, -0.119186, -0.118615, -0.118045, -0.117475, -0.116907, -0.116339, -0.115771, -0.115205, -0.114639, -0.114074, -0.113509, -0.112946, -0.112383, -0.111821, -0.111259, -0.110698, -0.110138, -0.109579, -0.10902, -0.108463, -0.107905, -0.107349, -0.106793, -0.106238, -0.105684, -0.10513, -0.104577, -0.104025, -0.103474, -0.102923, -0.102373, -0.101824, -0.101275, -0.100727, -0.100179, -0.099633, -0.099087, -0.098542, -0.097997, -0.097453, -0.09691, -0.096367, -0.095826, -0.095284, -0.094744, -0.094204, -0.093665, -0.093126, -0.092589, -0.092051, -0.091515, -0.090979, -0.090444, -0.089909, -0.089376, -0.088842, -0.08831, -0.087778, -0.087247, -0.086716, -0.086186, -0.085657, -0.085128, -0.0846, -0.084073, -0.083546, -0.08302, -0.082494, -0.08197, -0.081445, -0.080922, -0.080399, -0.079877, -0.079355, -0.078834, -0.078314, -0.077794, -0.077275, -0.076756, -0.076238, -0.075721, -0.075204, -0.074688, -0.074172, -0.073658, -0.073143, -0.07263, -0.072117, -0.071604, -0.071092, -0.070581, -0.07007, -0.06956, -0.069051, -0.068542, -0.068034, -0.067526, -0.067019, -0.066513, -0.066007, -0.065502, -0.064997, -0.064493, -0.063989, -0.063486, -0.062984, -0.062482, -0.061981, -0.06148, -0.06098, -0.060481, -0.059982, -0.059484, -0.058986, -0.058489, -0.057992, -0.057496, -0.057, -0.056505, -0.056011, -0.055517, -0.055024, -0.054531, -0.054039, -0.053548, -0.053057, -0.052566, -0.052076, -0.051587, -0.051098, -0.05061, -0.050122, -0.049635, -0.049149, -0.048662, -0.048177, -0.047692, -0.047208, -0.046724, -0.04624, -0.045757, -0.045275, -0.044793, -0.044312, -0.043832, -0.043351, -0.042872, -0.042393, -0.041914, -0.041436, -0.040959, -0.040482, -0.040005, -0.039529, -0.039054, -0.038579, -0.038105, -0.037631, -0.037157, -0.036684, -0.036212, -0.03574, -0.035269, -0.034798, -0.034328, -0.033858, -0.033389, -0.03292, -0.032452, -0.031984, -0.031517, -0.03105, -0.030584, -0.030118, -0.029653, -0.029188, -0.028724, -0.02826, -0.027797, -0.027334, -0.026872, -0.02641, -0.025949, -0.025488, -0.025028, -0.024568, -0.024109, -0.02365, -0.023192, -0.022734, -0.022276, -0.021819, -0.021363, -0.020907, -0.020452, -0.019997, -0.019542, -0.019088, -0.018634, -0.018181, -0.017729, -0.017277, -0.016825, -0.016374, -0.015923, -0.015473, -0.015023, -0.014574, -0.014125, -0.013676, -0.013228, -0.012781, -0.012334, -0.011887, -0.011441, -0.010995, -0.01055, -0.010105, -0.009661, -0.009217, -0.008774, -0.008331, -0.007889, -0.007446, -0.007005, -0.006564, -0.006123, -0.005683, -0.005243, -0.004804, -0.004365, -0.003926, -0.003488, -0.003051, -0.002614, -0.002177, -0.001741, -0.001305, -0.000869, -0.000435, 0.0};

/*
 * Hello there.
 * This thing should be similar to Feynman's algorithm.
 * The main difference is that I use base 10 (or 100) to compute the result,
 * as bitwise operations are not possible (nor advantageous) on float numbers.
 *
 * Rewriting such functions is not fun, not even a bit. But I need to get
 * computing times, at the obvious expense of precision and memory.
 *
 * Logarithmic functions cannot be put simply into a table, as the domain
 * isn't constrained.
 *
 * Quick explanation my approach (on base 10):
 *  #1 as we know y = log_10(x) (y as the returned value)
 *  #2 y = log_10(x/divider * divider)
 *  #3 y = log_10(x/divider) + log_10(divider)
 *  #4 if we assume that the divider is greater or equal than x, then
 *     the possible arguments for 'log_10(x/divider)' are only between 0 and 1;
 *     this allows us to create a proper lookup table
 *  #5 if we assume that the divider is a power of 10, then we can avoid to
 *     calculate 'log_10(divider)', as it is equal to the exponent
 *  #6 the only problem is finding the divider, and that is achieved
 *     by starting from 1 and multiplying by 10 until it's bigger than x;
 *     note that we are keeping track of the exponent on a second variable
 *  #7 by calculating x / divider * samples_in_the_tables we find the index
 *     of the table's item
 *  #8 now we can just sum the table's item with the exponent and return y
 *
 *  Base 100 is used to reduce the number of iterations (which seem to be
 *  time consuming) at the cost of additional precision.
 */

float logarithm10(float x) {
	float result; // = log10(x)
	uint32_t exponent = 0; // log100(divider)
	float divider = 1.F; // = 100 ^ exponent

	while (x >= divider) {
		divider *= 100.F;
		exponent++;
	}

	// now (x < divider)
	// use look up table
	// log(x) = log(x*divider/divider) = log(x/divider) + log(divider)
	float lookup_f = x * 1000.F / divider;
	uint32_t lookup_i = lookup_f;

	// exponent is log100 of divider and 2 is log10(100)
	// is needed to change the base of the logarithm of the divider
	result = log_table_10[lookup_i] + exponent * 2;
	return result;
}

float logarithmE(float x) {
	return logarithm10(x) * 2.302585093F; // ... * log_e(10)
}

float logarithm2(float x) {
	return logarithm10(x) * 3.321928095F; // ... * log_2(10)
}

float logarithm12(float x) {
	return logarithm10(x) * 0.9226284080F; // ... * log_12(10)
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  while(1) {
		  c = logarithm10(a);
		  b = logarithmE(a);
	  }

	  HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);

	  for(uint32_t i = 0; i < TEST_SETS; i++) {
		  start_time[i] = HAL_GetTick();
		  for(uint32_t j = 0; j < TEST_REPETITIONS; j++) {
			  //c = log10f(a); // sinf, tanf, log10f, sqrtf, powf
			  c = logarithm10(a);
		  }
		  end_time[i] = HAL_GetTick();
		  elapsed[i] = end_time[i] - start_time[i];
	  }

	  HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);

	  HAL_Delay(500);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LED_B_Pin|LED_R_Pin|LED_O_Pin|LED_G_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_B_Pin LED_R_Pin LED_O_Pin LED_G_Pin */
  GPIO_InitStruct.Pin = LED_B_Pin|LED_R_Pin|LED_O_Pin|LED_G_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
