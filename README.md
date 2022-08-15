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

## Protocolo de comunicação
O microcontrolador irá se comunicar com o ESP32 utilizando SPI, e o buffer enviado seguirá um protocolo próprio de comunicação, que é detalhado a seguir. Devido a uma limitação do ESP32, todos os pacotes enviados deverão conter uma quantidade de bytes múltipla de 4, por isso, todos os pacotes que não atenderem a este requisito será completado com bytes nulos.
O cabeçalho é composto por 1 byte, indicando qual a informação a ser enviada. Logo após, haverá a carga útil do pacote, que varia o seu tamanho de acordo com a informação a ser enviada. Por fim, serão enviados 2 bytes de verificação, que utilização o algoritmo CRC-16/XMODEM para cálculo.

| Informação        | Cabeçalho | Carga útil | CRC-16 | Quantidade de bytes 0x00 |
|-------------------|-----------|------------|--------|--------------------------|
| Tensão da bateria | 0x01 | 2 bytes | 2 bytes | 3 |
| Corrente dos motores | 0x02 | 4 bytes | 2 bytes | 1 |
| Potência do motor esquerdo | 0x03 | 1 byte | 2 bytes | 0 |
| Potência do motor direito | 0x04 | 1 byte | 2 bytes | 0 |
| Velocidade dos motores | 0x05 | verificar | 2 bytes | verificar |
| Jogada em execução | 0x06 | 1 byte | 2 bytes | 0 |

### Tensão da bateria
O valor enviado é a tensão em passos de 0,1 [V].

### Corrente dos motores
O valor da corrente é o dado em passos de 10 [mA]. Os 2 primeiros bytes são referentes à corrente do motor esquerdo, e os 2 subsequentes ao motor direito.

### Potência dos motores
O valor dado é um inteiro entre 0 e 127, em que 0 significa totalmente para trás, 64 parado, e 127 totalmente para frente.

### Velocidade dos motores
verificar

### Jogada em execução
O valor indica qual a jogada está sendo executada. O último valor enviado é aquele a ser considerado como jogada em execução.

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