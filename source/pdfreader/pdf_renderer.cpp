#include "pdf_renderer.h"
#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// NOTE: For a real implementation, you would integrate a lightweight C PDF library.
// Here, we provide a stub interface for the PDF rendering functions.

struct PDF_Document {
    // Placeholder for PDF document context (e.g., mupdf context, etc.)
    FILE* fp;
    int numPages;
};

PDF_Document* pdf_open(const char* path) {
    // Stub: Just checks file exists and returns a dummy PDF_Document.
    FILE* fp = fopen(path, "rb");
    if (!fp) return nullptr;
    PDF_Document* doc = (PDF_Document*)malloc(sizeof(PDF_Document));
    doc->fp = fp;
    doc->numPages = 5; // Dummy value for demonstration
    return doc;
}

void pdf_close(PDF_Document* doc) {
    if (!doc) return;
    if (doc->fp) fclose(doc->fp);
    free(doc);
}

int pdf_page_count(PDF_Document* doc) {
    return doc ? doc->numPages : 0;
}

// A simple stub: render a colored rectangle with page number.
// Replace with actual PDF rendering logic using a real PDF library!
Result pdf_render_page(PDF_Document* doc, int page_num, u8* framebuffer, int width, int height) {
    if (!doc || !framebuffer) return -1;

    // Paint background white
    for (int y = 0; y < height; ++y) {
        u32* row = (u32*)(framebuffer + y * width * 3); // GFX_BOTTOM is RGB8
        for (int x = 0; x < width; ++x) {
            row[x] = 0xFFFFFF;
        }
    }

    // Draw a colored box for each page (just to show a change)
    u32 color = 0xFF0000 >> (page_num % 3) * 8;
    for (int y = 40; y < 200; ++y) {
        u32* row = (u32*)(framebuffer + y * width * 3);
        for (int x = 40; x < 280; ++x) {
            row[x] = color;
        }
    }

    // Draw the page number as a simple string (Console overlay would show this in menu3dsShowPDFReader)
    return 0;
}
