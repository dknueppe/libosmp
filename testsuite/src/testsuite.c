#include <stdio.h>
#include "testsuite.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "osmplib.h"
/*
OSMP_TEST

- ohne initialisirung starten 
    Erwartung:  
- ohne Finalize beenden
    Erwartung:
- zuviele nachrichten senden
    Erwartung:
- senden einer Nachricht an eine prozess der Finalize ist
    Erwartung:
- Nachricht beim Beenden noch ausstehend
    Erwartung:
- Init Finalize Init
    Erwartung:
- leere Nachricht
    Erwartung:
- Nachricht mit Falscher größe Senden
    Erwartung:
- Nachricht mit Falscher größe emfangen
    Erwartung:
- Nachricht an sich selber
    Erwartung:


 */

int main()
{
    int Fehler = 0;

    char TEST[] = "Das ist ein test\n";
    printf("%s", TEST);

    printf("\n\nProgram zum Testen der OSMP Startet!\n");

    printf("Test ohne initialisirung!\n");
    if (Test_ohne_ini()) // ruft die rutione zum testen von funktionen ohne vorher eine initialisirung
    {
        Fehler++;
        printf("Test nicht wie erwartet abgelaufen!\n");
    }
    else
    {

        printf("Test wie erwartet abgelaufen!\n");
    }

    printf("Test ohne Finalize!\n");
    if (Test_ohne_final()) // rufen der test rutine aut mit funktions test ohne finalize auf
    {
        Fehler++;
        printf("Test nicht wie erwartet abgelaufen!\n");
    }
    else
    {
        printf("Test wie erwartet abgelaufen!\n");
    }

    printf("\n");
    if (Test_zuviele_Nachrichten()) // ruft die Test rutine für zu viele nachrichten auf
    {
        Fehler++;
        printf("Test nicht wie erwartet abgelaufen!\n");
    }
    else
    {
        printf("Test wie erwartet abgelaufen!\n");
    }

    printf("\n");
    if (Test_Nachricht_an_Finalize()) // ruft die Rutine für nachricht an finalize prozess auf
    {
        Fehler++;
        printf("Test nicht wie erwartet abgelaufen!\n");
    }
    else
    {
        printf("Test wie erwartet abgelaufen!\n");
    }

    printf("\n");
    if (Test_Init_Final_Init()) // ruft die rutine für initalisirte dan finalize und wider initalisirte processe auf
    {
        Fehler++;
        printf("Test nicht wie erwartet abgelaufen!\n");
    }
    else
    {
        printf("Test wie erwartet abgelaufen!\n");
    }

    printf("\n");
    if (Test_leere_Nachricht()) // ruft die rutine für eine Leere nachricht auf
    {
        Fehler++;
        printf("Test nicht wie erwartet abgelaufen!\n");
    }
    else
    {
        printf("Test wie erwartet abgelaufen!\n");
    }

    printf("\n");
    if (Test_Falsche_groesse_senden()) // ruft die rutine für das senden von nachrichten mit falscher grösse auf
    {
        Fehler++;
        printf("Test nicht wie erwartet abgelaufen!\n");
    }
    else
    {
        printf("Test wie erwartet abgelaufen!\n");
    }

    printf("\n");
    if (Test_Falsche_groesse_empfangen()) // ruft die rutine für das emfangen von nachrichten falscher grösse
    {
        Fehler++;
        printf("Test nicht wie erwartet abgelaufen!\n");
    }
    else
    {
        printf("Test wie erwartet abgelaufen!\n");
    }

    printf("\n");
    if (Test_Nachricht_an_Mich()) // ruft die rutine für Nachrichten an sichselber auf
    {
        Fehler++;
        printf("Test nicht wie erwartet abgelaufen!\n");
    }
    else
    {
        printf("Test wie erwartet abgelaufen!\n");
    }

    printf("Tests Beendet\n");
    if (Fehler > 0) // überprüft ob Fehler aufgetreten sind
    {
        printf("Es gab %i Test(s) der/die nicht so ablifen wie erwartet!\n", Fehler);
    }
    else
    {
        printf("Alle Tests wahren erfolgreich!\n");
    }

    return 0;
}

//- ohne initialisirung starten
int Test_ohne_ini()
{
    int status;
    // int error = 0;
    int rank = -1;
    int size = -1;
    int source = -1;
    int len = -1;
    int sender = -1;
    osmp_byte bar[] = "hello world!";
    char recv[OSMP_MAX_PAYLOAD_LENGTH] = "";
    OSMP_Datatype foobar = OSMP_typeof(bar[0]);
    OSMP_Request myrequest1, myrequest2;

    // hab hier mal rumgepfuscht!
    void *irecv;
    

    if ((status = OSMP_Size(&size) == OSMP_SUCCESS))
    {
        printf("OSMP_Size");
        return 1;
    }

    if ((status = OSMP_Rank(&rank) == OSMP_SUCCESS))
    {
        printf("OSMP_Rank");
        return 1;
    }

    if ((status = OSMP_Send(bar, sizeof(bar), OSMP_typeof(bar[0]), rank) == OSMP_SUCCESS))
    {
        printf("OSMP_Send");
        return 1;
    }

    if ((status = OSMP_Recv(recv, 13, foobar, &source, &len) == OSMP_SUCCESS))
    {
        printf("OSMP_Recv");
        return 1;
    }

    if ((status = OSMP_CreateRequest(&myrequest1)) == OSMP_SUCCESS)
    {
        printf("OSMP_CreateRequest");
        return 1;
    }

    if ((status = OSMP_Isend(bar, sizeof(bar), OSMP_typeof(bar[0]), rank, myrequest1)) == OSMP_SUCCESS)
    {
        printf("OSMP_Isend");
        return 1;
    }

    if ((status = OSMP_Irecv(irecv, 13, OSMP_typeof(*bar), &sender, &len, myrequest2)) == OSMP_SUCCESS)
    {
        printf("OSMP_Irecv");
        return 1;
    }

    if ((status = OSMP_Wait(myrequest1)) == OSMP_SUCCESS)
    {
        printf("OSMP_Wait");
        return 1;
    }

    if ((status = OSMP_RemoveRequest(&myrequest1)) == OSMP_SUCCESS)
    {
        printf("OSMP_RemoveRequest");
        return 1;
    }

    return 0;
}

//- ohne Finalize beenden
int Test_ohne_final()
{
    printf("Test 2\n");
    return 0;
}

//- zuviele nachrichten senden
int Test_zuviele_Nachrichten()
{
    printf("Test 3\n");
    return 0;
}

//- senden einer Nachricht an eine prozess der Finalize ist
int Test_Nachricht_an_Finalize()
{
    printf("Test 4\n");
    return 0;
}

//- Nachricht beim Beenden noch ausstehend
int Test_Nachrichten_austehend()
{
    printf("Test 5\n");
    return 0;
}

//- Init Finalize Init
int Test_Init_Final_Init()
{
    printf("Test 6\n");
    return 0;
}

//- leere Nachricht
int Test_leere_Nachricht()
{
    printf("Test 7\n");
    return 0;
}

//- Nachricht mit Falscher größe Senden
int Test_Falsche_groesse_senden()
{
    printf("Test 8\n");
    return 0;
}

//- Nachricht mit Falscher größe emfangen
int Test_Falsche_groesse_empfangen()
{
    printf("Test 9\n");
    return 0;
}

//- Nachricht an sich selber
int Test_Nachricht_an_Mich()
{
    printf("Test 10\n");
    return 0;
}