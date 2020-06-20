# ILI9341_DMA_library
Library for ILI9341 SPI display for STM32F411 utilizing DMA for SPI data transfers and adding compatibility with AdafruitGFX fonts.

Based on https://github.com/martnak/STM32-ILI9341. 
 
Library is at early stage of development, so you may encounter many bugs and strange bahaviours. Project was created in CubeIDE for Nucleo F411RE. I have no idea if it will work for other MCUs. DMA allows to raise SPI utilization to 93.57% from about 60% without DMA. However, im not sure if original benchmark gives me reliable results. 

Worth to know:
* AdafruitGFX fonts are 2x slower than original font
* Bitmaps are sent to DMA with high byte first, but display expects low byte first. Sample bitmap "palette" has bytes reversed.
* Background color is ignored when using AdafruitGFX fonts
* DMA is used, but you have to wait for the end of transmission to leave local function

