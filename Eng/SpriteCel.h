#ifndef SPRITECEL_H
#define SPRITECEL_H

/******************************************************************************
**
**  SpriteCel – Sprite management based on a sprite sheet (3DO Cel Engine)
**  
**  Author: Christophe Geoffroy (Topper) – MIT License
**  Last Updated: 2025-07-30
**
**  This module provides a sprite management structure using a Cel treated
**  as a sprite sheet. A list of frames is defined by shared dimensions and
**  individual coordinates per frame. The module precomputes the necessary
**  information for each frame’s visual section. Functions allow displaying
**  a frame by index and navigating to the next or previous frame.
**
**  Important Notes:
**
**    - Only uncoded Cels can be used.
**
**    - Once a SpriteCel is initialized, it must be deleted with SpriteCelCleanup().
**      The associated Cel must be deleted separately.
**
**  Role of Structures:
**
**    SpriteCelFrame
**      - ccb_PRE0: Precomputed CCB preamble 0
**      - ccb_PRE1: Precomputed CCB preamble 1
**      - ccb_SourcePtr: Precomputed pointer to the start of the image
**
**    SpriteCel
**      - cel: Active CCB displayed on screen
**      - spriteSheet: Reference to the original CCB containing the Cel used as a sprite sheet
**      - frameIndex: Current index in the frames array
**      - framesCount: Total number of available frames
**      - frames: Array of SpriteCelFrame with precomputed data
**
**  Main Functions:
**
**    SpriteCelInitialization()
**      -> Initializes a SpriteCel structure, clones the base CCB, and allocates the frames array.
**
**    SpriteCelFrameConfiguration()
**      -> Calculates PRE0/PRE1 and source pointer for a frame at a given "(x, y)" position in the sprite sheet.
**
**    SpriteCelFramesConfiguration()
**      -> Configures multiple frames in a single pass using variadic arguments.
**
**    SpriteCelSetFrame()
**      -> Visually applies a frame to the active CCB by copying its preambles and source pointer.
**
**    SpriteCelNextFrame() / SpriteCelPreviousFrame()
**      -> Display the next or previous frame based on the current index.
**
**    SpriteCelCleanup()
**      -> Frees the memory used by the SpriteCel structure.
**
******************************************************************************/

// CCB
#include "graphics.h"
// int32
#include "types.h"

// Debug flags
#define DEBUG_SPRITECEL_INIT 0
#define DEBUG_SPRITECEL_SETUP 0
#define DEBUG_SPRITECEL_FUNCT 0
#define DEBUG_SPRITECEL_CLEAN 0

typedef struct {
    // Preamble 0: encodes height, format, etc.
    uint32 ccb_PRE0;
    // Preamble 1: encodes width, line offset, etc.
    uint32 ccb_PRE1;       
    // Absolute pointer to the start of the frame/image in the sprite sheet
    CelData *ccb_SourcePtr;
} SpriteCelFrame;

typedef struct {
    // Main CCB for the sprite
    CCB *cel;
    // CCB from the source sprite sheet
    CCB *spriteSheet;
    // Displayed frame
    uint32 frameIndex;
    // Total number of frames
    uint32 framesCount;
    // Frame array (precomputed values)
    SpriteCelFrame *frames;
} SpriteCel;

// Reference to the global context
extern SpriteCel spriteCel;

// Initialization of a SpriteCel
SpriteCel *SpriteCelInitialization(CCB *spriteSheet, uint32 sizeX, uint32 sizeY, uint32 framesCount);
// Configuration of a single frame in a SpriteCel
int32 SpriteCelFrameConfiguration(SpriteCel *spriteCel, uint32 frameIndex, uint32 positionX, uint32 positionY);
// Configuration of multiple frames in a SpriteCel
int32 SpriteCelFramesConfiguration(SpriteCel *spriteCel, int32 start, ...);
// Apply the corresponding frame to the SpriteCel
void SpriteCelSetFrame(SpriteCel *spriteCel, uint32 frameIndex);
// Switch to the next frame of the SpriteCel
void SpriteCelNextFrame(SpriteCel *spriteCel);
// Switch to the previous frame of the SpriteCel
void SpriteCelPreviousFrame(SpriteCel *spriteCel);
// Delete the SpriteCel
int32 SpriteCelCleanup(SpriteCel *spriteCel);

#endif // SPRITECEL_H