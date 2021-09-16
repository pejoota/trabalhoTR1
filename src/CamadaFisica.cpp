#include "../header/CamadaFisica.hpp"

#define BINARIA 0
#define MANCHESTER 1
#define BIPOLAR 2

#define CODIFICACAO 0
#define DECODIFICACAO 1

int tipoDeCodificacao = BIPOLAR; //alterar de acordo com o teste

void AplicacaoTransmissora(void){

	if(has_colors())
		start_color();

	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	attron(COLOR_PAIR(1));

	printw("Digite a mensagem: ");

	std::string mensagem = getstring();

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

	switch (tipoDeCodificacao){

		case BINARIA: // codificação binária
			quadro = CamadaFisicaTransmissoraCodificacaoBinaria(quadro);
			break;
		case MANCHESTER: // codificação manchester
			quadro = CamadaFisicaTransmissoraCodificacaoManchester(quadro);
			break;
		case BIPOLAR: // codificação bipolar
			quadro = CamadaFisicaTransmissoraCodificacaoBipolar(quadro);
			break;

	} // fim do switch/case

	interface(quadro, CODIFICACAO);
	printw("Pressione enter para continuar...");
	noecho();
	getch();

	MeioDeComunicacao(quadro);

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

		retorno.push_back(quadro[i] ^ clock[0]);	
		retorno.push_back(quadro[i] ^ clock[1]);
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

} // fim do método CamadaFisicaTransmissoraCodificacaoBipolar

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

	interface(quadro, DECODIFICACAO);
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

	for(int i = 0; i < bits.size() / 2; i++) 
		quadro[i] = bits[i*2] ^ clock[0];

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

			// printf("%c (%d)\n", (char)aux, aux);
			mensagem = (char)aux + mensagem;
			aux = 0;
		}

		aux = (aux << 1) | quadro[i];
	}
	
	mensagem = (char)aux + mensagem;

	AplicacaoReceptora(mensagem);
}

void AplicacaoReceptora(std::string mensagem) {

	printw("A mensagem recebida foi:\n");
	printw(mensagem.c_str());
	attroff(COLOR_PAIR(1));
	getch();
	endwin();
}

void interface(std::vector<int> quadro, int modo){

	std::string tipo = "";			

	switch (tipoDeCodificacao){

		case BINARIA:
			tipo = "Binária";
			break;

		case MANCHESTER:
			tipo = "Manchester";
			break;

		case BIPOLAR:
			tipo = "Bipolar";
			break;
	}

	switch(modo){

		case CODIFICACAO:
			printw("\n                                                                Codificação ");
			printw((tipo + "\n\nClock:\n\n" + geraClock() + "\n\nSinal de entrada:\n\n").c_str());
			break;
		case DECODIFICACAO:
			printw("\n                                                                Decodificação ");
			printw((tipo + "\n\nSinal de saída:\n\n").c_str());

			break;
	}

	geraOnda(quadro);	
}

std::string geraClock(){

	std::string	
		clock  = "           ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐\n";
	   	clock += "           │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │\n";
	   	clock += "           │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │\n";
	   	clock += "        ───┘   └───┘   └───┘   └───┘   └───┘   └───┘   └───┘   └───┘   └───┘   └───┘   └───┘   └───┘   └───┘   └───┘   └───┘   └───┘   └\n";

	return clock;
}

