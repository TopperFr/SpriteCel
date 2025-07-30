#include "SpriteCel.h"

// UNDEFINED, LIST_START, LIST_END
#include "DefinitionsArguments.h"
// AllocMem(), FreeMem(), MEMTYPE_DRAM
#include "mem.h"
// CloneCel
#include "celutils.h"
// memset(), memcpy(), strcpy(), strcat(), strlen(), strstr()
#include "string.h"
// printf()
#include "stdio.h"

// Initialization of a SpriteCel
SpriteCel *SpriteCelInitialization(CCB *spriteSheet, uint32 sizeX, uint32 sizeY, uint32 framesCount) {

    // SpriteCel
    SpriteCel *spriteCel = NULL;
        
    if (DEBUG_SPRITECEL_INIT == 1) { printf("*SpriteCelInitialization()*\n");   }
    
    // If the sprite sheet doesn't exist
    if (spriteSheet == NULL){
        // Display an error message
        printf("Error : SpriteSheet unknown.\n");
        return NULL;
    }   
    
    // Allocate memory for the SpriteCel
    spriteCel = (SpriteCel *)AllocMem(sizeof(SpriteCel), MEMTYPE_DRAM);
    // If it fails
    if (spriteCel == NULL) {
        // Display an error message
        printf("Error : Failed to allocate memory for SpriteCel.\n");
        return NULL;
    }

    // Fix parameters (Minimum size = 1×1 pixel)
    sizeX = (sizeX > 1) ? sizeX : 1;
    sizeY = (sizeY > 1) ? sizeY : 1;
    
    // SpriteCel source sheet CCB
    spriteCel->spriteSheet = spriteSheet;
    // Displayed frame
    spriteCel->frameIndex = 0;
    // Total number of frames
    spriteCel->framesCount = framesCount;
    // Copy the CCB from the sprite sheet
    spriteCel->cel = CloneCel(spriteCel->spriteSheet, CLONECEL_CCB_ONLY);
    // Modify Cel values
    spriteCel->cel->ccb_Width = sizeX;
    spriteCel->cel->ccb_Height = sizeY;
    // Force reading of preambles in the CCB
    spriteCel->cel->ccb_Flags |= CCB_CCBPRE;
    spriteCel->spriteSheet->ccb_Flags |= CCB_CCBPRE;

    // Allocate memory for the coordinates array in the sprite sheet
    spriteCel->frames = (SpriteCelFrame *)AllocMem(framesCount * sizeof(SpriteCelFrame), MEMTYPE_DRAM);
    // If it fails
    if (spriteCel->frames == NULL) {
        // Free previously allocated memory
        FreeMem(spriteCel, sizeof(SpriteCel));
        // Display an error message
        printf("Error : Failed to allocate memory for SpriteCel frames.\n");
        return NULL;
    }

    // Initialize frame values to 0 
    memset(spriteCel->frames, 0, (size_t)framesCount * sizeof(SpriteCelFrame));

    // Return the created SpriteCel
    return spriteCel;
}

