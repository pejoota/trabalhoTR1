#include <locale.h>
#include <signal.h>

#include <iostream>
#include <vector>

#include "../header/CamadaFisica.hpp"

int main(int argc, char **argv) {
  signal(SIGWINCH, resizeHandler);

  for (int i = 1; i < argc; i++) {
    std::string arg;

    for (int j = 0; argv[i][j] != '\0'; j++)
      arg.push_back(tolower(argv[i][j]));

    if (arg == "b" || arg == "binaria") {
      tipoDeCodificacao = BINARIA;
    } else if (arg == "m" || arg == "manches") {
      tipoDeCodificacao = MANCHESTER;
    } else if (arg == "p" || arg == "bipolar") {
      tipoDeCodificacao = BIPOLAR;
    } else {
      try {
        maxErrors = std::stoi(arg);
        maxErrors = maxErrors >= 0 ? maxErrors : INT32_MAX;
      } catch (...) {
        printf(
            "Usar\n"
            "%s [code] [maxErros]\n\n"
            "maxErros pode ser\n"
            "\t>= 0      \tDefine a quantidade maxima de erros\n"
            "\t < 0      \tNão limita a quantidade de erros\n\n"
            "code pode ser\n"
            "\tb binaria \tSeleciona codificação binária\n"
            "\tm manches \tSeleciona codificação manchester\n"
            "\tp bipolar \tSeleciona codificação bipolar\n",
            argv[0]);

        return 0;
      }
    }
  }

  setlocale(LC_ALL, "");
  initscr();
  cbreak();

  AplicacaoTransmissora();

  endwin();

  return 0;

}  // fim do método main