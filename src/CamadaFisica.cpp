#include "../header/CamadaFisica.hpp"
#define BINARIA 0
#define MANCHESTER 1
#define BIPOLAR 2

int tipoDeCodificacao = MANCHESTER; //alterar de acordo com o teste

void AplicacaoTransmissora(void){
	std::string mensagem;

	std::cout << "Digite uma mensagem: ";
	std::cin >> mensagem;

	//chama a próxima camada
	CamadaDeAplicacaoTransmissora(mensagem);

} //fim do método AplicacaoTransmissora


void CamadaDeAplicacaoTransmissora(std::string mensagem){
	std::vector<int> quadro;

	for(auto caracter:mensagem){
		for(int j = 0; j < 8; j++)
			quadro.insert(quadro.begin(), 0 != (caracter & (1 << j)));

	} // fim do for

 	//chama a próxima camada
 	CamadaFisicaTransmissora(quadro);

} //fim do método CamadaDeAplicacaoTransmissora

void CamadaFisicaTransmissora(std::vector<int> quadro){
	std::vector<int> fluxoBrutoDeBits;

	switch (tipoDeCodificacao){

		case BINARIA: // codificação binária
			fluxoBrutoDeBits = CamadaFisicaTransmissoraCodificacaoBinaria(quadro);
			break;
		case MANCHESTER: // codificação manchester
			fluxoBrutoDeBits = CamadaFisicaTransmissoraCodificacaoManchester(quadro);
			break;
		case BIPOLAR: // codificação bipolar
			fluxoBrutoDeBits = CamadaFisicaTransmissoraCodificacaoBipolar(quadro);
			break;

	} // fim do switch/case

	MeioDeComunicacao(fluxoBrutoDeBits);

} // fim do método CamadaFisicaTransmissora

std::vector<int> CamadaFisicaTransmissoraCodificacaoBinaria(std::vector<int> quadro){

	for (std::vector<int>::iterator i = quadro.begin(); i != quadro.end(); ++i){
		if(*i == 0)
			*i = -1;

	} // fim do for

	return quadro;

} // fim do método CamadaFisicaTransmissoraCodificacaoBinaria

std::vector<int> CamadaFisicaTransmissoraCodificacaoManchester(std::vector<int> quadro){

	std::vector<int>
		clock = {0,1},
		retorno;

	for (int i = 0; i < quadro.size(); i++){

		retorno.push_back(quadro[i]^clock[0]);	
		retorno.push_back(quadro[i]^clock[1]);
	}
	
	return retorno;

} // fim do método CamadaFisicaTransmissoraCodificacaoManchester

std::vector<int> CamadaFisicaTransmissoraCodificacaoBipolar(std::vector<int> quadro){
	int anterior = -1;

	for (std::vector<int>::iterator i = quadro.begin(); i != quadro.end(); ++i){
		
		if(*i == 1 && anterior == 1){

			*i = -1;
			anterior = -1;

		} else if(*i == 1 && anterior == -1)
			anterior = 1;
					
	} // fim do for

	return quadro;
}

void MeioDeComunicacao(std::vector<int> bitsEnviados) {
	std::vector<int> bitsRecebidos(bitsEnviados.size());

	for(int i = 0; i < bitsEnviados.size(); i++) {
		bitsRecebidos[i] = bitsEnviados[i];
	}

	CamadaFisicaReceptora(bitsRecebidos);
}

void CamadaFisicaReceptora(std::vector<int> bits) {
	std::vector<int> quadro(bits.size());

	switch(tipoDeCodificacao) {
		case BINARIA:
			quadro = CamadaFisicaReceptoraDecodificacaoBinaria(bits);
			break;
		case MANCHESTER:
			quadro = CamadaFisicaReceptoraDecodificacaoManchester(bits);
			break;
		case BIPOLAR:
			quadro = CamadaFisicaReceptoraDecodificacaoBipolar(bits);
			break;
	}

	CamadaDeAplicacaoReceptora(quadro);
}

std::vector<int> CamadaFisicaReceptoraDecodificacaoBinaria(std::vector<int> bits) {
	std::vector<int> quadro(bits.size());

	for(int i = 0; i < bits.size(); i++) {
		quadro[i] = bits[i];
		if(quadro[i] == -1)
			quadro[i] = 0;
	}

	return quadro;
}

std::vector<int> CamadaFisicaReceptoraDecodificacaoManchester(std::vector<int> bits) {
	std::vector<int> quadro(bits.size() / 2);

	int clock[] = {0,1};

	for(int i = 0; i < bits.size() / 2; i++) {
		quadro[i] = bits[i*2]^clock[0];
	}

	return quadro;
}

std::vector<int> CamadaFisicaReceptoraDecodificacaoBipolar(std::vector<int> bits) {
	std::vector<int> quadro(bits.size());

	for(int i = 0; i < bits.size(); i++) {
		if(bits[i] < 0)
			quadro[i] = -bits[i];
		else
			quadro[i] = bits[i];
	}

	return quadro;
}

void CamadaDeAplicacaoReceptora(std::vector<int> quadro) {
	std::string mensagem;

	int aux = 0;
	for(int i = 0; i < quadro.size(); i++) {
		if(i%8 == 0) {
			printf("%c (%d)\n", (char)aux, aux);
			mensagem = (char)aux + mensagem;
			aux = 0;
		}

		aux = (aux << 1) | quadro[i];
	}
	
	mensagem = (char)aux + mensagem;

	AplicacaoReceptora(mensagem);
}

void AplicacaoReceptora(std::string mensagem) {
	std::cout << "A mensagem recebida foi\n" << mensagem << "\n";
}