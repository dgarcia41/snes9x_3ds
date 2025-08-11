#include "pdfreader_menu.h"
#include "pdf_reader.h" // Your PDF rendering interface
#include <3ds.h>
#include <string>
#include <stdio.h>

// Simple file picker for PDFs in sdmc:/PDFs/
static std::string showPDFFilePicker() {
    FSUSER_OpenArchive(ARCHIVE_SDMC, fsMakePath(PATH_EMPTY), 0);
    DIR* d = opendir("sdmc:/PDFs/");
    if (!d) return "";

    char files[64][256];
    int count = 0;
    struct dirent* ent;
    while ((ent = readdir(d)) && count < 64) {
        size_t len = strlen(ent->d_name);
        if (len > 4 && strcasecmp(ent->d_name + len - 4, ".pdf") == 0) {
            strncpy(files[count], ent->d_name, 255);
            files[count][255] = 0;
            ++count;
        }
    }
    closedir(d);

    int selected = 0;
    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_B) return "";
        if (kDown & KEY_UP && selected > 0) selected--;
        if (kDown & KEY_DOWN && selected < count-1) selected++;
        if (kDown & KEY_A && count > 0) return "sdmc:/PDFs/" + std::string(files[selected]);

        // Draw
        consoleClear();
        printf("Select a PDF:\n\n");
        for (int i = 0; i < count; ++i) {
            if (i == selected) printf("> %s\n", files[i]);
            else printf("  %s\n", files[i]);
        }
        if (count == 0) printf("No PDFs in /PDFs/.\n");

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
    return "";
}

void menu3dsShowPDFReader() {
    // Show file picker for PDFs
    std::string pdfPath = showPDFFilePicker();
    if (pdfPath.empty()) return;

    PDF_Document *doc = pdf_open(pdfPath.c_str());
    if (!doc) {
        // Show error dialog
        consoleClear();
        printf("Failed to open PDF!\nPress B to return.\n");
        while (aptMainLoop()) {
            hidScanInput();
            if (hidKeysDown() & KEY_B) break;
            gspWaitForVBlank();
        }
        return;
    }
    int page = 0, numPages = pdf_page_count(doc);

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_B) break;
        if (kDown & KEY_RIGHT && page < numPages-1) page++;
        if (kDown & KEY_LEFT && page > 0) page--;

        // Render to bottom screen
        u8* fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
        pdf_render_page(doc, page, fb, 320, 240);

        // Overlay page number
        consoleSelect(NULL);
        printf("\x1b[28;0HPg %d/%d   B:Exit  LEFT/RIGHT:Page", page+1, numPages);

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    pdf_close(doc);
}
