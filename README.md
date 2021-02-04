This is a more low level/complete version of the STM32DUINO adc management.
It enables :
* Single DMA capture
* Dual DMA capture
* Single DMA/timer capture
* Dual DMA/timer capture

It can be used either with STM2F1x or GD32Fyy chips depending if GD32_ADC is set or not.
/!\ It is meant to be used with my other repos
  FreeRTOS as example/FreeRTOS 
  arduino_cmake_stm32 as example/cmake
  stm32duino (roger clark) as example/Arduino_STM32
+ edit platformConfig.cmake to point to your toolchain
