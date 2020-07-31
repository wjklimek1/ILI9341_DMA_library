# ILI9341_DMA_library
Library for ILI9341 SPI display for STM32F411 utilizing DMA for SPI data transfers and adding compatibility with AdafruitGFX fonts.

Based on https://github.com/martnak/STM32-ILI9341. 
 
Library is at a stage of development, so you may encounter bugs and unexpected bahaviours.  Project was created in CubeIDE for Nucleo F411RE, but it should be easy to port it for other STM32 microcontrollers. DMA allows to raise SPI utilization to 93.57% from about 60% without DMA. Library is compatible with AdafruitGFX fonts.

Worth to know:
* AdafruitGFX fonts were optimized, but they are still slightly slower than original font. 
* Bitmaps are sent to DMA with high byte first, but display expects low byte first. Sample bitmap "palette" has bytes reversed.
* AdafruitGFX fonts now accept background color, but because of different width of characters not everything may be overwritten. Simplest workaround is to place blank characer like space at the end of each string.
* DMA is used, but you have to wait for the end of transmission to leave library function. Whole operation is still much faster than without DMA.

Changes since last version:
* Optimized AdafruitGFX fonts - drawing time of sample string dropped from 32ms to just 1.8ms
* AdafruitGFX fonts now accept background color 
* AdafruitGFX fonts ignore size parameter. (It was broken anyway, i may add this feture in the future)

Any suggestions and bug reports are greatly aprreciated.