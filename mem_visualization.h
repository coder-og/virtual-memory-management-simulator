#ifndef MEM_VISUALIZATION_H
#define MEM_VISUALIZATION_H

#include <raylib.h>
#include <vector>
#include <string>
#include "TLB_table.h"
#include "Page_frame.h"

class MemoryVisualizer {
private:
    int screenWidth;
    int screenHeight;

public:
    MemoryVisualizer(int width = 1200, int height = 800)
        : screenWidth(width), screenHeight(height) {
        InitWindow(screenWidth, screenHeight, "OS Virtual Memory Visualizer");
        SetTargetFPS(1); // High FPS for smooth rendering
    }

    ~MemoryVisualizer() { CloseWindow(); }

    bool IsRunning() { return !WindowShouldClose(); }

    void Render(TLB& tlb, Page_table& pt, bool lastAccessWasHit, FileConfiguration& config, int lastAccessedIndex, std::string algoName) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // --- LEFT PANEL: CONFIGURATION & METRICS ---
        DrawRectangle(10, 10, 350, 780, Fade(LIGHTGRAY, 0.3f));
        DrawRectangleLines(10, 10, 350, 780, DARKGRAY);

        DrawText("ALGORITHM:", 25, 30, 22, BLACK);
        DrawText(algoName.c_str(), 25, 60, 24, MAROON);
        DrawLine(25, 90, 320, 90, DARKGRAY);

        DrawText("CONFIGURATION", 25, 110, 20, BLACK);
        DrawText(TextFormat("RAM Size: %llu B", config.physical_size), 25, 140, 18, DARKBLUE);
        DrawText(TextFormat("Page Size: %llu B", config.page_size), 25, 170, 18, DARKBLUE);
        DrawText(TextFormat("TLB Slots: %llu", config.TLB_size), 25, 200, 18, DARKBLUE);
        DrawText(TextFormat("Latency: %llu ns", config.latency), 25, 230, 18, DARKBLUE);

        DrawText("METRICS", 25, 500, 22, BLACK);
        DrawLine(25, 525, 320, 525, DARKGRAY);
        DrawText(TextFormat("TLB Hits: %d", tlb.tlb_hits), 25, 540, 20, DARKGREEN);
        DrawText(TextFormat("TLB Misses: %d", tlb.tlb_misses), 25, 570, 20, MAROON);
        DrawText(TextFormat("RAM Reads: %llu", pt.ram_read_count), 25, 600, 20, DARKBLUE);
        DrawText(TextFormat("Dirty Evictions: %llu", pt.dirty_evictions), 25, 630, 20, RED);

        DrawText("TLB (VPN -> Frame)", 380, 20, 20, BLACK);
        for (int i = 0; i < (int)config.TLB_size; i++) {
            Rectangle tlbRect = { 380.0f, 60.0f + (i * 45), 320.0f, 40.0f };
            Color cellColor = RAYWHITE;
            Color textColor = BLACK;

            if (i == tlb.lastEvictedSlot) {
                cellColor = BLACK;
                textColor = RAYWHITE;
            }
            else if (i == lastAccessedIndex) {
                cellColor = lastAccessWasHit ? GREEN : RED;
                textColor = RAYWHITE;
            }

            DrawRectangleRec(tlbRect, cellColor);
            DrawRectangleLinesEx(tlbRect, 2, DARKGRAY);

            auto entry = tlb.getEntryForVisual(i); 
            if (entry.active) {
                DrawText(TextFormat("VPN: %llu -> F: %llu", entry.vpn, entry.frame),
                    tlbRect.x + 10, tlbRect.y + 12, 18, textColor);
            }
            else {
                DrawText("Empty Slot", tlbRect.x + 10, tlbRect.y + 12, 18, Fade(GRAY, 0.5f));
            }
        }

        // --- RIGHT: PHYSICAL RAM (Grid Layout) ---
        DrawText("Physical RAM Page Frames", 720, 20, 20, BLACK);
        int cols = 5;
        int spacing = 85;
        for (int i = 0; i < pt.max_frames; i++) {
            int row = i / cols;
            int col = i % cols;
            Rectangle rect = { 720.0f + (col * (spacing + 10)), 60.0f + (row * (spacing + 10)), (float)spacing, (float)spacing };

            Color frameColor = LIGHTGRAY;

            if (i == pt.lastEvictedFrame) {
                frameColor = BLACK; // RAM Eviction indicator
            }
            else if (i < pt.frame_count) {
                frameColor = pt.isFrameDirty(i) ? RED : GREEN; // Dirty awareness
            }

            DrawRectangleRec(rect, frameColor);
            DrawRectangleLinesEx(rect, 2, DARKGRAY);

            unsigned long long physAddr = (unsigned long long)i * config.page_size; //
            Color labelColor = (frameColor.r + frameColor.g + frameColor.b < 300) ? WHITE : BLACK;
            DrawText(TextFormat("F:%d", i), rect.x + 5, rect.y + 5, 12, labelColor);
            DrawText(TextFormat("0x%llX", physAddr), rect.x + 5, rect.y + rect.height - 15, 11, labelColor);



           

           
        }
       
        int legendX = 1000;
        int legendY = 500;

        DrawRectangle(legendX - 10, legendY - 10, 200, 250, Fade(LIGHTGRAY, 0.3f));
        DrawRectangleLines(legendX - 10, legendY - 10, 200, 250, DARKGRAY);
        DrawText("LEGEND", legendX + 45, legendY, 20, BLACK);
        DrawLine(legendX, legendY + 25, legendX + 180, legendY + 25, DARKGRAY);

        
        DrawRectangle(legendX, legendY + 45, 20, 20, BLACK);
        DrawText("Eviction", legendX + 30, legendY + 45, 18, BLACK);

        DrawRectangle(legendX, legendY + 85, 20, 20, GREEN);
        DrawText("Hit / Clean", legendX + 30, legendY + 85, 18, BLACK);

        
        DrawRectangle(legendX, legendY + 125, 20, 20, RED);
        DrawText("Miss / Dirty", legendX + 30, legendY + 125, 18, BLACK);

        
        DrawRectangle(legendX, legendY + 165, 20, 20, LIGHTGRAY);
        DrawRectangleLines(legendX, legendY + 165, 20, 20, DARKGRAY);
        DrawText("Empty Slot", legendX + 30, legendY + 165, 18, BLACK);
        EndDrawing();
    }
};

#endif