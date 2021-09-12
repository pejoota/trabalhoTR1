#include "../header/CamadaFisica.hpp"
#define BINARIA 0
#define MANCHESTER 1
#define BIPOLAR 2

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

	int tipoDeCodificacao = BINARIA; //alterar de acordo com o teste
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

void MeioDeComunicacao(std::vector<int>);
void CamadaFisicaReceptora(std::vector<int>);
std::vector<int> CamadaFisicaReceptoraCodificacaoBinaria(std::vector<int>);
std::vector<int> CamadaFisicaReceptoraCodificacaoManchester(std::vector<int>);
std::vector<int> CamadaFisicaReceptoraCodificacaoBipolar(std::vector<int>);
void CamadaDeAplicacaoReceptora(std::vector<int>);
void AplicacaoReceptora(std::string);