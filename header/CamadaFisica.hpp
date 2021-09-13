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
std::vector<int> CamadaFisicaReceptoraDecodificacaoBinaria(std::vector<int>);
std::vector<int> CamadaFisicaReceptoraDecodificacaoManchester(std::vector<int>);
std::vector<int> CamadaFisicaReceptoraDecodificacaoBipolar(std::vector<int>);
void CamadaDeAplicacaoReceptora(std::vector<int>);
void AplicacaoReceptora(std::string);

#endif
