#include <iostream>
#include <vector>
#include <locale.h>
#include "CamadaFisica.cpp"

int main(void){

	setlocale(LC_ALL, "");
	initscr();
	cbreak();

	AplicacaoTransmissora();
 	return 0;

} // fim do método main