// Configuration of a SpriteCel frame
int32 SpriteCelFrameConfiguration(SpriteCel *spriteCel, uint32 frameIndex, uint32 positionX, uint32 positionY) {
    
    // First Cel preamble
    uint32 ccb_PRE0;
    // Second Cel preamble
    uint32 ccb_PRE1;
    // Source pointer for pixel data
    char *ccb_SourcePtr;

    // Bits per pixel (1, 2, 4, 6, 8, 16)
    int32 bpp;
    // Conversion index to bpp
    int32 bppTable[] = { 0, 1, 2, 4, 6, 8, 16, 0 };
    // Index in bppTable
    int32 bppIndex;
    // bpp shift values (for skipX)
    int32 bppShifts[] = { 0, 0, 1, 2, 0, 3, 4, 0 }; 
    int32 woffset;

    // Number of 32-bit words to skip horizontally
    int32 wordOffset;
    // Remaining bits after wordOffset
    int32 bitOffset;
    // Number of pixels to skip at the start of each line 
    int32 skipX;
    
    if (DEBUG_SPRITECEL_SETUP == 1) { printf("*SpriteCelFrameConfiguration()*\n"); }    

    // If sprite is unknown
    if (spriteCel == NULL){
        // Return an error
        printf("Error : SpriteCel unknown.\n");
        return -1;
    }
    
    // If sprite sheet is unknown
    if (spriteCel->spriteSheet == NULL){
        // Return an error
        printf("Error : SpriteCel sheet unknown.\n");
        return -1;
    }
    
    // If frame array is unknown
    if (spriteCel->frames == NULL){
        // Return an error
        printf("Error : SpriteCel frames unknown.\n");
        return -1;
    }
    
    // If sprite has less than one frame
    if (spriteCel->framesCount < 1){
        // Return an error
        printf("Error : SpriteCel needs at least one frame.\n");
        return -1;
    }

    // Fix parameters
    if (frameIndex >= spriteCel->framesCount) { 
        // Show warning
        printf("Warning : SpriteCel frameIndex %u out of bounds. Clamped to last index.\n", frameIndex);
        // Adjust index to the last available
        frameIndex = spriteCel->framesCount - 1; 
    }

    // Get preambles from the sprite sheet's CCB
    ccb_PRE0 = spriteCel->spriteSheet->ccb_PRE0;
    ccb_PRE1 = spriteCel->spriteSheet->ccb_PRE1;

    // Get the pixel data pointer from the sprite sheet
    ccb_SourcePtr = (char *)spriteCel->spriteSheet->ccb_SourcePtr;
    
    // Get BPP value from ccb_PRE0 preamble
    bppIndex = ccb_PRE0 & PRE0_BPP_MASK;
    // Determine BPP based on value in ccb_PRE0
    bpp = bppTable[bppIndex];
    
    // woffset (number of 32-bit words per line, excluding prefetch)
    woffset = (bpp < 8)
        ? ((ccb_PRE1 & PRE1_WOFFSET8_MASK)  >> PRE1_WOFFSET8_SHIFT)
        : ((ccb_PRE1 & PRE1_WOFFSET10_MASK) >> PRE1_WOFFSET10_SHIFT);

    // Calculate offset in 32-bit words (positionX * bits_per_pixel) / 32
    wordOffset = (positionX * bpp) >> 5;

    // Calculate skipX in pixels ((positionX * bpp) & 31) >> bppShifts[bppIndex]
    bitOffset = (positionX * bpp) & 31;
    skipX = bitOffset >> bppShifts[bppIndex];

    // Advance pointer to the correct line/pixel
    ccb_SourcePtr = ccb_SourcePtr
        + wordOffset * sizeof(uint32)
        + positionY * (woffset + PRE1_WOFFSET_PREFETCH) * sizeof(uint32);

    // Update PRE0 (VCNT + SKIPX)
    ccb_PRE0 = (ccb_PRE0 & ~(PRE0_VCNT_MASK | PRE0_SKIPX_MASK))
        | ((spriteCel->cel->ccb_Height - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT)
        | (skipX << PRE0_SKIPX_SHIFT);

    // Update PRE1 (TLHPCNT)
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

    // Store results in the frame
    spriteCel->frames[frameIndex].ccb_PRE0 = ccb_PRE0;
    spriteCel->frames[frameIndex].ccb_PRE1 = ccb_PRE1;
    spriteCel->frames[frameIndex].ccb_SourcePtr = (CelData *)ccb_SourcePtr;
    
    // If the configured frame is the displayed one
    if(frameIndex == spriteCel->frameIndex){
        // Update the main sprite CCB
        SpriteCelSetFrame(spriteCel, frameIndex);
    }
    
    // Return success
    return 1;
}

// Configuration of a SpriteCel frame
int32 SpriteCelFrameConfiguration(SpriteCel *spriteCel, uint32 frameIndex, uint32 positionX, uint32 positionY) {
    
    // First Cel preamble
    uint32 ccb_PRE0;
    // Second Cel preamble
    uint32 ccb_PRE1;
    // Source pointer for pixel data
    char *ccb_SourcePtr;

    // Bits per pixel (1, 2, 4, 6, 8, 16)
    int32 bpp;
    // Conversion from index to bpp
    int32 bppTable[] = { 0, 1, 2, 4, 6, 8, 16, 0 };
    // Index in bppTable
    int32 bppIndex;
    // bpp shift values (for skipX)
    int32 bppShifts[] = { 0, 0, 1, 2, 0, 3, 4, 0 }; 
    int32 woffset;

    // Number of 32-bit words to skip horizontally
    int32 wordOffset;
    // Remaining bits after wordOffset
    int32 bitOffset;
    // Number of pixels to skip at the start of each line 
    int32 skipX;
    
    if (DEBUG_SPRITECEL_SETUP == 1) { printf("*SpriteCelFrameConfiguration()*\n"); }    

    // If the sprite is unknown
    if (spriteCel == NULL){
        // Return an error
        printf("Error : SpriteCel unknown.\n");
        return -1;
    }
    
    // If the sprite sheet is unknown
    if (spriteCel->spriteSheet == NULL){
        // Return an error
        printf("Error : SpriteCel sheet unknown.\n");
        return -1;
    }
    
    // If the frame array is unknown
    if (spriteCel->frames == NULL){
        // Return an error
        printf("Error : SpriteCel frames unknown.\n");
        return -1;
    }
    
    // If the sprite does not contain at least one frame
    if (spriteCel->framesCount < 1){
        // Return an error
        printf("Error : SpriteCel needs at least one frame.\n");
        return -1;
    }

    // Fix parameters
    if (frameIndex >= spriteCel->framesCount) { 
        // Display a warning
        printf("Warning : SpriteCel frameIndex %u out of bounds. Clamped to last index.\n", frameIndex);
        // Set index to last available
        frameIndex = spriteCel->framesCount - 1; 
    }

    // Retrieve preambles from sprite sheet's CCB
    ccb_PRE0 = spriteCel->spriteSheet->ccb_PRE0;
    ccb_PRE1 = spriteCel->spriteSheet->ccb_PRE1;

    // Retrieve pixel data pointer from the sprite sheet
    ccb_SourcePtr = (char *)spriteCel->spriteSheet->ccb_SourcePtr;
    
    // Retrieve BPP value from ccb_PRE0 preamble
    bppIndex = ccb_PRE0 & PRE0_BPP_MASK;
    // Determine BPP from preamble value
    bpp = bppTable[bppIndex];
    
    // woffset (32-bit words per line, excluding prefetch)
    woffset = (bpp < 8)
        ? ((ccb_PRE1 & PRE1_WOFFSET8_MASK)  >> PRE1_WOFFSET8_SHIFT)
        : ((ccb_PRE1 & PRE1_WOFFSET10_MASK) >> PRE1_WOFFSET10_SHIFT);

    // Calculate offset in 32-bit words (positionX * bits_per_pixel) / 32
    wordOffset = (positionX * bpp) >> 5;

    // Calculate skipX in pixels ((positionX * bpp) & 31) >> bppShifts[bppIndex]
    bitOffset = (positionX * bpp) & 31;
    skipX = bitOffset >> bppShifts[bppIndex];

    // Advance pointer to correct line/pixel
    ccb_SourcePtr = ccb_SourcePtr
        + wordOffset * sizeof(uint32)
        + positionY * (woffset + PRE1_WOFFSET_PREFETCH) * sizeof(uint32);

    // Update PRE0 (VCNT + SKIPX)
    ccb_PRE0 = (ccb_PRE0 & ~(PRE0_VCNT_MASK | PRE0_SKIPX_MASK))
        | ((spriteCel->cel->ccb_Height - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT)
        | (skipX << PRE0_SKIPX_SHIFT);

    // Update PRE1 (TLHPCNT)
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

    // Store results in the frame
    spriteCel->frames[frameIndex].ccb_PRE0 = ccb_PRE0;
    spriteCel->frames[frameIndex].ccb_PRE1 = ccb_PRE1;
    spriteCel->frames[frameIndex].ccb_SourcePtr = (CelData *)ccb_SourcePtr;
    
    // If the configured frame is the one displayed
    if(frameIndex == spriteCel->frameIndex){
        // Update the main sprite's CCB
        SpriteCelSetFrame(spriteCel, frameIndex);
    }
    
    // Return success
    return 1;
}

// Configuration of a SpriteCel's frames
int32 SpriteCelFramesConfiguration(SpriteCel *spriteCel, int32 start, ...) {

    // Function's argument list
    va_list args;
    // Frame index
    uint32 frameIndex = 0;
    // Coordinates
    uint32 positionX = 0;
    uint32 positionY = 0;
    
    if (DEBUG_SPRITECEL_SETUP == 1) { printf("*SpriteCelFramesConfiguration()*\n"); }       

    // Start reading arguments
    va_start(args, start);

    // Loop until reaching the sentinel value
    while (1) {

        // Retrieve the index
        frameIndex = va_arg(args, uint32);

        // If it's the end of the list
        if (frameIndex == LIST_END) {
            // Exit the loop
            break;
        }

        // Retrieve remaining arguments
        positionX = va_arg(args, uint32);
        positionY = va_arg(args, uint32);

        // Configure a sprite frame
        if (DEBUG_SPRITECEL_SETUP == 1) { printf("-> SpriteCelFrameConfiguration(%p, %u, %u, %u)\n", spriteCel, frameIndex, positionX, positionY); }
        if (SpriteCelFrameConfiguration(spriteCel, frameIndex, positionX, positionY) < 0) {
            // End of argument list
            va_end(args);
            // Return error
            printf("Error <- SpriteCelFrameConfiguration()\n");
            return -1;
        }
    }

    // End of argument list
    va_end(args);

    // Return success
    return 1;
}

// Apply the corresponding frame to the SpriteCel
void SpriteCelSetFrame(SpriteCel *spriteCel, uint32 frameIndex) {
    
    if (DEBUG_SPRITECEL_FUNCT == 1) { printf("*SpriteCelSetFrame()*\n"); }  
    
    // If the sprite doesn't exist
    if (spriteCel == NULL){
        // Show error
        printf("Error : SpriteCel unknown.\n");
        return ;    
    } 
    
    // If the Cel of the sprite doesn't exist
    if (spriteCel->cel == NULL){
        // Show error
        printf("Error : SpriteCel Cel unknown.\n");
        return ;    
    }

    // If the frame array is unknown
    if (spriteCel->frames == NULL){
        // Return error
        printf("Error : SpriteCel frames unknown.\n");
        return ;
    }

    // Correct the parameters
    if (frameIndex >= spriteCel->framesCount) { 
        // Show warning
        printf("Warning : frameIndex %u out of bounds. Clamped to last index.\n", frameIndex);
        // Set index to last available
        frameIndex = spriteCel->framesCount - 1; 
    }
    
    // Update the main CCB with the frame's precomputed values
    spriteCel->frameIndex = frameIndex;
    spriteCel->cel->ccb_PRE0 = spriteCel->frames[spriteCel->frameIndex].ccb_PRE0;
    spriteCel->cel->ccb_PRE1 = spriteCel->frames[spriteCel->frameIndex].ccb_PRE1;
    spriteCel->cel->ccb_SourcePtr = spriteCel->frames[spriteCel->frameIndex].ccb_SourcePtr;

}

// Switch to the next frame of the SpriteCel
void SpriteCelNextFrame(SpriteCel *spriteCel){
    
    // Target frame
    uint32 frameIndex;
    
    if (DEBUG_SPRITECEL_FUNCT == 1) { printf("*SpriteCelNextFrame()*\n"); } 
    
    // If the sprite does not exist
    if (spriteCel == NULL){
        // Show error
        printf("Error : SpriteCel unknown.\n");
        return ;    
    } 
    
    // Increment the index
    frameIndex = spriteCel->frameIndex + 1;
    // If the index exceeds the bounds
    if (frameIndex >= spriteCel->framesCount) {
        // Return to the first frame
        frameIndex = 0;
    }
    
    // Apply the corresponding frame to the sprite
    SpriteCelSetFrame(spriteCel, frameIndex);   
}

// Switch to the previous frame of the SpriteCel
void SpriteCelPreviousFrame(SpriteCel *spriteCel){
    
    // Target frame
    uint32 frameIndex;
    
    if (DEBUG_SPRITECEL_FUNCT == 1) { printf("*SpriteCelPreviousFrame()*\n"); } 
    
    // If the sprite does not exist
    if (spriteCel == NULL){
        // Show error
        printf("Error : SpriteCel unknown.\n");
        return ;    
    } 
    
    // Decrement the index
    frameIndex = (spriteCel->frameIndex == 0) ? spriteCel->framesCount - 1 : spriteCel->frameIndex - 1;
    
    // Apply the corresponding frame to the sprite
    SpriteCelSetFrame(spriteCel, frameIndex);   
}

// Delete the SpriteCel
int32 SpriteCelCleanup(SpriteCel *spriteCel) {
        
    if (DEBUG_SPRITECEL_CLEAN == 1) { printf("*SpriteCelCleanup()*\n"); }   
    
    // If the SpriteCel does not exist
    if (spriteCel == NULL){
        // Show error
        printf("Error : SpriteCel unknown.\n");
        return -1;  
    } 

    // If there is a Cel
    if (spriteCel->cel != NULL) {
        // Delete the SpriteCel's Cel
        DeleteCel(spriteCel->cel);
        spriteCel->cel = NULL;
    }
    
    // If there are frames
    if (spriteCel->frames != NULL) {
        // Free memory used for the frame array
        FreeMem(spriteCel->frames, spriteCel->framesCount * sizeof(SpriteCelFrame));
        spriteCel->frames = NULL;
    }
    // Free memory used for the SpriteCel
    FreeMem(spriteCel, sizeof(SpriteCel));
    spriteCel->spriteSheet = NULL;
    
    // Finalize the cleanup
    spriteCel = NULL;

    // Return success
    return 1;
}
