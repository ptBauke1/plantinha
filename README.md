# Monitor e Irrigador Automático de Vaso de Planta 🌱💧

## Descrição

Este projeto utiliza um sensor de umidade do solo resistivo para monitorar a umidade em um vaso de planta. Com base nas leituras do sensor, um sistema de controle aciona uma pequena bomba d'água para irrigar a planta automaticamente, garantindo que ela receba a quantidade ideal de água.

---

## Funcionalidades ✨

* **Monitoramento em tempo real:** Acompanha a umidade do solo continuamente.
* **Irrigação automática:** Aciona a bomba d'água quando a umidade cai abaixo de um nível pré-definido.
---

## Hardware Necessário 🔩

* Raspberry Pi Pico
* Sensor de Umidade do Solo Resistivo
* Relé para controle da bomba d'Água
* Bomba d'Água Submersível AC
* Mangueira para a bomba
* Vaso de planta com terra e planta!

---

## Software e Bibliotecas 💻

* O software foi desenvolvido atraves do Visual Studio Code utilizando a SDK disponibilizada pela Raspberry Pi Foundation para o desenvolvimento de seus microcontroladores em C.
---

## Montagem e Conexões 🔌

Descreva aqui as conexões entre os componentes. Por exemplo:

1.  **Sensor de Umidade:**
    * Pino `S` (Sinal) do sensor conectado ao pino analógico `A0` do microcontrolador.
    * Pino `VCC` ou `+` do sensor conectado ao `5V` ou `3.3V` do microcontrolador (verifique a especificação do seu sensor).
    * Pino `GND` ou `-` do sensor conectado ao `GND` do microcontrolador.
2.  **Módulo Relé (ou Transistor) e Bomba:**
    * **Com Módulo Relé:**
        * Pino de Sinal (`IN` ou `S`) do relé conectado a um pino digital do microcontrolador (ex: `D5`).
        * `VCC` do relé ao `5V` do microcontrolador.
        * `GND` do relé ao `GND` do microcontrolador.
        * A bomba d'água é conectada aos terminais `COM` e `NO` (Normalmente Aberto) ou `NC` (Normalmente Fechado) do relé, dependendo da lógica desejada. A alimentação da bomba deve vir de uma fonte externa compatível, com o polo negativo da fonte da bomba conectado também ao `GND` do relé (se exigido pelo módulo).
    * **Com Transistor (ex: TIP120):**
        * Base do transistor conectada a um pino digital do microcontrolador através de um resistor (ex: 1kΩ).
        * Emissor do transistor conectado ao `GND`.
        * Coletor do transistor conectado ao terminal negativo da bomba. O terminal positivo da bomba é conectado à sua fonte de alimentação positiva. O `GND` da fonte da bomba deve ser comum ao `GND` do microcontrolador.
        * Um diodo de proteção (flyback diode) em paralelo com a bomba é recomendado (catodo no positivo da bomba, anodo no negativo).
3.  **Alimentação:**