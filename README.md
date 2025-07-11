# Monitor e Irrigador Automático de Vaso de Planta 🌱💧

## Descrição

Este projeto utiliza um sensor de umidade do solo resistivo para monitorar a umidade em um vaso de planta. Com base nas leituras do sensor, um sistema de controle aciona uma pequena bomba d'água para irrigar a planta automaticamente, garantindo que ela receba a quantidade ideal de água.

---

![Texto Alternativo](/arquivos_auxiliares/montagem.png "Montagem do projeto")

## Estrutura do repositorio 📂
```
├─── main.cpp
├─── CmakeLists.txt
├─── pico_sdk_import.cmkae
├─── README.md
├─── Relatorio.pdf
├─── Apresentacao.pdf
├─── calibracao_sensor.xlsx
├───arquivos_auxiliares
├───include
│   └───ssd1306
├───projeto mecanico
│   ├───caixa
│   └───suporte
└───src
    └───ssd1306
```

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
* Para o controle do display OLED, utilizou-se a biblioteca SSD1306_OLED, que pode ser conferida no link: https://github.com/gavinlyonsrepo/SSD1306_OLED_PICO
---

## Montagem e Conexões 🔌

Descreva aqui as conexões entre os componentes. Por exemplo:

![alt text](/arquivos_auxiliares/esquematico.png "Esquema eletrico")
1.  **Sensor de Umidade:**
    * A variação da corrente do sensor é transformada em variação de tensão pelo divisor resistivo exitente entre o R1 e o proprio sensor 
    * A leitura do sensor é realizada pelo pino 31 da Raspberry Pi Pico, pino no qual o ADC0 do microcontrolador está conectado.
2.  **Módulo Relé e Bomba:**
    * **Módulo Relé:**
        * Base do transistor NPN conectado ao pino GPIO2 do microcontrolador
        * Coletor conectado a bobina do relé
        * Emissor conectado ao GND
        * Outra extremidade da bobina do relé conectada ao +5V.
    * **Bomba:**
        * Fase da bomba conectada ao NF do relé.
3.  **Display OLED**
    * Display conectado nos pinos GPIO16 e GPIO17, utilizando a comunição I2C.
4.  **Alimentação:**
    * Alimentação é fornecida por uma fonte de bancada configurada para +5V.
