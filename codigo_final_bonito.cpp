#include <ctime>
#include <iostream>
#include <limits>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;

namespace Color {
enum Code { ROJO = 31, AZUL = 34,DF = 39};
std::ostream &operator<<(std::ostream &os, Code code) {
  return os << "\033[" << static_cast<int>(code) << "m";
}
}
#define ROJO Color::ROJO
#define AZUL Color::AZUL
#define DF Color::DF


template <typename T> struct Grafo {
    unordered_map<T, unordered_set<T>> g_map;
    unordered_set<T> visitados;
    unordered_map<T, unordered_map<T, int>> weights;
    void nueva_arista(T v1, T v2, int weight);
};
template <typename T> void Grafo<T>::nueva_arista(T v1, T v2, int weight) {
    g_map[v1].insert(v2);
    g_map[v2].insert(v1);
    weights[v1][v2] = weight;
    weights[v2][v1] = weight;
}
class Nodo {
private:
    int x;
    int y;

public:
    char valor = '-';
    Nodo(int _x, int _y) {
        x = _x;
        y = _y;
    }
    void set_valor(char _valor) { valor = _valor; }
    char get_valor() { return valor; }
    int get_x() { return x; }
    int get_y() { return y; }
};
class Laberinto {
private:
    vector<vector<Nodo *>> laberinto;
    // su version en arbol
    Grafo<Nodo *> arbol;

public:
    Laberinto(int m, int n) {
        laberinto.resize(m, vector<Nodo *>(n, nullptr));
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                Nodo *nodito = new Nodo(i, j);
                laberinto[i][j] = nodito;
            }
        }
    }
    vector<vector<Nodo *>> get_lab() { return laberinto; }
    void set_obstaculos() {
        srand((unsigned int)time(NULL));
        for (int i = 0; i < laberinto.size(); i++) {
            int columna1;
            int columna2;
            int cant_obs = 0;
            do {
                columna1 = rand() % laberinto[0].size();
                columna2 = rand() % laberinto[0].size();
                if (laberinto[i][columna1]->get_valor() == '-') {
                    laberinto[i][columna1]->set_valor('X');
                }
                cant_obs++;
            } while (cant_obs < 2);
        }
    }
    void print_mylaberinto() {
        system("clear");
        for (int i = 0; i < laberinto.size(); i++) {
            for (int j = 0; j < laberinto[0].size(); j++) {
                if(laberinto[i][j]->get_valor() == 'X'){
                    cout <<ROJO<< laberinto[i][j]->get_valor() << " ";
                }
                else{
                    cout <<AZUL<< laberinto[i][j]->get_valor() << " ";
                }
                
            }
            cout << endl;
        }
    }
    void arbolizar_nodos() {
        for (int i = 0; i < laberinto.size() - 1; i++) {
            for (int j = 0; j < laberinto[0].size() - 1; j++) {
                arbol.nueva_arista(laberinto[i][j], laberinto[i][j + 1], 1);
                arbol.nueva_arista(laberinto[i][j], laberinto[i + 1][j], 1);
            }
        }
    }
    bool dfs(Nodo *nodo, Nodo *end) {
        nodo->set_valor('*');
        print_mylaberinto();
        cout << endl;
        if (nodo == end)
            return true;
        // Recur for all the vertices adjacent to this vertex
        for (auto i = arbol.g_map[nodo].begin(); i != arbol.g_map[nodo].end();
             ++i) {
            if ((*i)->get_valor() != 'X' && (*i)->get_valor() != '*') {
                if (dfs(*i, end))
                    return true;
            }
        }
        return false;
    }
    bool bfs(Nodo *start, Nodo *end) {
        queue<Nodo *> q;
        unordered_set<Nodo *> visited;

        q.push(start);
        visited.insert(start);
        start->set_valor('*');

        while (!q.empty()) {
            Nodo *current = q.front();
            q.pop();

            if (current == end) // encuentra el camino
                return true;

            for (auto neighbor : arbol.g_map[current]) {
                if (neighbor->get_valor() != 'X' &&
                    visited.find(neighbor) == visited.end()) {
                    q.push(neighbor);
                    visited.insert(neighbor);
                    neighbor->set_valor('*');
                }
            }
        }

        return false;
    }

    void dijkstra(Nodo *start, Nodo *end) {
        unordered_map<Nodo *, int> dist;
        unordered_map<Nodo *, Nodo *> prev;
        priority_queue<pair<int, Nodo *>, vector<pair<int, Nodo *>>,
                greater<pair<int, Nodo *>>>
                pq;
        for (int i = 0; i < laberinto.size(); i++) {
            for (int j = 0; j < laberinto[0].size(); j++) {
                dist[laberinto[i][j]] = numeric_limits<int>::max();
                prev[laberinto[i][j]] = nullptr;
            }
        }
        dist[start] = 0;
        pq.push({0, start});
        while (!pq.empty()) {
            Nodo *u = pq.top().second;
            pq.pop();
            for (auto v : arbol.g_map[u]) {
                int alt = dist[u] + arbol.weights[u][v];
                if (alt < dist[v]) {
                    dist[v] = alt;
                    prev[v] = u;
                    pq.push({alt, v});
                }
            }
        }
        Nodo *curr = end;
        while (curr != nullptr) {
            curr->set_valor('*');
            curr = prev[curr];
        }
    }
};
    class LaberintoFactory {
    public:
        virtual Laberinto* crearLaberinto(int m, int n) = 0;
    };

    class LaberintoFactoryConcreta : public LaberintoFactory {
    public:
        Laberinto* crearLaberinto(int m, int n) override {
            return new Laberinto(m, n);
        }
    };

