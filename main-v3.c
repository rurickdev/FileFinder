#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>
#include <ctype.h>
#include <pwd.h>

#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/**********************************************
*                                             *
*  Seccion dedicada a la de claracion         *
*  de variables globales y funciones          *
*											  *
**********************************************/

//Declaracion de funciones
void comprobarModificadores(char modificadores[]);
void listarModificadores(char mod, int cant);
void comprobarArgumentos(int numMods);
void leerArchivos(char ruta[]);
void manejarError(const char *s);
void mensajeAyuda(int a);

//Variables Globales
int numArgs;
char listaModificadores[5];
char *listaArgumentos[5];

/**********************************************
*                                             *
*  		  Seccion de la funcion Main()        *
*                                             *
**********************************************/

//Inicio de la funcion "main"
int main(int argc, char * argv[]) {

  //Conocer el numero de argumentos
  numArgs=argc-3;

  char ruta[PATH_MAX+1];

  //Error cuando no hay argumentos
  if(argc==1){
    printf("No hay argumentos, usar \"./main --help\" para ver la ayuda\n");
  }else{
    strcpy(ruta, argv[1]);
    //Comprobar si se pide ayuda
    if ((strcmp(argv[1],"--help")==0)) {
      mensajeAyuda(7);
    }else{
      //Comprobar que al menos haya 2 argumentos en la entrada standar
      if(argc>3){
        comprobarModificadores(argv[2]);
        //Copiar lista de Argumentos
        for(int i=3;i<argc;i++){
          listaArgumentos[i-3] = argv[i];
        }
        leerArchivos(ruta);
      }else{
        mensajeAyuda(argc);
      }
    }
  }
  return EXIT_SUCCESS;
}

/**********************************************
*                                             *
*      Seccion dedicada al manejo de     		  *
*	   los modificadores y sus argumentos       *
*                                             *
**********************************************/

//Llena la lista con los modificadores y comprueba que sean validos
void comprobarModificadores(char modificadores[]) {

  //Variables de control para los modificadores
  int n=0, t=0, o=0, p=0, e=0, cantMods=0;

  char modificador;

  //El if comprueba que no haya mas de los modificadores maximos
  if(strlen(modificadores)>6){
    mensajeAyuda(3);
  }else{
    //Se recorre la lista de modificadores para comprobar que todos sean validos
    for(int i=1;i<strlen(modificadores);i++){
      //Filtrar modificadores repetidos y guardar la lista de los modificadores
      switch (modificadores[i]) {
        case 'n':{
          if(n==0){
            //Copiamos el modificador al arreglo
            listaModificadores[cantMods]=modificadores[i];
            cantMods++;
          }
          n++;
          break;
        }
        case 't':{
          if(t==0){
            //Copiamos el modificador al arreglo
            listaModificadores[cantMods]=modificadores[i];
            cantMods++;
          }
          t++;
          break;
        }
        case 'o':{
          if(o==0){
            //Copiamos el modificador al arreglo
            listaModificadores[cantMods]=modificadores[i];
            cantMods++;
          }
          o++;
          break;
        }
        case 'p':{
          if(p==0){
            //Copiamos el modificador al arreglo
            listaModificadores[cantMods]=modificadores[i];
            cantMods++;
          }
          p++;
          break;
        }
        case 'e':{
          if(e==0){
            //Copiamos el modificador al arreglo
            listaModificadores[cantMods]=modificadores[i];
            cantMods++;
          }
          e++;
          break;
        }
        default:{
          mensajeAyuda(3);
          break;
        }
      }
    }
    comprobarArgumentos(cantMods);
  }
}

//Funcion para controlar que todos los modificadores tienen su argumento
void comprobarArgumentos(int numMods){
  if(numArgs!=(numMods)){
    mensajeAyuda(5);
  }
}

/**********************************************
*                                             *
* Seccion dedicada a leerlos Archivos del Dir *
*                                             *
**********************************************/

