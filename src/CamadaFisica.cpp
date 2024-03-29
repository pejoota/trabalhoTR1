
#include "../header/CamadaFisica.hpp"

#include "../header/CamadaEnlace.hpp"
#include "../header/Interface.hpp"

#ifdef _WIN32

#include <Windows.h>
#define SLEEP(X) Sleep((X))

#else

#include <unistd.h>
#define SLEEP(X) usleep((X))

#endif

#define getCharNonBlocking(X) \
  timeout(0);                 \
  noecho();                   \
  (X) = getch();              \
  echo();                     \
  timeout(-1)

int tipoDeCodificacao = BINARIA;  //alterar de acordo com o teste

int errorCount = 0;
int maxErrors = INT32_MAX;

int width, height;

std::string mensagem;

FILE* logFile;

void AplicacaoTransmissora(void) {
  logFile = fopen("logFile.log", "w");

  getmaxyx(stdscr, height, width);

  if (has_colors())
    start_color();

  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  attron(COLOR_PAIR(1));

  printw("Digite a mensagem: ");

  mensagem = getstring();

  //chama a próxima camada
  CamadaDeAplicacaoTransmissora(mensagem);

}  //fim do método AplicacaoTransmissora

void CamadaDeAplicacaoTransmissora(std::string mensagem) {
  std::vector<int> quadro;

  for (int i = 0; i < mensagem.size(); i++) {
    for (int j = 7; j >= 0; j--)
      quadro.push_back(0 != (mensagem[i] & (1 << j)));

  }  // fim do for

  for (int j = 7; j >= 0; j--)
    quadro.push_back(0);

  //chama a próxima camada
  CamadaEnlaceTransmissora(quadro);

}  //fim do método CamadaDeAplicacaoTransmissora

void CamadaFisicaTransmissora(std::vector<int> quadro) {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_real_distribution<double> dist100(0.0, 100.0);

  double porcentagemDeErro = 5;

  for (int i = 0; i < quadro.size(); i++) {
    if (dist100(rng) < porcentagemDeErro && errorCount < maxErrors) {
      quadro[i] = !quadro[i];
      errorCount++;
      fprintf(logFile, "Erro %d adicionado [%d] de %d para %d\n", errorCount, i, !quadro[i], quadro[i]);
    } else {
      quadro[i] = quadro[i];
    }
  }

  switch (tipoDeCodificacao) {
    case BINARIA:  // codificação binária
      quadro = CamadaFisicaTransmissoraCodificacaoBinaria(quadro);
      break;
    case MANCHESTER:  // codificação manchester
      quadro = CamadaFisicaTransmissoraCodificacaoManchester(quadro);
      break;
    case BIPOLAR:  // codificação bipolar
      quadro = CamadaFisicaTransmissoraCodificacaoBipolar(quadro);
      break;

  }  // fim do switch/case

  MeioDeComunicacao(quadro);

}  // fim do método CamadaFisicaTransmissora

std::vector<int> CamadaFisicaTransmissoraCodificacaoBinaria(std::vector<int> quadro) {
  for (std::vector<int>::iterator i = quadro.begin(); i != quadro.end(); ++i) {
    if (*i == 0)
      *i = -1;

  }  // fim do for

  return quadro;

}  // fim do método CamadaFisicaTransmissoraCodificacaoBinaria

std::vector<int> CamadaFisicaTransmissoraCodificacaoManchester(std::vector<int> quadro) {
  const std::vector<int>
      clock = {0, 1};
  std::vector<int> retorno;

  for (int i = 0; i < quadro.size(); i++) {
    retorno.push_back((quadro[i] ^ clock[0]) * 2 - 1);
    retorno.push_back((quadro[i] ^ clock[1]) * 2 - 1);
  }

  return retorno;

}  // fim do método CamadaFisicaTransmissoraCodificacaoManchester

std::vector<int> CamadaFisicaTransmissoraCodificacaoBipolar(std::vector<int> quadro) {
  int anterior = -1;

  for (std::vector<int>::iterator i = quadro.begin(); i != quadro.end(); ++i) {
    if (*i == 1 && anterior == 1) {
      *i = -1;
      anterior = -1;

    } else if (*i == 1 && anterior == -1)
      anterior = 1;

  }  // fim do for

  return quadro;

}  // fim do método CamadaFisicaTransmissoraCodificacaoBipolar

