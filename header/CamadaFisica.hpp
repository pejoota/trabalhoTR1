#ifndef CAMADAFISICA_H_INCLUDED
#define CAMADAFISICA_H_INCLUDED

#include <iostream>
#include <vector>

void AplicacaoTransmissora(void);
void CamadaDeAplicacaoTransmissora(std::string);
void CamadaFisicaTransmissora(std::vector<int>);
std::vector<int> CamadaFisicaTransmissoraCodificacaoBinaria(std::vector<int>);
std::vector<int> CamadaFisicaTransmissoraCodificacaoManchester(std::vector<int>);
std::vector<int> CamadaFisicaTransmissoraCodificacaoBipolar(std::vector<int>);
void MeioDeComunicacao(std::vector<int>);
void CamadaFisicaReceptora(std::vector<int>);
std::vector<int> CamadaFisicaReceptoraCodificacaoBinaria(std::vector<int>);
std::vector<int> CamadaFisicaReceptoraCodificacaoManchester(std::vector<int>);
std::vector<int> CamadaFisicaReceptoraCodificacaoBipolar(std::vector<int>);
void CamadaDeAplicacaoReceptora(std::vector<int>);
void AplicacaoReceptora(std::string);

#endif