//Abre el directorio y obtiene los nombres de sus archivos
void leerArchivos(char *ruta){

  //Variables para el cambio de ruta
  char *directorio;
  int auxInt;

  //Variables para imprimir ruta absoluta
  long size;
  char buf[PATH_MAX] = "";
  char *ptr;

  size = pathconf(".", _PC_PATH_MAX);

  //Variables de control del while que compr argumentos
  bool control=true;
  int i=0;

  //Apertura de directorio
  DIR *dir;
  struct dirent *info;
  struct stat inf;
  dir=opendir(ruta);chdir(ruta);

  if(dir==NULL){
    mensajeAyuda(4);
  }

  //while para ver archivo por archivo de un directorio
  while((info=readdir(dir))!=NULL){
    //el if excluye los directorio . y .. de la lista
    if((strcmp(info->d_name, ".")!=0) && (strcmp(info->d_name, "..")!=0)){
      stat(info->d_name, &inf);
      //IFs para comprobar si los archivos son Directorios
      if(info->d_type==4){
        //Copiamos ruta para movernos entre directrios
        strcpy(ruta, info->d_name);
        //Llamada de recursividad con la nueva ruta
        leerArchivos(ruta);
      }
      //While para comprobar los argumentos de los modificadores
      while ((control)&&(i<(numArgs))) {
        //Switch que comprueba cada modificador
        switch (listaModificadores[i]) {
          case 'n':{
            //Comprobar tipo (case 'name')
            if((strcmp(info->d_name,listaArgumentos[i]))!=0)
              control=false;
            break;
          }
          case 't':{
            //Comprobar tipo (case 'type')
            int a;
            a=atoi(listaArgumentos[i]);
            if(info->d_type!=a)
              control=false;
            break;
          }
          case 'o':{
            //Comprobar dueño (case 'owner')
            struct passwd *pw = getpwuid(inf.st_uid);
            if((strcmp(pw->pw_name,listaArgumentos[i]))!=0)
              control=false;
            break;
          }
          case 'p':{
            //Comprobar permisos (case 'permisses')
            int permisos = inf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
            char *aux2;
            long octal = strtol(listaArgumentos[i], &aux2, 8);
            if(octal!=permisos)
              control=false;
            break;
          }
          case 'e':{
            //Comprobar tipo (case 'exec')
            printf("Tu comando fue %s \n", listaArgumentos[i]);
            break;
          }
          default:
            break;
        }
        i++;
      }
      //Si se cumplen todos los argumentos miestra la ruta del archivo/directorio
      if((control)&&(i==numArgs)){
        if(info->d_type==4){
          ptr = getcwd(buf, (size_t)size);
          //Imprime ruta de directorio
          printf(ANSI_COLOR_BLUE"%s/%s\n"ANSI_COLOR_RESET, ptr);
        }else{
          ptr = getcwd(buf, (size_t)size);
          //imprime ruta archivo
          printf(ANSI_COLOR_BLUE"%s/%s\n"ANSI_COLOR_RESET, ptr, info->d_name);
        }
      }
      control=true;
      i=0;
    }
  }
  //Cerrar el directorio actual
  closedir(dir);
  //Regresar al directorio padre
  chdir("..");
}

/**********************************************
*                                             *
*  Seccion dedicada al manejo de errores      *
*                                             *
**********************************************/

//Funcion para manejar errores
void manejarError(const char *s) {
  perror(s);
  exit(EXIT_FAILURE);
}

//Funcion con los mensajes de ayuda segun el tipo de error
void mensajeAyuda(int a){

  switch (a) {
    case 1:{
      manejarError("ERROR: Sin ruta o sin argumentos\n");
      break;
    }
    case 2:{
      manejarError("ERROR: Faltan modificadores o argumentos\n");
      break;
    }
    case 3:{
      printf("Modificadores permitidos:\nn = nombre\nt = tipo\no = dueño\np = permisos\ne = ejecutar\n");
      manejarError("ERROR: Modificadores erroneos\n");
      break;
    }
    case 4:{
      manejarError("ERROR: Ruta incorrecta");
      break;
    }
    case 5:{
      manejarError("ERROR: Faltan o son erroneos los argumentos");
      break;
    }
    case 6:{
      manejarError("ERROR: El argumento tipo t debe ser un digito");
      break;
    }
    case 7:{
        printf("Ayuda:\nla sintaxis es la siguiente:\n");
        printf("./buscar.bin [Directorio] [Modificadores] [Valores]\n");
        printf("Ejemplo:\nsearch Documentos -nte midoc.txt o ls -l\n");
        printf("Este ejemplo busca un archivo en la carpeta Documentos llamado midoc.txt de tipo ordinario y lo muestra con ls\n");
        printf("Modificadores:\nn=nombre\nt=tipo\no=dueño\np=permisos\ne=ejecutar");
        printf("Argumentos del modificador t:\n");
        printf("1 = FIFO\n2 = Character Device\n4 = directorio\n6 = Block Device\n8 = Regular\n10 = Link o Liga\n12 = Socket\n0 = Desconocido\n");
        printf("Mostrar este mensaje de ayuda \"./buscar.bin --help\"\n");
    }
    default:{
      manejarError("ERROR:\nLa sintaxis es la siguiente:\n search [Directorio] [Modificadores] [Argumentos]\n");
      break;
    }
  }
}
