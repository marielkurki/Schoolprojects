#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double tiedostoKasittely();                 //funktioden esittely
double saldoToiminto(double saldo);
int ottoToiminto(double *saldo);

int main(void) {                            //main funktio, jossa menu valikko
    char valinta;

    double saldo = tiedostoKasittely();     //haetaan saldo tieto tiedostonkäsittely funktiosta

    if (saldo >= 0.0) {                     //tarkistetaan onko saldoa
        while (1) {
            printf("Valitse toiminto: Otto (o)\nSaldo (s)\nLopeta (l) > ");
            scanf(" %c", &valinta);         //talletetaan käyttäjän syöttämä kirjain, jota käytetään toiminnon valinnassa
            getchar();                      //getchar funktiota pitkin koodia pelastamaan syötteet

            if (valinta == 'o') {           //mennään toimintoon riippuen syötetystä kirjaimesta
                ottoToiminto(&saldo);
            } else if (valinta == 's') {
                saldoToiminto(saldo);
            } else if (valinta == 'l') {
                printf("Kiitos kaynnista\ntervetuloa uudelleen ");
                break;
            } else {                        //Pidetään huolta, että käyttäjä syöttää varmasti jonkin menun vaihtoehdoista
                printf("Tallainen toiminto ei saatavilla. ");
            }

            getchar();
        }
    }

    return 0;
}

double tiedostoKasittely() {                //funktio tiedoston, avaamiseen, pinkoodin löytämiseen ja saldon tallettamiseen tiedostosta
    int syotettyPin;
    char tiedostoNimi[] = "12345.acc";
    FILE *tiedosto;

    while (1) {
        printf("Syota PIN-koodi: ");
        scanf("%d", &syotettyPin);
        getchar();

        if ((tiedosto = fopen(tiedostoNimi, "r")) == NULL) {        //mikäli tiedostoa ei löydy, kerrotaan avaamisen epäonnistuneen
            printf("Tiedoston avaaminen epäonnistui.\n");
            return -1.0;
        }

        char tiedostonPin[20];
        if (fgets(tiedostonPin, sizeof(tiedostonPin), tiedosto) != NULL) {  //jos tiedostosta ei löydetä oikeanlaista dataa, lukemine epäonnistuu
            tiedostonPin[strcspn(tiedostonPin, "\n")] = '\0';
        } else {
            printf("Tiedoston lukeminen epäonnistui.\n");
            fclose(tiedosto);
            return -1.0;
        }

        if (syotettyPin == atoi(tiedostonPin)) {                    //kun tiedosto luettu, suljetaan se heti käytön jälkeen
            fclose(tiedosto);

            double saldo;
            if ((tiedosto = fopen(tiedostoNimi, "r")) == NULL) {    //avataan teidosto mikäli pin löytynyt ja etsitään saldoa, mikäli tiedostoa ei löydy, kerrotaan vaamisen epäonnistuneen
                printf("Tiedoston avaaminen epäonnistui.\n");
                return -1.0;
            }
            if (fscanf(tiedosto, "%*s\n%lf", &saldo) != 1) {        //mikäli saldoon sopivaa dataa ei löydy kerrotaan sen epäonnistuneen
                printf("Saldon lukeminen epäonnistui.\n");
                fclose(tiedosto);
                return -1.0;
            }

            fclose(tiedosto);                                       //suljetaan tiedosto ja tuodaan saldo käytettäväksi koodiin
            return saldo;
        } else {
            printf("Väärä PIN-koodi. Yritä uudelleen.");
        }

        fclose(tiedosto);
    }
}

double saldoToiminto(double saldo) {                                //tarkastetaan tilin saldo saldotoiminnossa, huomioidaan sen muokkaantuminen ottofunktiossa
    printf("Tilin saldo %.2f\n", saldo);
    return saldo;
}

int ottoToiminto(double *saldo) {                                   //otto toiminto, käytetään tiedostosta löytynyttä saldo arvoa
    double nostettumaara;                                           //arvojen määrittely
    char buffer[100];
    int result;

    do {
        printf("Syota haluttu summa > ");
        result = scanf("%lf", &nostettumaara);

        if (result != 1) {                                          // Syötteenluku epäonnistui, käsitellään virhe
            printf("Virheellinen syote. Anna numeroarvo.\n");
            getchar();

            fgets(buffer, sizeof(buffer), stdin);                   // Tyhjennä syötepuskuri, jotta seuraavat syötteet eivät ole ongelmallisia
        } else if (nostettumaara < 20) {
            printf("Virheellinen syote. Yritä uudelleen.");         //huomioidaan, että nostetun summan täytyy olla vähintään 20
        } else if ((int)nostettumaara % 10 != 0 || ((int)nostettumaara % 20 != 0 && (int)nostettumaara % 50 != 0)) {        //ehdot, jotta nsoto tapahtuu kympin väleillä 50 eurosta eteenpäin
            printf("Nostosumman tulee olla 20, 50 tai kympin moninkertainen. Yritä uudelleen.");
        } else if (*saldo - nostettumaara < 0) {                                                                            //tarkastetaan onko tilillä rahaa
            printf("Nostosumma ylittää tilillä olevan summan. Yritä uudelleen.");
        } else if (nostettumaara > 1000) {                                                                                  //rajoitetaan nosto tuhanteen
            printf("Nostoraja on 1000 euroa. Yritä uudelleen.");
        } else {
            printf("Nostetaan %.2lf", nostettumaara);

            int maara20 = 0;                                       //määritellään muuttujat sille, että kerrotaan montako seteliä mihinkin vaaditaan
            int maara50 = 0;
            int jaannos = (int)nostettumaara;

            while (jaannos >= 50) {                                 //jos summastaa saa 50, lisätään yksi ja poistetaan jäännöksestä otettu 50                                 
                maara50++;
                jaannos -= 50;
            }

            while (jaannos >= 20) {                                 //sama 20 toteutettuna
                maara20++;
                jaannos -= 20;
            }

            if (maara50 > 0) {                                      //tulostetaan 50 setelien summa, sen pohjalt amontako saatu
                printf(". %d kpl 50€", maara50);
            }

            if (maara20 > 0) {                                      //sama 20 seteleille, huomioidaan myös se että jos on kumpiakin 50 ja 20 euron seteilä tulosteaan ja asna väliin
                if (maara50 > 0) {
                    printf(" ja ");
                }
                printf("%d kpl 20€", maara20);                      
            }

            printf(" seteliä.");

            *saldo -= nostettumaara;                                 //pidetään huolta, että saldo muuttuu nostojen myötä
            break;
        }
    } while (1);

    getchar();
    return 0;
}
