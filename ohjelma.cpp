#include <iostream>
#include <vector>
#include <string>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <pthread.h>
#include <cstring> // memcpy
#include <sys/ipc.h> // ftok
#include <sys/sem.h> // semget, semop, semctl
#include <ctime> // nanosleep
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <cstdlib>  // rand, srand
using namespace std;

/* VALIKKO */
/*
    Kaikki muutetut osiot on merkattu tehtävä numerolla! Voit löytää
    tehtävän hakemalla koodista joko "TEHTÄVÄ X" tai "_tehtx"
    (korvaa x haluamallasi numerolla). Toivottavasti tämä on tarpeeksi
    selkeä tapa erottaa muutokset koodista :))

    GitHub repon README-tiedostossa on kuvakaappaukset jokaisen
    tehtävän tärkeimmistä muutoksista! Kaikki tehtävät ovat myös
    erikseen saatavilla commit hirtoriasta
*/
int main_teht1();
int main_teht2();
int main_teht3();
int main_teht4();
int main_teht5();
int main_teht6();
int main_teht7();
int main_teht8();

/* TEHTÄVÄ 6 */
// Säikeiden suspend toiminnallisuus ilman signaaleja
std::mutex mtx;
std::condition_variable cv;
std::atomic<bool> paused{false};

#define KORKEUS 100
#define LEVEYS 100
/*#define KORKEUS 7
#define LEVEYS 7*/

#define ROTAT 3  // Rottien määrä

/* TEHTÄVÄ 3 */
int semid_teht3;

/* TEHTÄVÄ 4 */
/* TEHTÄVÄ 6 */
pthread_mutex_t labMutex = PTHREAD_MUTEX_INITIALIZER; // Labyrintin lukko
pthread_mutex_t tulostusLukko = PTHREAD_MUTEX_INITIALIZER;

/* TEHTÄVÄ 7 */
/* TEHTÄVÄ 8 */
#define BUF_SIZE 5  // Rajattu rengaspuskuri

/* TEHTÄVÄ 7 */
int semid_teht7;

int (*labyrintti)[LEVEYS]; // Pointteri labyrinttiin

/*int alkuLabyrintti[KORKEUS][LEVEYS] = {
                        {1,1,1,1,1,1,1},
                        {1,0,1,0,1,0,4},
                        {1,0,1,0,1,0,1},
                        {1,2,0,2,0,2,1},
                        {1,0,1,0,1,0,1},
                        {1,0,1,0,1,0,1},
                        {1,1,1,3,1,1,1}};*/

