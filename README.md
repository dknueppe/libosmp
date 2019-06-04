[![build status](https://git.fh-muenster.de/ospr/g05/badges/master/pipeline.svg)
](https://git.fh-muenster.de/ospr/g05/pipelines)
[![test coverage](https://git.fh-muenster.de/ospr/g05/badges/master/coverage.svg?test=coverage)
](https://git.fh-muenster.de/ospr/g05/-/jobs)

# Betriebssysteme Praktikum der Gruppe G05
## Daniel Knüppe, Roderick Topütt

### Ordnerübersicht 

* **osmpclient** enthält eine Beispielimplementation für die Nutzung der Bibliothek
* **osmplib** enthält die Bibliothek selbst
* **osmprun** enthält den Prozessmanager
* **testsuite** enthält Programme zum testen der Bibliotheksfunktionen


### Speicherlayout

Das Speicherlayout besteht aus zwei Teilen, dem statischen und dem dynamischen.
Im statischen Teil stehen Infos und Speicherbereich zur verfügung, welche unabhängig
von der Anzahl der zu startenden Prozesse sind. Im dynamischen Teil befindet sich
ein *Prozess Kontroll Block* pro zu startenden Prozess.

~~~~
typedef struct {
    size_t shm_size;
    unsigned int num_proc;
    OSMP_msg_node messages[OSMP_MAX_SLOTS];
    OSMP_queue empty_list;
} OSMP_base;

typedef struct {
    OSMP_queue inbox;
    pid_t pid;
} OSMP_pcb;
~~~~

