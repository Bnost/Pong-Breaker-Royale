// characters.c
#include "characters.h"

// Seçilen ID'ye göre karakteri hazırlayan fonksiyon
Character InitCharacter(int characterID) {
    Character c = {0}; // Belleği sıfırla

    switch(characterID) {
        case 1: // Örneğin Miku
            c.name = "Hatsune Miku";
            c.themeColor = SKYBLUE;
            c.skill = SKILL_LASER;
            c.cooldownMax = 10.0f; // 20 saniyede bir dolsun
            c.maxFrames = 4;
            c.frameSpeed = 15.0f;
            break;
        case 2: // Örneğin Teto
            c.name = "Kasane Teto";
            c.themeColor = RED;
            c.skill = SKILL_ROCKET;
            c.cooldownMax = 15.0f;
            break;
        case 3: // Örneğin Neru
            c.name = "Akita Neru";
            c.themeColor = YELLOW;
            c.skill = SKILL_STUN;
            c.cooldownMax = 12.0f;
            break;
    }
    
    // Oyun başında yetenek kapalı başlasın ve dolmaya başlasın
    c.currentCooldown = c.cooldownMax; 
    c.isSkillReady = false;
    c.isSkillActive = false;
    c.skillDuration = 0.0f;
    c.skillTimer = 0.0f;
    c.skillTickTimer = 0.0f;
    c.currentFrame = 0;
    c.frameTimer = 0.0f;
    c.currentCooldown = c.cooldownMax; 
    c.isSkillReady = false;
    
    return c;
}

// Oyun döngüsü içinde sürekli çağrılarak süreyi azaltan fonksiyon
void UpdateCooldown(Character *c, float deltaTime) {
    if (!c->isSkillReady && !c->isSkillActive) {
        c->currentCooldown -= deltaTime;
        if (c->currentCooldown <= 0.0f) {
            c->currentCooldown = 0.0f;
            c->isSkillReady = true;
        }
    }

    if (c->isSkillActive) {
        c->skillTimer -= deltaTime;
        c->skillTickTimer += deltaTime;
        if (c->skillTimer <= 0.0f) {
            c->isSkillActive = false;
            c->skillTimer = 0.0f;
            // Yetenek bitince cooldown yeniden başlasın
            c->currentCooldown = c->cooldownMax;
        }
    }
}