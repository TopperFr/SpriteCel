#ifndef SPRITECEL_H
#define SPRITECEL_H

/******************************************************************************
**
**  SpriteCel - Gestion d'un sprite basé sur un sprite sheet (3DO Cel Engine)
**  
**  Auteur : Christophe Geoffroy (Topper) - Licence MIT
**  Dernière mise à jour : 30-07-2025
**
**  Ce module fournit une structure de gestion de sprite à partir d'un Cel utilisée
**  en tant que spritesheet. Une liste de frames est définie par une dimension commune
**  et des coordonnées propre à chaque frame. Le module s'occupe de précalculer les 
**  informations correspondant à la section de l'image à afficher pour chaque frame.
**  Des fonctions permettent d'afficher une frame à partir de son index, passer à 
**  la suivante ou précédente.
**
**  Notes importantes :
**
**    - Seuls les Cels uncoded peuvent être utilisés.
**
**    - Lorsqu'un SpriteCel est initialisé, il doit être supprimé avec SpriteCelCleanup().
**      Le Cel qui lui a été associé doit être supprimé indépendamment.
**
**  Rôle des structures :
**
**    SpriteCelFrame
**      - ccb_PRE0 : Préambule 0 du CCB précalculé
**      - ccb_PRE1 : Préambule 1 du CCB précalculé
**      - ccb_SourcePtr : pointeur vers le début de l'image précalculé
**
**    SpriteCel
**      - cel : CCB actif affiché à l'écran
**      - spriteSheet : référence au CCB d'origine contenant le Cel faisant office de sprite sheet
**      - frameIndex : index courant dans le tableau de frames
**      - framesCount : nombre total de frames disponibles
**      - frames : tableau de SpriteCelFrame avec informations précalculées
**
**  Fonctions principales :
**
**    SpriteCelInitialization()
**      -> Initialise une structure SpriteCel, clone le CCB de base, et alloue le tableau de frames.
**
**    SpriteCelFrameConfiguration()
**      -> Calcule PRE0/PRE1 et le pointeur source pour une frame à partir d'une position "(x, y)" dans le sprite sheet.
**
**    SpriteCelFramesConfiguration()
**      -> Configure plusieurs frames en une seule passe avec des arguments variadiques.
**
**    SpriteCelSetFrame()
**      -> Applique visuellement une frame au CCB actif en recopiant ses préambules et pointeur source.
**
**    SpriteCelNextFrame() / SpriteCelPreviousFrame()
**      -> Permettent d'afficher la frame suivante ou précédente en fonction de l'index courant.
**
**    SpriteCelCleanup()
**      -> Libère la mémoire utilisée par la structure SpriteCel.
**
******************************************************************************/


// CCB
#include "graphics.h"
// int32
#include "types.h"

// Debug
#define DEBUG_SPRITECEL_INIT 0
#define DEBUG_SPRITECEL_SETUP 0
#define DEBUG_SPRITECEL_FUNCT 0
#define DEBUG_SPRITECEL_CLEAN 0

typedef struct {
	// Préambule 0 : encode la hauteur, le format, etc.
    uint32 ccb_PRE0;
	// Préambule 1 : encode la largeur, offset de ligne, etc.
    uint32 ccb_PRE1;       
	// Pointeur absolu sur le début de l'image/frame dans le sprite sheet
    CelData *ccb_SourcePtr;
} SpriteCelFrame;

typedef struct {
	// CCB principal du sprite
	CCB *cel;
	// CCB du SpriteCel sheet source
	CCB *spriteSheet;
	// Frame affichée
	uint32 frameIndex;
	// Nombre total de frames
    uint32 framesCount;
	// Tableau de frames (valeurs précalculées)
    SpriteCelFrame *frames;
} SpriteCel;

// Référence au contexte global
extern SpriteCel spriteCel;

// Initialisation d'un SpriteCel
SpriteCel *SpriteCelInitialization(CCB *spriteSheet, uint32 sizeX, uint32 sizeY, uint32 framesCount);
// Configuration d'une frame d'un SpriteCel
int32 SpriteCelFrameConfiguration(SpriteCel *spriteCel, uint32 frameIndex, uint32 positionX, uint32 positionY);
// Configuration des frames d'un SpriteCel
int32 SpriteCelFramesConfiguration(SpriteCel *spriteCel, int32 start, ...);
// Applique la frame correspondante au SpriteCel
void SpriteCelSetFrame(SpriteCel *spriteCel, uint32 frameIndex);
// Passe à la frame suivante du SpriteCel
void SpriteCelNextFrame(SpriteCel *spriteCel);
// Passe à la frame précédente du SpriteCel
void SpriteCelPreviousFrame(SpriteCel *spriteCel);
// Supprime le SpriteCel
int32 SpriteCelCleanup(SpriteCel *spriteCel);

#endif // SPRITECEL_H