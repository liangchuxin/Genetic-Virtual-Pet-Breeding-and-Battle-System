#ifndef STATUS_EFFECT_H
#define STATUS_EFFECT_H

#include <map>
#include <string>
#include <vector>
using namespace std;

// 持续的 buff
class StatusEffect {
   private:
    string name;
    int duration;  // turns
    map<string, int> modifiers;

   public:
    StatusEffect(const string& effectName, int dur)
        : name(effectName), duration(dur) {}

    void addModifier(const string& attribute, int value) {
        modifiers[attribute] = value;
    }

    int getModifier(const string& attribute) const {
        auto it = modifiers.find(attribute);
        return (it != modifiers.end()) ? it->second : 0;
    }

    bool tick() {  // 每回合调用，返回是否还有效
        duration--;
        return duration > 0;
    }

    string getName() const { return name; }
    int getDuration() const { return duration; }
    bool isExpired() const { return duration <= 0; }
};

// buff manager

class StatusEffectManager {
   private:
    vector<StatusEffect*> activeEffects;

   public:
    ~StatusEffectManager() {
        for (StatusEffect* effect : activeEffects) {
            delete effect;
        }
    }

    void addEffect(StatusEffect* effect) {
        // override if same effect exists
        for (auto i = 0u; i < activeEffects.size(); i++) {  // 这里是为什么
            if (activeEffects[i]->getName() == effect->getName()) {
                delete activeEffects[i];  // replace old buff
                activeEffects[i] = effect;
                return;
            }
        }
        activeEffects.push_back(effect);
    }

    int getTotalModifier(const string& attribute) const {
        int total = 0;
        for (StatusEffect* effect : activeEffects) {
            total += effect->getModifier(attribute);
        }
        return total;
    }

    void tickEffects() {  // activate buff every turn
        for (auto it = activeEffects.begin(); it != activeEffects.end();) {
            if (!(*it)->tick()) {
                delete *it;
                it = activeEffects.erase(it);
            } else {
                ++it;
            }
        }
    }

    void removeEffect(const string& effectName) {
        for (auto it = activeEffects.begin(); it != activeEffects.end(); ++it) {
            if ((*it)->getName() == effectName) {
                delete *it;
                activeEffects.erase(it);
                break;
            }
        }
    }

    vector<StatusEffect*> getActiveEffects() const { return activeEffects; }

    // remove all buffs

    void clear() {
        for (StatusEffect* effect : activeEffects) {
            delete effect;
        }
        activeEffects.clear();
    }
};

#endif