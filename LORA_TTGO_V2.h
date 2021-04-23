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

//Define a banda de sinal do radio

#define BANDSIGNAL 7.8E3

//Define o Preambulo
#define PREAMB 65000

//Define GAIN LNA
#define GAIN 6

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
        Estado do envio do dados:
            - 5 para pacote enviado com sucesso (fim do envio);
            - 1 para pacote em transferência
            - 2 para pacote indisponível,
            - 3 para pacote incompleto
            - 4 para solicitação de permissão de envio
            - 6 para permissão para receber
   @note Ao enviar dados, fazer a checagem de dados e 
        enviar o pacote, caso seja um pacote maior que 
        TAM_DATA sinalizar em Status que o pacote está
        em envio
*/
struct COM {
    uint8_t Rem_Add;
    char Data[TAM_DATA];
    char Check_Data[16];
    uint8_t Length;
    uint8_t Dest_Add;
    uint8_t Status;
    uint8_t ID;
};

/**
 * 
 * @class LORA_TTGOV2
 * 
 * @brief Classe de comunicação para 
 * 
 * */
class Lora_TTGOV2{
    private:
        bool receive;
        bool debug;
        uint8_t myAdd;
        bool sentPacket(COM package);
        COM receivePacket();
        COM getClearPack();
        void LoRa_txMode();
        void LoRa_rxMode();
        bool reqConfirm(uint8_t address);
        COM sentConfirm(uint8_t address);
        void generateCheck(char * data,
                    char* check, size_t lenData,
                    size_t lenChecks);

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