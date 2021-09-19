#include <iostream>
#include <vector>
#include <locale.h>
#include <signal.h>
#include "../header/CamadaFisica.hpp"

int main(int argc, char **argv){
	signal(SIGWINCH, resizeHandler);

	if(argc > 1) {
		std::string arg;

		for(int i = 0; argv[1][i] != '\0'; i++)
			arg.insert(arg.begin(), tolower(argv[1][i]));

		if(arg == "b" || arg == "binaria") {
			tipoDeCodificacao = BINARIA;
		} else if(arg == "m" || arg == "manches") {
			tipoDeCodificacao = MANCHESTER;
		} else if(arg == "p" || arg == "bipolar") {
			tipoDeCodificacao = BIPOLAR;
		} else {
			printf(
				"Usar\n"
				"\tb binaria \tSeleciona codificação binária\n"
				"\tm manches \tSeleciona codificação manchester\n"
				"\tp bipolar \tSeleciona codificação bipolar\n"
			);
			return 0;
		}
	}

	setlocale(LC_ALL, "");
	initscr();
	cbreak();

	AplicacaoTransmissora();

	endwin();

 	return 0;

} // fim do método main