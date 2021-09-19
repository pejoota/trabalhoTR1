#include <iostream>
#include <vector>
#include <locale.h>
#include "../header/CamadaFisica.hpp"

int main(void){

	setlocale(LC_ALL, "");
	initscr();
	cbreak();

	AplicacaoTransmissora();
 	return 0;

} // fim do método main