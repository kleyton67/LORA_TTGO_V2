#include "LORA_TTGO_V2.h"

/*!
    @brief  Constrói a classe Lora_TTGOV2
    @param  myAdd
			Configura endereço do equipamento
    @return None
			Nenhum retorno fornecido
    @note   Construtor da classe com o endereço do 
        equipamento
*/
Lora_TTGOV2::Lora_TTGOV2(uint8_t myAdd){
    this->myAdd = myAdd;
}

/*!
    @brief  Constrói a classe Lora_TTGOV2
    @param  myAdd
			Configura endereço do equipamento
    @param debug
            Habilita o debug no dispositivo
    @return None
			Nenhum retorno fornecido
    @note   Construtor da classe com o endereço do 
        equipamento, debug para testes
*/
Lora_TTGOV2::Lora_TTGOV2(uint8_t myAdd, bool debug){
    this->debug = debug;
    this->myAdd = myAdd;
}

/*!
    @brief  Inicializa os dispositivos
    @param  None
			Nenhum parâmetro de configuração
    @return None
			Nenhum retorno fornecido
    @note   Inicializa os equipamentos para operação
        do LORA
*/
bool Lora_TTGOV2::begin(){
    //SPI LoRa pins
    SPI.begin(SCK, MISO, MOSI, SS);
    //setup LoRa transceiver module
    LoRa.setPins(SS, RST, DIO0);

    LoRa.enableCrc();

    return LoRa.begin(BAND);
}

/*!
    @brief  Envia o pacote para a rede
    @param  package
            Uma estrutura de comunicação para ser enviado
        pelo LORA
    @return bool
			Retorna se teve sucesso no envio do pacote
    @note   Envia o pacote pelo LORA, o pacote é do tipo
        COM, este pacote é usado como tamanho mínimo de 
        comunicação entre os dispositivos.
*/
bool Lora_TTGOV2::sentPacket(COM package){
    uint8_t trySend = 0;
    //this->LoRa_txMode();
    while(!LoRa.beginPacket()){
        Serial.println("Falha ao enviar pacote!!!");
        delay(4000);
        trySend++;
        if(trySend > 10)
            return false;
    }
    const uint8_t * buffer = (unsigned char*) &package;
	LoRa.write(buffer, sizeof(COM));
	LoRa.endPacket();
    return true;

}

/*!
    @brief  Função que recebe os dados enviados para ele
    @param  None
			Nenhum parâmetro
    @return COM
			Retorna a estrutura de comunicação recebida
                pelo canal
    @note   Esta função recebe um pacote por vez do canal,
        ela recebe o paote apenas se o endereço destino
        for o equipamento e se o check de dados foi realizado
        com sucesso.
*/
COM Lora_TTGOV2::receivePacket(){
    COM package;

	String message;

	/**
	 * Dados do pacote
	 **/
    uint8_t * buffer = (unsigned char*) &package;

    memset(buffer,0, sizeof(COM));

    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        if(this->debug){
            Serial.println("Pacote recebido!");
        }
        // read packet
        while (LoRa.available()) {
            LoRa.readBytes(buffer, (size_t) sizeof(COM));
        }

        if(this->debug){
            Serial.print("Tamanho da mensagem: ");
            Serial.println(package.Length);
            Serial.print("Estado da mensagem: ");
            Serial.println(package.Status);
            Serial.print("Checksum MD5: ");
            Serial.println((char*)package.Check_Data);
            Serial.print("RSSI ");
            Serial.println(LoRa.packetRssi());
        }

        if(package.Dest_Add != myAdd){
            memset(package.Data,0 , sizeof(char)*TAM_DATA);
            package.Status = 2;
            package.Length = 0;
        }

        // char * data = package.Data;

        // unsigned char * hash = MD5::make_hash(data);

        // unsigned char * verify = (unsigned char *)package.Check_Data;
            
        // if(strcmp((const char*)hash, (const char*)verify)!=0){
        //     memset(package.Data,0, sizeof(char)*TAM_DATA);

        //     package.Status = 2;
        //     package.Length = 0;
        // }

        // free(hash);

        // free(data);

        // free(verify);

    }
    else{
        package.Status = 3;
    }

    return package;
}

/*!
    @brief  Prepara o módulo para enviar dados
    @param  None
			Nenhum parâmetro
    @return None
			Nenhum retorno fornecido
    @note   Muda o estado do equipamento, permitindo 
        que ele também envie dados
*/
void Lora_TTGOV2::LoRa_txMode(){
    LoRa.idle();                          // set standby mode
    LoRa.enableInvertIQ();                // active invert I and Q signals
}

/*!
    @brief  Prepara o módulo para receber dados
    @param  None
			Nenhum parâmetro
    @return None
			Nenhum retorno fornecido
    @note   Muda o estado do equipamento, permitindo 
        que ele receba dados
*/
void Lora_TTGOV2::LoRa_rxMode(){
    LoRa.disableInvertIQ();               // normal mode
    LoRa.receive();                       // set receive mode
}

/*!
    @brief  Envia os dados para a rede LORA
    @param  byte
			Ponteiro para os dados a serem enviados
    @param length
            Tamanho do pacote a ser transmitido
    @param  dest
            Destino na rede LORA para entrega dos dados
    @return BOOL
			Informa se o envio de toda informação foi 
        realizado
    @note   Esta função gerencia o envio de toda a
        informação fornecida para um destino na rede LORA,
        suporta o envio de uma mensagem maior que o pacote
        suportado pela rede LORA(fazendo envio de vários 
        pacotes).
*/
bool Lora_TTGOV2::sentAll(
    uint8_t *byte, 
    size_t length, 
    uint8_t dest){

        COM package;

        bool sendAll = false;
        
        package.Dest_Add = 000;
        package.Length = 000;
        package.Rem_Add = 000;
        
        package.Dest_Add = dest;
        package.Rem_Add = this->myAdd;

        while(!sendAll){
            memset(package.Data, 0, sizeof(char)*TAM_DATA);
            
            if(length < TAM_DATA){  
                package.Length = length;
                sendAll = true;
                package.Status = 5;
            }
            else{
                package.Length = TAM_DATA;
                length -= TAM_DATA;
                package.Status = 1;
            }
            memcpy(package.Data, byte, 
                    package.Length*sizeof(char));


            /**
             * Aterar função na lib da MD5
             */

            unsigned char * hash = MD5::make_hash(package.Data);
            
            memcpy(package.Check_Data, hash, sizeof(uint16_t));

            free(hash);

            byte += TAM_DATA;

            if(this->debug){
                Serial.print("Bytes restantes ");
                Serial.println(package.Length);
                delay(200);
            }
            
            if (this->sentPacket(package)){
                delay(500);
                continue;
            }
            else
                return false;
        }

        return true;
}

/*!
    @brief  Recebe dados enviados para o dispositivo
    @param  ptr
			Ponteiro para os dados a serem recebidos
    @return int
			Quantidade de bytes transmitidos na mensagem
    @note   Esta função gerencia o reebimento de dados para 
    o endereço do equipamento, aqui serão recebidos todos os
    dados verificado pelo check
*/
int Lora_TTGOV2::receiveData(uint8_t * ptr){
    
    COM package;
    int lengthReceive = 0;

    package = this->receivePacket();

    for(; package.Status!=5 ; ptr+= package.Length){
        package = this->receivePacket();
        if(package.Status != 3 ){
            memcpy(ptr, package.Data, 
                        package.Length*sizeof(char));
            lengthReceive += package.Length;

            if(this->debug){
            Serial.println("Montando pacote");
        }
        }

    }

    return lengthReceive;

}