void MeioDeComunicacao(std::vector<int> bitsEnviados) {
  std::vector<int> bitsRecebidos(bitsEnviados);

  // ERROS FORAM INSERIDOS NA FUNÇÃO CamadaFisicaTransmissora

  interface(bitsEnviados, bitsRecebidos);
  CamadaFisicaReceptora(bitsRecebidos);
}

void CamadaFisicaReceptora(std::vector<int> signal) {
  std::vector<int> bits = decodeBits(signal);

  CamadaEnlaceReceptora(bits);
}

std::vector<int> CamadaFisicaReceptoraDecodificacaoBinaria(std::vector<int> signal) {
  std::vector<int> quadro(signal.size());

  for (int i = 0; i < signal.size(); i++) {
    if (signal[i] == 1) {
      quadro[i] = signal[i];
    } else if (signal[i] == -1) {
      quadro[i] = 0;
    } else {
      mvprintw(2, 0, "Sinal sofreu erros que tornaram a decodificação binaria impossível");
      int input;
      getCharNonBlocking(input);
      endwin();
      printf("\n\n\nmano n sei oq ta rolando [%d]%d\n\n\n", i, signal[i]);
      exit(1);
    }
  }

  return quadro;
}

std::vector<int> CamadaFisicaReceptoraDecodificacaoManchester(std::vector<int> signal) {
  std::vector<int> quadro(signal.size() / 2);

  int clock[] = {0, 1},
      firstClock, secondClock;

  for (int i = 0; i < signal.size() / 2; i++) {
    firstClock = ((signal[i * 2] + 1) / 2) ^ clock[0];
    secondClock = ((signal[i * 2 + 1] + 1) / 2) ^ clock[1];

    if (firstClock != secondClock) {
      mvprintw(2, 0, "Sinal sofreu erros que tornaram a decodificação Manchester impossível");
      int input;
      getCharNonBlocking(input);
      endwin();
      exit(1);
    }

    quadro[i] = firstClock;
  }

  return quadro;
}

std::vector<int> CamadaFisicaReceptoraDecodificacaoBipolar(std::vector<int> signal) {
  std::vector<int> quadro(signal.size());

  int lastOne = -1;

  for (int i = 0; i < signal.size(); i++) {
    if (signal[i] == 0) {
      quadro[i] = 0;
    } else if (signal[i] == -lastOne) {
      quadro[i] = 1;
      lastOne = -lastOne;
    } else {
      mvprintw(2, 0, "Sinal sofreu erros que tornaram a decodificação bipolar impossível");
      int input;
      getCharNonBlocking(input);
      endwin();
      exit(1);
    }
  }

  return quadro;
}

void CamadaDeAplicacaoReceptora(std::vector<int> quadro, std::string errorMessage) {
  std::string mensagem = binToMessage(quadro);

  AplicacaoReceptora(mensagem, errorMessage);
}

void AplicacaoReceptora(std::string mensagem, std::string errorMessage) {
  printw(errorMessage.c_str());
  printw("\n");
  printw("A mensagem recebida foi:\n");
  printw(mensagem.c_str());
  int y, x, input;
  getyx(stdscr, y, x);

  mvprintw(y + 1, 0, "Pressione 'q' para sair");
  attroff(COLOR_PAIR(1));
  while (true) {
    getCharNonBlocking(input);
    if (input == 'q')
      break;
  }
  endwin();
}

std::string getEncodingName(int tipoDeCodificacao) {
  switch (tipoDeCodificacao) {
    case BINARIA:
      return "Binária";
    case MANCHESTER:
      return "Manchester";
    case BIPOLAR:
      return "Bipolar";
  }

  return "Unknown encoding";
}

void resizeHandler(int signalValue) {
  if (signalValue == SIGWINCH) {
    endwin();
    refresh();

    initscr();
    getmaxyx(stdscr, height, width);
    printf("%d, %d\n", height, width);
  }
}

