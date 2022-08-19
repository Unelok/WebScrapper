/*
**  Filename : struct.h
**
**  Made by : 3IABD
**
**  Description : Define the structures used in the main.c
*/


#ifndef STRUCT_H_INCLUDED
#define STRUCT_H_INCLUDED

//  Une option est constituée d'une clé (name), et d'une valeur (value)
typedef struct Option{
    char* name;
    char* value;
} Option;

//  Une action est constituée d'un nom (name), d'une url (url), et de plusieurs options (options)
typedef struct Action{
    char* name;
    char* url;
    int sizeOptions;
    int posOptions;
    Option** options;
} Action;

//  Structure permettant de garder en mémoire un tableau d'actions
typedef struct ArrayAction{
    Action** actions;
    int size;
    int pos;
} ArrayAction;

//  Définition d'une stucture "Tache"
typedef struct Tache{
    char* name;
    int sizeOptions;
    int posOptions;
    Option** options;
} Tache;

//  Définition d'un tableau de tâches
typedef struct ArrayTache{
    Tache** taches;
    int size;
    int pos;
} ArrayTache;

#endif // STRUCT_H_INCLUDED
