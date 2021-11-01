#include "../header/CamadaEnlace.hpp"

#include <ncurses.h>

#include "../header/CamadaFisica.hpp"

#define CHAR_COUNTING 0
#define BYTE_INSERTION 1

#define BIT_PARITY 0
#define CRC 1
#define HAMMING 2

#define SEM_ERRO "SEM ERROS DETECTÁVEIS"
#define MSG_ERRO "ERRO DETECTADO NA MENSAGEM"
#define MSG_ERRO_MULT "MAIS DE UM ERRO DETECTADO NA MENSAGEM"
#define MSG_ERRO_CORR "FOI CORRIGIDO UM ERRO ENCONTRADO NA MENSAGEM"
#define GERADOR_ERRO "ERRO DETECTADO NO POLINÔMIO GERADOR"

#define FLAG_BYTE 0xFF
#define ESC_BYTE 0xFE

std::vector<int> gerador = {1, 0, 0, 0, 0, 0, 1, 0,
                            0, 1, 1, 0, 0, 0, 0, 0,
                            1, 0, 0, 0, 1, 1, 1, 0,
                            1, 1, 0, 1, 1, 0, 1, 0, 1};

int tipoDeEnquadramento = CHAR_COUNTING;  // alterar de acordo com o teste
int tipoDeControleErro = HAMMING;

void CamadaEnlaceTransmissora(std::vector<int> &quadro) {
  CamadaEnlaceTransmissoraEnquadramento(quadro);

  CamadaEnlaceTransmissoraControleDeErro(quadro);

  CamadaFisicaTransmissora(quadro);
}

void CamadaEnlaceTransmissoraEnquadramento(std::vector<int> &quadro) {
  switch (tipoDeEnquadramento) {
    case CHAR_COUNTING:
      CamadaEnlaceTransmissoraEnquadramentoContagem(quadro);
      break;
    case BYTE_INSERTION:
      CamadaEnlaceTransmissoraEnquadramentoInsercao(quadro);
      break;
  }
}

void insertByteBitsIntoVector(int byte, std::vector<int> &vector) {
  for (int i = 7; i >= 0; i--) {
    vector.push_back(0 != (byte & (1 << i)));
  }
}

std::vector<int> CamadaEnlaceTransmissoraEnquadramentoContagem(std::vector<int> &quadro) {
  std::vector<int> quadroEnquadrado;
  insertByteBitsIntoVector(quadro.size() / 8, quadroEnquadrado);

  for (auto a : quadro) {
    quadroEnquadrado.push_back(a);
  }

  return quadroEnquadrado;
}

std::vector<int> CamadaEnlaceTransmissoraEnquadramentoInsercao(std::vector<int> &quadro) {
  std::vector<int> quadroEnquadrado;

  // insere byte que sinaliza começo do frame no vetor
  insertByteBitsIntoVector(FLAG_BYTE, quadroEnquadrado);

  int byte = 0;
  for (int i = 0; i < quadro.size(); i++) {
    byte = (byte << 1) | quadro[i];
    if (i % 8 == 7) {  // Read 8 bits
      if (byte == FLAG_BYTE || byte == ESC_BYTE) {
        // insere byte ESC no vetor
        insertByteBitsIntoVector(ESC_BYTE, quadroEnquadrado);
      }

      // insere byte no vetor
      insertByteBitsIntoVector(byte, quadroEnquadrado);
      byte = 0;
    }
  }

  // insere byte que sinaliza final do frame no vetor
  insertByteBitsIntoVector(FLAG_BYTE, quadroEnquadrado);

  return quadroEnquadrado;
}

void CamadaEnlaceTransmissoraControleDeErro(std::vector<int> &quadro) {
  switch (tipoDeControleErro) {
    case BIT_PARITY:
      CamadaEnlaceTransmissoraControleDeErroBitParidade(quadro);
      break;

    case CRC:
      CamadaEnlaceTransmissoraControleDeErroCRC(quadro);
      break;

    case HAMMING:
      CamadaEnlaceTransmissoraControleDeErroHamming(quadro);
      break;
  }
}

