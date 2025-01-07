#include "esp.h"

void ESP::Render() {
    if (!g_Settings.m_EnableESP)
        return;
        
    if (!m_AcState->m_EntityListPtr || !m_AcState->m_EntityListPtr->Entities) {
        Logger::Warning() << "[esp] m_AcState->EntityList is unavailable" << Logger::Endl;
        return;
    }

    glGetIntegerv(GL_VIEWPORT, reinterpret_cast<GLint*>(&m_Viewport));

    for (int i = 0; i < *m_AcState->m_PlayerCountPtr; i++) {
        AcEntity* entity = m_AcState->m_EntityListPtr->Entities[i];
        
        if (!m_AcState->IsValidEntity(entity))
            continue;

        geometry::Vector2 screenCoords;

        // if entity is not visible
        if (!geometry::WorldToScreen(entity->OriginCoords, screenCoords, m_AcState->m_ViewMatrixPtr, m_Viewport.width, m_Viewport.height))
            continue;

        if (g_Settings.m_PlayerBoxes) 
            DrawBox(entity, screenCoords);
        if (g_Settings.m_PlayerNames) 
            DrawName(entity, screenCoords);
        if (g_Settings.m_PlayerHealthBar) 
            DrawHealthBar(entity, screenCoords);
        if (g_Settings.m_PlayerSnaplines) 
            DrawSnapline(entity, screenCoords);
    }
}

void ESP::DrawBox(AcEntity* entity, geometry::Vector2 screenCoords) {
    // TODO can we reuse the same `Rect` instance for all entities ?
    geometry::Rect rect = geometry::Rect();
    GetScaledEntityBox(entity, screenCoords, &rect);

    Logger::Debug() << "[esp] " << entity->Name << " : "  << rect.width << "x" << rect.height << " at " << rect.x << ", " << rect.y << Logger::Endl;


    opengl::Draw::Outline(rect, BOX_LINE_WIDTH, GetEntityColor(entity));
}

void ESP::DrawName(AcEntity* entity, geometry::Vector2 screenCoords) {
    if (!m_Font.bBuilt) {
        m_Font.Build(FONT_HEIGHT);
        Logger::Debug() << "[esp] built font" << Logger::Endl;
    }

    geometry::Rect rect = geometry::Rect();
    GetScaledEntityBox(entity, screenCoords, &rect);

    float textX = m_Font.CenterText(rect.x, rect.width, (float)(strlen(entity->Name) * FONT_WIDTH));
    float textY = rect.y - FONT_HEIGHT / 2;
    m_Font.Print(textX, textY, GetEntityColor(entity), "%s", entity->Name);
}

void ESP::DrawHealthBar(AcEntity* entity, geometry::Vector2 screenCoords) {
    const GLubyte* color;
    if (entity->Health >= 66) {
        color = opengl::Color::GREEN;
    }
    else if (entity->Health >= 33) {
        color = opengl::Color::ORANGE;
    }
    else {
        color = opengl::Color::RED;
    }

    geometry::Rect rect = geometry::Rect();
    GetScaledEntityBox(entity, screenCoords, &rect);

    float tmp = rect.height * entity->Health / 100;
    rect.y += rect.height - tmp;
    rect.x -= 8.f;
    rect.width = 3.f;
    rect.height = tmp;

    opengl::Draw::FilledRect(rect, color);
}

void ESP::DrawSnapline(AcEntity* entity, geometry::Vector2 screenCoords) {
    geometry::Line line = geometry::Line(
        screenCoords,
        geometry::Point((float)m_Viewport.width / 2, (float)m_Viewport.height)
    );

    opengl::Draw::Line(line, GetEntityColor(entity));
}

const GLubyte* ESP::GetEntityColor(AcEntity* entity) {
    return (m_AcState->IsEnemy(entity)) ? opengl::Color::RED : opengl::Color::BLUE;
}

float ESP::GetDistanceTo(AcEntity* entity) {
    return m_AcState->m_LocalPlayerPtr->OriginCoords.Distance(entity->OriginCoords);
}

void ESP::GetScaledEntityBox(AcEntity* entity, geometry::Vector2 screenCoords, geometry::Rect* rect) {
    geometry::Vector3 headWorldCoords = geometry::Vector3(entity->OriginCoords.x, entity->OriginCoords.y, entity->OriginCoords.z);
    headWorldCoords.z += 0.8f;

    geometry::Vector3 feetWorldCoords = headWorldCoords.Copy();
    feetWorldCoords.z -= entity->EyeHeight;

    geometry::Point headScreenCoords, feetScreenCoords;
    geometry::WorldToScreen(headWorldCoords, headScreenCoords, m_AcState->m_ViewMatrixPtr, m_Viewport.width, m_Viewport.height);
    geometry::WorldToScreen(feetWorldCoords, feetScreenCoords, m_AcState->m_ViewMatrixPtr, m_Viewport.width, m_Viewport.height);

    float squaredDistance = headScreenCoords.Distance(feetScreenCoords) * 1.2f;

    rect->x = headScreenCoords.x - (squaredDistance / 4.0f);
    rect->y = headScreenCoords.y;
    rect->width = squaredDistance / 2.0f;
    rect->height = squaredDistance;
}

