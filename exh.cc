/* Cerca exhaustiva: Helena Ferran i Arnau Turch */

#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include <ctime>
#include <algorithm>
using namespace std;

/**************************** VARIABLES GLOBALS *********************************************************************************/
string filename; // nom del fitxer on s'haura d'escriure la solucio
double t1; // instant inici

struct jugador {
  string nom;
  string posicio;
  int preu;
  string club;
  int punts;
};

int n1,n2,n3,T,J; // variables introduides (numero de defenses, migcampistes, davanters i el preu maxim total i per cada jugador)
int max_punts = -1;// max de punts fins el moment

// variables auxiliars per saber quants jugadors de cada posicio hi ha fins al moment
int por_aux = 0;
int n1_aux = 0;
int n2_aux = 0;
int n3_aux = 0;

vector<jugador>solu(11); // solucio fins el moment
vector<jugador>players; // estructura de dades on es guarden els jugadors
int mida; // nombre de jugadors que hi ha al vector players

/*************************************************************************************************************************************/

// retorna el temps actual
double now() {
    return clock() / double(CLOCKS_PER_SEC);
}

// llegeix el document database i guarda les dades en el vector d'estructures players.
void read(string database) {
  ifstream in(database);
  if (in.fail()){
      cout << "Error: No s'ha trobat l'arxiu " << "'" << database << "'." << endl;
      exit(1);
  }
  while (not in.eof()) {
    string nom, posicio, club;
    int punts, preu;
    getline(in,nom,';');    if (nom == "") break;
    getline(in,posicio,';');
    in >> preu;
    char aux; in >> aux;
    getline(in,club,';');
    in >> punts;
    string aux2;
    getline(in,aux2);
    /* si el preu d'un jugador es mes gran que J o que T (si en algun cas J > T),
    no formara part de la solucio i nos'afegeix al vector */
    if (J >= preu and T >= preu) players.push_back({nom, posicio, preu, club, punts});
  }
  in.close();
}

// escriu en el fitxer la solucio trobada
void save (int cost) {
    // ordena el vector solució per tal que coincideixi amb el format de sortida
    vector<jugador> sol_ord(11);
    int def=1;
    int mig=n1+1;
    int dav=n1+n2+1;
    for(auto & p : solu){
      if(p.posicio=="por") sol_ord[0]=p;
      if(p.posicio=="def") sol_ord[def++]=p;
      if(p.posicio=="mig") sol_ord[mig++]=p;
      if(p.posicio=="dav") sol_ord[dav++]=p;
    }

    // guarda la solucio al fitxer
    ofstream myfile;
    myfile.setf(ios::fixed);
    myfile.precision(1);
    myfile.open (filename);
    double t2 = now(); //instant fi
    myfile << t2 - t1 << "\n";
    myfile << "POR: " << sol_ord[0].nom << "\n";
    myfile << "DEF: ";
    for (int i = 1; i <= n1; ++i) myfile << sol_ord[i].nom << (i==n1 ? "\n" : ";");
    myfile << "MIG: ";
    for (int i = n1+1; i <= n1+n2; ++i) myfile << sol_ord[i].nom << (i==n1+n2 ? "\n" : ";");
    myfile << "DAV: ";
    for (int i = n1+n2+1; i <= 10; ++i) myfile << sol_ord[i].nom << (i==10 ? "\n" : ";");
    myfile << "Punts: " << max_punts << "\n";
    myfile << "Preu: " << cost << "\n";
    myfile.close();
}

bool ordena (jugador & a, jugador & b) {
  return a.punts > b.punts;
}

/* genera combinacions de jugadors
  te com a parametres:
  - nombre de jugadors (k), punts i cost de la solucio actual fins al moment
  - aux -> posicio del vector per tal de seguir des del jugador on s'havia quedat i no repetir permutacions */
void search(int k, int punts, int cost, int aux) {
  // si la solucio te els 11 jugadors (i es millor que l'anterior), l'escriu
  if (k==11) {
    if (punts > max_punts) {
      max_punts = punts;
      save(cost);
    }
  }
  for(;aux<mida;++aux){
  jugador& p = players[aux];
    /* s'atura si no podrem superar la puntuacio maxima trobada fins ara.
    (com que estan ordenats per punts, mai s'obtindran mes que els punts del
      jugador actual multiplicat pels jugadors que li falten a la solucio) */
    if (punts + p.punts*(11-k) <= max_punts) aux=mida; // es viola la condicio del bucle perque acabi
    else {
      // segueix si el cost del jugador no fa que s'excedeixi el pressupost
      if (cost+p.preu<=T) {
        // afegeix el jugador a la solucio
        // diferencia segons el rol del jugador per actualitzar el comptador auxiliar
        if(p.posicio=="por" and por_aux==0){
          solu[k] = p;
          por_aux=1;
          search(k+1, punts+p.punts, cost+p.preu, aux+1);
          por_aux=0;
        }
        else if(p.posicio=="def" and n1_aux<n1){
          solu[k] = p;
          ++n1_aux;
          search(k+1, punts+p.punts, cost+p.preu, aux+1);
          --n1_aux;
        }
        else if(p.posicio=="mig" and n2_aux<n2){
          solu[k] = p;
          ++n2_aux;
          search(k+1, punts+p.punts, cost+p.preu, aux+1);
          --n2_aux;
        }
        else if(p.posicio=="dav" and n3_aux<n3){
          solu[k] = p;
          ++n3_aux;
          search(k+1, punts+p.punts, cost+p.preu, aux+1);
          --n3_aux;
} } } } }

int main(int argc, char** argv){
    t1 = now(); // instant inici
    // l'entrada ha de tenir 4 arguments
    if (argc != 4) {
      cout << "Syntax: " << argv[0] << " data_base.txt" << " query.txt" << " solution.txt." <<endl;
      exit(1);
    }
    // llegeix els requisits de la solucio
    ifstream in(argv[2]);
      if (in.fail()){
          cout << "Error: No s'ha trobat l'arxiu " << "'" << argv[2] << "'." << endl;
          exit(1);
      }
      in >> n1 >> n2 >> n3 >> T >> J;
    in.close();
    // llegeix la informacio de la base de dades
    read(argv[1]);
    // fitxer solucio
    filename = argv[3];

    mida=players.size();
    sort(players.begin(), players.end(), ordena); // ordena segons la puntuacio
    search(0,0,0,0);
}
