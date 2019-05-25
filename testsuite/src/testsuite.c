#include <stdio.h>
#include "testsuite.h"

/*
OSMP_TEST

- ohne initialisirung starten 
- ohne Finalize beenden
- zuviele nachrichten senden
- senden einer Nachricht an eine prozess der Finalize ist
- Nachricht beim Beenden noch ausstehend
- Init Finalize Init
- leere Nachricht
- Nachricht mit Falscher größe Senden
- Nachricht mit Falscher größe emfangen

 */


int main()
{
    printf("Hier könnte ihre Testimplementation laufen!\n");
    return 0;
}