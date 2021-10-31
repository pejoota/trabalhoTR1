#include "../header/CamadaEnlace.hpp"

#include <ncurses.h>

#include "../header/CamadaFisica.hpp"

#define CHAR_COUNTING 0
#define BYTE_INSERTION 1

#define BIT_PARITY 0
#define CRC 1

#define SEM_ERRO "SEM ERROS DETECTÁVEIS"
#define MSG_ERRO "ERRO DETECTADO NA MENSAGEM"
#define GERADOR_ERRO "ERRO DETECTADO NO POLINÔMIO GERADOR"

#define FLAG_BYTE 0xFF
#define ESC_BYTE 0xFE

std::vector<int> gerador = {1, 1, 0, 1};
int tipoDeEnquadramento = CHAR_COUNTING;  // alterar de acordo com o teste
int tipoDeControleErro = BIT_PARITY;

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
  int tamanhoQuadro = quadro.size();
  int tamanhoGerador = gerador.size();

  std::vector<int> atual, resto;

  if (tamanhoGerador > tamanhoQuadro || *gerador.begin() == 0 || *gerador.end() == 0)
    return;

  for (int i = 0; i < tamanhoGerador - 1; i++)
    atual.push_back(quadro[i]);

  for (int i = tamanhoGerador; i < tamanhoQuadro; i++) {
    atual.push_back(quadro[i]);

    if (atual[0] == 0) {
      for (int j = 0; j < tamanhoGerador; j++)
        resto[j] = 0;

    } else
      resto = gerador;

    for (int j = 0; j < tamanhoGerador; j++)
      atual[j] ^= resto[j];

    atual.erase(atual.begin());
  }

  somaBits(quadro, resto);
}

void CamadaEnlaceReceptora(std::vector<int> &quadro) {
  std::string msg = CamadaEnlaceReceptoraControleDeErro(quadro);

  CamadaEnlaceReceptoraEnquadramento(quadro);

  CamadaDeAplicacaoReceptora(quadro, msg);
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
  std::string msg;

  switch (tipoDeControleErro) {
    case BIT_PARITY:
      msg = CamadaEnlaceReceptoraControleDeErroBitParidade(quadro);
      break;

    case CRC:
      msg = CamadaEnlaceReceptoraControleDeErroCRC(quadro);
      break;
  }

  return msg;
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
  int tamanhoQuadro = quadro.size();
  int tamanhoGerador = gerador.size();

  std::vector<int> atual, resto;

  if (tamanhoGerador > tamanhoQuadro || *gerador.begin() == 0 || *gerador.end() == 0)
    return GERADOR_ERRO;

  for (int i = 0; i < tamanhoGerador - 1; i++)
    atual.push_back(quadro[i]);

  for (int i = tamanhoGerador; i < tamanhoQuadro; i++) {
    atual.push_back(quadro[i]);

    if (atual[0] == 0) {
      for (int j = 0; j < tamanhoGerador; j++)
        resto[j] = 0;

    } else
      resto = gerador;

    for (int j = 0; j < tamanhoGerador; j++)
      atual[j] ^= resto[j];

    atual.erase(atual.begin());
  }

  for (auto bit : resto) {
    if (bit) return MSG_ERRO;
  }

  return SEM_ERRO;
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