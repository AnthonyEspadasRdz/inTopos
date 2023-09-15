#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>

typedef struct _nodo NODO;

struct _nodo{
    bool operador;
    char caracter;
    double valor;
    int prioridad;
    NODO* siguiente;
};

struct _pila{
    NODO* head;
    NODO* tail;
    int num;
};

typedef struct _pila PILA;
typedef struct _pila COLA;

PILA* crear_pila();
NODO* crear_nodo(int prioridad, char caracter, double valor, bool operador);
bool push(PILA* pila, int prioridad, char caracter, double valor, bool operador);
bool pop(PILA* pila);
bool encolar(COLA* cola, int prioridad, char caracter, double valor, bool operador);
bool es_vacia(PILA *pila);
bool vaciar(PILA *pila);
void eliminar_pila(PILA *pila);
int determinar_prioridad(char caracter);
void imprimir_cola(COLA* cola);
void evaluar_cadena(COLA* cola);
bool guardar_cadena(COLA* cola);
bool leer_valores(COLA* cadena);

int main(int argc, char *argv[])
{
    //Iniciamos el arreglo que guaradrá el valor de la cadena infija
    char infija[150];

    // Abrimos flujo para tomar los datos del archivo
    FILE* flujo_infija = fopen(argv[1], "r");

    if (flujo_infija == NULL)
    {
        printf("No se encontró el archivo %s\n", argv[1]);
        return 0;
    }

    //Usamos un while para leer los caracteres
    int pos = 0;
    while(feof(flujo_infija) == 0)
    {
        fscanf(flujo_infija, "%c", &infija[pos]);
        pos++;
    }

    printf("La cadena infija en el archivo es: %s\n", infija);
    fclose(flujo_infija);    
    
    //Auxiliares para la creacion de los nodos
    char operador;
    int prioridad;
    //auxiliar para guardar cadena
	PILA* Operadores = crear_pila();
    COLA* Posfija = crear_pila();

    //Comenzamos a recorrer la cadena
    for (int i = 0; i < strlen(infija); i++)
    {
        // Si hay espacio en la cadena original, estos no se procesan
        while(isblank(infija[i]))
            i++;
        // Las variables pasan directo a la cadena posfija
        if (isalnum(infija[i]))
        {
            //Una vez con el número completo, ese valor se guarda en el nodo
            encolar(Posfija, 0, infija[i], 0, false);
            i++;
        }
        
        // Se vuelven a omitir los espacio de la cadena
        while(isblank(infija[i]))
            i++;
        
        // Pasamos a evaluar los operadores

        //Determinamos la prioridad
        operador = infija[i];
        prioridad = determinar_prioridad(operador);

        if (es_vacia(Operadores))
        {
            push(Operadores, prioridad, operador, 0, true);
        }
        // El parentesis de apertura se coloca sin medir prioridad
        else if (operador == '(')
        {
            push(Operadores, prioridad, operador, 0, true);
        }
        // En los parentesis de cierra vaciamos la pila
        else if (operador == ')')
        { // Vaciamos los operadores hasta encontrar un "("
            while (Operadores->head->caracter != '(')
            {
                encolar(Posfija, Operadores->head->prioridad, Operadores->head->caracter, 0, true);
                pop(Operadores);
            }
            // Eliminamos el "("
            pop(Operadores);
        }
        else // Colocamos los casos en los que se validan operadores
        {    // Si tiene mayor prioridad se agrega directamente
            if (prioridad > Operadores->head->prioridad)
            {
                push(Operadores, prioridad, operador, 0, true);
            }
            /* En los casos de mayor o igual prioridad se sacan
                operadores hasta encontrae uno con mayor prioridad*/
            else
            {
                while (prioridad <= Operadores->head->prioridad)
                {
                    encolar(Posfija, Operadores->head->prioridad, Operadores->head->caracter, 0, true);
                    pop(Operadores);
                    // Si la lista se vacia, terminamos el ciclo
                    if (es_vacia(Operadores))
                        break;
                }
                // Una vez fuera los operadores mayores introducimos el nuevo
                push(Operadores, prioridad, operador, 0, true);
            }
        }
    }

    // Se sacan los operadores restantes de la pila
    while (!es_vacia(Operadores))
    {
        encolar(Posfija, Operadores->head->prioridad, Operadores->head->caracter, 0, true);
        pop(Operadores);
    }

    printf("\nLa cadena posfija es: ");
    imprimir_cola(Posfija);

    // El usuario elige acción a realziar con la cadena
    int eleccion = 0;
    printf("\nQue accion quieres realizar?\n(1) Guardar cadena\n(2) Evaluar Cadena\n(3) Guardar y evaluar\n\n");
    scanf("%d", &eleccion);

    if (eleccion == 1 || eleccion == 3)
        guardar_cadena(Posfija);
    if (eleccion == 2 || eleccion == 3)
        evaluar_cadena(Posfija);
    
	eliminar_pila(Operadores);
	eliminar_pila(Posfija);

    return 0;
}

