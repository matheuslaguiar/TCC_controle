# TCC_controle
Este repositório apresenta a parte do código referente à solução para controle do robô. A IDE utilizada é a STM32CubeIDE, versão 1.7.0. O microcontrolador utilizado é o STM32F103C8T6, da ST Microeletronics.

## Clock
A frequência de operação do sistema é de 72 MHz, sendo composta por dois cristais, um de alta velocidade e outro de baixa velocidade. O Clock de alta velocidade (HSE) é composto por um cristal de 8 MHz, enquanto o Clock de baixa velocidade (LSE) é composto por um cristal de 32,768 KHz.

## Pinagem
A tabela a seguir indica os pinos utilizados do microcontrolador, seguido por sua função e nome (se aplicável).
| Pino | Função                                      | Nome do pino |
|------|---------------------------------------------|--------------|
| PA1  | Entrada digital                             | microSt      |
| PA2  | Interrupção Externa com transição de subida | ENC_ESQ      |
| PA6  | Interrupção Externa com transição de subida | ENC_DIR      |
| PB4  | Entrada digital                             | Radio3       |
| PB15 | Entrada digital                             | Radio4       |
| PB0  | Entrada analógica                           | -            |
| PB1  | Entrada analógica                           | -            |
| PC14 | Entrada oscilador                           | -            |
| PC15 | Saída oscilador                             | -            |
| PD0  | Entrada oscilador                           | -            |
| PD1  | Saída oscilador                             | -            |
| PA13 | Software Debug IO                           | SWDIO        |
| PA14 | Software Debug Clock                        | SWCLK        |
| PA9  | USART1 TX                                   | -            |
| PA10 | USART1 RX                                   | -            |
| PB10 | USART3 TX                                   | -            |
| PB11 | USART3 RX                                   | -            |
| PB3  | SPI Clock                                   | SPI1_SCK     |
| PB5  | SPI Master Output Slave Input               | SPI1_MOSI    |
| PB6  | SPI Chip Select                             | SPI1_CS      |

## Componentes
### Bluetooth
Essa componente faz o intermédio da comunicação UART do microcontrolador com o módulo Bluetooth HC05. A conectividade ocorre por meio da USART3, configurada de modo assíncrono, com baud rate de 115200 Bits/s, tamanho da palavra de 8 bits, sem bit de paridade e 1 bit de parada.

### Control
Essa componente é a responsável por fazer o controle do robô. Ela contém a chamada da interrupção do pino do encoder e demais funções necessárias para fazer o controle do robô;

### Motor
Essa componente utiliza a componente Packet Serial para controlar a potência aplicada nos motores. Além disso, ela possibilita a solicitação de leituras de corrente dos motores e tensão da bateria.

### Packet Serial
Essa componente é quem efetivamente comunica com a placa Roboclaw Motor Controller, que é o driver dos motores, utilizando comunicação UART. A conectividade ocorre por meio da USART1, configurada de modo assíncrono, com baud rate de 115200 Bits/s, tamanho da palavra de 8 bits, sem bit de paridade e 1 bit de parada.

### PCS
Essa componente adquire os valores de leitura do rádio controle ou sistema de controle proporcional (do inglês "Proportional Control System", PCS). Ela traduz o sinal PWM das entradas do rádio para valores inteiros proporcionais à posição dos joysticks.

### Periph SPI
Essa componente é responsável pela comunicação do microcontrolador STM32 com o periférico SPI, que é o microcontrolador ESP32. Por meio dela, é possível enviar dados do mestre (STM32) para o escravo (ESP32), utilizando o protocolo de comunicação SPI.

### User ADC
Essa componente faz a leitura dos canais analógicos do microcontrolador. O microcontrolador utiliza DMA (acesso direto a memória) para salvar os valores da leitura analógica em um vetor, e esta componente acessa esse vetor e retorna a média das leituras de um canal especificado, além de fazer a inicialização da captura.