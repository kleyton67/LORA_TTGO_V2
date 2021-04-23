# Biblioteca de comunicação LORA_TTGO_V2
Essa biblioteca implementa um tipo de comunicação em LORA, com a placa de prototipação TTGO_V2.

## Requisitos
- Adafruit_GFX
- [LORA](https://github.com/sandeepmistry/arduino-LoRa)

## Dispositivos compatíveis
- TTGO OLED V2;

## Conexão
Esta biblioteca utiliza o LORA como comunicação, os pinos, banda de operação
e demais detalhes estão no arquivo LORA_TTGO_V2.h.

## Dúvidas

### Essa biblioteca ordena os pacotes?
Sim, mas caso haja problemas no CheckSum ou na comunicação o pacote inteiro é descartado.

### Essa biblioteca tem checksum?
Sim usamos um algoritmo de MD5 para verificar os dados enviados pelo protocolo, a biblioteca usada é esta [MD5 Arduino](https://github.com/tzikis/ArduinoMD5)

### Em caso de falhas, é solicitado o reenvio de pacotes?
Ainda não mas, estamos desenvolvendo soluções duplex, como o propósito é trafegar pequenas quantidade de dados, é interessante você transportar objetos em cima, assim você já sabe o tamanho esperado do pacote.

## Problemas
A biblioteca pode deixar o cliente loop, esperando eternamente, para isso é interessante habilitar o Watchdog e caso ele pare no LOOP, o WatchDog reinicie o equipamento.

## Desenvolvedor
[Kleyton Leite](kleyton.sartori.leite@gmail.com), ainda estou desenvolvendo, mas podem entrar em contato para mais detalhes, exemplos de uso serão disponibilizados em breve.