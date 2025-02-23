#include "text.h"

void opengl::Text::Font::Build(int height) {
    hdc = wglGetCurrentDC();
    base = glGenLists(96);
    HFONT hFont = CreateFontA(-height, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_DONTCARE | DEFAULT_PITCH, "Consolas");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    wglUseFontBitmaps(hdc, 32, 96, base);
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);

    IsBuilt = true;
}

void opengl::Text::Font::Print(float x, float y, const unsigned char color[3], const char* format, ...) {
    glColor3ub(color[0], color[1], color[2]);
    glRasterPos2f(x, y);

    char text[100];
    // `va` stands for variadic function arguments
    // https://en.cppreference.com/w/cpp/language/variadic_arguments
    va_list args;

    // `va_start` is a variadic function that initializes the `args` list

    va_start(args, format);
    vsprintf_s(text, 100, format, args);
    va_end(args);

    glPushAttrib(GL_LIST_BIT);
    glListBase(base - 32);
    glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
    glPopAttrib();
}

geometry::Vector3 opengl::Text::Font::CenterText(float x, float y, float width, float height, float textWidth, float textHeight) {
    geometry::Vector3 text;
    text.x = x + (width - textWidth) / 2;
    text.y = y + textHeight;
    return text;
}

float opengl::Text::Font::CenterText(float x, float width, float textWidth) {
    if (width > textWidth) {
        float difference = width - textWidth;
        return (x + (difference / 2));
    }
    else {
        float difference = textWidth - width;
        return (x - (difference / 2));
    }
}

float opengl::Text::Font::EndText(float x, float width, float textWidth) {
    return x + width - textWidth;
}
