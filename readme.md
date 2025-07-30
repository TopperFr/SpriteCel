# SpriteCel – Sprite Management for 3DO Cel Engine

**Author**: Christophe Geoffroy (Topper)  
**License**: MIT  
**Last Updated**: 2025-07-30

## 🧩 Overview

**SpriteCel** is a module designed to manage sprites based on a sprite sheet using the 3DO Cel Engine. It leverages a list of frames defined by shared dimensions and individual coordinates. Frame display is handled by precomputing the visual section of each frame for efficient rendering.

## ⚠️ Important Notes

- Only **uncoded Cels** are supported.
- After initializing a SpriteCel, it must be cleaned up using `SpriteCelCleanup()`.
- The associated Cel must be deleted **separately**.

## 🏗️ Structures

### `SpriteCelFrame` Structure

- `ccb_PRE0`: Precomputed CCB Preamble 0
- `ccb_PRE1`: Precomputed CCB Preamble 1
- `ccb_SourcePtr`: Precomputed pointer to the start of the image

### `SpriteCel` Structure

- `cel`: Active CCB displayed on screen  
- `spriteSheet`: Reference to original CCB used as sprite sheet  
- `frameIndex`: Current index in the frame array  
- `framesCount`: Total number of frames  
- `frames`: Array of `SpriteCelFrame` with precomputed data

## 🛠️ Core Functions

### `SpriteCelInitialization()`
Initializes a SpriteCel structure, clones the base CCB, and allocates the frames array.

### `SpriteCelFrameConfiguration()`
Calculates PRE0/PRE1 and source pointer for a frame at a given "(x, y)" position in the sprite sheet.

### `SpriteCelFramesConfiguration(...)`
Configures multiple frames in a single pass using variadic arguments.

### `SpriteCelSetFrame()`
Visually applies a frame to the active CCB by copying its preambles and source pointer.

### `AnimSpriteCelNextStep()`
Moves to next animation step. Called by `AnimateSpriteCelRun()` or a trigger.

### `SpriteCelNextFrame()` / `SpriteCelPreviousFrame()`
 Display the next or previous frame based on the current index.

### `SpriteCelCleanup()`
Frees the memory used by the SpriteCel structure.
