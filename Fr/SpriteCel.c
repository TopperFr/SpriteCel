
#include "SpriteCel.h"

// UNDEFINED, LIST_START, LIST_END
#include "DefinitionsArguments.h"
// AllocMem(), FreeMem(), MEMTYPE_DRAM
#include "mem.h"
// CloneCel
#include "celutils.h"
// memset()
#include "string.h"
// printf()
#include "stdio.h"

// Initialisation d'un SpriteCel
SpriteCel *SpriteCelInitialization(CCB *spriteSheet, uint32 sizeX, uint32 sizeY, uint32 framesCount) {

	// SpriteCel
	SpriteCel *spriteCel = NULL;
		
	if (DEBUG_SPRITECEL_INIT == 1) { printf("*SpriteCelInitialization()*\n");	}
	
	// Si le sprite sheet n'existe pas
    if (spriteSheet == NULL){
		// Affiche un message d'erreur
		printf("Error : SpriteSheet unknow.\n");
        return NULL;
	} 	
	
	// Alloue de la mémoire pour le SpriteCel
	spriteCel = (SpriteCel *)AllocMem(sizeof(SpriteCel), MEMTYPE_DRAM);
	// Si c'est un échec
    if (spriteCel == NULL) {
		// Affiche un message d'erreur
        printf("Error : Failed to allocate memory for SpriteCel.\n");
        return NULL;
    }

	// Corrige les paramètres (Taille minimale = 1×1 pixel)
	sizeX = (sizeX > 1) ? sizeX : 1;
	sizeY = (sizeY > 1) ? sizeY : 1;
	
	// CCB du SpriteCel sheet source
	spriteCel->spriteSheet = spriteSheet;
	// Frame affichée
	spriteCel->frameIndex = 0;
	// Nombre total de frames
	spriteCel->framesCount = framesCount;
	// Copie le CCB du sprite sheet
	spriteCel->cel = CloneCel(spriteCel->spriteSheet, CLONECEL_CCB_ONLY);
	// Modifie les valeurs du Cel
	spriteCel->cel->ccb_Width = sizeX;
	spriteCel->cel->ccb_Height = sizeY;
	// Force la lecture des préambules dans le CCB
	spriteCel->cel->ccb_Flags |= CCB_CCBPRE;
	spriteCel->spriteSheet->ccb_Flags |= CCB_CCBPRE;

    // Alloue de la mémoire pour le tableau de coordonnées dans le sprite sheet
    spriteCel->frames = (SpriteCelFrame *)AllocMem(framesCount * sizeof(SpriteCelFrame), MEMTYPE_DRAM);
	// Si c'est un échec
    if (spriteCel->frames == NULL) {
		// Libère la mémoire précédemment allouée
        FreeMem(spriteCel, sizeof(SpriteCel));
		// Affiche un message d'erreur
        printf("Error : Failed to allocate memory for SpriteCel frames.\n");
        return NULL;
    }

    // Initialise les valeurs des frames à 0 
    memset(spriteCel->frames, 0, (size_t)framesCount * sizeof(SpriteCelFrame));

	// Retourne le SpriteCel créé
    return spriteCel;
}