void CamadaEnlaceTransmissoraControleDeErroBitParidade(std::vector<int> &quadro) {
  int count = 0;

  for (auto bit : quadro) {
    if (bit) count++;
  }

  quadro.push_back(count % 2);
}

void CamadaEnlaceTransmissoraControleDeErroCRC(std::vector<int> &quadro) {
  std::vector<int> quadroPadded(quadro);

  quadroPadded.insert(quadroPadded.end(), gerador.size() - 1, 0);

  uint64_t result = calcCRC(quadroPadded, gerador);

  for (uint64_t mask = ((uint64_t)1) << (gerador.size() - 2); mask != 0; mask >>= 1) {
    quadro.push_back((result & mask) != 0);
  }
}

void CamadaEnlaceTransmissoraControleDeErroHamming(std::vector<int> &quadro) {
  std::vector<int> quadroHamming;

  if (quadro.size() == 0) {
    return;
  }

  // Adiciona 0 na posição 0 pra alinhar os índices
  // e servir de paridade do bloco inteiro
  quadroHamming.push_back(0);

  uint32_t delta = 1, hammingBits = 0, parityAll = 0, numHammingAdded = 1;

  for (int i = 0; i < quadro.size(); i++) {
    while (quadroHamming.size() == delta) {
      delta = delta << 1;
      numHammingAdded++;

      // Para cada posição igual à um expoente de 2, insere um hamming bit
      quadroHamming.push_back(0);
    }

    quadroHamming.push_back(quadro[i]);

    parityAll ^= quadro[i];

    if (quadro[i] == 1) {
      // Calcula os bits de paridade de hamming
      hammingBits ^= i + numHammingAdded;
    }
  }

  for (int i = 1; i < delta; i <<= 1) {
    quadroHamming[i] = (hammingBits & i) != 0;
    parityAll ^= quadroHamming[i];
  }

  // Guarda a paridade do bloco
  quadroHamming[0] = parityAll;

  quadro = quadroHamming;
}

void CamadaEnlaceReceptora(std::vector<int> &quadro) {
  std::string errorMessage = CamadaEnlaceReceptoraControleDeErro(quadro);

  CamadaEnlaceReceptoraEnquadramento(quadro);

  CamadaDeAplicacaoReceptora(quadro, errorMessage);
}

void CamadaEnlaceReceptoraEnquadramento(std::vector<int> &quadro) {
  switch (tipoDeEnquadramento) {
    case CHAR_COUNTING:
      CamadaEnlaceReceptoraEnquadramentoContagem(quadro);
      break;
    case BYTE_INSERTION:
      CamadaEnlaceReceptoraEnquadramentoInsercao(quadro);
      break;
  }
}

std::string CamadaEnlaceReceptoraControleDeErro(std::vector<int> &quadro) {
  std::string errorMsg;

  switch (tipoDeControleErro) {
    case BIT_PARITY:
      errorMsg = CamadaEnlaceReceptoraControleDeErroBitParidade(quadro);
      break;

    case CRC:
      errorMsg = CamadaEnlaceReceptoraControleDeErroCRC(quadro);
      break;

    case HAMMING:
      errorMsg = CamadaEnlaceReceptoraControleDeErroHamming(quadro);
      break;
  }

  return errorMsg;
}

std::string CamadaEnlaceReceptoraControleDeErroBitParidade(std::vector<int> &quadro) {
  int count = 0;

  for (auto bit : quadro) {
    if (bit) count++;
  }

  quadro.pop_back();

  if (count % 2)
    return MSG_ERRO;
  else
    return SEM_ERRO;
}

std::string CamadaEnlaceReceptoraControleDeErroCRC(std::vector<int> &quadro) {
  uint64_t result = calcCRC(quadro, gerador);

  if (result != 0) {
    return MSG_ERRO;
  }

  return SEM_ERRO;
}

