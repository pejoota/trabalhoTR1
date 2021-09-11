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
	std::vector<std::vector<int>> quadro;
	std::vector<int> bits;

	for(auto caracter:mensagem){
		for(int j = 0; j < 8; j++)
			bits.push_back(0 != (caracter & (1 << j)));

 		quadro.push_back(bits);
 		bits.erase(bits.begin(), bits.end());

	} // fim do for

 	//chama a próxima camada
 	CamadaFisicaTransmissora(quadro);

} //fim do método CamadaDeAplicacaoTransmissora

void CamadaFisicaTransmissora(std::vector<std::vector<int>> quadro){

	int tipoDeCodificacao = BINARIA; //alterar de acordo com o teste
	std::vector<std::vector<int>> fluxoBrutoDeBits;

	switch (tipoDeCodificacao){

		case BINARIA: // codificação binária
			fluxoBrutoDeBits = CamadaFisicaTransmissoraCodificacaoBinaria(quadro);
			break;
		case MANCHESTER: // codificação manchester
			BrutoDeBits = CamadaFisicaTransmissoraCodificacaoManchester(quadro);
			break;
		case BIPOLAR: // codificação bipolar
			BrutoDeBits = CamadaFisicaTransmissoraCodificacaoBipolar(quadro);
			break;
	} // fim do switch/case

	MeioDeComunicacao(fluxoBrutoDeBits);

} // fim do método CamadaFisicaTransmissora

std::vector<std::vector<int>> CamadaFisicaTransmissoraCodificacaoBinaria(std::vector<std::vector<int>>){

	for(bits:quadro){
		for(bit:bits){

			if(bit == 0)
				bit = -1;

		} // fim do for interno
	} // fim do for externo

	return quadro;

} // fim do método CamadaFisicaTransmissoraCodificacaoBinaria

std::vector<std::vector<int>> CamadaFisicaTransmissoraCodificacaoManchester(std::vector<std::vector<int>> quadro){

	std::vector<int>
		clock = {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};

	std::vector<std::vector<int>>
		retorno;

	for (std::vector<std::vector<int>>::iterator i = quadro.begin(); i != quadro.end(); ++i)
		retorno.push_back((*i)^clock);
	
	return retorno;

} // fim do método CamadaFisicaTransmissoraCodificacaoManchester

std::vector<std::vector<int>> CamadaFisicaTransmissoraCodificacaoBipolar(std::vector<std::vector<int>>){


	for(bits:quadro){
		for(bit:bits){

			if(bit == 1 && anterior == 1){

				bit = -1;
				anterior = -1;

			} // fim do if		
		} // fim do for interno
	} // fim do for externo

	return quadro;
}

void MeioDeComunicacao(std::vector<std::vector<int>>);
void CamadaFisicaReceptora(std::vector<std::vector<int>>);
std::vector<std::vector<int>> CamadaFisicaReceptoraCodificacaoBinaria(std::vector<std::vector<int>>);
std::vector<std::vector<int>> CamadaFisicaReceptoraCodificacaoManchester(std::vector<std::vector<int>>);
std::vector<std::vector<int>> CamadaFisicaReceptoraCodificacaoBipolar(std::vector<std::vector<int>>);
void CamadaDeAplicacaoReceptora(std::vector<std::vector<int>>);
void AplicacaoReceptora(std::string);