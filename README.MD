# Biblioteca de comunicação LORA_TTGO_V2
Essa biblioteca implementa um tipo de comunicação em LORA, com a placa de prototipação TTGO_V2.

## Requisitos
- Adafruit_GFX
- [LORA](https://github.com/sandeepmistry/arduino-LoRa)

## Conexão
Esta biblioteca utiliza o LORA como comunicação, os pinos, banda de operação
e demais detalhes estão no arquivo LORA_TTGO_V2.h.

## Dúvidas

### Essa biblioteca ordena os pacotes?
Inicilamente não, mas em testes percebemos que a biblioteca LORA, se comporta de uma forma aceitável, enviando e recebendo os pacotes de forma ordenada, mas estamos abertos a discussão sobre como ordená-los em coma de uma rede com uma banda tão baixa.

### Essa biblioteca tem checksum?
Verificamos que o CRC do próprio protocolo, as vezes falha e não conseguimos obter os dados realmente transmitidos, mas fazemos um checksum MD5 em cima da nossa estrutura COM, caso o da origem seja diferente do destino, descartamos o pacote.

### Em caso de falhas, é solicitado o reenvio de pacotes?
Ainda não mas, estamos desenvolvendo soluções duplex.

## Desenvolvedor
Kleyton Leite.