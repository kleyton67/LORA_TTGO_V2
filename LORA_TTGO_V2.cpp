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

    SPI.begin(SCK, MISO, MOSI, SS);

    LoRa.setPins(SS, RST, DIO0);

    LoRa.enableCrc();

    LoRa.setGain(GAIN);

    LoRa.setPreambleLength(PREAMB);

    LoRa.setSignalBandwidth(BANDSIGNAL);

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

        while (LoRa.available()) {
            LoRa.readBytes(buffer, (size_t) sizeof(COM));
        }

        if(this->debug){
            Serial.println("Dados do pacote: ");
            Serial.print("Destino: ");
            Serial.println(package.Dest_Add);
            Serial.print("Hash: ");
            Serial.println((char*)package.Check_Data);
            Serial.print("Dados recebidos: ");
            Serial.println(package.Data);
            Serial.print("Id do pacote:");
            Serial.println(package.ID);
            Serial.print("Estado do pacote: ");
            Serial.println(package.Status);
            Serial.print("Dados da conexão: ");
            Serial.print("RSSI ");
            Serial.println(LoRa.packetRssi());
            
        }

        if(package.Dest_Add != myAdd){
            if(this->debug){
                Serial.println("Endereço diferente!!!");
            }
            memset(package.Data,0 , sizeof(char)*TAM_DATA);
            package.Status = 2;
            package.Length = 0;
        }

        if(package.Status == 4){
            package = this->sentConfirm(package.Rem_Add);
        }
    
        char hashDigest[16];

        memset(hashDigest, 0, 16*sizeof(char) );

        this->generateCheck(package.Data,
                hashDigest, 
                TAM_DATA*sizeof(char),
                16*sizeof(char));
        
        if((memcmp(hashDigest, package.Check_Data, 16*sizeof(char))!=0)
            || !this->receive){

            memset(package.Data,0, sizeof(char)*TAM_DATA);

            package.Status = 2;
            package.Length = 0;

            if(this->debug){
            Serial.println("Falha no hash ou na confirmação!");

            }
        }
    }
    else{
        package.Status = 3;
        if(this->debug){
        }
    }

    return package;
}

/*!
    @brief  Recebe o pacote de comunicação limpo
    @param  None
			Nenhum parâmetro
    @return None
			Nenhum retorno fornecido
    @note   Retorn um pacote criado e limpo
*/
COM Lora_TTGOV2::getClearPack(){
    COM package;
    package.Dest_Add = 000;
    package.Length = 000;
    package.Rem_Add = 000;
    
    package.Dest_Add = 000;
    package.Rem_Add = this->myAdd;

    memset(package.Check_Data, 0, 16*sizeof(char));

    memset(package.Data, 0, sizeof(char)*TAM_DATA);

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
    @brief  Solicita confirmação para enviar pacotes
    @param  None
			Nenhum parâmetro
    @return BOOL
			True, caso recebeu confirmação de que pode enviar os
                dados;
            False, caso não possa enviar os dados;
    @note   Apenas envia uma mesnagem de solicitando permissão
        paraenvio de dados
*/
bool Lora_TTGOV2::reqConfirm(uint8_t address){
    COM package;
    package = this->getClearPack();
    package.Dest_Add = address;
    package.Status = 4;
    package.ID = 0;

    this->generateCheck(package.Data,
                package.Check_Data, 
                TAM_DATA*sizeof(char),
                16*sizeof(char));

    while(!this->sentPacket(package)){
        if(this->debug){
            Serial.println("Tentando enviar confirmação!");
        }

        continue;
    }

    this->receive = false;

    COM recPackage;

    //Retorna a confirmação de que pode enviar o pacote
    uint16_t timeout = 0;
    while((recPackage.Status!=6) && (timeout < 300)){
        recPackage = this->receivePacket();
        timeout++;
        if(this->debug){
            if(timeout%50 == 0)
                Serial.println(".");
            Serial.print(".");
        }
        delay(10);
    }

    if(this->debug){
            Serial.println("Fim da solicitação de confirmação!");
        }

    return (recPackage.Status==6) ? true : false;
    
}

/*!
    @brief  Recebe uma solicitação de envio de pacote
    @param  None
			Nenhum parâmetro
    @return COM
            O primeiro pacote enviado após permissão de recebimento
    @note   Envia uma confirmação de que pode receber pacotes
*/
COM Lora_TTGOV2::sentConfirm(uint8_t address){
    COM package;
    
    package = this->getClearPack();

    if(this->debug){
        Serial.println("Início da confirmação!!");
    }

    package.Status = 6;
    package.Dest_Add = address;
    package.ID = 0;

    this->generateCheck(package.Data,
                package.Check_Data, 
                TAM_DATA*sizeof(char),
                16*sizeof(char));

    while(!this->sentPacket(package)){
        if(this->debug){
            Serial.println("Tentando enviar pacote de permissão!");
        }
        continue;
    }

    if(this->debug){
            Serial.print("Permissão a ");
            Serial.println(package.Dest_Add);
            Serial.println("Fim da confirmação.");
            
        }

    this->receive = true;

    return this->receivePacket();
    
}

/*!
    @brief  Gera o hash para o campo de checagem do pacote
    @param  data
			Ponteiro para o local que será guardado o hash
    @param  len
            Tamanho em bytes de data
    @return NONE
    @note   Gera o hash para dos dados para a estrtura de
        comunicação
*/
void Lora_TTGOV2::generateCheck(
    char * data,
    char* check,
    size_t lenData,
    size_t lenCheck
    ){

    //Gera o hash em hexadecimal
    char * hash = MD5::make_digest(
                    MD5::make_hash(data, lenData),16);

    memset(check, 0, lenCheck);
            
    memcpy(check, hash, lenCheck);

    free(hash);
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

        package = this->getClearPack();

        bool sendAll = false;

        uint8_t countPack = 0;

        //Não pode conectar ao equipamento
        if(!this->reqConfirm(dest)){
            if(this->debug){
                Serial.println("Não foi possível obter permissão!!!");
            }

            return false;

        }
        else{
            if(this->debug){
                Serial.println("Pemissão concedida, enviando pacote!!!");
            }
        }

        //Primeiro Id é o de confirmação
        countPack++;


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

            this->generateCheck(package.Data,
                package.Check_Data, 
                TAM_DATA*sizeof(char),
                16*sizeof(char));

            package.ID = countPack;

            package.Dest_Add = dest;

            byte += TAM_DATA;

            if(this->debug){
                Serial.print("Bytes restantes: ");
                Serial.println(package.Length);
                Serial.print("Check: ");
                Serial.println(package.Check_Data);
                delay(200);
            }
            
            if (this->sentPacket(package)){
                countPack++;
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

    int indexPack = 0;

    package = this->receivePacket();

    for(; package.Status!=5 ; ptr+= package.Length){
        package = this->receivePacket();

        if((package.ID != indexPack) && (package.Status == 4)){
            if(this->debug){
                Serial.print("Index Esperado / Index do pacote.");
		        Serial.print(indexPack);
                Serial.print("/");
                Serial.println(package.ID);
            }
             return 0;
        }

        if(package.Status == 1 || package.Status == 5)
        {
            if(this->debug){
                Serial.println("Pacote em envio...");
            }
            indexPack++;
            //Término de envio do pacote
            this->receive = false;
        }

        if(!this->receive && package.Status == 2){
            return 0;
        }

        if(package.Status != 3 ){
            memcpy(ptr, package.Data, 
                        package.Length*sizeof(char));
            lengthReceive += package.Length;
        }

        delay(10);
    }
    return lengthReceive;

}
