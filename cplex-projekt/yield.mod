// Yield Management - example from Wiley Book

{string} Klasy = ...;

// Parametry wejœciowe (wczytywane z pliku .dat)
float CurrentTable[1..3][1..3][Klasy] = ...;  // Macierz cen biletów w ka¿dym okresie i opcji
float Scenerios[1..3] = ...;  // Prawdopodobieñstwa scenariuszy
int ForecastDemand[1..9][1..3][Klasy] = ...;  // Prognozowany popyt
int ActualDemand[1..3][1..3][Klasy] = ...;  // Rzeczywisty popyt

// Liczba dostêpnych miejsc w ka¿dej klasie
int Capacity[Klasy] = [47, 38, 37];  // Przyk³adowe wartoœci

// Zmienne decyzyjne
dvar int+ x[1..3][Klasy];  // Liczba biletów sprzedanych w ka¿dej klasie w okresach
dvar int+ y[1..3][Klasy];  // Liczba biletów niesprzedanych
dvar float+ revenue;

maximize 
    sum( p in 1..3, o in 1..3, i in Klasy ) 
        ( Scenerios[o] * CurrentTable[p][o][i] * x[p][i] );

subject to {
    // Ograniczenie liczby dostêpnych miejsc
    forall(p in 1..3) sum(i in Klasy) x[p][i] <= sum(i in Klasy) Capacity[i];

    // Ca³kowity popyt podzielony na bilety sprzedane i niesprzedane
    forall(p in 1..3, i in Klasy)
        x[p][i] + y[p][i] == ForecastDemand[p][1][i];

    // Przenoszenie miejsc miêdzy kategoriami (do 10%)
    forall(p in 1..3, i in Klasy, j in Klasy)
        x[p][i] <= 1.1 * x[p][j];
}
