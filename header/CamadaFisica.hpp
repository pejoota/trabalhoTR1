#ifndef CAMADAFISICA_H_INCLUDED
#define CAMADAFISICA_H_INCLUDED

#include <iostream>

void AplicacaoTransmissora(void);
void CamadaDeAplicacaoTransmissora(std::string);
void CamadaFisicaTransmissora(int[]);
int[] CamadaFisicaTransmissoraCodificacaoBinaria(std::vector<int>);
int[] CamadaFisicaTransmissoraCodificacaoManchester(int[]);
int[] CamadaFisicaTransmissoraCodificacaoBipolar(int[]);
void MeioDeComunicacao(int[]);
void CamadaFisicaReceptora(int[]);
int[] CamadaFisicaReceptoraCodificacaoBinaria(int[]);
int[] CamadaFisicaReceptoraCodificacaoManchester(int[]);
int[] CamadaFisicaReceptoraCodificacaoBipolar(int[]);
void CamadaDeAplicacaoReceptora(int[]);
void AplicacaoReceptora(std::string);

#endif
