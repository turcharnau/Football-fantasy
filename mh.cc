/* Metaurística : Helena Ferran i Arnau Turch */

#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <numeric>   // For iota()
#include <time.h>    // For time()
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

int cost_aux = 0;
int punts_aux = 0;
int punts = 0;
int cost = 0;

// variables auxiliars per saber quants jugadors de cada posicio hi ha fins al moment
int por_aux = 0;
int n1_aux = 0;
int n2_aux = 0;
int n3_aux = 0;

int mida; // nombre de jugadors que hi ha al vector players
vector<int>solu(mida); // solucio fins el moment
vector<jugador>players; // estructura de dades on es guarden els jugadors

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
        no formara part de la solucio i no s'afegeix al vector */
        if (J >= preu and T >= preu) {
            players.push_back({nom, posicio, preu, club, punts});
        }
    }
    in.close();
}

// escriu en el fitxer la solucio trobada
void save () {
    // ordena el vector solució per tal que coincideixi amb el format de sortida
    vector<jugador> sol_ord(11);
    int def=1;
    int mig=n1+1;
    int dav=n1+n2+1;
    for(int i = 0; i<mida; ++i){
        if(solu[i]){
            jugador p = players[i];
            if(p.posicio=="por") sol_ord[0]=p;
            if(p.posicio=="def") sol_ord[def++]=p;
            if(p.posicio=="mig") sol_ord[mig++]=p;
            if(p.posicio=="dav") sol_ord[dav++]=p;
        }
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
    myfile << "Punts: " << punts << "\n";
    myfile << "Preu: " << cost << "\n";
    myfile.close();
}

// ordena els jugadors segons la proporcio de punts per cost
bool ordena (jugador & a, jugador & b) {
    // si cap dels dos es gratuit, es posa primer el que tingui millor proporcio
    if(a.preu > 0 and b.preu > 0) {
        // s'utilitza el logaritme en el preu per tal de donar mes importancia als punts
        // es diferencia el cas on el preu es 1 per no dividir entre 0
        if(a.preu == 1 or b.preu == 1) return double(a.punts)/double(a.preu) > double(b.punts)/double(b.preu);
        else return double(a.punts)/log(a.preu) > double(b.punts)/log(b.preu);
    // si un dels dos es gratuit, es posa primer el que tingui mes punts
    } else return a.punts > b.punts;
}

// afegeix el jugador p a la solucio efectuant els canvis corresponents
void evalua (int & k, int & aux, vector<int> & s, jugador& p) {
    s[aux]=1; // escollim aquest jugador
    ++k;
    cost+=p.preu;
    punts+=p.punts;
}

// afegeix els 11 millors jugadors a la solucio tenint en compte les seves posicions
// k es el nombre de jugadors que te la solucio i aux es la posicio actual dins el vector jugadors
// retorna un vector de 0 i 1 on els jugadors afegits tenen 1
vector<int> generateInitialSolution(int k, int aux) {
    vector<int> s(mida, 0); // vector solucio
    while(k<11){
        jugador& p = players[aux];
        // si el cost del jugador no fa que s'excedeixi el pressupost, s'afegeix
        if (cost+p.preu<=T){
            // diferencia segons el rol del jugador per actualitzar el comptador auxiliar
            if(p.posicio=="por" and 0==por_aux++) evalua (k, aux, s, p);
            else if(p.posicio=="def" and n1>n1_aux++) evalua (k, aux, s, p);
            else if(p.posicio=="mig" and n2>n2_aux++) evalua (k, aux, s, p);
            else if(p.posicio=="dav" and n3>n3_aux++) evalua (k, aux, s, p);
        }
        ++aux;
    }
    return s;
}

// genera un nombre aleatori entre 0 i 1
int random(int l, int u) {
    return l + rand() % (u-l+1);
}

// canviem la solucio actual per una del seu veinat, es a dir,
// canviem un jugador del vector
vector<int> neighbour(const vector<int>& s) {
    vector<int> sprime = s;
    vector<int> sup; // vector on es guarden els indexs de la solucio actual
    for (int k = 0; k < mida; ++k) if (s[k]) sup.push_back(k);

    int i = sup[random(0, sup.size()-1)]; // jugador a reemplaçar

    jugador pi=players[i];
    bool found = false;
    // iterem fins que haguem pogut substituir el jugador
    for (int j = 0; j < mida and not found; ++j) {
        jugador pj=players[j];
        // substituim el jugador si te la mateixa posicio, no hi es a la solucio i no excedeix el cost
        if (pj.posicio==pi.posicio and not s[j] and cost - pi.preu + pj.preu <= T) {
            found = true;
            sprime[i] = 0;
            sprime[j] = 1;
            cost_aux = cost-pi.preu+pj.preu;
            punts_aux = punts-pi.punts+pj.punts;
        }
    }
    return sprime;
}

// probabilitat calculada a partir de la "Boltzmann distribution"
double p(double temp) {
    return exp(-(punts-punts_aux)/temp);
}

// simula el proces de la metalurgia (com mes alta es la temperatura mes moldeable es la solucio)
// modifiquem la solucio fins que la temperatura es massa baixa
void search() {
    solu = generateInitialSolution(0,0); //genera la solucio inicial (greedy)
    for(double temp=1000000000; temp>0.01; temp*=0.99){
        vector<int> s = neighbour(solu);
        // substituim la solucio en el cas que sigui millor o per una probabilitat que va lligada a la temperatura
        if(punts_aux > punts or (p(temp) >= ((double)rand()/(double)RAND_MAX))){
            solu=s;
            punts=punts_aux;
            cost=cost_aux;
        }
    }
    save();
}

int main(int argc, char** argv){
    int rs = time(NULL); // seed
    srand(rs);
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

    filename = argv[3]; // nom del fitxer solucio

    mida=players.size();

    sort(players.begin(), players.end(), ordena); // Ordenem per trobar la solucio inicial (greedy)
    search();
}
