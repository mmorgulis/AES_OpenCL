/* 
	Nella libreria Botan le chiavi sono passate come
	stato interno dell'oggetto. Su OpenCL questa cosa
	non � molto possibile in quanto non � presente una
	reale astrazione di classi/oggetti. 
	La struttura che ho pensato � quindi di creare le chiavi
	sul device (all'interno del main) tramite la libreria 
	botan, inserirle nel contesto di un safe_vector (un vettore
	con un allocator che azzera la memoria prima di uscire) e 
	passarle (come vettori di uint8(=byte)) al kernel come parametro.
	
*/


// Passo la macro del .cpp per rendere il codice pi� snello	
#if defined(AES_128)
    #define AES_KEY_SIZE 16

#elif defined(AES_192)
    #define AES_KEY_SIZE 24

#elif defined(AES_256)
    #define AES_KEY_SIZE 32

#endif

/*
Passo completamente l'input (puntatore e dimensione=num blocchi) e l'output 
(puntatore) su cui scrivere e la chiave (puntatore ad un array)
*/
__kernel void encrypt_n(__global uint8 *input, uint8 num_blocks, __global uint8 *output,
						__global uint8 *round_key) {


}

__kernel void decrypt_n(__global uint8 *input, uint8 num_blocks, __global uint8 *output,
						__global uint8 *round_key) {


}