PILA* crear_pila()
{
    PILA* nueva_pila = (PILA*) malloc(sizeof(PILA));
    nueva_pila -> head = nueva_pila -> tail = NULL;
    nueva_pila -> num = 0; 
    return nueva_pila;
}

NODO* crear_nodo(int prioridad, char caracter, double valor, bool operador)
{
    NODO *nuevo_nodo  = (NODO*) malloc(sizeof(NODO));
    nuevo_nodo -> siguiente = NULL;
    nuevo_nodo -> prioridad = prioridad;
    nuevo_nodo -> caracter = caracter;
    nuevo_nodo -> valor = valor;
    nuevo_nodo -> operador = operador;
    return nuevo_nodo;
}

bool push(PILA *pila, int prioridad, char caracter, double valor, bool operador)
{
    NODO* nuevo = crear_nodo(prioridad, caracter, valor, operador);
    
    if (pila -> num == 0)
    {
        pila -> head = pila -> tail = nuevo;
    }
    else
    {
        nuevo -> siguiente = pila -> head;
        pila -> head = nuevo;
    }
    pila -> num++;
    
    return true;
}

bool pop(PILA *pila)
{
    if (pila -> num == 0) return false;
    
    NODO *temporal = pila -> head;
    
    if (pila -> num == 1)
        pila -> head = pila -> tail = NULL;
    else
        pila -> head = pila -> head -> siguiente;
    
    temporal -> siguiente = NULL; 
    free(temporal);
    pila -> num--;
    
    return true;
}

bool encolar(COLA* cola, int prioridad, char caracter, double valor, bool operador)
{
    NODO* temporal = crear_nodo(prioridad, caracter, valor, operador);

    if (cola -> num == 0)
    {
        cola -> head = cola -> tail = temporal;
        cola -> num++;
    }
    else
    {
        cola -> tail -> siguiente = temporal;
        cola -> tail = temporal;
        cola -> num++;
    }

    return true;
}

bool es_vacia(PILA *pila){
    if(pila->head == NULL && pila->tail == NULL) return true;
    return false;
}

bool vaciar(PILA *pila){
    if(es_vacia(pila)) return false;
    NODO* temporal = pila->head;
    while(temporal != NULL){
        pila->head = temporal->siguiente;
        temporal -> siguiente = NULL;
        free(temporal);
        temporal = pila->head;
    }
    pila->head = pila->tail = NULL;
    pila->num = 0;
    return true;
}

void eliminar_pila(PILA *pila){
    if(pila == NULL) return;
    if(!es_vacia(pila)){
        vaciar(pila);
    }
    free(pila);
    pila = NULL;
}

void imprimir_cola(COLA* cola)
{
    NODO *temporal = cola -> head;

    if (es_vacia(cola))
    {
        printf("NULL");
        return;
    }

    if (cola -> num > 1)
    {
        while(temporal -> siguiente != NULL)
        {
            printf("%c ", temporal -> caracter);
            
            temporal = temporal -> siguiente;
        }
    } 
    printf("%c\n", temporal -> caracter);
}

int determinar_prioridad(char caracter)
{
    int prioridad = 0;
    
    switch (caracter)
    {
        case '(':
            prioridad = 0;
            break;
        case ')':
            prioridad = 0;
            break;
        case '+':
            prioridad = 1;
            break;
        case '-':
            prioridad = 1;
            break;
        case '*':
            prioridad = 2;
            break;
        case '/':
            prioridad = 2;
            break;
        case '%':
            prioridad = 2;
        case '^':
            prioridad = 3;
            break;
        default:
            break;
    }

    return prioridad;
}

