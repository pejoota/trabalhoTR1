#ifndef CAMADAENLACE_H_INCLUDED
#define CAMADAENLACE_H_INCLUDED

#include <vector>
#include <iostream>

void CamadaEnlaceTransmissora(std::vector<int>&);
void CamadaEnlaceTransmissoraEnquadramento(std::vector<int>&);
void CamadaEnlaceTransmissoraControleDeErro(std::vector<int>&);
void CamadaEnlaceTransmissoraControleDeErroBitParidade(std::vector<int>&);
void CamadaEnlaceTransmissoraControleDeErroCRC(std::vector<int>&);
std::vector<int> CamadaEnlaceTransmissoraEnquadramentoContagem(std::vector<int>&);
std::vector<int> CamadaEnlaceTransmissoraEnquadramentoInsercao(std::vector<int>&);

void CamadaEnlaceReceptora(std::vector<int>&);
void CamadaEnlaceReceptoraEnquadramento(std::vector<int>&);
std::string CamadaEnlaceReceptoraControleDeErro(std::vector<int>&);
std::string CamadaEnlaceReceptoraControleDeErroBitParidade(std::vector<int>&);
std::string CamadaEnlaceReceptoraControleDeErroCRC(std::vector<int>&);
std::vector<int> CamadaEnlaceReceptoraEnquadramentoContagem(std::vector<int>&);
std::vector<int> CamadaEnlaceReceptoraEnquadramentoInsercao(std::vector<int>&);

void insertByteBitsIntoVector(int, std::vector<int>&);
void somaBits(std::vector<int>&, std::vector<int>)

#endif  // CAMADAENLACE_H_INCLUDED