int alkuLabyrintti[KORKEUS][LEVEYS] = {
    {1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,2,0,2,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,2,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,2,0,0,1,0,0,0,1,0,0,2,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,1},
    {1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,1},
    {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,1,2,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,2,0,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,2,0,0,0,0,0,0,1,0,0,0,0,2,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1,1},
    {1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1},
    {1,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,2,0,0,1,0,0,0,1,2,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,2,0,0,1,1},
    {1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,1},
    {1,0,0,0,1,0,1,2,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,2,0,0,1,0,0,2,0,0,1,0,0,2,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,2,0,2,0,0,0,0,1,0,1,0,0,2,0,0,0,0,1,0,0,0,1,1},
    {1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1},
    {1,0,1,0,0,0,1,0,1,0,0,2,0,0,0,0,1,0,0,0,0,0,0,2,0,2,0,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,2,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,2,0,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,2,0,0,1,0,1,0,0,0,1,1},
    {1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1},
    {1,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,2,0,0,0,0,0,0,0,2,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,2,0,0,1,0,0,0,1,0,0,2,1,0,0,0,1,0,0,2,0,0,1,0,0,0,1,0,0,0,1,1},
    {1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1},
    {1,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,2,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,2,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,2,1,0,0,0,1,0,0,2,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,1},
    {1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,1},
    {1,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,2,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,2,1,0,0,0,1,0,0,0,1,0,0,2,1,0,0,2,0,2,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,1},
    {1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,1},
    {1,0,1,0,0,2,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,0,2,0,0,1,2,0,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0,2,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,1,1},
    {1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1},
    {1,2,0,0,1,0,0,0,0,0,1,2,0,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,0,2,0,0,1,0,0,0,1,0,0,2,1,0,0,0,1,0,1,0,1,0,0,0,0,0,1,2,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,2,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,1,1},
    {1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1},
    {1,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,2,0,0,1,0,1,0,0,0,0,0,0,0,1,0,0,2,1,2,0,0,0,0,1,0,1,0,1,1},
    {1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,1},
    {1,0,1,0,1,0,1,0,0,0,0,0,0,2,1,0,1,0,0,2,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,2,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0,2,0,0,1,1},
    {1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1},
    {1,2,0,0,1,0,0,0,0,2,0,0,0,0,1,0,0,2,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,2,0,0,1,0,0,2,0,0,1,0,0,0,1,0,0,2,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,2,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,2,0,0,1,0,0,0,0,2,1,2,0,0,0,0,0,0,1,1},
    {1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1},
    {1,0,1,0,0,2,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,2,1,0,0,0,1,0,0,0,1,0,0,2,0,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,1,1},
    {1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1},
    {1,0,1,0,0,0,1,0,1,0,0,2,0,0,1,0,0,0,1,2,0,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0,2,0,0,1,0,1,0,1,0,0,0,0,2,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,2,0,0,1,0,0,0,0,2,1,0,1,0,0,0,1,2,0,0,1,1},
    {1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1},
    {1,0,0,0,1,0,1,0,1,0,0,2,0,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,2,0,0,1,0,0,2,1,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0,2,1,0,1,0,0,2,1,0,0,0,1,1},
    {1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1},
    {1,0,0,0,1,2,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,0,2,0,0,0,0,0,0,1,0,0,2,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,1},
    {1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1},
    {1,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,0,2,0,0,0,2,0,0,1,0,0,0,1,0,0,0,0,2,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,2,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,1},
    {1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1},
    {1,0,1,0,0,0,1,0,0,0,0,2,0,0,1,0,0,0,1,0,0,2,1,0,1,0,0,0,0,2,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,2,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,1,0,1,2,0,0,1,0,0,2,0,0,1,0,0,0,1,1},
    {1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1},
    {1,2,0,0,1,0,0,0,0,0,1,0,0,0,0,2,0,0,1,0,0,0,1,2,0,0,0,0,1,0,0,0,0,0,0,0,0,2,0,0,1,0,1,0,1,2,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1,2,0,0,1,0,0,0,1,1},
    {1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1},
    {1,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,2,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,2,0,0,1,0,0,2,0,0,1,0,0,2,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,1},
    {1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,1},
    {1,0,1,0,0,2,1,2,0,0,1,0,1,0,0,0,1,0,0,2,0,0,1,0,0,0,1,0,1,0,0,2,0,0,0,0,1,0,1,2,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,2,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,2,0,0,1,0,0,0,0,0,1,0,1,0,1,1},
    {1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,1},
    {1,0,0,0,1,0,1,0,1,0,0,2,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,2,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,2,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,2,1,1},
    {1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,1},
    {1,0,0,0,0,2,1,0,0,0,1,0,0,0,1,0,0,2,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,2,1,0,1,0,0,0,1,0,0,0,1,0,1,2,0,0,1,0,0,0,1,0,0,0,1,2,0,0,1,0,0,0,1,0,1,0,1,0,0,2,0,0,0,2,1,0,1,2,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,1},
    {1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,1},
    {1,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,1,2,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,0,2,1,0,0,0,0,0,1,0,1,2,0,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,2,0,0,1,0,0,0,1,0,0,0,1,0,1,1},
    {1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1},
    {1,0,1,0,0,2,1,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,0,2,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,2,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,2,0,0,1,0,0,0,1,0,0,0,1,1},
    {1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,1,0,1,2,0,2,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,2,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,1},
    {1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,1},
    {1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,2,1,0,1,0,1,2,0,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,0,2,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1},
    {1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,1},
    {1,0,0,0,1,0,0,2,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,2,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,2,1,0,1,0,1,1},
    {1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1},
    {1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,2,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,2,1,0,0,2,0,2,0,2,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,2,0,0,0,0,1,0,1,2,0,0,0,0,1,0,1,0,0,2,0,0,1,0,0,0,1,2,0,0,1,0,0,2,0,0,1,0,1,1},
    {1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1},
    {1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,2,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,2,0,0,1,0,1,0,1,0,0,0,1,2,0,0,1,0,0,0,1,0,0,0,1,0,1,2,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,2,1,1},
    {1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1},
    {1,2,0,0,1,0,1,2,0,2,0,0,1,0,1,0,0,0,0,2,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,2,1,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,2,1,0,1,0,1,0,1,1},
    {1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1},
    {1,0,0,0,1,0,0,2,0,0,1,0,1,0,0,2,0,0,1,0,0,0,0,0,1,0,0,0,0,2,0,0,1,0,1,2,0,0,0,0,0,2,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,2,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,2,0,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,1,1},
    {1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,1},
    {1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,2,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,2,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,2,0,0,1,0,0,0,1,0,0,0,1,0,1,1},
    {1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1},
    {1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,0,2,0,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,0,2,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,1},
    {1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1},
    {1,0,1,0,0,0,1,2,0,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,2,1,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1,2,0,0,1,0,0,0,0,2,1,0,0,0,0,2,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,1},
    {1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,1},
    {1,0,1,0,0,2,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,2,1,0,0,0,1,0,0,2,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,2,0,0,1,0,1,1},
    {1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1},
    {1,0,1,0,1,0,1,0,0,2,0,0,0,0,1,0,1,2,0,0,1,2,0,0,1,0,0,2,0,0,0,0,1,0,0,0,1,0,0,2,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,2,0,0,1,0,1,0,0,0,1,0,0,0,1,1},
    {1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,1},
    {1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,2,0,2,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0,2,0,0,0,0,0,0,1,0,0,2,0,0,0,0,0,0,0,2,1,0,1,0,0,2,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,1},
    {1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1},
    {1,2,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,2,1,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,2,1,0,0,2,1,0,0,0,0,0,1,0,1,2,0,0,1,1},
    {1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,1},
    {1,0,1,2,0,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,0,2,1,0,1,0,1,0,1,0,0,2,1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,2,0,0,0,0,0,0,1,0,0,2,0,2,0,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,1,1},
    {1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,1},
    {1,0,0,0,1,0,1,0,1,0,0,2,1,0,0,2,0,0,0,0,1,0,1,0,1,0,1,2,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,2,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,1,1},
    {1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1},
    {1,2,0,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,2,1,2,0,0,0,2,0,0,0,0,1,0,0,0,0,0,1,2,0,2,0,0,0,0,1,0,0,2,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,2,0,0,0,0,0,2,0,0,0,0,0,0,1,2,0,2,1,1},
    {1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,1},
    {1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,1,1},
    {1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,1},
    {1,0,1,0,1,0,0,0,1,0,1,0,0,0,0,2,0,2,0,0,1,0,1,0,1,0,0,0,0,2,1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,2,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,2,1,0,0,0,1,0,0,0,1,2,0,0,1,0,1,0,1,0,0,0,1,0,1,1},
    {1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1},
    {1,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,1,2,0,0,1,0,0,0,1,0,0,0,0,0,0,2,0,2,0,0,1,0,0,0,1,0,0,0,0,2,0,0,1,0,0,2,0,0,1,0,0,0,0,2,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,1,1},
    {1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,1},
    {1,0,1,0,1,0,1,0,1,0,0,2,1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,2,0,0,0,0,1,2,0,0,1,0,0,0,1,0,0,0,0,2,1,0,0,2,0,0,1,0,0,2,0,0,1,0,1,1},
    {1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1},
    {1,0,1,0,1,0,1,0,0,0,0,0,1,2,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,2,0,2,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,1},
    {1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1},
    {1,0,0,2,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,1,0,0,2,0,0,0,0,0,0,1,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,2,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,2,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,1,1},
};

struct Sijainti {
    int ykoord {0};
    int xkoord {0};
};

enum LiikkumisSuunta {
    UP, DOWN, LEFT, RIGHT, DEFAULT
};

enum Ristausve {
    WALL = 1,
    OPENING = 0
};

struct Suunta {
    Ristausve jatkom;
    bool tutkittu {false};
};

struct Ristaus {
    Sijainti kartalla;
    Suunta up;
    Suunta down;
    Suunta left;
    Suunta right;
    LiikkumisSuunta tutkittavana = DEFAULT;
};

struct Karttavirhe {
    int koodi {0};
    string msg;
};

/* TEHTÄVÄ 7 */
// FIFO puskurin rakenne ja jaettu muisti
struct Puskuri_teht7 {
    int data[BUF_SIZE];
    int head;
    int tail;
    int count;
};

/* TEHTÄVÄ 8 */
// FIFO puskurin rakenne ja jaettu muisti
struct Puskuri_teht8 {
    int data[BUF_SIZE];
    int head = 0;
    int tail = 0;
    int count = 0;
    mutex mtx;
    condition_variable not_full;
    condition_variable not_empty;
};

Puskuri_teht8 puskuri; // Jaettu rengaspuskuri

//int aloitaRotta();
struct RotanTulos;
RotanTulos aloitaRotta(); // Aloittaa rotan etsinnän labyrintissä
RotanTulos aloitaRotta_teht3();
RotanTulos aloitaRotta_teht4();
RotanTulos aloitaRotta_teht6();

