
#include "Mathematical.h"

// ReadHardwareRandomNumber()
#include "hardware.h"

// Obtient une valeur al�atoire entre start et end
int32 GetRandomValue(int32 start, int32 end) {
	
	// Plage de valeurs
    int32 range;
	// Nombre al�atoire
    uint32 randomNumber;
	
	// printf("*GetRandomValue()*\n");	
	
	// Initialise la plage de valeurs attendue
    range = end - start + 1;
	// Obtient un nombre al�atoire
    randomNumber = ReadHardwareRandomNumber();
	// Renvoie le r�sultat
    return start + (randomNumber % range);
}