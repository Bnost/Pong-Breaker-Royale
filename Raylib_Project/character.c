#include "characters.h"

Character InitCharacter(int characterID) {
    Character c = { 0 };

    switch (characterID) {
    case 1:
        c.name = "Hatsune Miku";
        c.themeColor = SKYBLUE;
        c.skill = SKILL_LASER;
        c.cooldownMax = 10.0f;
        c.maxFrames = 4;
        c.frameSpeed = 15.0f;
        break;
    case 2:
        c.name = "Kasane Teto";
        c.themeColor = RED;
        c.skill = SKILL_ROCKET;
        c.cooldownMax = 15.0f;
        break;
    case 3:
        c.name = "Akita Neru";
        c.themeColor = YELLOW;
        c.skill = SKILL_STUN;
        c.cooldownMax = 12.0f;
        break;
    }

    // Oyun başında yetenek hazır olarak başlar
    c.currentCooldown = 0.0f;
    c.isSkillReady = true;
    c.isSkillActive = false;
    c.skillTimer = 0.0f;
    c.skillTickTimer = 0.0f;
    c.currentFrame = 0;
    c.frameTimer = 0.0f;

    return c;
}

// character.c
void UpdateCooldown(Character* c, float deltaTime) {
    // Yetenek hazır değilse (kullanıldıysa) süreyi azalt
    if (!c->isSkillReady) {
        c->currentCooldown -= deltaTime;
        if (c->currentCooldown <= 0.0f) {
            c->currentCooldown = 0.0f;
            c->isSkillReady = true; // Süre dolunca tekrar hazır yap
        }
    }

    // Aktif kullanım süresi (Lazer/Matkap süresi)
    if (c->isSkillActive) {
        c->skillTimer -= deltaTime;
        c->skillTickTimer += deltaTime;
        if (c->skillTimer <= 0.0f) {
            c->isSkillActive = false;
            c->skillTimer = 0.0f;
        }
    }
}