Sijainti etsiKartasta(int kohde){
    Sijainti kartalla;
    for (int y = 0; y<KORKEUS ; y++) {
        for (int x = 0; x<LEVEYS ; x++){
            if (labyrintti[y][x] == kohde) {
                kartalla.xkoord = x;
                kartalla.ykoord = KORKEUS-1-y;
                return kartalla;
            }  
        }
    }
    return kartalla;
}

Sijainti findBegin(){
    Sijainti alkusijainti;
    alkusijainti = etsiKartasta(3);
    return alkusijainti;
}

// Palauttaa mahdollisesti satunnaisesti eri suunnan
// Kaikki rotat eivät tee samoja valintoja
// Pidentää huomattavasti reitin löytämistä joten kommentoitu ulos (voi kokeilla)
/*LiikkumisSuunta satunnainenPaatoSuunta(LiikkumisSuunta ehdotettuSuunta) {
    int satunnaisluku = rand() % 100;
    if (satunnaisluku < 30) {
        int vaihtoehto = rand() % 4;
        switch (vaihtoehto) {
            case 0: return UP;
            case 1: return DOWN;
            case 2: return LEFT;
            case 3: return RIGHT;
        }
    }
    return ehdotettuSuunta;
}*/

bool tutkiUp(Sijainti nykysijainti, auto& reitti, LiikkumisSuunta prevDir){
    int yindex = KORKEUS-1-nykysijainti.ykoord-1;
    if (yindex < 0) return false;
    if (labyrintti[yindex][nykysijainti.xkoord] == 1) return false;
    
    if (labyrintti[yindex][nykysijainti.xkoord] == 2 && prevDir != DOWN) {
        Ristaus ristaus;
        ristaus.kartalla.ykoord = nykysijainti.ykoord+1;
        ristaus.kartalla.xkoord = nykysijainti.xkoord;
        ristaus.down.tutkittu = true;
        ristaus.down.jatkom = OPENING;
        reitti.push_back(ristaus);

        return true;
    }
    return true;
}

bool tutkiDown(Sijainti nykysijainti, auto& reitti, LiikkumisSuunta prevDir){
    int yindex = KORKEUS-1-nykysijainti.ykoord+1;
    if (yindex > KORKEUS-1) return false;
    if (labyrintti[yindex][nykysijainti.xkoord] == 1) return false;
    
    if (labyrintti[yindex][nykysijainti.xkoord] == 2 && prevDir != UP){
        Ristaus ristaus;
        ristaus.kartalla.ykoord = nykysijainti.ykoord-1;
        ristaus.kartalla.xkoord = nykysijainti.xkoord;
        ristaus.up.tutkittu = true;
        ristaus.up.jatkom = OPENING;
        reitti.push_back(ristaus);

        return true;
    }
    return true;
}

bool tutkiLeft(Sijainti nykysijainti, auto& reitti, LiikkumisSuunta prevDir){
    int yindex = KORKEUS-1-nykysijainti.ykoord;
    int xindex = nykysijainti.xkoord-1;
    if (xindex < 0) return false;
    if (labyrintti[yindex][xindex] == 1) return false;
 
    if (labyrintti[yindex][xindex] == 2 && prevDir != RIGHT){
        Ristaus ristaus;
        ristaus.kartalla.ykoord = nykysijainti.ykoord;
        ristaus.kartalla.xkoord = nykysijainti.xkoord-1;
        ristaus.right.tutkittu = true;
        ristaus.right.jatkom = OPENING;
        reitti.push_back(ristaus);

        return true;
    }
    return true;
}

bool tutkiRight(Sijainti nykysijainti, auto& reitti, LiikkumisSuunta prevDir){
    int yindex = KORKEUS-1-nykysijainti.ykoord;
    int xindex = nykysijainti.xkoord+1;
    if (xindex > LEVEYS) return false;
    if (labyrintti[yindex][xindex] == 1) return false;
    
    if (labyrintti[yindex][xindex] == 2 && prevDir != LEFT){
        Ristaus ristaus;
        ristaus.kartalla.ykoord = nykysijainti.ykoord;
        ristaus.kartalla.xkoord = nykysijainti.xkoord+1;
        ristaus.left.tutkittu = true;
        ristaus.left.jatkom = OPENING;
        reitti.push_back(ristaus);

        return true;
    }
    return true;
}



LiikkumisSuunta findNext(bool onkoRistaus, Sijainti nykysijainti, LiikkumisSuunta prevDir, auto& reitti){
    if (!onkoRistaus) {        
        if (tutkiLeft(nykysijainti, reitti, prevDir) && prevDir != RIGHT){
        return LEFT;
        }
        if (tutkiUp(nykysijainti, reitti, prevDir) && prevDir != DOWN){
        return UP;
        }
        if (tutkiDown(nykysijainti, reitti, prevDir) && prevDir != UP){
        return DOWN;
        }
        if (tutkiRight(nykysijainti, reitti, prevDir) && prevDir != LEFT){
        return RIGHT;
        }
        return DEFAULT;
    }
    else if (onkoRistaus) {
        if (tutkiLeft(nykysijainti, reitti, prevDir) && reitti.back().tutkittavana != LEFT && !reitti.back().left.tutkittu){
        return LEFT;
        }
        if (tutkiUp(nykysijainti, reitti, prevDir) && reitti.back().tutkittavana != UP && !reitti.back().up.tutkittu){
        return UP;
        }
        if (tutkiDown(nykysijainti, reitti, prevDir) && reitti.back().tutkittavana != DOWN && !reitti.back().down.tutkittu){
        return DOWN;
        }
        if (tutkiRight(nykysijainti, reitti, prevDir) && reitti.back().tutkittavana != RIGHT && !reitti.back().right.tutkittu){
        return RIGHT;
        }
        return DEFAULT;
    }
}

Sijainti moveUp(Sijainti nykysijainti){
    nykysijainti.ykoord++;
    return nykysijainti;
}
Sijainti moveDown(Sijainti nykysijainti){
    nykysijainti.ykoord--;
    return nykysijainti;
}
Sijainti moveLeft(Sijainti nykysijainti){
    nykysijainti.xkoord--;
    return nykysijainti;
}
Sijainti moveRight(Sijainti nykysijainti){
    nykysijainti.xkoord++;
    return nykysijainti;
}

LiikkumisSuunta doRistaus(Sijainti risteyssijainti, LiikkumisSuunta prevDir, auto& reitti){
    LiikkumisSuunta nextDir; 
    nextDir = findNext(true, risteyssijainti, prevDir, reitti); 

    // Satunnaista päätöksentekoa!
    //nextDir = satunnainenPaatoSuunta(nextDir);

    if (nextDir == LEFT) reitti.back().tutkittavana = LEFT;
    else if (nextDir == UP) reitti.back().tutkittavana = UP;
    else if (nextDir == RIGHT) reitti.back().tutkittavana = RIGHT;
    else if (nextDir == DOWN) reitti.back().tutkittavana = DOWN;
    else if (nextDir == DEFAULT) reitti.pop_back();
    return nextDir;
}

