// characters.h
#ifndef CHARACTERS_H  // Include Guard: Dosyanın iki kez dahil edilmesini önler
#define CHARACTERS_H

#include "raylib.h"

// Yetenek türlerini tanımlayalım
typedef enum {
    SKILL_LASER,      // Hızlıca puan veren ışın
    SKILL_ROCKET,     // Blok kıran ve rakibi donduran roket
    SKILL_STUN        // Direkt rakibi sersemleten yetenek
} SkillType;

// Karakter veri yapısı
typedef struct {
    const char* name;
    Color themeColor;
    SkillType skill;
    float cooldownMax;       // Yeteneğin dolma süresi (saniye)
    float currentCooldown;   // Şu anki bekleme süresi
    bool isSkillReady;       // Yetenek kullanıma hazır mı?
    bool isSkillActive;      // Yetenek şu an aktif mi?
    float skillDuration;     // Yeteneğin toplam süresi
    float skillTimer;        // Yeteneğin kalan süresi
    float skillTickTimer;    // Lazer gibi yetenekler için tick süresi
    int currentFrame;        // Animasyon karesi
    int maxFrames;           // Toplam animasyon karesi sayısı
    float frameTimer;        // Kare geçiş süresi sayacı
    float frameSpeed;        // Saniyedeki animasyon karesi hızı
} Character;

// Fonksiyon prototipleri
Character InitCharacter(int characterID);
void UpdateCooldown(Character *c, float deltaTime);

#endif