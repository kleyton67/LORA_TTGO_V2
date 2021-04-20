#include <Wire.h>
#include <LoRa.h>
#include <MD5.h>

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 915E6

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//Tamanho do campo de dados
#define TAM_DATA 200

#ifndef LORA_TTGOV2_H
#define LORA_TTGOV2_H

    //Estrutura de comunicação entre os equipamentos
    /*!
    @brief Estrutura de comunicação que utiliza 
        o protocolo LORA para comunicação

    @param Rem_Add
        Endereço de remetente do endereço
    @param Data
        Dados a serem transmitidos
    @param Check_Data
        Checagem de integridade dos arquivos
        com MD5
    @param Lenght
        Quantidade de bytes enviados em Data
    @param Dest_Add
        Endereço do destino dos equipamentos
    @param Status
        Estado do envio do dados, 5 para pacote
        enviado com sucesso, 1 para pacote em
        transferência, 2 para pacote indisponível,
        3 para pacote incompleto
    @note Ao enviar dados, fazer a checagem de dados e 
        enviar o pacote, caso seja um pacote maior que 
        TAM_DATA sinalizar em Status que o pacote está
        em envio
*/
struct COM {
    uint8_t Rem_Add;
    char Data[TAM_DATA];
    uint16_t Check_Data[16];
    uint8_t Length;
    uint8_t Dest_Add;
    uint8_t Status;
};

class Lora_TTGOV2{
    private:
        bool debug;
        uint8_t myAdd;
        bool sentPacket(COM package);
        COM receivePacket();
        void LoRa_txMode();
        void LoRa_rxMode();

    public:
        Lora_TTGOV2(uint8_t myAdd);
        Lora_TTGOV2(uint8_t myAdd, bool debug);
        bool begin();
        bool sentAll(
            uint8_t *byte, 
            size_t length, 
            uint8_t dest);
        int receiveData(uint8_t * ptr);   
};

#endif //LORA_TTGOV2_H