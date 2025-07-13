#ifndef RKDBG_H
#define RKDBG_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

// --- constants --------------------------------------------------------------

#define RK_MAX_DBG_TXT_SIZE 128 // Maximum length for the debug texts

// --- type definitions -------------------------------------------------------

// Struct for the data structure handle
typedef struct rkDebugOverlay rkDebugOverlay;

// Struct detailing the screen xy coordinates for the text to be drawn at
typedef struct rkDebugPosition
{
    int32_t x; // The x-coordinate of the text
    int32_t y; // The y-coordinate of the text
} rkDebugPosition;

// Struct containing the debug text and screen position
typedef struct rkDebugText
{
    char            text[RK_MAX_DBG_TXT_SIZE+1]; // The debug text
    rkDebugPosition position;                    // The position to draw the text at
} rkDebugText;

// --- rkdbg interface --------------------------------------------------------

/**
 * Creates a new debug overlay with the specified font size
 *
 * @param[in] fontSize
 *      The size of the font to draw the debug text with
 *
 * @return
 *      A handle to the debug overlay data structure
 */
rkDebugOverlay *rkCreateDebugOverlay(int32_t fontSize);

/**
 * Frees all resources used by the debug overlay
 *
 * @param[in] debugOverlay
 *      A handle to the debug overlay
 */
void rkFreeDebugOverlay(rkDebugOverlay *debugOverlay);

/**
 * Clears all the elements (debug texts) in the debug overlay, effectively
 * setting the size to zero.
 *
 * NOTE: This has to be called every frame to prevent a memory leak
 *
 * @param[in] debugOverlay
 *      A handle to the debug overlay
 */
void rkClearDebugOverlay(rkDebugOverlay *debugOverlay);

/**
 * Adds new debug text to the debug overlay, in a traditional C-formatted style
 *
 * @param[in] debugOverlay
 *      A handle to the debug overlay
 * @param[in] fmt
 *      The format specifier
 */
void rkAddDebugTextFormat(rkDebugOverlay *debugOverlay, const char *fmt, ...);

/**
 * Adds new debug text to the debug overlay using a variadic argument list handle
 *
 * @param[in] debugOverlay
 *      A handle to the debug overlay
 * @param[in] fmt
 *      The format specifier
 * @param[in] args
 *      The variadic arguments
 */
void rkAddDebugTextArgs(rkDebugOverlay *debugOverlay, const char *fmt, va_list args);

/**
 * Gets the debug text at index `idx`
 *
 * @param[in] debugOverlay
 *      A handle to the debug overlay
 * @param[in] idx
 *      The index of the debug text to get from the overlay
 */
rkDebugText rkDebugTextAt(const rkDebugOverlay *debugOverlay, size_t idx);

/**
 * Gets the number of debug text elements contained within the debug overlay
 *
 * @param[in] debugOverlay
 *      A handle to the debug overlay
 */
size_t rkDebugOverlaySize(const rkDebugOverlay *debugOverlay);

#if defined(RK_DBG_IMPLEMENTATION)

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- useful macros ----------------------------------------------------------

#define C_LITERAL(type) (type)

#define RK_DBG_POS(xp, yp)      \
    C_LITERAL(rkDebugPosition) {\
        .x = (int32_t)(xp),     \
        .y = (int32_t)(yp),     \
    }                           \

// --- constants --------------------------------------------------------------

#define RK_INIT_POS RK_DBG_POS(5, 5) // Starting screen coordinates of the text
#define RK_INIT_CAPACITY 8           // Initial capacity of the overlay

// --- rkdbg implementation ---------------------------------------------------

/**
 * Ensures that there's enough space in `buffer` to contain another debug text
 *
 * @param[in] debugOverlay
 *      The overlay to ensure space for
 */
static void rkDebugOverlayEnsureSpace(rkDebugOverlay *debugOverlay);

/**
 * Creates a new debug text element
 *
 * @param[in] position
 *      The screen position to draw the text at
 * @param[in] fmt
 *      The format specifier
 * @param[in] args
 *      The variadic argument list of the text
 *
 * @return
 *      A new debug text instance
 */
static rkDebugText rkNewDebugText(rkDebugPosition position, const char *fmt, va_list args);

// Definition of the debug overlay struct
struct rkDebugOverlay
{
    rkDebugText    *buffer;       // The buffer containing the debug texts
    size_t          size;         // The current number of elements in `buffer`
    size_t          capacity;     // The current capacity of `buffer`
    rkDebugPosition currPosition; // The position for the next debug text
    int32_t         fontSize;     // The size of the font used
};

rkDebugOverlay *rkCreateDebugOverlay(int32_t fontSize)
{
    rkDebugOverlay *const debugOverlay = (rkDebugOverlay *)calloc(1, sizeof(rkDebugOverlay));
    if (!debugOverlay)
    {
        return NULL;
    }

    debugOverlay->buffer = (rkDebugText *)calloc(RK_INIT_CAPACITY, sizeof(rkDebugText));
    debugOverlay->size = 0;
    debugOverlay->capacity = RK_INIT_CAPACITY;
    debugOverlay->currPosition = RK_INIT_POS;
    debugOverlay->fontSize = fontSize;

    return debugOverlay;
}

void rkFreeDebugOverlay(rkDebugOverlay *debugOverlay)
{
    free(debugOverlay->buffer);
    free(debugOverlay);
}

void rkClearDebugOverlay(rkDebugOverlay *debugOverlay)
{
    debugOverlay->currPosition = RK_INIT_POS;
    debugOverlay->size = 0;
    memset(debugOverlay->buffer, 0x00, debugOverlay->capacity);
}

void rkAddDebugTextFormat(rkDebugOverlay *debugOverlay, const char *fmt, ...)
{
    va_list args = { 0 };

    va_start(args, fmt);
    rkAddDebugTextArgs(debugOverlay, fmt, args);
    va_end(args);
}

void rkAddDebugTextArgs(rkDebugOverlay *debugOverlay, const char *fmt, va_list args)
{
    rkDebugOverlayEnsureSpace(debugOverlay);
    debugOverlay->buffer[(debugOverlay->size)++] = rkNewDebugText(debugOverlay->currPosition, fmt, args);
    debugOverlay->currPosition.y += debugOverlay->fontSize;
}

rkDebugText rkDebugTextAt(const rkDebugOverlay *debugOverlay, size_t idx)
{
    assert(idx < debugOverlay->size);
    return debugOverlay->buffer[idx];
}

size_t rkDebugOverlaySize(const rkDebugOverlay *debugOverlay)
{
    return debugOverlay->size;
}

static void rkDebugOverlayEnsureSpace(rkDebugOverlay *debugOverlay)
{
    if (debugOverlay->size < debugOverlay->capacity)
    {
        return;
    }

    debugOverlay->capacity = 3 * debugOverlay->capacity / 2;
    debugOverlay->buffer = (rkDebugText *)realloc(debugOverlay->buffer, debugOverlay->capacity * sizeof(rkDebugText));
}

static rkDebugText rkNewDebugText(rkDebugPosition position, const char *fmt, va_list args)
{
    rkDebugText debugText = { 0 };

    vsnprintf(debugText.text, RK_MAX_DBG_TXT_SIZE, fmt, args);
    debugText.position = position;

    return debugText;
}

#endif /* RK_DBG_IMPLEMENTATION */

#endif /* RKDBG_H */