struct RotanTulos {
    int liikkuCount;
    vector<Ristaus> reitti;  // Jäljelle jäänyt risteyspino
};

/* TEHTÄVÄ 3 */
void sem_wait_custom_teht3(int semid_teht3) {
        struct sembuf op = {0, -1, 0}; // P-operaatio (varaa)
        semop(semid_teht3, &op, 1);
}

void sem_signal_custom_teht3(int semid_teht3) {
        struct sembuf op = {0, 1, 0}; // V-operaatio (vapauta)
        semop(semid_teht3, &op, 1);
}

/* TEHTÄVÄ 7 */
void sem_wait_custom_teht7(int semid_teht7, int index) {
    struct sembuf op = {index, -1, 0}; // P-operaatio (varaa)
    semop(semid_teht7, &op, 1);
}

void sem_signal_custom_teht7(int semid_teht7, int index) {
    struct sembuf op = {index, 1, 0}; // V-operaatio (vapauta)
    semop(semid_teht7, &op, 1);
}

/* TEHTÄVÄ 3 */
// Merkitsee labyrinttiin rotan nykyisen sijainnin käyttäen semaforia
// Eksklusiivisen pääsyn varmistaminen
void merkitseLabyrinttiin_teht3(Sijainti sij, int id) {
    sem_wait_custom_teht3(semid_teht3); // Lukitsee semaforin
    int yindex = KORKEUS - 1 - sij.ykoord;
    int xindex = sij.xkoord;
    if (yindex >= 0 && yindex < KORKEUS && xindex >= 0 && xindex < LEVEYS) {
        if (labyrintti[yindex][xindex] == 0) // Vain yksi rotta voi olla ruudussa
            labyrintti[yindex][xindex] = id; // Kirjoittaa jaettuun muistiin
    }
    sem_signal_custom_teht3(semid_teht3); // Vapauttaa semaforin
}

/* TEHTÄVÄ 4 */
// Eksklusiivinen kirjoitus labyrinttiin
void merkitseLabyrinttiin_teht4(Sijainti sij, int id) {
    pthread_mutex_lock(&labMutex); // Lukitsee labyrintin kirjoitusta varten
    int yindex = KORKEUS - 1 - sij.ykoord;
    int xindex = sij.xkoord;
    if (yindex >= 0 && yindex < KORKEUS && xindex >= 0 && xindex < LEVEYS) {
        if (labyrintti[yindex][xindex] == 0)
            labyrintti[yindex][xindex] = id; // Kirjoittaa jaettuun muistiin
    }
    pthread_mutex_unlock(&labMutex); // Vapauttaa lukon
}

RotanTulos aloitaRotta(){
    int liikkuCount=0;
    vector<Ristaus> reitti;
    Sijainti rotanSijainti = findBegin();
    LiikkumisSuunta prevDir {DEFAULT};
    LiikkumisSuunta nextDir {DEFAULT};

    while (labyrintti[KORKEUS-1-rotanSijainti.ykoord][rotanSijainti.xkoord] != 4) {

        if (labyrintti[KORKEUS-1-rotanSijainti.ykoord][rotanSijainti.xkoord] == 2){
            nextDir = doRistaus(rotanSijainti, prevDir, reitti);
        }

        else nextDir = findNext(false /* ei risteys */, rotanSijainti, prevDir, reitti);
        switch (nextDir) {
        case UP:
        rotanSijainti = moveUp(rotanSijainti);
        prevDir = UP;
        break;
        case DOWN:
        rotanSijainti = moveDown(rotanSijainti);
        prevDir = DOWN;
        break;
        case LEFT:
        rotanSijainti = moveLeft(rotanSijainti);
        prevDir = LEFT;
        break;
        case RIGHT:
        rotanSijainti = moveRight(rotanSijainti);
        prevDir = RIGHT;
        break;
        case DEFAULT:
        cout << "Umpikuja: " << "Ruutu: " << rotanSijainti.ykoord << "," << rotanSijainti.xkoord << endl; 
        rotanSijainti.ykoord = reitti.back().kartalla.ykoord;
        rotanSijainti.xkoord = reitti.back().kartalla.xkoord;
        cout << "Palattu: " << "Ruutu: " << rotanSijainti.ykoord << "," << rotanSijainti.xkoord << endl;
            switch (reitti.back().tutkittavana){
            case UP:
                reitti.back().up.tutkittu = true;
                reitti.back().up.jatkom = OPENING;
                break;
            case DOWN:
                reitti.back().down.tutkittu = true;
                reitti.back().down.jatkom = OPENING;
                break;
            case LEFT:
                reitti.back().left.tutkittu = true;
                reitti.back().left.jatkom = OPENING;
                break;

            case RIGHT:
                reitti.back().right.tutkittu = true;
                reitti.back().right.jatkom = OPENING;
                break;
            default:
            cout << "Ei pitäisi tapahtua! Joku ongelma jos tämä tulostus tulee!" << endl;
            break;
        } 
        break;
    }

    liikkuCount++;

    usleep(10);
}
    return RotanTulos{liikkuCount, reitti}; // Palauta liikkujen määrä ja jäljelle jäänyt risteyspino
}

/* TEHTÄVÄ 3 */
RotanTulos aloitaRotta_teht3(){
    int liikkuCount=0;
    vector<Ristaus> reitti;
    Sijainti rotanSijainti = findBegin();
    LiikkumisSuunta prevDir {DEFAULT};
    LiikkumisSuunta nextDir {DEFAULT};

    while (labyrintti[KORKEUS-1-rotanSijainti.ykoord][rotanSijainti.xkoord] != 4) {

        if (labyrintti[KORKEUS-1-rotanSijainti.ykoord][rotanSijainti.xkoord] == 2){
            nextDir = doRistaus(rotanSijainti, prevDir, reitti);
        }

        else nextDir = findNext(false /* ei risteys */, rotanSijainti, prevDir, reitti);
        switch (nextDir) {
        case UP:
        rotanSijainti = moveUp(rotanSijainti);
        prevDir = UP;
        // Rotta merkitsee labyrinttiin kulkemaansa reittiä
        merkitseLabyrinttiin_teht3(rotanSijainti, getpid() % 100);
        break;
        case DOWN:
        rotanSijainti = moveDown(rotanSijainti);
        prevDir = DOWN;
        merkitseLabyrinttiin_teht3(rotanSijainti, getpid() % 100);
        break;
        case LEFT:
        rotanSijainti = moveLeft(rotanSijainti);
        prevDir = LEFT;
        merkitseLabyrinttiin_teht3(rotanSijainti, getpid() % 100);
        break;
        case RIGHT:
        rotanSijainti = moveRight(rotanSijainti);
        prevDir = RIGHT;
        merkitseLabyrinttiin_teht3(rotanSijainti, getpid() % 100);
        break;
        case DEFAULT:
        cout << "Umpikuja: " << "Ruutu: " << rotanSijainti.ykoord << "," << rotanSijainti.xkoord << endl; 
        rotanSijainti.ykoord = reitti.back().kartalla.ykoord;
        rotanSijainti.xkoord = reitti.back().kartalla.xkoord;
        cout << "Palattu: " << "Ruutu: " << rotanSijainti.ykoord << "," << rotanSijainti.xkoord << endl;
            switch (reitti.back().tutkittavana){
            case UP:
                reitti.back().up.tutkittu = true;
                reitti.back().up.jatkom = OPENING;
                break;
            case DOWN:
                reitti.back().down.tutkittu = true;
                reitti.back().down.jatkom = OPENING;
                break;
            case LEFT:
                reitti.back().left.tutkittu = true;
                reitti.back().left.jatkom = OPENING;
                break;

            case RIGHT:
                reitti.back().right.tutkittu = true;
                reitti.back().right.jatkom = OPENING;
                break;
            default:
            cout << "Ei pitäisi tapahtua! Joku ongelma jos tämä tulostus tulee!" << endl;
            break;
        } 
        break;
    }

    liikkuCount++;

    usleep(10);
}
    return RotanTulos{liikkuCount, reitti}; // Palauta liikkujen määrä ja jäljelle jäänyt risteyspino
}

