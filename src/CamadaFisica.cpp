#include "../header/CamadaFisica.hpp"
#include "../header/Interface.hpp"

#ifdef _WIN32

#include <Windows.h>
#define SLEEP(X) Sleep((X))

#else

#include <unistd.h>
#define SLEEP(X) usleep((X))

#endif

#define CODIFICACAO 0
#define DECODIFICACAO 1

int tipoDeCodificacao = BINARIA; //alterar de acordo com o teste

int width, height;

std::string mensagem;

void AplicacaoTransmissora(void){
	getmaxyx(stdscr, height, width);

	if(has_colors())
		start_color();

	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	attron(COLOR_PAIR(1));

	printw("Digite a mensagem: ");

	mensagem = getstring();

	//chama a próxima camada
	CamadaDeAplicacaoTransmissora(mensagem);

} //fim do método AplicacaoTransmissora


void CamadaDeAplicacaoTransmissora(std::string mensagem){
	std::vector<int> quadro;

	for(auto caracter:mensagem){
		for(int j = 0; j < 8; j++)
			quadro.insert(quadro.begin(), 0 != (caracter & (1 << j)));

	} // fim do for

	for(int j = 0; j < 8; j++)
		quadro.insert(quadro.begin(), 0);

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
		clock = {-1,1},
		retorno;

	for (int i = 0; i < quadro.size(); i++){

		retorno.push_back(quadro[i] * clock[0]);	
		retorno.push_back(quadro[i] * clock[1]);
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

	interface(bitsEnviados, bitsRecebidos);
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

	int clock[] = {-1,1};

	for(int i = 0; i < bits.size() / 2; i++) 
		quadro[i] = bits[i*2] * clock[0];

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
	int y, x;
	getyx(stdscr, y, x);

	mvprintw(y + 1, 0, "Pressione 'q' para sair");
	while(true) {
		attroff(COLOR_PAIR(1));
		if(getch()== 'q')
			break;
	}
	endwin();
}

std::string getEncodingName(int tipoDeCodificacao) {
	switch (tipoDeCodificacao){
		case BINARIA:
			return "Binária";
		case MANCHESTER:
			return "Manchester";
		case BIPOLAR:
			return "Bipolar";
	}

	return "Unknown encoding";
}

void resizeHandler(int signal_value) {
	if(signal_value == SIGWINCH) {
		endwin();
		refresh();

		initscr();
		getmaxyx(stdscr, height, width);
		printf("%d, %d\n", height, width);
	}
}

void interface(std::vector<int> bitsEnviados, std::vector<int> bitsRecebidos){
	std::string tipo = getEncodingName(tipoDeCodificacao);

	int y, x;

	std::vector<int> clock = {-1, 1};

	int level_width = 3;
	int initial_pos = 0;

	int level_number = bitsEnviados.size() * (level_width + 1);

	int input;

	int usleep_amount = 50000;

	int text_offset = 3, wave_offset = 1;

	clear();

	for(; true; initial_pos = (initial_pos + 1 ) % level_number) {
		clear;

		mvprintw(0, text_offset, "Codificação %s", tipo.c_str());
		mvprintw(2, text_offset, "Mensagem de entrada: %s", mensagem.c_str());
		mvprintw(3, text_offset, "Clock:");
		
		getyx(stdscr, y, x);
		showWave(clock, y + 1, wave_offset, initial_pos, width - 2, level_width, REPEAT_WAVE);

		getyx(stdscr, y, x);

		y += 2;
		mvprintw(y, text_offset, "Sinal:");

		showWave(bitsEnviados, y + 1, wave_offset, initial_pos, width - 2, level_width, REPEAT_WAVE);

		getyx(stdscr, y, x);
		int i = initial_pos / (level_width + 1);
		mvprintw(y + 1, wave_offset, "^ %d: %+d", i, bitsEnviados[i]);

		mvprintw(y + 2, text_offset, "Pressione 'q' para continuar...");

		timeout(0);
		noecho();
		input = getch();
		echo();
		notimeout(stdscr, true);

		if(input == 'q')
			break;

		if(input == '1')
			usleep_amount = 50000 * 2;
		else if(input == '2')
			usleep_amount = 50000;
		else if(input == '3')
			usleep_amount = 50000 / 2;
		else if(input == '4')
			usleep_amount = 50000 / 4;


		SLEEP(usleep_amount);
	}
}
/*
01100001
*/
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