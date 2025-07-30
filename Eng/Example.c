
// Argument definitions
#include "DefinitionsArguments.h"
// LoadCel()
#include "celutils.h"
// printf()
#include "stdio.h"
// SpriteCel
#include "SpriteCel.h"

int32 main() {
    
    // Cel
    CCB *cel = NULL;
    // SpriteCel
    SpriteCel *spriteCel = NULL;
    // AnimSpriteCel instances
    AnimSpriteCel *animSpriteCel1 = NULL;
    AnimSpriteCel *animSpriteCel2 = NULL;
    AnimSpriteCel *animSpriteCel3 = NULL;
    
    // Load the CEL
    // printf("LoadCel()\n");
    cel = LoadCel("image.cel", MEMTYPE_DRAM);
    // If loading fails
    if (cel == NULL) {
        // Return error
        printf("Error <- LoadCel()\n");
        return -1;
    }   
    
    // Create a SpriteCel
    // printf("SpriteCelInitialization()\n");
    spriteCel = SpriteCelInitialization(cel, 14, 14, 9);
    // If initialization fails
    if (spriteCel == NULL) {
        // Return error
        printf("Error <- SpriteCelInitialization()\n");
        return -1;
    }   
    
    // Configure the SpriteCel
    printf("-> SpriteCelFramesConfiguration()\n");
    if (SpriteCelFramesConfiguration(
        spriteCel,
        LIST_START,
        0, 0, 0,
        1, 14, 0,
        2, 28, 0,
        3, 42, 0,
        4, 56, 0,
        5, 70, 0,
        6, 84, 0,
        7, 98, 0,
        8, 112, 0,
        LIST_END
    ) == 0) {
        printf("Error <- SpriteCelFramesConfiguration()\n");
        return -1;
    }   
    
    // Create an AnimSpriteCel
    // printf("AnimSpriteCelInitialization()\n");
    animSpriteCel1 = AnimSpriteCelInitialization(
        spriteCel,
        NORMAL,
        QUARTER,
        INFINITE,
        1,
        0,
        2
    );
    // If initialization fails
    if (animSpriteCel1 == NULL) {
        // Return error
        printf("Error <- AnimSpriteCelInitialization()\n");
        return -1;
    }
    
    // Configure the AnimSpriteCel
    printf("-> AnimSpriteCelStepsConfiguration()\n");
    if (AnimSpriteCelStepsConfiguration(
        animSpriteCel1,
        LIST_START,
        0, 0, -240, NULL,
        1, 1, 4, NULL,
        LIST_END
    ) == 0) {
        printf("Error <- AnimSpriteCelStepsConfiguration()\n");
        return -1;
    }   
    
    // Create a second AnimSpriteCel
    // printf("AnimSpriteCelInitialization()\n");
    animSpriteCel2 = AnimSpriteCelInitialization(
        spriteCel,
        NORMAL,
        HALF,
        INFINITE,
        1,
        0,
        3
    );
    // If initialization fails
    if (animSpriteCel2 == NULL) {
        // Return error
        printf("Error <- AnimSpriteCelInitialization()\n");
        return -1;
    }
    
    // Create a third AnimSpriteCel
    // printf("AnimSpriteCelInitialization()\n");
    animSpriteCel3 = AnimSpriteCelInitialization(
        spriteCel,
        NORMAL,
        HALF,
        INFINITE,
        1,
        2,
        3
    );
    // If initialization fails
    if (animSpriteCel3 == NULL) {
        // Return error
        printf("Error <- AnimSpriteCelInitialization()\n");
        return -1;
    }
    
    // Configure the second AnimSpriteCel
    printf("-> AnimSpriteCelStepsConfiguration()\n");
    if (AnimSpriteCelStepsConfiguration(
        animSpriteCel2,
        LIST_START,
        0, 3, 60, NULL,
        1, 1, 4, NULL,
        2, 3, 0, animSpriteCel3,
        LIST_END
    ) == 0) {
        printf("Error <- AnimSpriteCelStepsConfiguration()\n");
        return -1;
    }   
    
    // Configure the third AnimSpriteCel
    printf("-> AnimSpriteCelStepsConfiguration()\n");
    if (AnimSpriteCelStepsConfiguration(
        animSpriteCel3,
        LIST_START,
        0, 3, 60, NULL,
        1, 1, 4, NULL,
        2, 8, 0, animSpriteCel2,
        LIST_END
    ) == 0) {
        printf("Error <- AnimSpriteCelStepsConfiguration()\n");
        return -1;
    }   
    
    // Main loop until interrupted
    while (1) {
        // Run animations
        AnimSpriteCelRun(animSpriteCel1);
        AnimSpriteCelRun(animSpriteCel2);
        AnimSpriteCelRun(animSpriteCel3);
    }
    
    // Clean up AnimSpriteCels
    AnimSpriteCelCleanup(animSpriteCel1);
    AnimSpriteCelCleanup(animSpriteCel2);
    AnimSpriteCelCleanup(animSpriteCel3);
    
    // Clean up SpriteCel
    SpriteCelCleanup(spriteCel);
    
    // Unload Cel
    UnloadCel(cel);
    
    // Exit program
    return 0;
}
