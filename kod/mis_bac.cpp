/*
// oblicz warstwy
// dla kazdej warstwy stworz graf
// i policz odpowiednie rzeczy


zeby policzyc wartwy wystarczy znac ich wiek
wierzcholki moga miec krawedzie tylko pomiedzy trzema warstwami warstw
wiec dla kazdego mozemy pamietac 3 vectory //0 - warstwy wczesniejszej, 1 - moja warstwa , 2 - warstwy nastepnej
przechodze przez embedding i sprawdzam co do czego nalezy

funkcja - policz max_ind_set dla warstw pomiedzy pocz, kon
*/
//stworz graf z grafu
//policz embedding i wiek vectora
// dodaj do grafu nastepujace krawedzie


#include "defines.hpp"

std::vector<vert_desc> V[MAX_SIZE][3]; //warstwy 
std::vector<vert_desc> lay[MAX_SIZE]; // dla kazdego wierzcholka policz warstwe

void create_graph(){
  std::vector<vert_desc>& emb = g.get_embedding();
  int* A = g.get_vert_age();
  int x;
  fori(i, 0, g.get_size()){
    lay[A[i]].push_back(i);
    int i_age = A[i];
    fori(j, 0, emb[i].size()){
      y;
      if(A[i] < A[y]){
        V[i][0].push_back(y);
      }
      if(A[i] == A[y])
        V[i][1].push_back(y);
      if(A[i] > A[y])
        V[i][2].push_back(y);
    }
  }
}

int main(){
  
return 0;
}