std::string CamadaEnlaceReceptoraControleDeErroHamming(std::vector<int> &quadro) {
  uint32_t delta = 1, hammingBits = 0, parityAll = quadro[0];

  std::string errorMsg = SEM_ERRO;

  for (int i = 1; i < quadro.size(); i++) {
    parityAll ^= quadro[i];
    if (quadro[i] == 1) {
      hammingBits ^= i;
    }
  }

  if (hammingBits != 0 && parityAll == 0) {
    errorMsg = MSG_ERRO_MULT;
  } else if (hammingBits != 0) {
    quadro[hammingBits] = !quadro[hammingBits];
    errorMsg = MSG_ERRO_CORR;
  }

  std::vector<int> quadroOriginal;

  for (int i = 1; i < quadro.size(); i++) {
    if (i == delta) {
      delta <<= 1;
    } else {
      quadroOriginal.push_back(quadro[i]);
    }
  }

  quadro = quadroOriginal;

  return errorMsg;
}

std::vector<int> CamadaEnlaceReceptoraEnquadramentoContagem(std::vector<int> &quadroEnquadrado) {
  int sizeInBytes = 0;
  for (int i = 0; i < 8; i++) {
    sizeInBytes = (sizeInBytes << 1) | quadroEnquadrado[i];
  }

  std::vector<int> quadro;

  for (int i = 8; i < sizeInBytes * 8; i++) {
    quadro.push_back(quadroEnquadrado[i]);
  }

  return quadro;
}

std::vector<int> CamadaEnlaceReceptoraEnquadramentoInsercao(std::vector<int> &quadroEnquadrado) {
  std::vector<int> quadro;

  int byte = 0;
  bool previousWasEsc = false,
       insideFrame = false;

  for (int i = 0; i < quadroEnquadrado.size(); i++) {
    byte = (byte << 1) | quadroEnquadrado[i];
    if (i % 8 == 7) {
      if (previousWasEsc) {
        insertByteBitsIntoVector(byte, quadro);
        previousWasEsc = false;
      } else if (byte == ESC_BYTE) {
        previousWasEsc = true;
      } else if (byte == FLAG_BYTE) {
        insideFrame = !insideFrame;
      } else {
        insertByteBitsIntoVector(byte, quadro);
      }

      byte = 0;
    }
  }

  return quadro;
}

void somaBits(std::vector<int> &quadro, std::vector<int> resto) {
  int tamanhoResto = resto.size(),
      tamanhoQuadro = quadro.size();
  int resultado, carry = 0;

  for (int i = tamanhoResto - 1; i >= 0; i--) {
    resultado = carry + quadro[tamanhoQuadro - tamanhoResto + i] + resto[i];

    switch (resultado) {
      case 0:
        carry = 0;
        quadro[tamanhoQuadro - tamanhoResto + i] = 0;
        break;
      case 1:
        carry = 0;
        quadro[tamanhoQuadro - tamanhoResto + i] = 1;
        break;
      case 2:
        carry = 1;
        quadro[tamanhoQuadro - tamanhoResto + i] = 0;
        break;
      case 3:
        carry = 1;
        quadro[tamanhoQuadro - tamanhoResto + i] = 1;
        break;
    }
  }
}

uint64_t calcCRC(std::vector<int> &quadroPadded, std::vector<int> &generator) {
  uint64_t dividend = 0, divisor = 0;
  int i = 0;

  for (; i < generator.size(); i++) {
    divisor = divisor << 1 | (generator[i] == 1);
    dividend = dividend << 1 | (quadroPadded[i] == 1);
  }

  while (i < quadroPadded.size()) {
    while (i < quadroPadded.size() && dividend < divisor) {
      dividend = dividend << 1 | (quadroPadded[i] == 1);
      i++;
    }

    if (dividend >= divisor) {
      dividend = dividend ^ divisor;
    }
  }
  return dividend;
}