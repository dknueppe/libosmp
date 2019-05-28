#include <stdio.h>
#include "testsuite.h"

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

    printf("TEST ohne initialisirung!\n");

    if (Test_ohne_ini())
    {
        Fehler++;
        printf("Test nicht wie erwartet abgelaufen!\n");
    }
    else
    {
        printf("Test wie erwartet abgelaufen!\n");
    }

    printf("TEST ohne Finalize!\n");

    if (Test_ohne_final())
    {
        Fehler++;
        printf("Test nicht wie erwartet abgelaufen!\n");
    }
    else
    {
        printf("Test wie erwartet abgelaufen!\n");
    }
    if (Fehler)
    {
        printf("Es gab %i Test(s) der/die nicht so ablifen wie erwartet!\n", Fehler);
    }
    else
    {
        printf("Alle Tests wahren erfolgreich!\n");
    }

    return 0;
}

int Test_ohne_ini()
{

    printf("Test 1\n");

    return 0;
}

int Test_ohne_final()
{

    printf("Test 2\n");

    return 1;
}





