#ifndef CHARACTERS_H
#define CHARACTERS_H

#include "raylib.h"

typedef enum {
    SKILL_LASER,
    SKILL_ROCKET,
    SKILL_STUN
} SkillType;

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

Character InitCharacter(int characterID);
void UpdateCooldown(Character *c, float deltaTime);

#endif