#pragma once
#include "hack.h"
#include "../game/ac_state.h"
#include "./settings.h"
#include "../opengl/opengl.h"

#define BOX_LINE_WIDTH 2.0f
#define FONT_HEIGHT 15
#define FONT_WIDTH 9

struct Viewport {
    int x, y, width, height = 0;
};

class ESP {
private:
    // TODO why `virtual` ?
    virtual void Render(AcEntity* entity, geometry::Vector2 screenCoordinates) {};

    // TODO much repetition
    void DrawBox(AcEntity* entity, geometry::Vector2 screenCoordinates);
    void DrawName(AcEntity* entity, geometry::Vector2 screenCoordinates);
    void DrawHealthBar(AcEntity* entity, geometry::Vector2 screenCoordinates);
    void DrawSnapline(AcEntity* entity, geometry::Vector2 screenCoordinates);

    void GetScaledEntityBox(AcEntity* entity, geometry::Vector2 screenCoordinates, geometry::Rect* rect);
    const GLubyte* GetEntityColor(AcEntity* entity);
    float GetDistanceTo(AcEntity* entity);

    Viewport m_Viewport;
    opengl::Text::Font m_Font;

    AcState* m_AcState;
public:
    ESP() {
        m_AcState = &AcState::GetInstance();
    }

    static ESP& GetInstance() {
        static ESP instance;
        return instance;
    }

    void Render();
};
