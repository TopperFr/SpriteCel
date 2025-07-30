
// Définitions des arguments
#include "DefinitionsArguments.h"
// LoadCel()
#include "celutils.h"
// printf()
#include "stdio.h"
// SpriteCel
#include "SpriteCel.h"

int32 main(){
	
	// Cel
	CCB *cel = NULL;
	// SpriteCel
	SpriteCel *spriteCel = NULL;
	// AnimSpriteCel
	AnimSpriteCel *animSpriteCel1 = NULL;
	AnimSpriteCel *animSpriteCel2 = NULL;
	AnimSpriteCel *animSpriteCel3 = NULL;
	
	// Charge le CEL
	// printf("LoadCel()\n");
	cel = LoadCel("image.cel", MEMTYPE_DRAM);
	// Si c'est un échec
	if(cel == NULL){
		// Retourne une erreur
		printf("Error <- LoadCel()\n");
		return -1;
	}	
	
	// Crée un SpriteCel
	// printf("SpriteCelInitialization()\n");
	spriteCel = SpriteCelInitialization(cel, 14, 14, 9);
	// Si c'est un échec
	if(spriteCel == NULL){
		// Retourne une erreur
		printf("Error <- SpriteCelInitialization()\n");
		return -1;
	}	
	
	// Configure le SpriteCel
	printf("-> SpriteCelFramesConfiguration()\n");
	if (SpriteCelFramesConfiguration(
		// SpriteCel
		spriteCel,
		// Début de la liste
		LIST_START,
		// frameIndex, positionX, positionY
		0, 0, 0,
		1, 14, 0,
		2, 28, 0,
		3, 42, 0,
		4, 56, 0,
		5, 70, 0,
		6, 84, 0,
		7, 98, 0,
		8, 112, 0,
		// Fin de la liste
		LIST_END
	) == 0) {
		printf("Error <- SpriteCelFramesConfiguration()\n");
		return -1;
	}	
	
	// Crée un AnimSpriteCel
	// printf("AnimSpriteCelInitialization()\n");
	animSpriteCel1 = AnimSpriteCelInitialization(
		// SpriteCel
		spriteCel,
		// Type de boucle d'animation
		NORMAL,
		// Plage de valeurs de l'aléatoire
		QUARTER,
		// Répétitions du cycle d'animation
		INFINITE,
		// Sens d'animation en cours
		1,
		// Etape en cours
		0,
		// Nombre total d'étapes
		2
	);
	// Si c'est un échec
	if(animSpriteCel1 == NULL){
		// Retourne une erreur
		printf("Error <- AnimSpriteCelInitialization()\n");
		return -1;
	}
	
	// Configure le AnimSpriteCel
	printf("-> AnimSpriteCelStepsConfiguration()\n");
	if (AnimSpriteCelStepsConfiguration(
		// AnimSpriteCel
		animSpriteCel1,
		// Début de la liste
		LIST_START,
		// stepIndex, frameIndex, frameDuration, AnimSpriteCel
		0, 0, -240, NULL,
		1, 1, 4, NULL,
		// Fin de la liste
		LIST_END
	) == 0) {
		printf("Error <- AnimSpriteCelStepsConfiguration()\n");
		return -1;
	}	
	
	// Crée un AnimSpriteCel
	// printf("AnimSpriteCelInitialization()\n");
	animSpriteCel2 = AnimSpriteCelInitialization(
		// SpriteCel
		spriteCel,
		// Type de boucle d'animation
		NORMAL,
		// Plage de valeurs de l'aléatoire
		HALF,
		// Répétitions du cycle d'animation
		INFINITE,
		// Sens d'animation en cours
		1,
		// Etape en cours
		0,
		// Nombre total d'étapes
		3
	);
	// Si c'est un échec
	if(animSpriteCel2 == NULL){
		// Retourne une erreur
		printf("Error <- AnimSpriteCelInitialization()\n");
		return -1;
	}
	
	// Crée un AnimSpriteCel
	// printf("AnimSpriteCelInitialization()\n");
	animSpriteCel3 = AnimSpriteCelInitialization(
		// SpriteCel
		spriteCel,
		// Type de boucle d'animation
		NORMAL,
		// Plage de valeurs de l'aléatoire
		HALF,
		// Répétitions du cycle d'animation
		INFINITE,
		// Sens d'animation en cours
		1,
		// Etape en cours
		2,
		// Nombre total d'étapes
		3
	);
	// Si c'est un échec
	if(animSpriteCel3 == NULL){
		// Retourne une erreur
		printf("Error <- AnimSpriteCelInitialization()\n");
		return -1;
	}
	
	// Configure le AnimSpriteCel
	printf("-> AnimSpriteCelStepsConfiguration()\n");
	if (AnimSpriteCelStepsConfiguration(
		// AnimSpriteCel
		animSpriteCel2,
		// Début de la liste
		LIST_START,
		// stepIndex, frameIndex, frameDuration, AnimSpriteCel
		0, 3, 60, NULL,
		1, 1, 4, NULL,
		2, 3, 0, animSpriteCel3,
		// Fin de la liste
		LIST_END
	) == 0) {
		printf("Error <- AnimSpriteCelStepsConfiguration()\n");
		return -1;
	}	
	
	// Configure le AnimSpriteCel
	printf("-> AnimSpriteCelStepsConfiguration()\n");
	if (AnimSpriteCelStepsConfiguration(
		// AnimSpriteCel
		animSpriteCel3,
		// Début de la liste
		LIST_START,
		// stepIndex, frameIndex, frameDuration, AnimSpriteCel
		0, 3, 60, NULL,
		1, 1, 4, NULL,
		2, 8, 0, animSpriteCel2,
		// Fin de la liste
		LIST_END
	) == 0) {
		printf("Error <- AnimSpriteCelStepsConfiguration()\n");
		return -1;
	}	
	
	// Boucle jusqu'à l'interruption
	while (1) {
		
		// Exécute l'animation
		AnimSpriteCelRun(animSpriteCel1);
		AnimSpriteCelRun(animSpriteCel2);
		AnimSpriteCelRun(animSpriteCel3);
	}
	
	// Supprime les AnimSpriteCels
	AnimSpriteCelCleanup(animSpriteCel1);
	AnimSpriteCelCleanup(animSpriteCel2);
	AnimSpriteCelCleanup(animSpriteCel3);
	
	// Supprime le SpriteCel
	SpriteCelCleanup(spriteCel);
	
	// Supprime le Cel
	UnloadCel(cel);
	
    // Termine le programme
    return 0;
}