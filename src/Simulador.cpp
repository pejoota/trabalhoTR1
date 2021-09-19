#include <iostream>
#include <vector>
#include <locale.h>
#include <signal.h>
#include "../header/CamadaFisica.hpp"

int main(void){
	signal(SIGWINCH, resizeHandler);

	setlocale(LC_ALL, "");
	initscr();
	cbreak();

	AplicacaoTransmissora();

	endwin();

 	return 0;

} // fim do método main