void evaluar_cadena(COLA* cola)
{   
    // Pila que almacenara los valores con los que se haran operaciones
    PILA* evaluacion = crear_pila();
    // Pila para guardar los valores a usar
    COLA* variables = crear_pila();
    // Registrar respuesta del usuario
    int metodo = 0;
    // Guarda valor para los operandos
    double operando_auxiliar = 0;
    // Para evaluar los nodos con mismos caracteres
    bool repetido = false;

    printf("\nAgregar valores desde: (1) Archivo (2) Consola\n\n");
    scanf("%i", &metodo);

    if (metodo == 1)
        leer_valores(variables);

    for (NODO* auxiliar = cola -> head; auxiliar -> siguiente != NULL; auxiliar = auxiliar -> siguiente)
    {
        // Si es un número se agrega a la pila
        if (auxiliar -> operador == false)
        {
            if (variables -> num > 0)
            {   // Revismamos si ya se tiene registrada esa variable
                for (NODO* verificador = variables -> head; verificador != NULL; verificador = verificador -> siguiente)
                {
                    if (auxiliar -> caracter == verificador -> caracter)
                    {   // Si ya existe, se asigna valor sin preguntarlo
                        auxiliar -> valor = verificador -> valor;
                        repetido = true;
                        break;
                    }
                }
            }

            if (repetido == false)
            {   
                // Si la variable no se encuentra registrada se pregunta al usuario por su valor
                if (metodo == 1)
                    printf("\nValor para '%c' no encontrado\n", auxiliar -> caracter);
                
                printf("Introduce valor para %c: ", auxiliar -> caracter);
                scanf("%lf", &auxiliar -> valor);
                // Se agrega el nuevo valor a la cola
                encolar(variables, auxiliar -> prioridad, auxiliar -> caracter, auxiliar -> valor, auxiliar -> operador);
            }
            
            push(evaluacion, auxiliar -> prioridad, auxiliar -> caracter, auxiliar -> valor, auxiliar -> operador);
            repetido = false;
        }
        // Si es un operador, se efectua la operacion
        else 
        {
            // Se guarda el valor del segundo operando
            operando_auxiliar = evaluacion -> head -> valor;
            // Se elimina de la pila
            pop(evaluacion);

            // Se cambia el valor del primer operando segun la operacion
            switch (auxiliar -> caracter)
            {
                case '+':
                    evaluacion -> head -> valor += operando_auxiliar;
                    break;
                case '-':
                    evaluacion -> head -> valor -= operando_auxiliar;
                    break;
                case '*':
                    evaluacion -> head -> valor = (evaluacion -> head -> valor)*(operando_auxiliar);
                    break;
                case '/':
                    evaluacion -> head -> valor = (evaluacion -> head -> valor) / (operando_auxiliar);
                    break;
                case '%':
                    evaluacion -> head -> valor = fmod(evaluacion -> head -> valor, operando_auxiliar);
                    break;
                case '^':
                    evaluacion -> head -> valor = pow(evaluacion -> head -> valor, operando_auxiliar);
                    break;
            }
        }
    }

    // Si el resultado no cuenta con parte decimal se imprime sólo el entero
    if (fmod(evaluacion -> head -> valor, 1) == 0)
        printf("\nLa evaluacion de la cadena posfija es: %.0f\n", evaluacion -> head -> valor);
    else
        printf("\nLa evaluacion de la cadena posfija es: %.2f\n", evaluacion -> head -> valor);

    eliminar_pila(evaluacion);
    eliminar_pila(variables);
}

bool guardar_cadena(COLA* cola)
{
    char archivo[50];
    fflush(stdin);
    printf("\nIngresa el nombre del archivo donde se guardara la cadena: ");
    scanf("%s", &archivo);

    FILE* flujo_posfija = fopen(archivo, "a");

    if (flujo_posfija == NULL)
    {
        printf("\nError generando el archivo\n");
        return false;
    }

    for (NODO* auxiliar = cola -> head; auxiliar -> siguiente != NULL; auxiliar = auxiliar -> siguiente)
    {
        fprintf(flujo_posfija, "%c ", auxiliar -> caracter);
    }

    // En caso de que en el archivo se guarde más de una cadena
    fprintf(flujo_posfija, "\n");

    printf("\nCadena guardada con exito en %s\n", archivo);
    fclose(flujo_posfija);
}

bool leer_valores(COLA* cadena)
{
    char archivo[50];
    fflush(stdin);
    printf("\nIngresa el nombre del archivo: ");
    scanf("%s", &archivo);

    // Auxiliares en la creacion de nodos
    char caracter = '0';
    char igual = '0';
    double valor = 0;

    FILE* flujo_respuestas = fopen(archivo, "r");

    if (flujo_respuestas == NULL)
    {
        printf("No se encontró el archivo %s", archivo);
        return false;
    }

    while(feof(flujo_respuestas) == 0)
    {
        
        fscanf(flujo_respuestas, "%c %c %lf", &caracter, &igual, &valor);
        // Corroboramos que tengan la estructura de ingreso
        if (isalpha(caracter) && igual == '=')
        {
            if (fmod(valor, 1) == 0)
                printf("El valor de %c es %0.f\n", caracter, valor);
            else 
                printf("El valor de %c es %2.f\n", caracter, valor);

            encolar(cadena, 0, caracter, valor, false);
        }
        // Eliminamos el caracter de escape.
        fscanf(flujo_respuestas, "%c", &caracter);
    }

    fclose(flujo_respuestas);
}