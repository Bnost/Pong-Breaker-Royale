#ifndef CHARACTERS_H
#define CHARACTERS_H

#include "raylib.h"

// 1. Yetenek türlerini tanımlayalım (KALMALI)
typedef enum {
    SKILL_LASER,
    SKILL_ROCKET,
    SKILL_STUN
} SkillType;

// 2. Karakter veri yapısı (KALMALI)
typedef struct {
    const char* name;
    Color themeColor;
    SkillType skill;
    float cooldownMax;
    float currentCooldown;
    bool isSkillReady;
    bool isSkillActive;
    float skillDuration;
    float skillTimer;
    float skillTickTimer;
    int currentFrame;
    int maxFrames;
    float frameTimer;
    float frameSpeed;
} Character;

// 3. Fonksiyon prototipleri (SADECE İSİMLERİ KALMALI)
Character InitCharacter(int characterID);
void UpdateCooldown(Character* c, float deltaTime);

#endif