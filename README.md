# rkdbg - A Graphical Debug Overlay

`rkdbg` is a graphical debug overlay to be used with any graphics related programs. When working on projects using raylib, I keep finding myself making one of these. So, I thought I might spend some time to make a bit of a library for myself to use instead.

![[image]](resources/screenshot000.png)

This is a header-only library, so there's no need for precompiling when using the library yourself!

## Example

This example can be found [here](examples/example.c)

```c
#define RK_DBG_IMPLEMENTATION
#include "../rkdbg/rkdbg.h"

#include <raylib.h>

int main(void)
{
    const int32_t fontSize = 20;
    const char *text = "Press [F3] to toggle debug overlay";

    InitWindow(1024, 512, "rkdbg - example");
    SetTargetFPS(60);

    // Create a new debug overlay with a specified font size
    rkDebugOverlay *const debugOverlay = rkCreateDebugOverlay(fontSize);

    bool debug = false;
    while (!WindowShouldClose())
    {
        // Toggle debug mode on/off
        if (IsKeyPressed(KEY_F3))
        {
            debug = !debug;
        }

        // If in debug mode, add some text to the debug overlay
        if (debug)
        {
            rkAddDebugTextFormat(debugOverlay, "Some General Information:");
            rkAddDebugTextFormat(debugOverlay, "- Resolution: %dx%d", GetScreenWidth(), GetScreenHeight());
            rkAddDebugTextFormat(debugOverlay, "- %d FPS (%.2fms)", GetFPS(), GetFrameTime() * 1000.0f);
        }

        BeginDrawing();
        ClearBackground(BLACK);

        const int32_t x = (GetScreenWidth() - MeasureText(text, fontSize)) / 2;
        const int32_t y = (GetScreenHeight() - fontSize) / 2;
        DrawText(text, x, y, fontSize, WHITE);

        // If in debug mode...
        if (debug)
        {
            const size_t length = rkDebugOverlaySize(debugOverlay);
            
            // Iterate over all the debug texts...
            for (size_t i = 0; i < length; i++)
            {
                const rkDebugText debugText = rkDebugTextAt(debugOverlay, i);
                // And then draw it to the window!
                DrawText(debugText.text, debugText.position.x, debugText.position.y, fontSize, GREEN);
            }

            // ALSO DONT FORGET THIS! After every frame we MUST clear the debug overlay
            rkClearDebugOverlay(debugOverlay);
        }

        EndDrawing();
    }

    // Finally, free the overlay after use
    rkFreeDebugOverlay(debugOverlay);
    CloseWindow();
}
```

To compile this example; just run `make` in your terminal (in the examples directory)