/* TEHTÄVÄ 4 */
RotanTulos aloitaRotta_teht4(){
    int liikkuCount=0;
    vector<Ristaus> reitti;
    Sijainti rotanSijainti = findBegin();
    LiikkumisSuunta prevDir {DEFAULT};
    LiikkumisSuunta nextDir {DEFAULT};

    while (labyrintti[KORKEUS-1-rotanSijainti.ykoord][rotanSijainti.xkoord] != 4) {

        if (labyrintti[KORKEUS-1-rotanSijainti.ykoord][rotanSijainti.xkoord] == 2){
            nextDir = doRistaus(rotanSijainti, prevDir, reitti);
        }

        else nextDir = findNext(false /* ei risteys */, rotanSijainti, prevDir, reitti);
        switch (nextDir) {
        case UP:
        rotanSijainti = moveUp(rotanSijainti);
        prevDir = UP;
        // Rotta merkitsee labyrinttiin kulkemaansa reittiä
        merkitseLabyrinttiin_teht4(rotanSijainti, getpid() % 100);
        break;
        case DOWN:
        rotanSijainti = moveDown(rotanSijainti);
        prevDir = DOWN;
        merkitseLabyrinttiin_teht4(rotanSijainti, getpid() % 100);
        break;
        case LEFT:
        rotanSijainti = moveLeft(rotanSijainti);
        prevDir = LEFT;
        merkitseLabyrinttiin_teht4(rotanSijainti, getpid() % 100);
        break;
        case RIGHT:
        rotanSijainti = moveRight(rotanSijainti);
        prevDir = RIGHT;
        merkitseLabyrinttiin_teht4(rotanSijainti, getpid() % 100);
        break;
        case DEFAULT:
        cout << "Umpikuja: " << "Ruutu: " << rotanSijainti.ykoord << "," << rotanSijainti.xkoord << endl; 
        rotanSijainti.ykoord = reitti.back().kartalla.ykoord;
        rotanSijainti.xkoord = reitti.back().kartalla.xkoord;
        cout << "Palattu: " << "Ruutu: " << rotanSijainti.ykoord << "," << rotanSijainti.xkoord << endl;
            switch (reitti.back().tutkittavana){
            case UP:
                reitti.back().up.tutkittu = true;
                reitti.back().up.jatkom = OPENING;
                break;
            case DOWN:
                reitti.back().down.tutkittu = true;
                reitti.back().down.jatkom = OPENING;
                break;
            case LEFT:
                reitti.back().left.tutkittu = true;
                reitti.back().left.jatkom = OPENING;
                break;

            case RIGHT:
                reitti.back().right.tutkittu = true;
                reitti.back().right.jatkom = OPENING;
                break;
            default:
            cout << "Ei pitäisi tapahtua! Joku ongelma jos tämä tulostus tulee!" << endl;
            break;
        } 
        break;
    }

    liikkuCount++;

    usleep(10);
}
    return RotanTulos{liikkuCount, reitti}; // Palauta liikkujen määrä ja jäljelle jäänyt risteyspino
}

RotanTulos aloitaRotta_teht6(){
    int liikkuCount=0;
    vector<Ristaus> reitti;
    Sijainti rotanSijainti = findBegin();
    LiikkumisSuunta prevDir {DEFAULT};
    LiikkumisSuunta nextDir {DEFAULT};

    while (labyrintti[KORKEUS-1-rotanSijainti.ykoord][rotanSijainti.xkoord] != 4) {

        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, []{ return !paused.load(); });
        }

        if (labyrintti[KORKEUS-1-rotanSijainti.ykoord][rotanSijainti.xkoord] == 2){
            nextDir = doRistaus(rotanSijainti, prevDir, reitti);
        }

        else nextDir = findNext(false /* ei risteys */, rotanSijainti, prevDir, reitti);
        switch (nextDir) {
        case UP:
        rotanSijainti = moveUp(rotanSijainti);
        prevDir = UP;
        break;
        case DOWN:
        rotanSijainti = moveDown(rotanSijainti);
        prevDir = DOWN;
        break;
        case LEFT:
        rotanSijainti = moveLeft(rotanSijainti);
        prevDir = LEFT;
        break;
        case RIGHT:
        rotanSijainti = moveRight(rotanSijainti);
        prevDir = RIGHT;
        break;
        case DEFAULT:
        cout << "Umpikuja: " << "Ruutu: " << rotanSijainti.ykoord << "," << rotanSijainti.xkoord << endl; 
        rotanSijainti.ykoord = reitti.back().kartalla.ykoord;
        rotanSijainti.xkoord = reitti.back().kartalla.xkoord;
        cout << "Palattu: " << "Ruutu: " << rotanSijainti.ykoord << "," << rotanSijainti.xkoord << endl;
            switch (reitti.back().tutkittavana){
            case UP:
                reitti.back().up.tutkittu = true;
                reitti.back().up.jatkom = OPENING;
                break;
            case DOWN:
                reitti.back().down.tutkittu = true;
                reitti.back().down.jatkom = OPENING;
                break;
            case LEFT:
                reitti.back().left.tutkittu = true;
                reitti.back().left.jatkom = OPENING;
                break;

            case RIGHT:
                reitti.back().right.tutkittu = true;
                reitti.back().right.jatkom = OPENING;
                break;
            default:
            cout << "Ei pitäisi tapahtua! Joku ongelma jos tämä tulostus tulee!" << endl;
            break;
        } 
        break;
    }

    liikkuCount++;

    usleep(10000);
}
    return RotanTulos{liikkuCount, reitti}; // Palauta liikkujen määrä ja jäljelle jäänyt risteyspino
}

