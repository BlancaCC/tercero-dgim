
// g++ -std=c++11 -pthread -I. -o exe fumadores-plantilla.cpp Semaphore.cpp

#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;


// Variables compartidas y valores iniciales

Semaphore mostr_vacio( 1 ); //1 si mostrador vacío, 0 si ocupado
Semaphore fumadores[3] = {0, 0, 0};
Semaphore estanquero( 0 ); // a 0 porque al principio no hay ningun ingrediente creado

int ingrediente_producido = 0; // aqui indicara el proveedor que ingrediente ha producido

//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)
//----------------------------------------------------------------------

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

//----------------------------------------------------------------------
// función que ejecuta la hebra del proveedor

void funcion_hebra_proveedor(  )
{
  while ( true ) {
    sem_wait( mostr_vacio );
    ingrediente_producido = aleatorio<0,2>();
    cout << "Proveedor provee ingrediente: " << ingrediente_producido << endl << flush;
    sem_signal( estanquero );
  }
}

//----------------------------------------------------------------------
// función que ejecuta la hebra del estanquero

void funcion_hebra_estanquero( )
{
    while( true ) {
        sem_wait( estanquero );
        cout << "Estanquero produce ingrediente:  " << ingrediente_producido << endl << flush ;
        sem_signal( fumadores[ingrediente_producido] );
    }
}

//-------------------------------------------------------------------------
// Función que simula la acción de fumar, como un retardo aleatoria de la hebra

void fumar( int num_fumador )
{

   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_fumar( aleatorio<20,200>() );

   // informa de que comienza a fumar

    cout << "Fumador " << num_fumador << "  :"
          << " empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl;

   // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
   this_thread::sleep_for( duracion_fumar );

   // informa de que ha terminado de fumar

    cout << "Fumador " << num_fumador << "  : termina de fumar, comienza espera de ingrediente." << endl;

}

//----------------------------------------------------------------------
// función que ejecuta la hebra del fumador
void  funcion_hebra_fumador( int num_fumador )
{
   while( true )
   {
     sem_wait( fumadores[num_fumador] );
     cout << "Fumador " << num_fumador << " retira ingrediente:  " << num_fumador << endl << flush ;
     sem_signal( mostr_vacio );
     fumar( num_fumador );
   }
}

//----------------------------------------------------------------------

int main()
{
   cout << "--------------------------------------------------------" << endl
        << "Problema de los fumadores." << endl
        << "--------------------------------------------------------" << endl
        << flush ;

   thread proveedor ( funcion_hebra_proveedor ),
          estanquero ( funcion_hebra_estanquero ),
          fumador0( funcion_hebra_fumador, 0 ),
          fumador1( funcion_hebra_fumador, 1 ),
          fumador2( funcion_hebra_fumador, 2 );

    proveedor.join() ;
   estanquero.join() ;
   fumador0.join() ;
   fumador1.join() ;
   fumador2.join() ;


}
