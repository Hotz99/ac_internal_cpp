#include "./aimbot.h"

void Aimbot::Work() {
	if (!g_Settings.m_EnableAimbot || !(GetAsyncKeyState(VK_RBUTTON) & 0x8000))
		return;

	AcEntity* target = GetNextTarget();
    if (target) {
        Logger::Debug() << "[aimbot] aiming at " << target->Name << Logger::Endl;
        AimAtEntity(target);
    }
}

AcEntity* Aimbot::GetNextTarget() {
    if (!m_AcState->m_EntityListPtr || !m_AcState->m_EntityListPtr->Entities) {
        Logger::Warning() << "[aimbot] \"m_AcState->EntityList\" is unavailable" << Logger::Endl;
        return nullptr;
    }

    AcEntity* closestEntity = nullptr;
    AcEntity* entity = nullptr;

    float shortestDistance = 0;
    float distance = 0;

    for (int i = 0; i < *m_AcState->m_PlayerCountPtr; i++) {
        if (!m_AcState->IsValidEntity(m_AcState->m_EntityListPtr->Entities[i]))
            continue;

        entity = m_AcState->m_EntityListPtr->Entities[i];
        if (!m_AcState->IsEnemy(entity) || !IsVisible(entity))
            continue;

        distance = m_AcState->m_LocalPlayerPtr->OriginCoords.Distance(entity->OriginCoords);
        if (distance < shortestDistance || shortestDistance == 0) {
            shortestDistance = distance;
            closestEntity = entity;
        }
    }
    return closestEntity;
}

bool Aimbot::IsVisible(AcEntity* target) {
    AcEntity* localPlayer = m_AcState->m_LocalPlayerPtr;

    ac_structs::vec from = ac_structs::vec(localPlayer->OriginCoords.x, localPlayer->OriginCoords.y, localPlayer->OriginCoords.z);
    ac_structs::vec to = ac_structs::vec(target->OriginCoords.x, target->OriginCoords.y, target->OriginCoords.z);

    ac_functions::IntersectGeometry(from, to);
    
    return to.x == target->OriginCoords.x && to.y == target->OriginCoords.y && to.z == target->OriginCoords.z;
}

void Aimbot::AimAtEntity(AcEntity* entity) {
    geometry::Vector3 angle = ComputeAimAngle(entity);
    m_AcState->m_LocalPlayerPtr->Angle.x = angle.x;
    m_AcState->m_LocalPlayerPtr->Angle.y = angle.y;
}


geometry::Vector3 Aimbot::ComputeAimAngle(AcEntity* target) {
    geometry::Vector3 targetCoords = target->HeadCoords.x >= 0 ? target->HeadCoords : target->OriginCoords;

    targetCoords = m_AcState->m_LocalPlayerPtr->OriginCoords - targetCoords;

    geometry::Vector3 viewAngle;
    // TODO is this magic number a smoothing factor ?
    viewAngle.x = ::atanf(targetCoords.x / targetCoords.y) * (-M_RADIAN);
    viewAngle.y = ::atanf(targetCoords.z / ::sqrtf(targetCoords.x * targetCoords.x + targetCoords.y * targetCoords.y)) * (-M_RADIAN);
    viewAngle.z = 0.f;

    if (targetCoords.y < 0.0f)
        viewAngle.x += 180.0f;

    while (viewAngle.x < 0.f)
        viewAngle.x += 360.f;

    while (viewAngle.x >= 360.f)
        viewAngle.x -= 360.f;

    if (viewAngle.y > 90.f)
        viewAngle.y = 90.f;

    if (viewAngle.y < -90.f)
        viewAngle.y = -90.0;

    return viewAngle;
}
