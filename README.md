# Controle de Cadeira Motorizada com Arduino

Projeto de controle para uma cadeira motorizada utilizando dois motores DC de patinete, controlados por duas pontes H IBT-2 (BTS7960) e um Arduino Uno.

## Componentes utilizados

- Arduino Uno R3
- 2x Driver IBT-2 (BTS7960 43A)
- 2x Motores DC de patinete elétrico
- Joystick analógico (XY + botão)
- Fonte/Bateria compatível com os motores
- Cabos de conexão

## Funcionalidades

- Controle por joystick analógico
- Movimento para frente
- Movimento para trás
- Giro para esquerda
- Giro para direita
- Freio através do botão do joystick
- Monitor Serial para depuração
- Controle independente dos dois motores

## Hardware

### Microcontrolador
- Arduino Uno

### Drivers
- 2x IBT-2 (BTS7960)

### Motores
- 2x Motores DC de patinete elétrico

## Ligações

### Joystick

| Joystick | Arduino |
|----------|---------|
| VRx | A0 |
| VRy | A1 |
| SW | D2 |
| VCC | 5V |
| GND | GND |

### IBT-2 Esquerda

| IBT-2 | Arduino |
|--------|----------|
| RPWM | D4 |
| LPWM | D6 |
| R_EN | D5 |
| L_EN | D7 |

### IBT-2 Direita

| IBT-2 | Arduino |
|--------|----------|
| RPWM | D8 |
| LPWM | D10 |
| R_EN | D9 |
| L_EN | D11 |

## Funcionamento

O joystick é utilizado para controlar os dois motores.

- Frente → ambos os motores giram para frente.
- Trás → ambos os motores giram para trás.
- Direita → motores giram em sentidos opostos.
- Esquerda → motores giram em sentidos opostos.
- Botão do joystick → aplica freio eletrônico.

Durante os testes, todas as leituras do joystick e comandos enviados aos motores são exibidos pelo Monitor Serial.

## Estrutura do projeto

- Leitura do joystick
- Interpretação da direção
- Controle dos dois drivers IBT-2
- Controle dos motores
- Rotinas de freio e parada

## Objetivo

Este projeto faz parte do desenvolvimento de um sistema de motorização para uma cadeira elétrica, servindo como base para futuras melhorias, como:

- Controle proporcional de velocidade
- Rampa de aceleração/desaceleração
- Controle por ESP32
- Controle via Bluetooth
- Controle via Wi-Fi
- Integração com aplicativos móveis

## WebSite
https://autoroda.com.br/

## Autores

Antonny Enzo Quiroz, Gabriel Nunes, Gabriel Soares, Gustavo Silva, José Augusto Alves, Maria Clara Assunção, Roger Alves.

