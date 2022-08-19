/*
**  Filename : main.c
**
**  Made by : 3IABD
**
**  Description : Webscrapper, download the content of a webpage
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl.h>
#include "struct.h"
#include <time.h>
#include<sys/types.h>
#include<sys/stat.h>

//déclaration fonctions
ArrayAction* getActions(ArrayAction*, char*);
ArrayTache* getTaches(ArrayTache*, char*);
ArrayAction* createArray(int, int);
void freeArray(ArrayAction*);
ArrayAction* addAction(ArrayAction*, char*, char*);
void displayArray(ArrayAction*);
void displayArrayTache(ArrayTache*);
ArrayAction* addOpt(ArrayAction*, int, char*, char*);
int indexOf(char*, int, int, char*);
char* getPartOfString(char*, int, int);
void egal(char*, char*);
char** getKeyValue(char*, int);
ArrayTache* createArrayTache(int, int);
void freeArrayTache(ArrayTache*);
ArrayTache* addTache(ArrayTache*, char*);
ArrayTache* addOptTache(ArrayTache*, int, char*, char*);
void log(Tache*, Action*, char*, char*);
int curling(Tache*, Action*, char*);
int getStart(Tache*, Action*);
int getOption(Action*, char*);
char* getDateHour(char, char, char);
char** parseMultipleOptions(char*);
int checkUrl(char *url);


int main()
{
    //  Chemin du fichier de config
    char filepPath[] = "config.sconf";

    //  Création d'une structure contenant toutes les actions du fichier de config
    ArrayAction* arrayAction = createArray(1,1);
    arrayAction = getActions(arrayAction, filepPath);

    //  Création d'une structure contenant toutes les tâches du fichier de config
    ArrayTache* arrayTache = createArrayTache(1,1);
    arrayTache = getTaches(arrayTache, filepPath);

    //  Affichage des actions
    displayArray(arrayAction);

    //Interroge l'utilisateur sur l'URL à aspirer
    char* urlEntry;
    //printf("\n Entrer URL à aspirer : ");
    //scanf("%c", urlEntry);
    urlEntry = "kjshdkjsqhdkjhf";
    while(checkUrl(urlEntry) != 1){
        printf("\n URL incorrecte, recommencer :");
        scanf("%c", urlEntry);
    }

    // Interroge l'utilisateur sur les actions à effectuer
    char indexAction[] = "00";
    do{
    printf("\nChoisir une action a executer : ");
    scanf("%s",indexAction);
    }while (atoi(indexAction) < 0 || atoi(indexAction) > arrayAction->pos);

    //  Lance curl
    int result = getStart(NULL, arrayAction->actions[atoi(indexAction)-1]);

    //  Libère les structure d'actions et de tâches
    freeArray(arrayAction);
    freeArrayTache(arrayTache);

    return 0;
}

int getStart(Tache* tch, Action* act){
    char** types;
    int result = 0;
    int index = getOption(act, "type");
    if(index != -1){
        types = parseMultipleOptions(act->options[index]->value);
        int x = atoi(types[0]);
        for(int a = 0; a < x; a++){
            if(a != 0){
                result = curling(NULL, act, types[a]);
            }
            free(types[a]);
        }
        free(types);
    } else {
        result = curling(NULL, act, "text/html");
    }
    return result;
}

//  Renvoie un tableau avec toutes les valeurs contenues dans une chaîne de caractère séparé par une virgule
char** parseMultipleOptions(char* types){

    int y = 0;
    int length = 0;
    int x = 1;
    while(types[length] != 0){
        length++;
    }

    while((y = indexOf(types, length, y+1, ",")) != - 1){
        x++;
    }
    x++;
    char** result = malloc(sizeof(char*) * x);
    for(int o = 0; o < x; o++){
        result[o] = malloc(sizeof(char) * 2048);
        for(int l = 0; l < 2048; l++){
            result[o][l] = 0;
        }
    }
    sprintf(result[0], "%d", x);
    x = 0;
    y = 0;
    int r = 1;
    while((y = indexOf(types, length, y+1, ",")) != - 1){
        while(types[x] == ' ' || types[x] == '(' || types[x] == ','){
            x++;
        }
        memcpy(result[r], &types[x], y-x);
        x = y;
        r++;
    }
    while(types[x] == ' ' || types[x] == ','){
        x++;
    }
    int u = x;
    while(types[u] != ')'){
        u++;
    }
    memcpy(result[r], &types[x], u - x);

    return result;
}

int curling(Tache* tch, Action* act, char* type){
    //  Variables de la fonction
    int result = 0;
    CURL *curl;
    CURLcode res;
    FILE* file;
    char *ct = NULL;
    //CURL
    curl = curl_easy_init(); // Initialise et retourne NULL en cas d'erreur
    if( curl != NULL ){
        //  Initialisation options curl
        curl_easy_setopt(curl,CURLOPT_URL, act->url);
        curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,0);
        curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,0);
        res  = curl_easy_perform(curl); //Recupère le contenu
        //res == 0 si c'est ok
        if(!res) {
            printf("file ok\n");
            //  Recupère le type de contenu
            res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);

            printf("Content-Type: %s\n", ct);
            //  Vérification d'erreur
            if (strstr(ct, type) != 0) {
                // Créé les répertoires et le fichier qui recevra le contenu de curl
                char* fileName = "index";
                char extension[50];
                int b = 0;
                for(b = 0; b < 50; b++){
                    extension[b] = 0;
                }
                b= 0;
                while(type[b] != 0){
                    b++;
                }
                int indexSlash = indexOf(type, b, 0, "/") + 1;
                memcpy(extension, &type[indexSlash], b - indexSlash);
                printf("%s", extension);
                char* fullPath = malloc(sizeof(char) * 2048);
                for(int u = 0; u < 2048; u++){
                    fullPath[u] = 0;
                }
                char* time = getDateHour('_', ' ', '-');
                strcat(fullPath, act->name);
                //  Création d'un répertoire avec le nom de l'action executée
                mkdir(fullPath, 0700);
                strcat(fullPath, "/");
                strcat(fullPath, time);
                free(time);
                //  Création d'un sous répertoire avec la date et l'heure du moment de l'exécution
                mkdir(fullPath, 0700);
                strcat(fullPath, "/");
                strcat(fullPath, fileName);
                strcat(fullPath, ".");
                strcat(fullPath, extension);
                //  Création du fichier de réception
                file = fopen(fullPath, "wb");
                if ( file != NULL ) {
                    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
                    res = curl_easy_perform(curl); //   Recupère le contenu
                    printf("file write\n");
                    fclose(file);
                    curl_easy_cleanup(curl);
                    //  Ecrit dans le fichier de log, si l'option est indiquée
                    if(getOption(act, "versioning") != -1){
                        if(strcmp(act->options[getOption(act, "versioning")]->value, "on") == 0){
                            log(tch, act, "successful", fullPath);
                        }
                    }
                }
                //  Libère la mémoire
                free(fullPath);
                result = 15;
            }
            result = 5;
        } else {
            log(tch, act, "Error Curl", "");
            result = 10;
        }
    }else{
        printf("Error Curl");
        log(tch, act, "Error Curl", "");
        result = 50;
    }

    return result;
}

//  L'index de l'option si celle-ci existe
int getOption(Action* act, char* name){
    int result = -1;
    for(int i = 0; i < act->posOptions; i++){
        if(strcmp(act->options[i]->name, name) == 0){
            result = i;
            break;
        }
    }

    return result;
}

//  Renvoie la date et l'heure au moment de l'exécution
char* getDateHour(char sepDate, char sep, char sepHour){

    time_t rawtime;
    struct tm* timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    char* result = malloc(sizeof(char) * 17);

    //  Récupère le jour, si celui est inférieur à 10 ajoute 0 devant
    char buff[2];
    sprintf(buff, "%d", timeinfo->tm_mday);
    if(timeinfo->tm_mday < 10){
        result[0] = '0';
        result[1] = buff[0];
    } else {
        result[0] = buff[0];
        result[1] = buff[1];
    }
    result[2] = sepDate;

    //  Récupère le mois, si celui est inférieur à 10 ajoute 0 devant
    sprintf(buff, "%d", timeinfo->tm_mon);
    if(timeinfo->tm_mon < 10){
        result[3] = '0';
        result[4] = buff[0];
    } else {
        result[3] = buff[0];
        result[4] = buff[1];
    }
    result[5] = sepDate;

    //  Récupère l'année
    sprintf(buff, "%d", timeinfo->tm_year - 100);
    if(timeinfo->tm_year - 100 < 10){
        result[6] = '0';
        result[7] = buff[0];
    } else {
        result[6] = buff[0];
        result[7] = buff[1];
    }
    result[8] = sep;

    //  Récupère l'heure
    sprintf(buff, "%d", timeinfo->tm_hour);
    if(timeinfo->tm_hour < 10){
        result[9] = '0';
        result[10] = buff[0];
    } else {
        result[9] = buff[0];
        result[10] = buff[1];
    }
    result[11] = sepHour;

    //  Récupère les minutes
    sprintf(buff, "%d", timeinfo->tm_min);
    if(timeinfo->tm_min < 10){
        result[12] = '0';
        result[13] = buff[0];
    } else {
        result[12] = buff[0];
        result[13] = buff[1];
    }
    result[14] = sepHour;

    //  Récupère les secondes
    sprintf(buff, "%d", timeinfo->tm_sec);
    if(timeinfo->tm_sec < 10){
        result[15] = '0';
        result[16] = buff[0];
    } else {
        result[15] = buff[0];
        result[16] = buff[1];
    }

    return result;
}

//  Ouvre le flux d'écriture du fichier de log
void log(Tache* tache, Action* action, char* result, char* resultPath){
    FILE* log;
    log = fopen("log.txt", "a");

    //  Si l'action provient d'une tâche, écrit le nom de la tâche
    if(tache != NULL){
        int j = 0;
        while(tache->name[j] != 0 && j < 1024){
            j++;
        }
        fwrite(tache->name, j, 1, log);
        fwrite(" -> ", 4, 1, log);
    }
    int i = 0;
    //  Ecrit le nom de l'action dans le fichier de log
    while(action->name[i] != 0 && i < 1024){
        i++;
    }
    fwrite(action->name, i, 1, log);
    fwrite(" (", 2,1,log);

    //  Ecrit l'url dans le fichier de log
    while(action->url[i] != 0 && i < 1024){
        i++;
    }
    fwrite(action->url,i,1,log);
    fwrite(")", 1,1,log);
    fwrite(" : ", 3, 1, log);

    //  Ecrit la date et l'heure dans le fichier de log
    char* date = getDateHour('/', ' ', ':');
    fwrite(date, 17, 1, log);
    free(date);

    fwrite(" | ", 3, 1, log);
    i = 0;
    while(result[i] != 0 && i < 1024){
        i++;
    }
    fwrite(result, i, 1, log);


    //  Le chemin du fichier généré
    if(resultPath != NULL){
        fwrite(" -> ", 4, 1, log);

        i = 0;
        while(resultPath[i] != 0 && i < 1024){
            i++;
        }
        fwrite(resultPath, i, 1, log);
    }

    fwrite("\n", 1, 1, log);
    fclose(log);    // Fermeture du flux
}

//  Renvoie toutes les tâches présentes dans le fichier de config
ArrayTache* getTaches(ArrayTache* array, char* filePath){
    //  Ouvre le flux d'écriture du fichier de log
    FILE* conf;
    conf = fopen(filePath, "r");

    //  retourne NULL, si le fichier n'existe pas
    if(conf == NULL){
        return NULL;
    }

    //  Initialisation du buffer qui servira à récupérer la ligne lue
    int bufferSize = 2048;
    char* buffer = malloc(sizeof(char) * bufferSize);
    for(int i = 0; i < bufferSize; i++){
        buffer[i] = 0;
    }

    int readTache = 0;
    int numTache = -1;

    //  Récupère la ligne active du fichier
    while((fgets(buffer, bufferSize, conf)) != NULL){
        //  Si la ligne commence par "==" alors
        if(buffer[0] == '=' && buffer[1] == '='){
            //  Incremente le nombre de tâche
            numTache++;
            //  Passe à la ligne d'après
            fgets(buffer, bufferSize, conf);

            char* name = malloc(sizeof(char) * bufferSize);

            //  Récupère le nom de la tâche
            char** option = getKeyValue(buffer, bufferSize);
            if(option == NULL){
                return NULL;
            }

            //  Vérifie qu'il s'agit bien du nom
            if(indexOf(option[0], bufferSize, 0, "name") != -1){
                name = option[1];
            } else {
                return NULL;
            }

            readTache = 1;

            //  Ajoute la tâche dans l'array principal
            array = addTache(array, name);

            free(option[0]);
            free(option[1]);
            free(option);

            //  Tant qu'il n'y a pas de saut de ligne on reste dans la même tâche
            while((fgets(buffer, bufferSize, conf)) != NULL && buffer[0] != '\n' && buffer[0] != '='){
                //  Si on rencontre un '+' on passe dans les actions que la tâche execute
                if(buffer[0] == '+'){
                    readTache = 0;
                    fgets(buffer, bufferSize, conf);
                }
                if(readTache == 1){
                    //  Si nous sommes dans les options de la tâche on récupère le nom et la valeur de l'option
                    char** option = getKeyValue(buffer, bufferSize);

                    if(option == NULL){
                        return NULL;
                    }
                    //  Ajout de l'option à la tâche
                    array = addOptTache(array, numTache, option[0], option[1]);
                    free(option[0]);
                    free(option[1]);
                    free(option);
                } else {

                    char** parsed = parseMultipleOptions(buffer);

                    int x = atoi(parsed[0]);
                    for(int a = 0; a < x; a++){
                        if(a != 0){
                            array = addOptTache(array, numTache, "action", parsed[a]);
                        }
                        free(parsed[a]);
                    }
                    free(parsed);
                }
            }
        }
    }

    free(buffer);
    fclose(conf);

    //  Retourne le tableau contenant toutes les tâches
    return array;
}

//  On fait la même chose pour les actions
ArrayAction* getActions(ArrayAction* array, char* filePath){
    FILE* conf;
    conf = fopen(filePath, "r");

    if(conf == NULL){
        return NULL;
    }

    int bufferSize = 2048;
    char* buffer = malloc(sizeof(char) * bufferSize);
    for(int i = 0; i < bufferSize; i++){
        buffer[i] = 0;
    }
    int readAction = 0;
    int numAction = -1;

    while((fgets(buffer, bufferSize, conf)) != NULL){
        if(buffer[0] == '=' && buffer[1] != '='){
            numAction++;
            fgets(buffer, bufferSize, conf);

            char* name = malloc(sizeof(char) * bufferSize);

            char** option = getKeyValue(buffer, bufferSize);
            if(option == NULL){
                return NULL;
            }

            if(indexOf(option[0], bufferSize, 0, "name") != -1){
                name = option[1];
            } else {
                return NULL;
            }

            fgets(buffer, bufferSize, conf);
            option = getKeyValue(buffer, bufferSize);

            if(indexOf(option[0], bufferSize, 0, "url") == -1){
                return NULL;
            }

            readAction = 1;

            array = addAction(array, name, option[1]);

            free(option[0]);
            free(option[1]);
            free(option);
        }
        if(buffer[0] == '+' && readAction == 1){
            while((fgets(buffer, bufferSize, conf)) != NULL && buffer[0] != '\n' && buffer[0] != '='){
                char** option = getKeyValue(buffer, bufferSize);

                if(option == NULL){
                    return NULL;
                }

                array = addOpt(array, numAction, option[0], option[1]);
                free(option[0]);
                free(option[1]);
                free(option);
            }
            readAction = 0;
        }
    }

    free(buffer);
    fclose(conf);

    return array;
}

//  Permet de récupèrer le nom et la valeur d'une option
char** getKeyValue(char* str, int size){

    char** result = malloc(sizeof(char*) * 2);

    int keyStart = 0;
    int keyStop = indexOf(str, size, 0, "->");
    int valueStart = keyStop + 2;
    int valueStop = indexOf(str, size, keyStop, "}");

    if(keyStop == -1 || valueStop == -1){
        return NULL;
    }

    while(str[keyStart] == ' ' || str[keyStart] == '{'){
        keyStart++;
    }
    while(str[keyStop] == ' ' || str[keyStop] == '-'){
        keyStop--;
    }
    keyStop++;
    while(str[valueStart] == ' '){
        valueStart++;
    }

    int sizeKey = keyStop - keyStart;
    int sizeValue = valueStop - valueStart;

    if(sizeKey <= 0 || sizeValue <= 0){
        return NULL;
    }

    char* key = malloc(sizeof(char) * (sizeKey + 1));
    memcpy(key, &str[keyStart], sizeKey);
    key[sizeKey] = 0;

    char* value = malloc(sizeof(char) * (sizeValue + 1));
    memcpy(value, &str[valueStart], sizeValue);
    value[sizeValue] = 0;

    result[0] = key;
    result[1] = value;
    return result;
}

//  Permet de récupérer l'index d'une sous-chaîne, renvoie -1 si inexistant
int indexOf(char* source, int size, int begin, char* find){
    int sizeFind = 0;
    int index = -1;
    while(find[sizeFind] != 0){
        sizeFind++;
    }
    int i = begin;
    while(i < size - sizeFind && index == -1){
        int same = 0;
        for(int j = 0; j < sizeFind; j++){
            if(source[i+j] == find[j]){
                same++;
            }
        }
        if(same == sizeFind){
            index = i;
        }
        i++;
    }

    return index;
}

//  Affiche les actions contenues dans le tableau arr passé en paramètre
void displayArray(ArrayAction* arr){
    for(int i = 0; i < arr->pos; i++){
        printf("%d)\n", i+1);
        printf("%s -> %s\n", arr->actions[i]->name, arr->actions[i]->url);
        for(int j = 0; j < arr->actions[i]->posOptions; j++){
            printf("    %s : %s\n", arr->actions[i]->options[j]->name, arr->actions[i]->options[j]->value);
        }
        printf("\n\n\n\n");
    }
}

//  Affiche les tâches
void displayArrayTache(ArrayTache* arr){
    for(int i = 0; i < arr->pos; i++){
        printf("%s\n", arr->taches[i]->name);
        for(int j = 0; j < arr->taches[i]->posOptions; j++){
            printf("    %s : %s\n", arr->taches[i]->options[j]->name, arr->taches[i]->options[j]->value);
        }
        printf("\n\n");
    }
}

//  Crée un tableau dynamique qui contiendra toutes les actions présentes dans le fichier de config
ArrayAction* createArray(int nbAct, int nbOpt){

    int sizeChar = 2048;

    ArrayAction* arr = malloc(sizeof(ArrayAction));
    arr->size = nbAct;
    arr->pos = 0;
    arr->actions = malloc(sizeof(Action**) * nbAct);
    for(int i = 0; i < nbAct; i++){
        arr->actions[i] = malloc(sizeof(Action));
        arr->actions[i]->name = malloc(sizeof(char) * sizeChar);
        arr->actions[i]->url = malloc(sizeof(char) * sizeChar);
        for(int x = 0; x < sizeChar; x++){
            arr->actions[i]->name[x] = 0;
            arr->actions[i]->url[x] = 0;
        }
        arr->actions[i]->sizeOptions = nbOpt;
        arr->actions[i]->posOptions = 0;
        arr->actions[i]->options = malloc(sizeof(Option**) * nbOpt);
        for(int j = 0; j < nbOpt; j++){
            arr->actions[i]->options[j] = malloc(sizeof(Option));
            arr->actions[i]->options[j]->name = malloc(sizeof(char) * sizeChar);
            arr->actions[i]->options[j]->value = malloc(sizeof(char) * sizeChar);
            for(int x = 0; x < sizeChar; x++){
                arr->actions[i]->options[j]->name[x] = 0;
                arr->actions[i]->options[j]->value[x] = 0;
            }
        }
    }

    return arr;
}

//  Crée un tableau dynamique qui contiendra toutes les tâches présentes dans le fichier de config
ArrayTache* createArrayTache(int nbTch, int nbOpt){

    int sizeChar = 2048;

    ArrayTache* arr = malloc(sizeof(ArrayTache));
    arr->size = nbTch;
    arr->pos = 0;
    arr->taches = malloc(sizeof(Tache**) * nbTch);
    for(int i = 0; i < nbTch; i++){
        arr->taches[i] = malloc(sizeof(Tache));
        arr->taches[i]->name = malloc(sizeof(char) * sizeChar);
        for(int x = 0; x < sizeChar; x++){
            arr->taches[i]->name[x] = 0;
        }
        arr->taches[i]->sizeOptions = nbOpt;
        arr->taches[i]->posOptions = 0;
        arr->taches[i]->options = malloc(sizeof(Option**) * nbOpt);
        for(int j = 0; j < nbOpt; j++){
            arr->taches[i]->options[j] = malloc(sizeof(Option));
            arr->taches[i]->options[j]->name = malloc(sizeof(char) * sizeChar);
            arr->taches[i]->options[j]->value = malloc(sizeof(char) * sizeChar);
            for(int x = 0; x < sizeChar; x++){
                arr->taches[i]->options[j]->name[x] = 0;
                arr->taches[i]->options[j]->value[x] = 0;
            }
        }
    }

    return arr;
}

//  Libère le tableau contenant toutes les actions
void freeArray(ArrayAction* arr){
    for(int i = 0; i < arr->size; i++){
        for(int j = 0; j < arr->actions[i]->sizeOptions; j++){
            free(arr->actions[i]->options[j]->name);
            free(arr->actions[i]->options[j]->value);
            free(arr->actions[i]->options[j]);
        }
        free(arr->actions[i]->options);
        free(arr->actions[i]->name);
        free(arr->actions[i]->url);
        free(arr->actions[i]);
    }
    free(arr->actions);
    free(arr);
}

//  Libère le tableau contenant toutes les tâches
void freeArrayTache(ArrayTache* arr){
    for(int i = 0; i < arr->size; i++){
        for(int j = 0; j < arr->taches[i]->sizeOptions; j++){
            free(arr->taches[i]->options[j]->name);
            free(arr->taches[i]->options[j]->value);
            free(arr->taches[i]->options[j]);
        }
        free(arr->taches[i]->options);
        free(arr->taches[i]->name);
        free(arr->taches[i]);
    }
    free(arr->taches);
    free(arr);
}

//  Ajoute une action au super tableau
ArrayAction* addAction(ArrayAction* arr, char* name, char* url){
    int pos = arr->pos;
    int size = arr->size;
    //  Double la taille du tableau si sa limite est atteinte
    if(pos >= size){
        ArrayAction* result = createArray(size * 2, arr->actions[0]->sizeOptions);
        result->pos = pos;
        result->size = size * 2;
        for(int i = 0; i < size; i++){
            egal(result->actions[i]->name, arr->actions[i]->name);
            egal(result->actions[i]->url, arr->actions[i]->url);
            result->actions[i]->posOptions = arr->actions[i]->posOptions;
            result->actions[i]->sizeOptions = arr->actions[i]->sizeOptions;
            for(int j = 0; j < arr->actions[i]->posOptions; j++){
                egal(result->actions[i]->options[j]->name, arr->actions[i]->options[j]->name);
                egal(result->actions[i]->options[j]->value, arr->actions[i]->options[j]->value);
            }
        }

        egal(result->actions[pos]->name, name);
        egal(result->actions[pos]->url, url);
        result->pos = pos + 1;

        freeArray(arr);

        return result;
    } else {
        egal(arr->actions[pos]->name, name);
        egal(arr->actions[pos]->url, url);
        arr->pos++;

        return arr;
    }
}

//  Ajoute une tâche au super tableau
ArrayTache* addTache(ArrayTache* arr, char* name){
    int pos = arr->pos;
    int size = arr->size;
    if(pos >= size){
        ArrayTache* result = createArrayTache(size * 2, arr->taches[0]->sizeOptions);
        result->pos = pos;
        result->size = size * 2;
        for(int i = 0; i < size; i++){
            egal(result->taches[i]->name, arr->taches[i]->name);
            result->taches[i]->posOptions = arr->taches[i]->posOptions;
            result->taches[i]->sizeOptions = arr->taches[i]->sizeOptions;
            for(int j = 0; j < arr->taches[i]->posOptions; j++){
                egal(result->taches[i]->options[j]->name, arr->taches[i]->options[j]->name);
                egal(result->taches[i]->options[j]->value, arr->taches[i]->options[j]->value);
            }
        }

        egal(result->taches[pos]->name, name);
        result->pos = pos + 1;

        freeArrayTache(arr);

        return result;
    } else {
        egal(arr->taches[pos]->name, name);
        arr->pos++;

        return arr;
    }
}

//  Ajoute une option au super tableau
ArrayAction* addOpt(ArrayAction* arr, int indexAction, char* name, char* value){
    int pos = arr->actions[indexAction]->posOptions;
    int size = arr->actions[indexAction]->sizeOptions;
    if(pos >= size){
        ArrayAction* result = createArray(arr->size, size * 2);
        result->pos = arr->pos;
        result->size = arr->size;
        for(int i = 0; i < arr->pos; i++){
            egal(result->actions[i]->name, arr->actions[i]->name);
            egal(result->actions[i]->url, arr->actions[i]->url);
            result->actions[i]->posOptions = arr->actions[i]->posOptions;
            result->actions[i]->sizeOptions = size * 2;
            for(int j = 0; j < size; j++){
                egal(result->actions[i]->options[j]->name, arr->actions[i]->options[j]->name);
                egal(result->actions[i]->options[j]->value, arr->actions[i]->options[j]->value);
            }
        }

        egal(result->actions[indexAction]->options[pos]->name ,name);
        egal(result->actions[indexAction]->options[pos]->value, value);
        result->actions[indexAction]->posOptions = pos + 1;

        freeArray(arr);

        return result;
    } else {

        egal(arr->actions[indexAction]->options[pos]->name, name);
        egal(arr->actions[indexAction]->options[pos]->value, value);
        arr->actions[indexAction]->posOptions++;

        return arr;
    }
}

ArrayTache* addOptTache(ArrayTache* arr, int indexTache, char* name, char* value){
    int pos = arr->taches[indexTache]->posOptions;
    int size = arr->taches[indexTache]->sizeOptions;
    if(pos >= size){
        ArrayTache* result = createArrayTache(arr->size, size * 2);
        result->pos = arr->pos;
        result->size = arr->size;
        for(int i = 0; i < arr->pos; i++){
            egal(result->taches[i]->name, arr->taches[i]->name);
            result->taches[i]->posOptions = arr->taches[i]->posOptions;
            result->taches[i]->sizeOptions = size * 2;
            for(int j = 0; j < size; j++){
                egal(result->taches[i]->options[j]->name, arr->taches[i]->options[j]->name);
                egal(result->taches[i]->options[j]->value, arr->taches[i]->options[j]->value);
            }
        }

        egal(result->taches[indexTache]->options[pos]->name ,name);
        egal(result->taches[indexTache]->options[pos]->value, value);
        result->taches[indexTache]->posOptions = pos + 1;

        freeArrayTache(arr);

        return result;
    } else {

        egal(arr->taches[indexTache]->options[pos]->name, name);
        egal(arr->taches[indexTache]->options[pos]->value, value);
        arr->taches[indexTache]->posOptions++;

        return arr;
    }
}

void egal(char* target, char* source){
    int i = 0;
    while(source[i] != 0 && i < 2048){
        target[i] = source[i];
        i++;
    }
}

int checkUrl(char *url){
    CURL *curl;
    CURLcode response;
    curl = curl_easy_init();                                //  Ouvre la connexion pour CURL

    if(curl){                                               // Vérifie si la connexion est ouverte

        curl_easy_setopt(curl, CURLOPT_URL, url);

        curl_easy_setopt(curl, CURLOPT_NOBODY, 1);          // Ajoute une option pour ne pas télécharger la page

        response = curl_easy_perform(curl);                 // Exécute la requête

        curl_easy_cleanup(curl);                            // Ferme les connexions curl ouvertes
    }

    if(response == CURLE_OK){
        return 1;
    }
    else{
        return 0;
    }
}
