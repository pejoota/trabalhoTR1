#ifndef CAMADAFISICA_H_INCLUDED
#define CAMADAFISICA_H_INCLUDED

#include <ncurses.h>
#include <signal.h>

#include <iostream>
#include <random>
#include <vector>

#define BINARIA 0
#define MANCHESTER 1
#define BIPOLAR 2

extern FILE *logFile;

extern int tipoDeCodificacao;

extern int maxErrors;

void AplicacaoTransmissora();
void CamadaDeAplicacaoTransmissora(std::string);
void CamadaFisicaTransmissora(std::vector<int>);
std::vector<int> CamadaFisicaTransmissoraCodificacaoBinaria(std::vector<int>);
std::vector<int> CamadaFisicaTransmissoraCodificacaoManchester(std::vector<int>);
std::vector<int> CamadaFisicaTransmissoraCodificacaoBipolar(std::vector<int>);
void MeioDeComunicacao(std::vector<int>);
void CamadaFisicaReceptora(std::vector<int>);
std::vector<int> CamadaFisicaReceptoraDecodificacaoBinaria(std::vector<int>);
std::vector<int> CamadaFisicaReceptoraDecodificacaoManchester(std::vector<int>);
std::vector<int> CamadaFisicaReceptoraDecodificacaoBipolar(std::vector<int>);
void CamadaDeAplicacaoReceptora(std::vector<int>, std::string);
void AplicacaoReceptora(std::string, std::string);

void resizeHandler(int);

void interface(std::vector<int>, std::vector<int>);

std::vector<int> decodeBits(std::vector<int> &signal);
std::string binToMessage(std::vector<int> &bits);

std::string getstring();

#endif
