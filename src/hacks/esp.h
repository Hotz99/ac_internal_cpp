#pragma once
#include "./settings.h"
#include "../game/ac_state.h"
#include "../opengl/opengl.h"

constexpr float BOX_LINE_WIDTH = 1.5f;
constexpr int FONT_HEIGHT = 15;
constexpr int FONT_WIDTH = 9;
/*constexpr float ENEMY_NAME_COLOR 0xFF0000FF
constexpr float FRIENDLY_NAME_COLOR 0x00FF00FF
constexpr float ENEMY_BOX_COLOR 0xFF0000FF
constexpr float FRIENDLY_BOX_COLOR 0x00FF00FF*/

struct Viewport {
    int x, y, width, height;
};

enum DrawType {
	BOX,
	NAME,
	HEALTH_BAR,
	SNAPLINE
};

class ESP {
private:
    void Render(AcEntity* entity, geometry::Vector2 screenCoordinates) {};

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

    AcState& m_AcState;

public:
    ESP() : m_AcState(AcState::GetInstance()) {}

    static ESP& GetInstance() {
        static ESP s_Instance;
        return s_Instance;
    }

    void Render();
};