void interface(std::vector<int> bitsEnviados, std::vector<int> bitsRecebidos) {
  std::string tipo = getEncodingName(tipoDeCodificacao);

  int y, x;
  int wavePos = 0;
  const int
      levelWidth = 3,
      textOffset = 3, waveOffset = 1,
      levelsPerBit = tipoDeCodificacao == MANCHESTER ? 2 : 1,
      levelsPerByte = tipoDeCodificacao == MANCHESTER ? 16 : 8;

  int signalLength = bitsEnviados.size() * (levelWidth + 1);

  std::vector<int> clock = {-1, 1};

  int usleepAmount = 50000;

  int input;

  std::vector<int> receivedSignal;
  std::vector<int> receivedBits;
  std::string receivedMessage;

  for (; true; wavePos++) {
    if (wavePos >= signalLength) {
      receivedSignal.clear();
      receivedBits.clear();
      receivedMessage.clear();
    }

    wavePos %= signalLength;

    // First time a signal level is seen
    // add it to the vector
    if (wavePos % (levelWidth + 1) == 0) {
      receivedSignal.push_back(bitsEnviados[wavePos / (levelWidth + 1)]);
    }

    if (receivedSignal.size() % levelsPerBit == 0) {
      receivedBits = decodeBits(receivedSignal);
    }

    if (receivedSignal.size() % levelsPerByte == 0) {
      receivedMessage = binToMessage(receivedBits);
    }

    clear();

    mvprintw(0, textOffset, "Entrada: %s", mensagem.c_str());
    mvprintw(1, textOffset, "Codificação %s", tipo.c_str());
    mvprintw(2, textOffset, "Clock:");

    getyx(stdscr, y, x);
    showWave(clock, y + 1, waveOffset, wavePos, width - 2, levelWidth, REPEAT_WAVE);

    getyx(stdscr, y, x);

    mvprintw(y + 1, textOffset, "Sinal:\t\tErros inseridos:%d", errorCount);

    showWave(bitsEnviados, y + 2, waveOffset, wavePos, width - 2, levelWidth, REPEAT_WAVE);

    int index = wavePos / (levelWidth + 1);
    getyx(stdscr, y, x);

    mvprintw(y + 1, waveOffset, "^ %d: %+d", index, bitsEnviados[index]);
    mvprintw(y + 2, textOffset, "Received: %s", receivedMessage.c_str());
    mvprintw(y + 3, textOffset, "Bits:     ");

    bool hasFullBytesOnly = receivedBits.size() % 8 == 0 && receivedBits.size() > 0;
    if (hasFullBytesOnly) {
      for (int i = receivedBits.size() - 8 - receivedBits.size() % 8; i < receivedBits.size(); i++) {
        printw("%d", receivedBits[i]);
      }
      printw("(%c)", receivedMessage[receivedMessage.size() - 1]);
    } else {
      for (int i = receivedBits.size() - receivedBits.size() % 8; i < receivedBits.size(); i++) {
        printw("%d", receivedBits[i]);
      }
    }

    mvprintw(y + 4, textOffset, "Pressione 'q' para continuar ou 1-5 para mudar a velocidade");

    getCharNonBlocking(input);

    if (input == 'q')
      break;

    if (input == '1')
      usleepAmount = 50000 * 8;
    else if (input == '2')
      usleepAmount = 50000 * 2;
    else if (input == '3')
      usleepAmount = 50000;
    else if (input == '4')
      usleepAmount = 50000 / 2;
    else if (input == '5')
      usleepAmount = 50000 / 4;

    SLEEP(usleepAmount);
  }
}

std::vector<int> decodeBits(std::vector<int>& signal) {
  std::vector<int> bits(signal.size());

  switch (tipoDeCodificacao) {
    case BINARIA:
      bits = CamadaFisicaReceptoraDecodificacaoBinaria(signal);
      break;
    case MANCHESTER:
      bits = CamadaFisicaReceptoraDecodificacaoManchester(signal);
      break;
    case BIPOLAR:
      bits = CamadaFisicaReceptoraDecodificacaoBipolar(signal);
      break;
  }

  return bits;
}

std::string binToMessage(std::vector<int>& bits) {
  std::string mensagem;
  int aux = 0;

  for (int i = 0; i < bits.size(); i++) {
    if (i % 8 == 0 && i > 0) {
      // printf("%c (%d)\n", (char)aux, aux);
      mensagem.push_back((char)aux);
      aux = 0;
    }

    aux = (aux << 1) | bits[i];
  }

  mensagem.push_back((char)aux);

  return mensagem;
}

std::string getstring() {
  std::string input;

  // let the terminal do the line editing
  nocbreak();
  echo();

  // this reads from buffer after <ENTER>, not "raw"
  // so any backspacing etc. has already been taken care of
  int ch = getch();

  while (ch != '\n') {
    input.push_back(ch);
    ch = getch();
  }

  // restore your cbreak / echo settings here

  return input;
}