struct JaettuMuisti {
    void* shmaddr;
    int shmid;
};

// Jaetun muistin luonti ja labyrintin kopiointi sinne
// Palauttaa jaetun muistin osoitteen ja ID:n
JaettuMuisti luoJaettuLabyrintti() {
    size_t size = sizeof(int) * KORKEUS * LEVEYS;
    int shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        return {nullptr, -1};
    }

    // Liitetään jaettu muisti prosessin osoiteavaruuteen
    void* shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (void*) -1) {
        perror("shmat failed");
        shmctl(shmid, IPC_RMID, NULL);
        return {nullptr, -1};
    }

    labyrintti = reinterpret_cast<int (*)[LEVEYS]>(shmaddr);
    memcpy(labyrintti, alkuLabyrintti, size);

    return {shmaddr, shmid};
}

// Jaetun muistin vapautus
void poistaJaettuLabyrintti(void* shmaddr, int shmid) {
    if (shmdt(shmaddr) == -1) {
        perror("shmdt failed");
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
    }

    shmctl(shmget(IPC_PRIVATE, 0, 0), IPC_RMID, NULL);
}

/* TEHTÄVÄ 5 */
/* TEHTÄVÄ 6 */
void tallennaLabyrinttiTiedostoon(const char* tiedostonimi) {
    FILE* tiedosto = fopen(tiedostonimi, "a");
    if (!tiedosto) {
        perror("fopen");
        return;
    }

    // Kirjoita labyrintin tila tiedostoon
    for (int y = 0; y < KORKEUS; y++) {
        for (int x = 0; x < LEVEYS; x++) {
            fprintf(tiedosto, "%d ", labyrintti[y][x]);
        }
        fprintf(tiedosto, "\n");
    }
    fprintf(tiedosto, "\n");
    fclose(tiedosto);
}

/* TEHTÄVÄ 6 */
void* rottaSaie_teht6(void* arg) {
    int id = *(int*)arg; // Jokainen säie liikkuu itsenäisesti

    // Mutex lukitsee tulostuksen, jotta se tulee siistimmin ulos
    pthread_mutex_lock(&tulostusLukko);
    cout << "Rotta " << id 
         << " (säie " << pthread_self() << ") aloittaa liikkumisen!" << endl;
    pthread_mutex_unlock(&tulostusLukko);

    RotanTulos tulos = aloitaRotta_teht6();

    pthread_mutex_lock(&tulostusLukko);
    cout << "Rotta " << id << " valmis liikkein: " << tulos.liikkuCount << endl;
    pthread_mutex_unlock(&tulostusLukko);
    for (size_t j = 0; j < tulos.reitti.size(); ++j) {
        cout << "  Risteys " << j << ": (" << tulos.reitti[j].kartalla.ykoord 
        << "," << tulos.reitti[j].kartalla.xkoord << ")" << endl;
    }

    return nullptr;
}

/* TEHTÄVÄ 2 */
void* rottaSaie_teht2(void* arg) {
    int id = *(int*)arg;
    cout << "Rotta " << id 
         << " (säie " << pthread_self() << ") aloittaa liikkumisen!" << endl;
    RotanTulos tulos = aloitaRotta(); // Jokainen säie liikkuu itsenäisesti

    // Tulostetaan reitin sisältö testimielessä!
    cout << "Rotta " << id << " valmis liikkein: " << tulos.liikkuCount << endl;
    
    for (size_t j = 0; j < tulos.reitti.size(); ++j) {
        cout << "  Risteys " << j << ": (" << tulos.reitti[j].kartalla.ykoord 
        << "," << tulos.reitti[j].kartalla.xkoord << ")" << endl;
    }

    return nullptr;
}

/* TEHTÄVÄ 4 */
void* rottaSaie_teht4(void* arg) {
    int id = *(int*)arg; // Jokainen säie liikkuu itsenäisesti

    // Mutex lukitsee tulostuksen, jotta se tulee siistimmin ulos
    pthread_mutex_lock(&tulostusLukko);
    cout << "Rotta " << id 
         << " (säie " << pthread_self() << ") aloittaa liikkumisen!" << endl;
    pthread_mutex_unlock(&tulostusLukko);

    RotanTulos tulos = aloitaRotta_teht4();

    pthread_mutex_lock(&tulostusLukko);
    cout << "Rotta " << id << " valmis liikkein: " << tulos.liikkuCount << endl;
    pthread_mutex_unlock(&tulostusLukko);
    for (size_t j = 0; j < tulos.reitti.size(); ++j) {
        cout << "  Risteys " << j << ": (" << tulos.reitti[j].kartalla.ykoord 
        << "," << tulos.reitti[j].kartalla.xkoord << ")" << endl;
    }

    return nullptr;
}

/* TEHTÄVÄ 8 */
void* tuottaja_teht8(void*) {
    for (int viesti = 1; viesti <= 9; viesti++) {
        unique_lock<mutex> lukko(puskuri.mtx);

        // Odota jos puskuri on täynnä
        puskuri.not_full.wait(lukko, []{ return puskuri.count < BUF_SIZE; });

        // Lisää viesti puskuriin
        puskuri.data[puskuri.head] = viesti;
        puskuri.head = (puskuri.head + 1) % BUF_SIZE;
        puskuri.count++;
        cout << "Parent lähetti viestin: " << viesti << endl;

        lukko.unlock();
        puskuri.not_empty.notify_one(); // Ilmoita, että dataa on saatavilla

        sleep(1);
    }
    return nullptr;
}

void* rotta_teht8(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 3; i++) {
        unique_lock<mutex> lukko(puskuri.mtx);

        // Odota jos puskuri on tyhjä
        puskuri.not_empty.wait(lukko, []{ return puskuri.count > 0; });

        // Lue viesti rengaspuskurista
        int viesti = puskuri.data[puskuri.tail];
        puskuri.tail = (puskuri.tail + 1) % BUF_SIZE;
        puskuri.count--;
        cout << "Rotta " << id << " sai viestin: " << viesti << endl;

        lukko.unlock();
        puskuri.not_full.notify_one(); // Ilmoita, että yksi paikka vapautui

        sleep(1);
    }
    return nullptr;
}