void menu(Laberinto lab) {
    cout << "-------BIENVENIDO AL LABERINTO LABERINTOSO-----------" << endl;
    cout << "Elija una opcion para comenzar a jugar" << endl;
    cout << "1. DFS" << endl;
    cout << "2. BFS" << endl;
    cout << "3. Dijkstra" << endl;
    int opcion = 0;
    cout << "Opcion: ";
    cin >> opcion;
    cout << endl;
    int X1, X2, Y1, Y2;
    if (opcion == 1) {
        cout << "Elija su punto de partida X: ";
        cin >> X1;
        cout << "Elija su punto de partida Y: ";
        cin >> Y1;
        cout << "Elija su punto de llegada X: ";
        cin >> X2;
        cout << "Elija su punto de llegada Y: ";
        cin >> Y2;

        cout << "after" << endl;
        if (lab.dfs(lab.get_lab()[X1][Y1], lab.get_lab()[X2][Y2])) {
            lab.get_lab()[X2][Y2]->set_valor('L');
            lab.print_mylaberinto();
        } else {
            cout << "No se ha logrado encontrar un camino"<< endl;
        }
    }

    if (opcion == 2) {
        cout << "Elija su punto de partida X: ";
        cin >> X1;
        cout << "Elija su punto de partida Y: ";
        cin >> Y1;
        cout << "Elija su punto de llegada X: ";
        cin >> X2;
        cout << "Elija su punto de llegada Y: ";
        cin >> Y2;

        cout << "after" << endl;
        if (lab.bfs(lab.get_lab()[X1][Y1], lab.get_lab()[X2][Y2])) {
            lab.get_lab()[X2][Y2]->set_valor('L');
            lab.print_mylaberinto();
        } else {
            cout << "No hay caminito pipipi." << endl;
        }
    }
    if (opcion == 3) {
        cout << "Elija su punto de partida X: ";
        cin >> X1;
        cout << "Elija su punto de partida Y: ";
        cin >> Y1;
        cout << "Elija su punto de llegada X: ";
        cin >> X2;
        cout << "Elija su punto de llegada Y: ";
        cin >> Y2;

        cout << "after" << endl;
        lab.dijkstra(lab.get_lab()[X1][Y1], lab.get_lab()[X2][Y2]);
        lab.get_lab()[X2][Y2]->set_valor('L');
        lab.print_mylaberinto();
    }
};

int main() {
    Laberinto lab(15, 15);
    lab.set_obstaculos();
    lab.print_mylaberinto();
    lab.arbolizar_nodos();
    cout << "Después" << endl;
    menu(lab);
    /* PARA CREAR LABERINTOS CON FACTORY
    LaberintoFactoryConcreta factory;
    Laberinto* lab1 = factory.crearLaberinto(15, 15);
    Laberinto* lab2 = factory.crearLaberinto(10, 10);

    lab1->set_obstaculos();
    lab2->set_obstaculos();
    lab1->print_mylaberinto();
    cout << endl;
    lab2->print_mylaberinto();
    cout << endl;
    delete lab1;
    delete lab2;
    
    
    */

   return 0;
}
