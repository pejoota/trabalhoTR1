#ifndef CAMADAENLACE_H_INCLUDED
#define CAMADAENLACE_H_INCLUDED

#include <vector>

void CamadaEnlaceTransmissora(std::vector<int>&);
void CamadaEnlaceTransmissoraEnquadramento(std::vector<int>&);
void CamadaEnlaceTransmissoraControleDeErro(std::vector<int>&);
std::vector<int> CamadaEnlaceTransmissoraEnquadramentoContagem(std::vector<int>&);
std::vector<int> CamadaEnlaceTransmissoraEnquadramentoInsercao(std::vector<int>&);

void CamadaEnlaceReceptora(std::vector<int>&);
void CamadaEnlaceReceptoraEnquadramento(std::vector<int>&);
void CamadaEnlaceReceptoraControleDeErro(std::vector<int>&);
std::vector<int> CamadaEnlaceReceptoraEnquadramentoContagem(std::vector<int>&);
std::vector<int> CamadaEnlaceReceptoraEnquadramentoInsercao(std::vector<int>&);

#endif  // CAMADAENLACE_H_INCLUDED