/* TEHTÄVÄ 1 */
int main_teht1(){

    JaettuMuisti jm = luoJaettuLabyrintti(); // Luo jaettu labyrintti
    if (!jm.shmaddr) return 1; // Virhe luonnissa

    // Luo lapsiprosessit
    pid_t lapset[ROTAT];
    for(int i=0;i<ROTAT;i++){
        pid_t pid = fork(); // Rinnakkainen prosessien luonti
        if(pid==0){
            //srand(time(NULL) ^ getpid());
            // Lapsiprosessi
            RotanTulos tulos = aloitaRotta();
            cout << "Rotta " << getpid() << " ulkona liikkein: " << tulos.liikkuCount << endl;
            _exit(0);
        } else lapset[i]=pid;
    }
    // Parent odottaa kaikkien valmistumista
    for(int i=0;i<ROTAT;i++) waitpid(lapset[i], nullptr, 0);

    std::cout << "Kaikki rotat ulkona!" << endl;

    poistaJaettuLabyrintti(jm.shmaddr, jm.shmid); // Vapauta

    return 0;
}

/* TEHTÄVÄ 2 */
int main_teht2(){
    
    JaettuMuisti jm = luoJaettuLabyrintti(); // Luo jaettu labyrintti
    if (!jm.shmaddr) return 1; // Virhe luonnissa

    pthread_t rotat[ROTAT];
    int idt[ROTAT];

    // Luo säikeet
    for(int i=0;i<ROTAT;i++){
        idt[i] = i+1;
        if(pthread_create(&rotat[i], nullptr, rottaSaie_teht2, &idt[i]) != 0){
            perror("pthread_create failed");
            return 1;
        }
    }

    // Odota kaikkien säikeiden valmistumista
    for(int i=0;i<ROTAT;i++){
        pthread_join(rotat[i], nullptr);
    }

    std::cout << "Kaikki rotat ulkona!" << endl;

    poistaJaettuLabyrintti(jm.shmaddr, jm.shmid); // Vapauta

    return 0;
}

/* TEHTÄVÄ 3 */
int main_teht3(){

    JaettuMuisti jm = luoJaettuLabyrintti(); // Luo jaettu labyrintti
    if (!jm.shmaddr) return 1; // Virhe luonnissa

    // Semafori joka toimii lukituksena lapsiprosesseille
    semid_teht3 = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (semid_teht3 < 0) {
        perror("semget failed");
        return 1;
    }

    semctl(semid_teht3, 0, SETVAL, 1); // Binäärinen semafori

    // Luo lapsiprosessit
    pid_t lapset[ROTAT];
    for(int i=0;i<ROTAT;i++){
        pid_t pid = fork(); // Rinnakkainen prosessien luonti
        if(pid==0){
            //srand(time(NULL) ^ getpid());
            // Lapsiprosessi
            RotanTulos tulos = aloitaRotta_teht3();
            sem_wait_custom_teht3(semid_teht3);
            cout << "Rotta " << getpid() << " ulkona liikkein: " << tulos.liikkuCount << endl;

            cout << "Rotta " << getpid() << " reitin koko: " << tulos.reitti.size() << endl;
            sem_signal_custom_teht3(semid_teht3);
    
            for (size_t j = 0; j < tulos.reitti.size(); ++j) {
                cout << "  Risteys " << j << ": (" << tulos.reitti[j].kartalla.ykoord 
                << "," << tulos.reitti[j].kartalla.xkoord << ")" << endl;
            }
            
            sem_signal_custom_teht3(semid_teht3);
            
            _exit(0);
        } else lapset[i]=pid;
    }
    // Parent odottaa kaikkien valmistumista
    for(int i=0;i<ROTAT;i++) waitpid(lapset[i], nullptr, 0);
    
    std::cout << "Kaikki rotat ulkona!" << endl;

    poistaJaettuLabyrintti(jm.shmaddr, jm.shmid); // Vapauta
    semctl(semid_teht3, 0, IPC_RMID);

    return 0;
}

/* TEHTÄVÄ 4 */
int main_teht4(){

    JaettuMuisti jm = luoJaettuLabyrintti(); // Luo jaettu labyrintti
    if (!jm.shmaddr) return 1; // Virhe luonnissa

    pthread_t rotat[ROTAT];
    int idt[ROTAT];

    // Luo säikeet
    for(int i=0;i<ROTAT;i++){
        idt[i] = i+1;
        pthread_create(&rotat[i], nullptr, rottaSaie_teht4, &idt[i]);
    }

    // Odota kaikkien säikeiden valmistumista
    for(int i=0;i<ROTAT;i++){
        pthread_join(rotat[i], nullptr);
    }
    
    std::cout << "Kaikki rotat ulkona!" << endl;

    poistaJaettuLabyrintti(jm.shmaddr, jm.shmid); // Vapauta

    return 0;
}

/* TEHTÄVÄ 5 */
int main_teht5(){
    
    JaettuMuisti jm = luoJaettuLabyrintti(); // Luo jaettu labyrintti
    if (!jm.shmaddr) return 1; // Virhe luonnissa

    pid_t lapset[ROTAT];
    for (int i = 0; i < ROTAT; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            //srand(time(NULL) ^ getpid());
            // Lapsiprosessi
            RotanTulos tulos = aloitaRotta();
            cout << "Rotta " << getpid() << " ulkona liikkein: " << tulos.liikkuCount << endl;
            _exit(0);
        } else lapset[i] = pid;
    }

    // Valokuvaussilmukka
    struct timespec nukkumisaika = {0, 200000000L};
    int kaikkiValmiit = 0;

    while (!kaikkiValmiit) {
        nanosleep(&nukkumisaika, NULL); // Parent nukkuu hetken

        // Jäädytä kaikki lapset
        for (int i = 0; i < ROTAT; i++) kill(lapset[i], SIGSTOP);
        
        // Ota valokuva
        tallennaLabyrinttiTiedostoon("labyrintti_tilat.txt");
        cout << "Tallennettu labyrintin tila tiedostoon!" << endl;

        // Jatka lapsia
        for (int i = 0; i < ROTAT; i++) kill(lapset[i], SIGCONT);

        // Tarkista ovatko kaikki jo lopettaneet
        kaikkiValmiit = 1;
        for (int i = 0; i < ROTAT; i++) {
            int status;
            pid_t result = waitpid(lapset[i], &status, WNOHANG);
            if (result == 0) kaikkiValmiit = 0; // vielä käynnissä
        }
    }

    std::cout << "Kaikki rotat ulkona!" << endl;

    poistaJaettuLabyrintti(jm.shmaddr, jm.shmid); // Vapauta

    return 0;
}

