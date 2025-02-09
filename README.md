# COMUNICAÇÃO SERIAL UART E I2C
# Temporizador de Um Disparo (One Shot)

## Descrição
Este projeto utiliza a comunicação serial via **UART USB** para enviar caracteres ao display da placa **BitDogLab** usando protocolo **I2C**. Além disso, a matriz de LED **WS2812** é usada para representar números. 

Também foram implementados os botões **A** e **B**, com um sistema de **debouncing de 200ms** chamados por **interrupção**, para atualizar o estado dos LEDs **verde** e **azul**, respectivamente.

## Funcionalidades
### **Entrada de Caracteres via PC**
- Utilize o **Serial Monitor do VS Code** para digitar caracteres.
- Cada caractere digitado deve ser exibido no **display SSD1306**.
- Se um número entre **0 e 9** for digitado, um **símbolo correspondente** será exibido na matriz **5x5 WS2812**.

### **Interação com o Botão A**
- Pressionar o **Botão A** alterna o estado do **LED RGB Verde** (ligado/desligado).
- A operação é registrada de duas formas:
  - Uma **mensagem informativa** sobre o estado do LED será exibida no **display SSD1306**.
  - Um **texto descritivo** sobre a operação será enviado ao **Serial Monitor**.

### **Interação com o Botão B**
- Pressionar o **Botão B** alterna o estado do **LED RGB Azul** (ligado/desligado).
- A operação é registrada de duas formas:
  - Uma **mensagem informativa** sobre o estado do LED será exibida no **display SSD1306**.
  - Um **texto descritivo** sobre a operação será enviado ao **Serial Monitor**.

## Compilando e Executando
### **Requisitos**
- **SDK do Raspberry Pi Pico** instalado e configurado
- **VS Code** com as extensões:
  - Raspberry Pi Pico Project
  - Wokwi Simulator
- **CMake** configurado

### **Passos**
#### **Simulação no Wokwi (VS Code)**
1. Importe o projeto (pasta) utilizando a extensão **Raspberry Pi Pico Project**.
2. Compile o projeto pela extensão.
3. No **Wokwi**, abra o arquivo `diagram.json` e execute (Play).

#### **Execução na Placa BitDogLab**
1. Compile o projeto para gerar o arquivo `.uf2`.
2. Envie o arquivo `.uf2` para a placa **Raspberry Pi Pico**.

## Dependências
Este projeto requer a biblioteca **pico-sdk**, que deve estar corretamente configurada no ambiente de desenvolvimento.

## Demonstração
Assista ao vídeo demonstrativo da execução na BitDogLab e simulação no Wokwi:
[![Link para o vídeo](https://img.youtube.com/vi/pq3OyeLgHHw/0.jpg)](https://youtu.be/pq3OyeLgHHw)

---
**Autor:** *Leonam S. Rabelo*