// Configuration d'une frame d'un SpriteCel
int32 SpriteCelFrameConfiguration(SpriteCel *spriteCel, uint32 frameIndex, uint32 positionX, uint32 positionY) {
	
    // Premier préambule du Cel
    uint32 ccb_PRE0;
    // Second préambule du Cel
    uint32 ccb_PRE1;
	// Pointeur source des données de pixels
	char *ccb_SourcePtr;

	// Nombre de bits par pixels (1, 2, 4, 6, 8, 16)
	int32 bpp;
	// Conversion de l'index en bpp
	int32 bppTable[] = { 0, 1, 2, 4, 6, 8, 16, 0 };
	// Index de bppTable
	int32 bppIndex;
	// Décalage des bpp (pour skipX)
	int32 bppShifts[] = { 0, 0, 1, 2, 0, 3, 4, 0 };	
	int32 woffset;

	// Nombre de mots 32 bits à sauter horizontalement
	int32 wordOffset;
	// Reste en bits après wordOffset
	int32 bitOffset;
	// Nombre de pixels à ignorer au début de chaque ligne 
	int32 skipX;
	
	if (DEBUG_SPRITECEL_SETUP == 1) { printf("*SpriteCelFrameConfiguration()*\n"); }	

	// Si le sprite est inconnu
	if (spriteCel == NULL){
		// Retourne une erreur
		printf("Error : SpriteCel unknow.\n");
		return -1;
		
	}
	
	// Si le sprite sheet est inconnu
	if (spriteCel->spriteSheet == NULL){
		// Retourne une erreur
		printf("Error : SpriteCel sheet unknow.\n");
		return -1;
		
	}
	
	// Si le tableau de frames est inconnu
	if (spriteCel->frames == NULL){
		// Retourne une erreur
		printf("Error : SpriteCel frames unknow.\n");
		return -1;
		
	}
	
	// Si le sprite ne comporte pas au moins une frame
	if (spriteCel->framesCount < 1){
		// Retourne une erreur
		printf("Error : SpriteCel needs at least one frame.\n");
		return -1;
		
	}

	// Corrige les paramètres
	if (frameIndex >= spriteCel->framesCount) { 
		// Affiche un avertissement
		printf("Warning : SpriteCel frameIndex %u out of bounds. Clamped to last index.\n", frameIndex);
		// Modifie l'index au dernier disponible
		frameIndex = spriteCel->framesCount - 1; 
	}

	// Récupère les préambules dans le CCB du sprite sheet
    ccb_PRE0 = spriteCel->spriteSheet->ccb_PRE0;
    ccb_PRE1 = spriteCel->spriteSheet->ccb_PRE1;

	// Récupère le pointeur des données de pixels du sprite sheet
	ccb_SourcePtr = (char *)spriteCel->spriteSheet->ccb_SourcePtr;
	
    // Récupère la valeur BPP dans le préambule ccb_PRE0
    bppIndex = ccb_PRE0 & PRE0_BPP_MASK;
	// Détermine le BPP selon la valeur BPP dans le préambule ccb_PRE0
    bpp = bppTable[bppIndex];
	
    //  Woffset (mots de 32 bits par ligne, hors prélecture)
    woffset = (bpp < 8)
        ? ((ccb_PRE1 & PRE1_WOFFSET8_MASK)  >> PRE1_WOFFSET8_SHIFT)
        : ((ccb_PRE1 & PRE1_WOFFSET10_MASK) >> PRE1_WOFFSET10_SHIFT);

	// Calcule l'offset en mots de 32 bits (positionX * bits_par_pixel) / 32
	wordOffset = (positionX * bpp) >> 5;

	// Calcule le skipX en pixels ((positionX * bpp) & 31) >> bppShifts[bppIndex]
	bitOffset = (positionX * bpp) & 31;
	skipX = bitOffset >> bppShifts[bppIndex];

    // Avance du pointeur sur la bonne ligne/pixel
    ccb_SourcePtr = ccb_SourcePtr
		+ wordOffset * sizeof(uint32)
		+ positionY * (woffset + PRE1_WOFFSET_PREFETCH) * sizeof(uint32);

    // Mise à jour de PRE0 (VCNT + SKIPX)
    ccb_PRE0 = (ccb_PRE0 & ~(PRE0_VCNT_MASK | PRE0_SKIPX_MASK))
		| ((spriteCel->cel->ccb_Height - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT)
		| (skipX << PRE0_SKIPX_SHIFT);

    // Mise à jour de PRE1 (TLHPCNT)
    ccb_PRE1 = (ccb_PRE1 & ~PRE1_TLHPCNT_MASK)
		| ((spriteCel->cel->ccb_Width - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT);	
	
	if (DEBUG_SPRITECEL_SETUP == 1) { 
		printf("spriteCel->cel : %p\n", spriteCel->cel);
		printf("spriteCel->cel->ccb_SourcePtr : %p\n", spriteCel->cel->ccb_SourcePtr);
		printf("positionY : %u\n", positionY);
		printf("positionX : %u\n", positionX);
		printf("skipX : %u\n", skipX);
		printf("spriteCel->frames[%u].ccb_PRE0=0x%08X\n", frameIndex, ccb_PRE0);
		printf("spriteCel->frames[%u].ccb_PRE1=0x%08X\n", frameIndex, ccb_PRE1);
	}

    // Stocke les résultats dans la frame
    spriteCel->frames[frameIndex].ccb_PRE0 = ccb_PRE0;
    spriteCel->frames[frameIndex].ccb_PRE1 = ccb_PRE1;
    spriteCel->frames[frameIndex].ccb_SourcePtr = (CelData *)ccb_SourcePtr;
	
	// Si la frame configurée est celle affichée
	if(frameIndex == spriteCel->frameIndex){
		// Mets à jour le CCB principal du sprite
		SpriteCelSetFrame(spriteCel, frameIndex);
	}
	
	// Retourne un succès
	return 1;
}

// Configuration des frames d'un SpriteCel
int32 SpriteCelFramesConfiguration(SpriteCel *spriteCel, int32 start, ...) {

	// Liste des arguments de la fonction
    va_list args;
	// Index de la frame
	uint32 frameIndex = 0;
	// Coordonnées
    uint32 positionX = 0;
	uint32 positionY = 0;
	
	if (DEBUG_SPRITECEL_SETUP == 1) { printf("*SpriteCelFramesConfiguration()*\n"); }		

    // Débute la lecture des arguments
    va_start(args, start);

    // Boucle jusqu'à la valeur sentinelle
    while (1) {

        // Récupère l'index
        frameIndex = va_arg(args, uint32);

        // Si c'est la fin de la liste
        if (frameIndex == LIST_END) {
			// Sort de la boucle
            break;
        }

        // Récupère le reste des arguments
		positionX = va_arg(args, uint32);
		positionY = va_arg(args, uint32);

		// Configuration d'une frame d'un sprite
		if (DEBUG_SPRITECEL_SETUP == 1) { printf("-> SpriteCelFrameConfiguration(%p, %u, %u, %u)\n", spriteCel, frameIndex, positionX, positionY); }
		if (SpriteCelFrameConfiguration(spriteCel, frameIndex, positionX, positionY) < 0) {
			// Fin de la liste des arguments 
			va_end(args);
            // Retourne une erreur
            printf("Error <- SpriteCelFrameConfiguration()\n");
            return -1;
        }
    }

    // Fin de la liste des arguments 
    va_end(args);

    // Retourne un succès
    return 1;
}

// Applique la frame correspondante au SpriteCel
void SpriteCelSetFrame(SpriteCel *spriteCel, uint32 frameIndex) {
	
	if (DEBUG_SPRITECEL_FUNCT == 1) { printf("*SpriteCelSetFrame()*\n"); }	
	
	// Si le sprite n'existe pas
    if (spriteCel == NULL){
		// Affiche une erreur
		printf("Error : SpriteCel unknow.\n");
		return ;	
	} 
	
	// Si le cel du sprite n'existe pas
    if (spriteCel->cel == NULL){
		// Affiche une erreur
		printf("Error : SpriteCel Cel unknow.\n");
		return ;	
	}

	// Si le tableau de frames est inconnu
	if (spriteCel->frames == NULL){
		// Retourne une erreur
		printf("Error : SpriteCel frames unknow.\n");
		return ;
		
	}

	// Corrige les paramètres
	if (frameIndex >= spriteCel->framesCount) { 
		// Affiche un avertissement
		printf("Warning : frameIndex %u out of bounds. Clamped to last index.\n", frameIndex);
		// Modifie l'index au dernier disponible
		frameIndex = spriteCel->framesCount - 1; 
	}
	
    // Mets à jour le CCB principal avec les valeurs précalculées de la frame
	spriteCel->frameIndex = frameIndex;
    spriteCel->cel->ccb_PRE0 = spriteCel->frames[spriteCel->frameIndex].ccb_PRE0;
    spriteCel->cel->ccb_PRE1 = spriteCel->frames[spriteCel->frameIndex].ccb_PRE1;
    spriteCel->cel->ccb_SourcePtr = spriteCel->frames[spriteCel->frameIndex].ccb_SourcePtr;

}

// Passe à la frame suivante du SpriteCel
void SpriteCelNextFrame(SpriteCel *spriteCel){
	
	// Frame cible
	uint32 frameIndex;
	
	if (DEBUG_SPRITECEL_FUNCT == 1) { printf("*SpriteCelNextFrame()*\n"); }	
	
	// Si le sprite n'existe pas
    if (spriteCel == NULL){
		// Affiche une erreur
		printf("Error : SpriteCel unknow.\n");
		return ;	
	} 
	
	// Incrémente l'index
	frameIndex = spriteCel->frameIndex + 1;
	// Si l'index dépasse 
	if (frameIndex >= spriteCel->framesCount) {
		// Reviens à la première frame
		frameIndex = 0;
	}
	
	// Applique la frame correspondante au sprite
	SpriteCelSetFrame(spriteCel, frameIndex);	
}

// Passe à la frame précédente du SpriteCel
void SpriteCelPreviousFrame(SpriteCel *spriteCel){
	
	// Frame cible
	uint32 frameIndex;
	
	if (DEBUG_SPRITECEL_FUNCT == 1) { printf("*SpriteCelPreviousFrame()*\n"); }	
	
	// Si le sprite n'existe pas
    if (spriteCel == NULL){
		// Affiche une erreur
		printf("Error : SpriteCel unknow.\n");
		return ;	
	} 
	
	// Décrémente l'index
	frameIndex = (spriteCel->frameIndex == 0) ? spriteCel->framesCount - 1 : spriteCel->frameIndex - 1;
	
	// Applique la frame correspondante au sprite
	SpriteCelSetFrame(spriteCel, frameIndex);	
}

// Supprime le SpriteCel
int32 SpriteCelCleanup(SpriteCel *spriteCel) {
		
	if (DEBUG_SPRITECEL_CLEAN == 1) { printf("*SpriteCelCleanup()*\n"); }	
	
	// Si le SpriteCel n'existe pas
    if (spriteCel == NULL){
		// Affiche une erreur
		printf("Error : SpriteCel unknow.\n");
		return -1;	
	} 

	// Si il y a un Cel
    if (spriteCel->cel != NULL) {
		// Supprime le Cel du SpriteCel
		DeleteCel(spriteCel->cel);
		spriteCel->cel = NULL;
    }
	
	// Si il y a des frames
    if (spriteCel->frames != NULL) {
		// Libère la mémoire utilisée pour le tableau de frames
        FreeMem(spriteCel->frames, spriteCel->framesCount * sizeof(SpriteCelFrame));
        spriteCel->frames = NULL;
    }
	// Libère la mémoire utilisée pour le SpriteCel
    FreeMem(spriteCel, sizeof(SpriteCel));
	spriteCel->spriteSheet = NULL;
	
	// Finalise le nettoyage
	spriteCel = NULL;

	// Retourne un succès
    return 1;
}