/* TETHTÄVÄ 6 */
int main_teht6(){
    
    JaettuMuisti jm = luoJaettuLabyrintti(); // Luo jaettu labyrintti
    if (!jm.shmaddr) return 1; // Virhe luonnissa

    pthread_t rotat[ROTAT];
    int idt[ROTAT];

    // Luo säikeet
    for(int i=0;i<ROTAT;i++){
        idt[i] = i+1;
        if(pthread_create(&rotat[i], nullptr, rottaSaie_teht6, &idt[i]) != 0){
            perror("pthread_create failed");
            return 1;
        }
    }

    // Simuloidaan "pause/jatka" toimintaa
    for (int kierros = 0; kierros < 3; ++kierros) {
        sleep(1); // Anna rottien liikkua hetki
        {
            std::lock_guard<std::mutex> lock(mtx);
            paused = true;
        }

        tallennaLabyrinttiTiedostoon("labyrintti_tilat.txt");
        cout << "Tallennettu labyrintin tila tiedostoon!" << endl;
        sleep(2); // Jäädytys

        {
            std::lock_guard<std::mutex> lock(mtx);
            paused = false;
        }
        cv.notify_all(); // Jatketaan liikettä
    }

    for (int i = 0; i < ROTAT; ++i) {
        pthread_join(rotat[i], nullptr);
    }

    std::cout << "Kaikki rotat ulkona!" << endl;

    poistaJaettuLabyrintti(jm.shmaddr, jm.shmid); // Vapauta

    return 0;
}

/* TEHTÄVÄ 7 */
int main_teht7(){
    
    JaettuMuisti jm = luoJaettuLabyrintti(); // Luo jaettu labyrintti
    if (!jm.shmaddr) return 1; // Virhe luonnissa

    // Luo jaettu puskuri ja semaforit
    int shmid = shmget(IPC_PRIVATE, sizeof(Puskuri_teht7), IPC_CREAT | 0666);
    Puskuri_teht7* buf = (Puskuri_teht7*) shmat(shmid, NULL, 0);
    buf->head = buf->tail = buf->count = 0;

    // Semaforit: 0 = mutex, 1 = vapaat paikat, 2 = täytetyt paikat
    int semid = semget(IPC_PRIVATE, 3, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 1);
    semctl(semid, 1, SETVAL, BUF_SIZE);
    semctl(semid, 2, SETVAL, 0);

    // Luo rottaprosessit
    pid_t lapset[ROTAT];
    for (int i = 0; i < ROTAT; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            //srand(time(NULL) ^ getpid());

            for (int j = 0; j < 3; j++) {
                sem_wait_custom_teht7(semid, 2); // Odota täytettyä paikkaa
                sem_wait_custom_teht7(semid, 0); // Varaa mutex
                int viesti = buf->data[buf->tail];
                buf->tail = (buf->tail + 1) % BUF_SIZE;
                buf->count--;
                cout << "Rotta " << getpid() << " sai viestin: " << viesti << endl;
                sem_signal_custom_teht7(semid, 0); // Vapauta mutex
                sem_signal_custom_teht7(semid, 1); // Ilmoita vapautetusta paikasta
                sleep(1);
            }
            _exit(0);
        } else lapset[i] = pid;
    }

    // Lähetä viestejä rotille
    for (int viesti = 1; viesti <= 9; viesti++) {
        sem_wait_custom_teht7(semid, 1);
        sem_wait_custom_teht7(semid, 0);
        buf->data[buf->head] = viesti;
        buf->head = (buf->head + 1) % BUF_SIZE;
        buf->count++;
        cout << "Parent lähetti viestin: " << viesti << endl;
        sem_signal_custom_teht7(semid, 0);
        sem_signal_custom_teht7(semid, 2);
        sleep(1);
    }

    // Odota rottien valmistumista
    for (int i = 0; i < ROTAT; i++) waitpid(lapset[i], nullptr, 0);
    shmdt(buf);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);

    std::cout << "Kaikki rotat ulkona!" << endl;

    poistaJaettuLabyrintti(jm.shmaddr, jm.shmid); // Vapauta

    return 0;
}

/* TEHTÄVÄ 8 */
int main_teht8(){

    //srand(time(NULL));
    
    JaettuMuisti jm = luoJaettuLabyrintti(); // Luo jaettu labyrintti
    if (!jm.shmaddr) return 1; // Virhe luonnissa

    pthread_t tuottaja_sae;
    pthread_t rotat[ROTAT];
    int idt[ROTAT];

    // Käynnistä säikeet
    pthread_create(&tuottaja_sae, nullptr, tuottaja_teht8, nullptr);

    for (int i = 0; i < ROTAT; i++) {
        idt[i] = i + 1;
        pthread_create(&rotat[i], nullptr, rotta_teht8, &idt[i]);
    }

    // Odota että kaikki säikeet valmistuvat
    pthread_join(tuottaja_sae, nullptr);
    for (int i = 0; i < ROTAT; i++) {
        pthread_join(rotat[i], nullptr);
    }

    std::cout << "Kaikki rotat ulkona!" << endl;

    poistaJaettuLabyrintti(jm.shmaddr, jm.shmid); // Vapauta

    return 0;
}

/* VALIKKO: Aja mikä vaan tehtävistä 1-8 */
int main(){
    int valinta = 0;
    do {
        cout << "\n=============================\n";
        cout << " ROTAT LABYRINTISSA\n";
        cout << "=============================\n";
        cout << "Valitse suoritettava tehtävä:\n";
        cout << " 1) Prosessiversio (teht. 1)\n";
        cout << " 2) Säieversio (teht. 2)\n";
        cout << " 3) Prosessit + semafori (teht. 3)\n";
        cout << " 4) Säikeet + eksklusiivisuus (teht. 4)\n";
        cout << " 5) Prosessien suspend-toiminnallisuus (teht. 5)\n";
        cout << " 6) Säikeiden suspend-toiminnallisuus (teht. 6)\n";
        cout << " 7) FIFO rajattu rengaspuskuri prosesseilla (teht. 7)\n";
        cout << " 8) FIFO rajattu rengaspuskuri säikeillä (teht. 8)\n";
        cout << " 0) Lopeta\n";
        cout << "Valintasi: ";
        cin >> valinta;

        switch(valinta) {
            case 1:
                cout << "\n--- Suoritetaan tehtävä 1 ---\n";
                main_teht1();
                break;
            case 2:
                cout << "\n--- Suoritetaan tehtävä 2 ---\n";
                main_teht2();
                break;
            case 3:
                cout << "\n--- Suoritetaan tehtävä 3 ---\n";
                main_teht3();
                break;
            case 4:
                cout << "\n--- Suoritetaan tehtävä 4 ---\n";
                main_teht4();
                break;
            case 5:
                cout << "\n--- Suoritetaan tehtävä 5 ---\n";
                main_teht5();
                break;
            case 6:
                cout << "\n--- Suoritetaan tehtävä 6 ---\n";
                main_teht6();
                break;
            case 7:
                cout << "\n--- Suoritetaan tehtävä 7 ---\n";
                main_teht7();
                break;
            case 8:
                cout << "\n--- Suoritetaan tehtävä 8 ---\n";
                main_teht8();
                break;
            case 0:
                cout << "Ohjelma lopetetaan.\n";
                break;
            default:
                cout << "Virheellinen valinta, yritä uudelleen.\n";
        }

    } while(valinta != 0);

    return 0;
}