void geraOnda(std::vector<int> quadro){

	int
		x,
		y;

	std::string
		topo    = "        ",
		intSup1 = "        ", 
		intSup2 = "        ", 
		centro  = "        ", 
		intInf1 = "        ", 
		intInf2 = "        ",
		fundo   = "        ";

	getyx(stdscr, y, x);

	int count = 0;

	for (std::vector<int>::iterator i = quadro.begin(); i != quadro.end(); ++i, count++){

		if(count%16 == 0 && count != 0){
			
			if(tipoDeCodificacao == BIPOLAR)
				printw((topo + "\n" + intSup1 + "\n" + intSup2 + "\n" + centro + "\n" + intInf1 + "\n" + intInf2 + "\n" + fundo).c_str());
			else
				printw((topo + "\n" + intSup1 + "\n" + intSup2 + "\n" + centro).c_str());

			printw("\nPressione enter para ver o restante do sinal...");
			getch();
			noecho();
			move(y, x);
			clrtobot();

			topo    = "        ";
			intSup1 = "        "; 
			intSup2 = "        "; 
			centro  = "        "; 
			intInf1 = "        "; 
			intInf2 = "        ";
			fundo   = "        ";
		}

		if(*i == 1){

			if(i == quadro.end()-1 || *(i+1) == 1){

				topo    += "────────";
	 			intSup1 += "        ";
	 			intSup2 += "        ";
	 			centro  += "        ";
	 			intInf1 += "        ";
	 			intInf2 += "        ";
	 			fundo   += "        ";


			} else if (*(i+1) == 0){

	 			topo    += "───────┐";
	 			intSup1 += "       │";
	 			intSup2 += "       │";
	 			centro  += "       └";
	 			intInf1 += "        ";
	 			intInf2 += "        ";
	 			fundo   += "        ";


			} else if(*(i+1) == -1){

				topo    += "───────┐";
	 			intSup1 += "       │";
	 			intSup2 += "       │";
	 			centro  += "       │";
	 			intInf1 += "       │";
	 			intInf2 += "       │";
	 			fundo   += "       └";	 			
			}

		} else if (*i == 0){

			if(*(i+1) == 1){

				topo    += "       ┌";
	 			intSup1 += "       │";
	 			intSup2 += "       │";
	 			centro  += "───────┘";
	 			intInf1 += "        ";
	 			intInf2 += "        ";
	 			fundo   += "        ";


			} else if (i == quadro.end()-1 || *(i+1) == 0){

	 			topo    += "        ";
	 			intSup1 += "        ";
	 			intSup2 += "        ";
	 			centro  += "────────";
	 			intInf1 += "        ";
	 			intInf2 += "        ";
	 			fundo   += "        ";


			} else if(*(i+1) == -1){

				topo    += "        ";
	 			intSup1 += "        ";
	 			intSup2 += "        ";
	 			centro  += "───────┐";
	 			intInf1 += "       │";
	 			intInf2 += "       │";
	 			fundo   += "       └";	 			
			}

		} else {

			if(*(i+1) == 1){

				topo    += "       ┌";
	 			intSup1 += "       │";
	 			intSup2 += "       │";
	 			centro  += "       │";
	 			intInf1 += "       │";
	 			intInf2 += "       │";
	 			fundo   += "───────┘";


			} else if (*(i+1) == 0){

	 			topo    += "        ";
	 			intSup1 += "        ";
	 			intSup2 += "        ";
	 			centro  += "       ┌";
	 			intInf1 += "       │";
	 			intInf2 += "       │";
	 			fundo   += "───────┘";


			} else if(i == quadro.end()-1 || *(i+1) == -1){
				topo    += "        ";
	 			intSup1 += "        ";
	 			intSup2 += "        ";
	 			centro  += "        ";
	 			intInf1 += "        ";
	 			intInf2 += "        ";
	 			fundo   += "────────";	 			
			}
		}
	}

	if(tipoDeCodificacao == BIPOLAR)
		printw((topo + "\n" + intSup1 + "\n" + intSup2 + "\n" + centro + "\n" + intInf1 + "\n" + intInf2 + "\n" + fundo + "\n\n").c_str());
	else
		printw((topo + "\n" + intSup1 + "\n" + intSup2 + "\n" + centro + "\n\n").c_str());
}

std::string getstring(){
    std::string input;

    // let the terminal do the line editing
    nocbreak();
    echo();

    // this reads from buffer after <ENTER>, not "raw" 
    // so any backspacing etc. has already been taken care of
    int ch = getch();
    

    while ( ch != '\n' )
    {
        input.push_back( ch );
        ch = getch();
        
    }

    // restore your cbreak / echo settings here

    return input;
}