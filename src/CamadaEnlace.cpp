#include "../header/CamadaEnlace.hpp"

#include <ncurses.h>

#include "../header/CamadaFisica.hpp"

#define CHAR_COUNTING 0
#define BYTE_INSERTION 1

#define FLAG_BYTE 0xFF
#define ESC_BYTE 0xFE

int tipoDeEnquadramento = CHAR_COUNTING;  // alterar de acordo com o teste

void CamadaEnlaceTransmissora(std::vector<int> &quadro) {
  CamadaEnlaceTransmissoraEnquadramento(quadro);

  CamadaEnlaceTransmissoraControleDeErro(quadro);

  CamadaFisicaTransmissora(quadro);
}

void CamadaEnlaceTransmissoraEnquadramento(std::vector<int> &quadro) {
  switch (tipoDeEnquadramento) {
    case CHAR_COUNTING:
      quadro = CamadaEnlaceTransmissoraEnquadramentoContagem(quadro);
      break;
    case BYTE_INSERTION:
      quadro = CamadaEnlaceTransmissoraEnquadramentoInsercao(quadro);
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

void CamadaEnlaceTransmissoraControleDeErro(std::vector<int> &quadro) {}

void CamadaEnlaceReceptora(std::vector<int> &quadro) {
  CamadaEnlaceReceptoraControleDeErro(quadro);

  CamadaEnlaceReceptoraEnquadramento(quadro);

  CamadaDeAplicacaoReceptora(quadro);
}

void CamadaEnlaceReceptoraEnquadramento(std::vector<int> &quadro) {
  switch (tipoDeEnquadramento) {
    case CHAR_COUNTING:
      quadro = CamadaEnlaceReceptoraEnquadramentoContagem(quadro);
      break;
    case BYTE_INSERTION:
      quadro = CamadaEnlaceReceptoraEnquadramentoInsercao(quadro);
      break;
  }
}

void CamadaEnlaceReceptoraControleDeErro(std::vector<int> &quadro) {}

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