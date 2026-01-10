# Project KORA

> **Lyra Framework 기반의 소울라이크 액션 RPG**
>
> Unreal Engine 5의 Lyra Starter Game을 기반으로, 소울라이크 장르 특유의 긴장감 있는 전투와 RPG 요소를 결합한 3인칭 액션 게임입니다.
<img width="2752" height="1536" alt="KORA_Benner" src="https://github.com/user-attachments/assets/a01277d4-b50d-4ef2-ac78-714c7f801045" />
<br>
<br>
<img width="555" height="318" alt="image" src="https://github.com/user-attachments/assets/bb248df2-a8c2-4c3d-a12d-28db6cb102aa" />
<br>
<br>
Game Trailer:
https://youtu.be/71doj8meEhY

---

## Table of Contents

1. [Project Overview](#project-overview)
2. [Key Features](#key-features)
3. [Technical Stack](#technical-stack)
4. [Architecture](#architecture)
5. [Combat System](#combat-system)
6. [GAS (Gameplay Ability System)](#gas-gameplay-ability-system)
7. [AI System](#ai-system)
8. [Quest System](#quest-system)
9. [Inventory & Equipment System](#inventory--equipment-system)
10. [UI/UX System](#uiux-system)
11. [Animation System](#animation-system)
12. [Camera System](#camera-system)
13. [Troubleshooting](#troubleshooting)
14. [Getting Started](#getting-started)
15. [Project Structure](#project-structure)
16. [Team](#team)

---

## Project Overview

### Lyra에서 소울라이크 RPG로의 전환

**Project KORA**는 Epic Games의 **Lyra Starter Game**을 기반으로 시작되었습니다. Lyra는 원래 멀티플레이어 슈터 게임을 위한 프레임워크로 설계되었지만, 우리는 이를 **소울라이크 액션 RPG**로 완전히 재구성했습니다.

#### 왜 Lyra인가?

Lyra Framework를 선택한 이유는 다음과 같습니다:

| Lyra의 강점 | Project KORA에서의 활용 |
|------------|----------------------|
| **Gameplay Ability System (GAS)** | 소울라이크 전투의 복잡한 상태 관리 및 능력 시스템 |
| **Modular Gameplay Framework** | 무기, 장비, 적 타입별 모듈화된 설계 |
| **Game Features Plugin** | 콘텐츠별 독립적인 기능 활성화/비활성화 |
| **Enhanced Input System** | 복잡한 콤보 입력 및 상황별 입력 전환 |
| **Common UI** | 계층적 메뉴 시스템 및 게임패드 지원 |

#### 주요 전환 작업

```
Lyra (Multiplayer Shooter)          →    Project KORA (Souls-like RPG)
─────────────────────────────────────────────────────────────────────
건 슈터 전투 시스템                 →    스태미나 기반 근접/원거리 전투
팀 기반 멀티플레이어               →    싱글플레이어 PvE
체력 재생 시스템                    →    아이템을 통한 체력 재생 + 체크포인트 시스템
간단한 무기 교체                    →    장비/강화/모듈 시스템
```

---

## Key Features

### 1. 소울라이크 전투 시스템
- **스태미나 기반 액션**: 공격, 회피, 가드 모든 행동이 스태미나를 소모
- **패링 시스템**: 0.2초 퍼펙트 가드 윈도우로 적의 공격을 무력화
- **포이즈(Poise) 시스템**: 적의 경직도를 축적하여 처형 기회 생성
- **Core Drive**: 타격/처치 시 충전되는 궁극기 게이지

### 2. 깊이 있는 RPG 시스템
- **Fragment 기반 아이템**: 모듈화된 아이템 기능 시스템
- **모듈 장착**: 무기에 추가 능력 부여
- **장비 장착**: 장비별 능력 부여 가능 및 캐릭터 커스터마이징 기능
- **다양한 소비 아이템**: 회복, 버프, 특수 효과 등

### 3. 지능적인 AI
- **State Tree 기반**: 시각적으로 편집 가능한 AI 행동 트리
- **EQS 연동**: Environment Query System을 활용한 전술적 위치 선정
- **AI Perception**: 시야 기반 타겟 감지 시스템
- **다양한 적 타입**: Human, FerroLoader, Arbid, Spinderbreed, Nerras 등

### 4. 몰입감 있는 UI/UX
- **Common UI 기반**: 키보드/마우스 및 게임패드 완벽 지원
- **Common Input 연동**: 입력 장치별/위젯별 UI 인풋 리바인딩
- **계층적 메뉴 시스템**: Layer 기반 UI 관리
- **실시간 전투 UI**: 체력, 스태미나, 보스 HP 표시

### 5. State Tree 기반 퀘스트 시스템
- **시각적 퀘스트 흐름 설계**: State Tree 에디터로 퀘스트 진행을 노드 기반으로 설계
- **모듈형 목표 시스템**: 몬스터 처치, 아이템 획득, NPC 대화, 위치 진입 등 6가지 조건 체커
- **계층적 서브퀘스트**: 메인 퀘스트 아래 다단계 서브퀘스트 구조, 시간제한/진행률 추적

---

## Technical Stack

### Engine & Framework
- **Unreal Engine 5.6+**
- **Lyra Starter Game** (Base Framework)

### Core Plugins
| Plugin | 용도 |
|--------|-----|
| **Gameplay Abilities** | 전투 능력 및 상태 관리 |
| **Enhanced Input** | 복잡한 입력 시스템 |
| **Common UI** | 크로스플랫폼 UI |
| **Motion Warping** | 공격 시 위치 보정 |
| **State Tree** | AI 행동 및 퀘스트 관리 |
| **Mass Entity** | 대규모 NPC 처리 |
| **Gameplay Message Router** | 이벤트 기반 통신 |

### Source Code Scale
```
KORAProject Module
├── 282 C++ Source Files
├── 289 Header Files
├── 56 GAS-related Files
├── 13 Subsystems
└── 24+ Hero Abilities / 6 Enemy Abilities
```

---

## Architecture

### 전체 아키텍처 개요

```
┌─────────────────────────────────────────────────────────────────────┐
│                        Game Instance                                │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    Subsystems (13개)                         │   │
│  │  ┌──────────────┐ ┌──────────────┐ ┌──────────────────────┐ │   │
│  │  │ Inventory    │ │ Quest        │ │ Shop                 │ │   │
│  │  │ Subsystem    │ │ Subsystem    │ │ Subsystem            │ │   │
│  │  └──────────────┘ └──────────────┘ └──────────────────────┘ │   │
│  │  ┌──────────────┐ ┌──────────────┐ ┌──────────────────────┐ │   │
│  │  │ DataTables   │ │ Effect       │ │ Sound                │ │   │
│  │  │ Subsystem    │ │ Subsystem    │ │ Subsystem            │ │   │
│  │  └──────────────┘ └──────────────┘ └──────────────────────┘ │   │
│  │  ┌──────────────┐ ┌──────────────┐ ┌──────────────────────┐ │   │
│  │  │ UI Router    │ │ UI Input     │ │ Map Travel           │ │   │
│  │  │ Subsystem    │ │ Subsystem    │ │ Subsystem            │ │   │
│  │  └──────────────┘ └──────────────┘ └──────────────────────┘ │   │
│  └─────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────┘
                                  │
                                  ▼
┌─────────────────────────────────────────────────────────────────────┐
│                        Game Mode                                    │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │              Experience Definition                           │   │
│  │  • DefaultPawnData                                           │   │
│  │  • GameFeaturesToEnable                                      │   │
│  │  • ActionSets                                                │   │
│  └─────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────┘
                                  │
                                  ▼
┌─────────────────────────────────────────────────────────────────────┐
│                       Character System                              │
│                                                                     │
│   AKRBaseCharacter                                                  │
│   ├── IAbilitySystemInterface                                       │
│   ├── UKRPawnExtensionComponent (Ability 초기화)                    │
│   ├── UKRCombatComponent (전투 로직)                                │
│   └── UKREquipmentManagerComponent (장비 관리)                      │
│       │                                                             │
│       ├── AKRHeroCharacter (플레이어)                               │
│       │   ├── UKRStaminaComponent                                   │
│       │   ├── UKRCoreDriveComponent                                 │
│       │   ├── UKRGuardRegainComponent                               │
│       │   └── UMotionWarpingComponent                               │
│       │                                                             │
│       └── AKREnemyPawn (적)                                         │
│           ├── UStateTreeAIComponent (AI)                            │
│           └── UKREnemyAttributeSet (Poise)                          │
└─────────────────────────────────────────────────────────────────────┘
```

### Lyra 아키텍처 활용

#### 1. PawnData 시스템
Lyra의 PawnData 시스템을 확장하여 캐릭터별 설정을 관리합니다:

```cpp
// UKRPawnData (PrimaryDataAsset)
UPROPERTY() TSubclassOf<APawn> PawnClass;
UPROPERTY() TArray<UKRAbilitySet*> AbilitySets;
UPROPERTY() UKRAbilityTagRelationshipMapping* TagRelationshipMapping;
UPROPERTY() UDataAsset_InputConfig* InputConfig;
UPROPERTY() TSubclassOf<UKRCameraMode> DefaultCameraMode;
UPROPERTY() TArray<FGameplayTag> DefaultInventoryItemTags;
UPROPERTY() TArray<FGameplayTag> DefaultEquipItemTags;
```

#### 2. AbilitySet 시스템
능력, 이펙트, 속성을 세트로 관리합니다:

```cpp
// UKRAbilitySet
UPROPERTY() TArray<FKRAbilitySet_GameplayAbility> GrantedGameplayAbilities;
UPROPERTY() TArray<FKRAbilitySet_GameplayEffect> GrantedGameplayEffects;
UPROPERTY() TArray<TSubclassOf<UAttributeSet>> GrantedAttributeSets;
```

---

## Combat System

### 소울라이크 전투의 핵심 철학

Project KORA의 전투 시스템은 **"위험과 보상의 균형"** 을 핵심으로 설계되었습니다. 모든 공격 행동은 스태미나를 소모하고 빈틈을 만들며, 플레이어는 적의 패턴을 읽고 적절한 타이밍에 행동해야 합니다.
전투는 총과 검 두가지가 있고, 총은 특정 포인트로 와이어를 던져 이동하거나 적을 끌어올 수 있는 스킬이 있는 대신에 가드나 특수 공격을 시전할 수 없고, 검은 가드와 특수 공격 등으로 소울라이크류 전투를 온전히 즐길 수 있습니다.

<img width="1513" height="740" alt="Screenshot 2026-01-10 211428" src="https://github.com/user-attachments/assets/c99838bc-3302-48e4-9ee2-030170dd2163" />
<img width="1693" height="687" alt="Screenshot 2026-01-10 211635" src="https://github.com/user-attachments/assets/8c61a921-d140-477a-b303-abf1f898ea51" />


### 스태미나 시스템

```cpp
// UKRStaminaComponent
class UKRStaminaComponent : public UActorComponent
{
    bool ConsumeStamina(float Amount);
    void StartContinuousConsumption(float RatePerSecond);
    void SetRecoveryState(EStaminaRecoveryState NewState);
    void StartRecoveryDelay(float DelaySeconds);

    FOnStaminaDepletedSignature OnStaminaDepleted;
    FOnStaminaChangedSignature OnStaminaChanged;
};
```

| 행동 | 스태미나 소모 | 회복 지연 |
|-----|------------|----------|
| 경공격 | 12 | 0.5초 |
| 강공격 | 25 | 0.8초 |
| 회피 | 18 | 0.3초 |
| 가드 (유지) | 5/초 | - |
| 스프린트 | 8/초 | 0.2초 |

### 가드 & 패링 시스템

소울라이크 장르의 핵심인 **패링 시스템**을 구현했습니다:
<img width="1142" height="588" alt="image" src="https://github.com/user-attachments/assets/e4c053c6-3887-4bee-a28c-b2b7e07b2614" />
<span style="color:#808080">퍼펙트가드 성공 시 이미지</span>

```cpp
// KRGA_HeroGuard - 가드 상태 머신
enum class EGuardState : uint8
{
    None,              // 비활성
    ParryWindow,       // 퍼펙트 가드 (0.2초)
    StandardGuard,     // 일반 가드
    PerfectGuardHit,   // 패리 성공 반응
    StandardGuardHit,  // 일반 가드 피격
    GuardBroken        // 가드 브레이크
};
```

### 데미지 계산 공식

```cpp
// GEExecCalc_DamageTaken
FinalDamage = (BaseDamage × DealDamageMult × CritMultiplier - DefensePower)
              × TakeDamageMult
              × (1 - GuardReduction)

// 가드 감소율
if (PerfectParry)     GuardReduction = 1.0;   // 100% 감소
else if (StandardGuard) GuardReduction = 0.7; // 70% 감소
else                   GuardReduction = 0.0;  // 감소 없음
```
### Core Drive 시스템

전투 중 충전되는 궁극기 게이지입니다. 다양한 전투 행동으로 게이지를 쌓아 강력한 스킬을 발동합니다.
<img width="2408" height="1064" alt="image" src="https://github.com/user-attachments/assets/4b8aea78-55eb-4c1d-98ef-87acbc5a8479" />


| 충전 소스 | 충전량 |
|----------|-------|
| 타격 | 데미지 × 0.1 |
| 패링 성공 | +15 |
| 적 처치 | +25 |

```cpp
// UKRCoreDriveComponent
void AddGaugeFromHit(float DamageDealt);   // 타격 시
void AddGaugeFromParry();                   // 패링 성공 시
void AddGaugeFromKill();                    // 처치 시
bool ConsumeGauge(float Amount);            // 스킬 사용 시 소모
```

### Guard Regain 시스템 (Grey HP)

가드 중 받은 데미지의 일부를 **Grey HP**로 전환하여, 일정 시간 내 공격하면 회복할 수 있는 시스템입니다.
<img width="1390" height="551" alt="image" src="https://github.com/user-attachments/assets/ed72376b-e97d-40e8-85ed-e0478ac2f9f1" />


```
┌─────────────────────────────────────────────────────────┐
│  [■■■■■■■■░░░░░░░░░░░░]  HP Bar                         │
│           └── Grey HP (회복 가능 영역)                   │
│                                                         │
│  가드 피격 → Grey HP 생성 → 3초 내 공격 → HP 회복       │
│                          → 3초 경과 or 피격 → Grey HP 소멸│
└─────────────────────────────────────────────────────────┘
```

| 속성 | 값 |
|-----|-----|
| 회복 가능 시간 | 3초 |
| 회복 비율 | Grey HP의 80% |

---
## GAS (Gameplay Ability System)

### 아키텍처 개요

Project KORA는 Unreal Engine의 **Gameplay Ability System (GAS)**을 전투, 상태 관리, 아이템 효과 등 모든 게임플레이 로직에 활용합니다.

```
┌─────────────────────────────────────────────────────────────────┐
│                      GAS ARCHITECTURE                             │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│   UKRAbilitySystemComponent (ASC)                                │
│   ├── 입력 처리: AbilityInputTagPressed/Released                  │
│   ├── 태그 관계: UKRAbilityTagRelationshipMapping                 │
│   └── 활성 GA 조회: GetActiveAbilityByClass<T>()                  │
│                                                                   │
│   UKRAbilitySet (DataAsset)                                      │
│   ├── GrantedGameplayAbilities[] → GA 목록                       │
│   ├── GrantedGameplayEffects[]   → GE 목록                       │
│   └── GrantedAttributeSets[]     → AttributeSet 목록             │
│                                                                   │
│   UKRGameplayAbility (GA)                                        │
│   ├── ActivationPolicy: OnInputTriggered / WhileInputActive / OnSpawn │
│   └── 헬퍼: GetKRCharacter, GetCombatComponent, GetController    │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

### AttributeSet 구조

#### UKRCombatCommonSet (공통 전투 속성)

| 속성 | 설명 |
|-----|------|
| CurrentHealth / MaxHealth | 현재/최대 체력 |
| AttackPower | 공격력 |
| DefensePower | 방어력 |
| DealDamageMult | 가하는 데미지 배율 |
| TakeDamageMult | 받는 데미지 배율 |
| AttackSpeed | 공격 속도 |
| CritChance / CritMulti | 치명타 확률/배율 |
| WeaponRange | 무기 사거리 |

#### UKRPlayerAttributeSet (플레이어 전용)

| 속성 | 설명 |
|-----|------|
| CurrentStamina / MaxStamina | 스태미나 |
| GreyHP | 회복 가능 HP (Guard Regain) |
| StaminaRegenRate / StaminaRegenDelay | 스태미나 회복 속도/지연 |
| CoreDrive / MaxCoreDrive | 궁극기 게이지 |
| CurrentAmmo / MaxAmmo | 현재/최대 탄약 |

#### UKREnemyAttributeSet (적 전용)

| 속성 | 설명 |
|-----|------|
| CanAttackRange | 공격 가능 사거리 |
| EnterRageStatusRate | 분노 상태 진입 HP 비율 |

### Ability Tag Relationship

GA 간의 블록/취소 관계를 데이터 기반으로 정의합니다.

```cpp
// FKRAbilityTagRelationship
struct FKRAbilityTagRelationship
{
    FGameplayTag AbilityTag;              // 이 GA가 활성화되면
    FGameplayTagContainer AbilityTagsToBlock;   // 이 태그들을 블록
    FGameplayTagContainer AbilityTagsToCancel;  // 이 태그들을 취소
    FGameplayTagContainer ActivationRequiredTags;  // 활성화 필요 태그
    FGameplayTagContainer ActivationBlockedTags;   // 활성화 차단 태그
};
```

### 주요 Gameplay Abilities

#### Hero Abilities

| GA | 설명 | 활성화 정책 |
|----|------|------------|
| **KRGA_HeroGuard** | P의 거짓 스타일 가드 (퍼펙트 가드 0.2초 윈도우) | WhileInputActive |
| **KRGA_HeroDash** | 회피 (스태미나 소모) | OnInputTriggered |
| **KRGA_HeroSprint** | 달리기 (지속 스태미나 소모) | WhileInputActive |
| **KRGA_LockOn** | 타겟 락온 | OnInputTriggered |
| **KRGA_CoreDriveBurst** | 코어 드라이브 궁극기 | OnInputTriggered |
| **KRGameplayAbility_LightAttack** | 경공격 콤보 | OnInputTriggered |
| **KRGameplayAbility_ChargeAttack** | 차지 강공격 | WhileInputActive |
| **KRGameplayAbility_RangeAttack** | 원거리 공격 | OnInputTriggered |

#### Enemy Abilities

| GA | 설명 |
|----|------|
| **KRGA_EnemySlash** | 근접 공격 |
| **KRGA_Enemy_Hit** | 피격 반응 |
| **KRGA_Enemy_Stun** | 스턴 상태 |
| **KRGA_Enemy_Die** | 사망 처리 |
| **KRGA_Enemy_Alert** | 경계 상태 |

### Execution Calculation

데미지 계산은 **UGEExecCalc_DamageTaken**에서 처리합니다.

```cpp
// 데미지 공식
FinalDamage = (BaseDamage × DealDamageMult × CritMultiplier - DefensePower)
              × TakeDamageMult × (1 - GuardReduction)

// 가드/패리 처리
- KRTAG_STATE_ACTING_PARRY → 100% 데미지 감소 + 패리 이벤트
- KRTAG_STATE_ACTING_GUARD → GuardConfig.StandardGuardReduction 적용
- 전방 공격만 가드/패리 적용 (IsHitFromFront 체크)
```

### Gameplay Cues

시각/청각 피드백을 위한 GameplayCue 시스템:

| Cue | 설명 |
|-----|------|
| **KRGameplayCue_Combat** | 전투 이펙트 (히트, 블록) |
| **KRGameplayCue_AerialCombo** | 특수 공격 이펙트 |
| **KRGameplayCue_StarDash** | 특수 공격 이펙트 |
| **KRGameplayCueNotify_CoreDrive*** | 코어 드라이브 이펙트 |

### 파일 구조

```
Source/KORAProject/GAS/
├── KRAbilitySystemComponent.h/cpp      # ASC 확장
├── KRAbilityTagRelationshipMapping.h   # 태그 관계 매핑
│
├── Abilities/
│   ├── KRGameplayAbility.h/cpp         # GA 베이스 클래스
│   ├── HeroAbilities/                  # 플레이어 GA (24+)
│   │   ├── Attack/                     # 공격 GA
│   │   ├── KRGA_HeroGuard.h            # 가드
│   │   ├── KRGA_HeroDash.h             # 회피
│   │   └── KRGA_CoreDrive*.h           # 궁극기
│   └── EnemyAbility/                   # 적 GA (6+)
│
├── AbilitySet/
│   └── KRAbilitySet.h/cpp              # AbilitySet DataAsset
│
├── AttributeSets/
│   ├── KRCombatCommonSet.h/cpp         # 공통 전투 속성
│   ├── KRPlayerAttributeSet.h/cpp      # 플레이어 속성
│   └── KREnemyAttributeSet.h/cpp       # 적 속성
│
├── ExecCalc/
│   └── GEExecCalc_DamageTaken.h/cpp    # 데미지 계산
│
└── GameplayCues/                       # 시각/청각 피드백
    ├── KRGameplayCue_Combat.h
    └── KRGameplayCue_AerialCombo.h etc...
```
---
## AI System

### 아키텍처 개요

Project KORA의 적 AI는 Unreal Engine 5의 **State Tree**를 활용합니다. State Tree는 행동 트리(Behavior Tree)의 현대적 대안으로, 시각적 편집과 유연한 상태 관리를 제공합니다.

### 핵심 설계 원칙
- **State Tree 기반 상태 관리**: AI 행동을 StateTree로 시각적 제어
- **AI Perception 기반 타겟 감지**: 시야(Sight) 센서로 플레이어 탐지
- **EQS 연동**: Environment Query System으로 전술적 위치 선정
- **데이터 주도 설계**: DataTable로 적 속성 및 StateTree 정의

### AI 상태 흐름
<img width="3582" height="1831" alt="image" src="https://github.com/user-attachments/assets/3c482ad9-7f1f-46b6-8565-ab98a16cb22d" />

```
┌─────────────────────────────────────────────────────────────────┐
│                        AI STATE FLOW                            │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   ┌─────────┐                                                   │
│   │  IDLE   │ ◄─────────────────────────────────────────┐       │
│   └────┬────┘                                           │       │
│        │ (시간 경과)                                     │       │
│        ▼                                                │       │
│   ┌─────────┐     EQS로 타겟 탐색                       │       │
│   │ PATROL  │ ─────────────────┐                        │       │
│   └────┬────┘                  │                        │       │
│        │                       ▼                        │       │
│        │ (AI Perception)  ┌─────────────┐               │       │
│        │ (플레이어 발견)   │  Run EQS    │               │       │
│        │                  │  Query      │               │       │
│        ▼                  └──────┬──────┘               │       │
│   ┌─────────┐                    │                      │       │
│   │ COMBAT  │ ◄──────────────────┘                      │       │
│   └────┬────┘                                           │       │
│        │                                                │       │
│        ▼                                                │       │
│   ┌──────────────┐    공격 GA 선택                      │       │
│   │ SelectAttack │ ─────────────────┐                   │       │
│   └──────┬───────┘                  │                   │       │
│          │                          ▼                   │       │
│          │                   ┌─────────────┐            │       │
│          │                   │ Attack      │            │       │
│          │                   │ Evaluator   │            │       │
│          ▼                   └─────────────┘            │       │
│   ┌─────────┐                       │                   │       │
│   │  MOVE   │ ◄─────────────────────┘                   │       │
│   │(플레이어│   (사거리 밖)                              │       │
│   │ 에게로) │                                           │       │
│   └────┬────┘                                           │       │
│        │ (사거리 내)                                     │       │
│        ▼                                                │       │
│   ┌─────────┐     GA 발동                               │       │
│   │ ATTACK  │ ─────────────────┐                        │       │
│   └────┬────┘                  │                        │       │
│        │                       ▼                        │       │
│        │               ┌─────────────┐                  │       │
│        │               │  Perform    │                  │       │
│        │               │  Attack GA  │                  │       │
│        │               └─────────────┘                  │       │
│        │                       │                        │       │
│        ▼                       │                        │       │
│   ┌─────────┐                  │                        │       │
│   │  RAGE   │ ◄─ (HP <= RageRate)                       │       │
│   │(분노 시)│                  │                        │       │
│   └────┬────┘                  │                        │       │
│        │                       │                        │       │
│        ▼                       │                        │       │
│   ┌─────────┐                  │                        │       │
│   │   HIT   │ ◄─ (피격 시)      │                        │       │
│   │REACTION │                  │                        │       │
│   └────┬────┘                  │                        │       │
│        │                       │                        │       │
│        ▼                       │                        │       │
│   ┌─────────┐                  │                        │       │
│   │  DEAD   │ ◄─ (HP <= 0) ────┘                        │       │
│   └─────────┘                                           │       │
│                                                         │       │
│   (플레이어 시야 상실) ─────────────────────────────────┘       │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 클래스 구조

```
┌─────────────────────────────────────────────────────────────────┐
│                    AKRAIEnemyController                         │
│  - StateTreeAIComponent 보유                                     │
│  - AIPerceptionComponent (시야 감지)                             │
│  - TargetActor 관리                                              │
└──────────────────────┬──────────────────────────────────────────┘
                       │ controls
                       ▼
┌─────────────────────────────────────────────────────────────────┐
│                      AKREnemyPawn                               │
│  - IAbilitySystemInterface 구현                                  │
│  - EnemyASC (AbilitySystemComponent)                            │
│  - CombatComponent                                               │
│  - OnSelectAttack / OnUnselectAttack 델리게이트                  │
└──────────────────────┬──────────────────────────────────────────┘
                       │ uses
                       ▼
┌─────────────────────────────────────────────────────────────────┐
│               UKRAIStateTree_EnemySchema                        │
│  - ContextActorClass = AKREnemyPawn                             │
│  - AIControllerClass = AKRAIEnemyController                     │
│  - KRAI 전용 Task/Evaluator/Condition 클래스만 허용             │
└─────────────────────────────────────────────────────────────────┘
```

### AI Controller

```cpp
// KRAIEnemyController.h
UCLASS()
class AKRAIEnemyController : public AModularAIController
{
    UPROPERTY() TObjectPtr<UStateTreeAIComponent> StateTreeComponent;
    UPROPERTY() TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;
    UPROPERTY() TObjectPtr<AActor> TargetActor;

protected:
    // AI Perception으로 플레이어 감지 시 호출
    UFUNCTION()
    void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
```

### Evaluator 시스템

Evaluator는 StateTree에서 매 틱마다 상태를 평가하고 데이터를 제공합니다.

#### Enemy Evaluator
적 자신의 상태를 평가합니다.

```cpp
// KRAIStateTree_EnemyEvaluator.h
UCLASS()
class UKRAIStateTree_EnemyEvaluator : public UKRAIStateTreeEvaluatorBase
{
    // Context
    TObjectPtr<AKREnemyPawn> Actor;
    TObjectPtr<AKRAIEnemyController> AIController;

    // Output - 다른 노드에서 참조 가능
    bool bCanAttackRange = false;      // 공격 가능 거리인가?
    bool bIsEnemyDead = false;         // 적이 죽었는가?
    bool bIsRageStatus = false;        // 분노 상태인가?
    float CanAttackRange = 0.f;        // 공격 가능 사거리
    float DistanceToTarget = 0.f;      // 타겟까지 거리
    float CurrentHealthPercent = 0.f;  // 현재 HP 비율

    // Parameter
    float EnterRageHealthPercent = 0.f; // 분노 진입 HP 비율

    virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override
    {
        // HP 체크 → 분노 상태 전환
        if (!bIsRageStatus && CurrentHealthPercent <= EnterRageStatusRate)
        {
            bIsRageStatus = true;
            Context.SendEvent(KRTAG_ENEMY_AISTATE_RAGE);
        }

        // HP 0 이하 → 사망 이벤트
        if (CurrentHealth <= 0.f)
        {
            bIsEnemyDead = true;
            Context.SendEvent(KRTAG_ENEMY_AISTATE_DEAD, Payload);
        }
    }
};
```

#### Player Evaluator
플레이어의 상태를 추적합니다.

```cpp
// KRAIStateTree_PlayerEvaluator.h
UCLASS()
class UKRAIStateTree_PlayerEvaluator : public UKRAIStateTreeEvaluatorBase
{
    // Output
    TObjectPtr<AActor> PlayerActor = nullptr;
    float DistanceToPlayer = 0.f;
    bool bIsPlayerAttacking = false;
    bool bIsPlayerDead = false;

    virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override
    {
        PlayerActor = AIController->TargetActor;
        DistanceToPlayer = FVector::Dist(Actor->GetActorLocation(), PlayerActor->GetActorLocation());
    }
};
```

#### Attack Evaluator
공격 능력 선택 상태를 관리합니다.

```cpp
// KRAIStateTree_AttackEvaluator.h
UCLASS()
class UKRAIStateTree_AttackEvaluator : public UKRAIStateTreeEvaluatorBase
{
    // Output
    bool bIsAttacking = false;
    TSubclassOf<UGameplayAbility> AttackAbilityClass = nullptr;
    float AcceptableAttackRange = 0.f;

    virtual void TreeStart(FStateTreeExecutionContext& Context) override
    {
        // SelectAttack Task에서 브로드캐스트하는 이벤트 수신
        Actor->OnSelectAttack.AddUObject(this, &ThisClass::OnSelectAttack);
        Actor->OnUnselectAttack.AddUObject(this, &ThisClass::OnUnselectAttack);
    }

    void OnSelectAttack(TSubclassOf<UGameplayAbility> InAttackAbility, float InRange)
    {
        AttackAbilityClass = InAttackAbility;
        AcceptableAttackRange = InRange;
    }
};
```

### Task 시스템

Task는 StateTree에서 실제 행동을 수행합니다.

#### Select Attack Task
공격 능력을 선택합니다.

```cpp
// KRAIStateTree_SelectAttackTask.h
UCLASS()
class UKRAIStateTree_SelectAttackTask : public UKRAIStateTreeTaskBase
{
    TObjectPtr<AKREnemyPawn> Actor;
    TSubclassOf<UGameplayAbility> AbilityClass;  // 선택할 공격 GA
    float AcceptableAttackRange;                  // 공격 가능 사거리

    virtual EStateTreeRunStatus EnterState(...) override
    {
        // Attack Evaluator에게 선택된 공격 정보 전달
        Actor->OnSelectAttack.Broadcast(AbilityClass, AcceptableAttackRange);
        return EStateTreeRunStatus::Succeeded;
    }
};
```

#### Perform Attack Task
선택된 공격 GA를 실행합니다.

```cpp
// KRAIStateTree_PerformAttackTask.h
UCLASS()
class UKRAIStateTree_PerformAttackTask : public UKRAIStateTreeTaskBase
{
    TObjectPtr<AKREnemyPawn> Actor;
    TSubclassOf<UGameplayAbility> AbilityClass;

    virtual EStateTreeRunStatus EnterState(...) override
    {
        if (!Actor->TryActivateAbility(AbilityClass))
            return EStateTreeRunStatus::Failed;
        return EStateTreeRunStatus::Running;
    }

    virtual EStateTreeRunStatus Tick(...) override
    {
        // GA가 끝날 때까지 Running 유지
        if (FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(AbilityClass))
            return Spec->IsActive() ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Succeeded;
        return Super::Tick(Context, DeltaTime);
    }

    virtual void ExitState(...) override
    {
        Actor->OnUnselectAttack.Broadcast();  // 공격 선택 해제
    }
};
```

#### Run EQS Query Task
Environment Query System을 실행하여 타겟을 찾습니다.
<img width="1302" height="711" alt="image" src="https://github.com/user-attachments/assets/5417941c-d241-454d-a8d5-01814f840bb9" />


```cpp
// KRAIStateTree_RunEQSQuery.h
UCLASS()
class UKRAIStateTree_RunEQSQuery : public UKRAIStateTreeTaskBase
{
    TObjectPtr<AKRAIEnemyController> AIController;
    TObjectPtr<UEnvQuery> EnvQuery;
    TArray<FAIDynamicParam> QueryConfig;
    TEnumAsByte<EEnvQueryRunMode::Type> RunMode;

    virtual EStateTreeRunStatus EnterState(...) override
    {
        FEnvQueryRequest Request(EnvQuery, AIController);
        RequestId = Request.Execute(RunMode,
            FQueryFinishedSignature::CreateLambda([...](TSharedPtr<FEnvQueryResult> Result)
            {
                if (Result && Result->IsSuccessful())
                {
                    TArray<AActor*> Targets;
                    Result->GetAllAsActors(Targets);
                    for (AActor* Target : Targets)
                    {
                        if (AKRHeroCharacter* Player = Cast<AKRHeroCharacter>(Target))
                        {
                            AIController->TargetActor = Player;
                            break;
                        }
                    }
                }
            }));
        return EStateTreeRunStatus::Running;
    }
};
```

#### Send Event Task
StateTree에 이벤트를 발송합니다.

```cpp
// KRAIStateTree_SendEventTask.h
USTRUCT()
struct FKRAIStateTree_SendEventTask : public FStateTreeTaskCommonBase
{
    virtual EStateTreeRunStatus EnterState(...) const override
    {
        if (InstanceData.EventTag.IsValid())
        {
            if (UStateTreeAIComponent* StateTreeComp = AIController->FindComponentByClass<UStateTreeAIComponent>())
            {
                StateTreeComp->SendStateTreeEvent(FStateTreeEvent(InstanceData.EventTag));
            }
        }
        return EStateTreeRunStatus::Succeeded;
    }
};
```

### AI Perception 연동

```cpp
// KRAIEnemyController.cpp
void AKRAIEnemyController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    const bool bIsSight = (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
                          && IsPlayerCharacter(Actor);
    if (!bIsSight) return;

    if (Stimulus.WasSuccessfullySensed())
    {
        // 플레이어 발견 → Combat 상태로 전환
        TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
        SetFocus(TargetActor);
        StateTreeComponent->SendStateTreeEvent(KRTAG_ENEMY_AISTATE_COMBAT);
    }
    else
    {
        // 시야 상실 → 타겟 해제
        ClearFocus(EAIFocusPriority::Gameplay);
        TargetActor = nullptr;
    }
}
```

### 핵심 설계 요약

| 특징 | 구현 방식 | 장점 |
|------|----------|------|
| **상태 관리** | StateTree + Evaluator/Task | 시각적 편집, 유연한 상태 전이 |
| **타겟 감지** | AI Perception (Sight) | 현실적인 시야 기반 감지 |
| **위치 선정** | EQS (Environment Query) | 전술적 위치 자동 계산 |
| **공격 시스템** | GAS 어빌리티 연동 | 재사용 가능한 공격 정의 |
| **데이터 주도** | DataTable (FEnemyDataStruct) | 코드 수정 없이 적 추가 |
| **이벤트 기반** | StateTree Event | 느슨한 결합, 상태 전환 용이 |

---

## Quest System

### 아키텍처 개요

Project KORA의 퀘스트 시스템은 **Unreal Engine StateTree**를 활용하여 퀘스트 진행 상태를 관리합니다.
<img width="1563" height="935" alt="image" src="https://github.com/user-attachments/assets/8a2e3ed5-38bf-48b7-b971-b6d666a5d010" />
<img width="2573" height="1196" alt="Screenshot 2026-01-10 213648" src="https://github.com/user-attachments/assets/942cca8a-9f61-4b34-9da2-4227dbab7139" />


**핵심 설계 원칙:**
- **StateTree 기반 상태 관리**: 퀘스트 흐름을 StateTree로 제어
- **이벤트 기반 조건 검사**: Gameplay Message Subsystem으로 느슨한 결합
- **데이터 주도 설계**: DataTable로 퀘스트 정의
- **모듈식 Condition Checker**: 새 목표 유형을 쉽게 추가 가능

### 클래스 구조

```
┌─────────────────────────────────────────────────────────────┐
│                    UKRQuestSubsystem                        │
│  - 퀘스트 수락/포기 관리                                      │
│  - Condition Checker 레지스트리                              │
│  - 활성 퀘스트 인스턴스 추적                                   │
└──────────────────────┬──────────────────────────────────────┘
                       │ manages
                       ▼
┌─────────────────────────────────────────────────────────────┐
│                    AKRQuestActor                            │
│  - StateTree 컴포넌트 보유                                    │
│  - UKRQuestInstance 참조                                     │
└──────────────────────┬──────────────────────────────────────┘
                       │ contains
                       ▼
┌─────────────────────────────────────────────────────────────┐
│                   UKRQuestInstance                          │
│  - 퀘스트 상태 (NotStarted, InProgress, Completed, Failed)   │
│  - 서브퀘스트 진행 상황 관리                                   │
│  - Condition Checker 인스턴스 보유                           │
└──────────────────────┬──────────────────────────────────────┘
                       │ owns
                       ▼
┌─────────────────────────────────────────────────────────────┐
│              UQuestConditionChecker (다형성)                 │
│  ├─ UKillMonsterChecker      (몬스터 처치)                   │
│  ├─ UQuestAddItemChecker     (아이템 수집)                   │
│  ├─ UQuestEnterLocationChecker (위치 진입)                   │
│  ├─ UQuestTalkNPCChecker     (NPC 대화)                     │
│  └─ UPlayAbilityChecker      (어빌리티 사용)                  │
└─────────────────────────────────────────────────────────────┘
```

### Condition Checker 시스템

모든 Condition Checker는 동일한 패턴을 따릅니다:
1. **Initialize**: 메시지 리스너 등록
2. **ReceiveMessage**: 메시지 수신 → 검증 → AddCount() 호출
3. **Uninitialize**: 리스너 해제

```cpp
// QuestConditionChecker.h
UCLASS()
class UQuestConditionChecker : public UObject
{
    virtual UQuestConditionChecker* Initialize(
        UKRQuestInstance* NewQuestInstance,
        const FSubQuestEvalDataStruct& EvalData);
    virtual void Uninitialize();
    virtual bool CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag);
};
```

### 퀘스트 생명주기

```
1. ACCEPTANCE (수락)
   Player → AKRQuestAcceptTriggerBox 진입
   → UKRQuestSubsystem::AcceptQuest(Index)
   → DataTable에서 퀘스트 데이터 로드
   → UKRQuestInstance 생성
   → Condition Checker 생성
   → AKRQuestActor 스폰

2. EXECUTION (실행)
   StateTree Start
   → QuestInstance::StartQuest()
   → 매 프레임 Evaluator::Tick() 호출

3. PROGRESS TRACKING (진행 추적)
   게임 이벤트 발생
   → GameplayMessageSubsystem 브로드캐스트
   → Condition Checker 수신
   → QuestInstance->AddCount()

4. COMPLETION (완료)
   모든 서브퀘스트 완료
   → QuestInstance::CompleteQuest()
   → Condition Checker Uninitialize
   → OnQuestCompleted 브로드캐스트
```

---

## Inventory & Equipment System

### Fragment 기반 아이템 시스템

Project KORA의 아이템 시스템은 **Fragment Pattern**을 사용하여 아이템 기능을 모듈화합니다.

```
┌─────────────────────────────────────────────────────────────────┐
│                    ITEM ARCHITECTURE                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│   UKRInventoryItemInstance (런타임 인스턴스)                       │
│   ├── ItemTag (아이템 식별 태그)                                   │
│   ├── UKRInventoryItemDefinition (정적 정의)                       │
│   │   └── FragmentContainer (TMap<Tag, Fragment>)                │
│   │       ├── DisplayUI      → 이름, 설명, 아이콘                  │
│   │       ├── EquippableItem → 장비 인스턴스 생성                  │
│   │       ├── ConsumableItem → 효과, 쿨다운, 스택                  │
│   │       ├── EnhanceableItem→ 강화 레벨, 비용                    │
│   │       ├── ModuleItem     → 스탯 수정자                        │
│   │       ├── SetStats       → 무기 스탯 캐시                      │
│   │       └── QuickSlot      → 퀵슬롯 등록 가능                    │
│   └── InstanceFragments (런타임 복제본 - 강화 등 변경 가능)         │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

### Fragment 종류

| Fragment | 설명 | 주요 속성 |
|----------|------|----------|
| **DisplayUI** | UI 표시 정보 | 이름, 설명, 아이콘 |
| **EquippableItem** | 장착 가능 | EquipmentInstance |
| **ConsumableItem** | 소비 아이템 | 효과, 쿨다운, 스택 |
| **EnhanceableItem** | 강화 가능 | 강화 레벨, 비용 |
| **ModuleItem** | 모듈 | 스탯 수정자 |
| **SetStats** | 스탯 설정 | 공격력, 속도, 사거리 |
| **QuickSlot** | 퀵슬롯 | 슬롯 등록 가능 여부 |

### 소비 아이템 시스템

소비 아이템은 **GameplayEffect**를 통해 효과를 적용하며, 쿨다운과 스택을 관리합니다.
<img width="602" height="350" alt="image" src="https://github.com/user-attachments/assets/253b4b6b-875c-4dc8-9966-c9497ff71ccf" />


```cpp
// FConsumableEffectConfig - 소비 아이템 효과 설정
struct FConsumableEffectConfig
{
    TSubclassOf<UGameplayEffect> MainEffectClass;  // 적용할 GE
    EConsumableEffectType EffectType;               // Instant / Duration / Infinite
    float Power;                                    // 효과 강도
    float Duration;                                 // 지속 시간
    int32 StackMax;                                 // 최대 중첩 수
};
```

### 장비 시스템

**UKREquipmentManagerComponent**가 장비 장착/해제와 전투 모드를 관리합니다.
<img width="2317" height="1215" alt="image" src="https://github.com/user-attachments/assets/e7223635-7515-4f76-9ef7-1d0202b9dbe3" />

```
┌─────────────────────────────────────────────────────────────────┐
│                    EQUIPMENT FLOW                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  EquipItem(ItemInstance)                                         │
│      │                                                           │
│      ├─→ WeaponSlot ─→ WeaponEquipmentMap에서 Entry 조회        │
│      │       └─→ 무기 액터 생성/Attach (숨김 상태)                 │
│      │                                                           │
│      └─→ ModuleSlot ─→ 스탯 수정자 GE 적용                       │
│                                                                   │
│  ActivateCombatMode(WeaponTypeTag)                               │
│      ├─→ GA 부여 (GrantedAbilities)                              │
│      ├─→ IMC 추가 (InputMappingContext)                          │
│      ├─→ AnimLayer 변경 (무기별 애니메이션)                        │
│      └─→ 무기 가시성 ON                                          │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

### 무기 스탯

```cpp
// FWeaponStatsCache - 무기 스탯 캐시
struct FWeaponStatsCache
{
    float AttackPower;    // 공격력
    float AttackSpeed;    // 공격 속도
    float Range;          // 사거리
    float CritChance;     // 치명타 확률
    float CritMulti;      // 치명타 배율
    int32 Capacity;       // 탄창 (원거리)
    float ReloadTime;     // 재장전 시간 (원거리)
};
```

### 퀵슬롯 시스템
<img width="1204" height="643" alt="image" src="https://github.com/user-attachments/assets/3684c486-9460-4111-9c79-e646fa7b1ea0" />

인벤토리 서브시스템에서 퀵슬롯 바인딩과 사용을 관리합니다.

```cpp
// UKRInventorySubsystem - 퀵슬롯 API
bool BindItemByQuickSlotTag(FGameplayTag SlotTag, FGameplayTag ItemTag);  // 슬롯에 아이템 바인딩
bool UseQuickSlotItem(FGameplayTag SlotTag);                              // 슬롯 아이템 사용
bool SelectQuickSlotClockwise();                                          // 시계방향 슬롯 선택
bool SelectQuickSlotCounterClockwise();                                   // 반시계방향 슬롯 선택
FGameplayTag GetCurrentSelectedQuickSlot() const;                         // 현재 선택된 슬롯
```

### 모듈 시스템

무기에 장착하여 스탯을 수정하는 모듈 아이템입니다.

| 속성 | 설명 |
|-----|------|
| **TargetWeaponSlotTag** | 장착 가능한 무기 슬롯 |
| **CachedModifiers** | 스탯 수정자 배열 (공격력+, 치명타+, 등) |

모듈 장착 시 **GameplayEffect**로 스탯이 실시간 적용됩니다.

---

## UI/UX System

### 아키텍처 개요

Project KORA의 UI 시스템은 **Common UI** 플러그인을 기반으로, 다음 핵심 원칙을 따릅니다:

- **재사용 가능한 컴포넌트**: ItemSlotBase, SlotGridBase, ItemDescriptionBase를 조합하여 다양한 UI 구성
- **느슨한 결합**: Gameplay Message Subsystem을 통한 이벤트 통신
- **데이터 분리**: Adapter 패턴을 통해 게임 데이터를 UI 전용 구조체로 변환
- **자동화**: Router/Input Subsystem이 라우트 관리 및 입력 컨텍스트 전환을 자동 처리

### 재사용 가능한 UI 컴포넌트
<img width="2761" height="947" alt="Screenshot 2026-01-10 214525" src="https://github.com/user-attachments/assets/1c0b1b64-023f-46e2-a965-f9481de8d6e2" />

#### UKRItemSlotBase (아이템 슬롯)

인벤토리, 상점, 장비 등 모든 아이템 표시에 재사용되는 기본 슬롯 컴포넌트입니다.

```cpp
// 표시할 필드를 비트마스크로 선택
UENUM(BlueprintType, meta = (Bitflags))
enum class EKRItemField : uint8
{
    Icon, Name, Description, Count, Price, ShopStock
};

UCLASS()
class UKRItemSlotBase : public UCommonButtonBase
{
    // 표시할 필드 설정 (비트마스크)
    UPROPERTY(EditAnywhere, meta = (Bitmask, BitmaskEnum = "EKRItemField"))
    int32 VisibleFields = Bit(Icon) | Bit(Name);

    // 바인딩 가능한 위젯들 (선택적)
    UPROPERTY(meta = (BindWidgetOptional)) UCommonLazyImage* ItemIcon;
    UPROPERTY(meta = (BindWidgetOptional)) UCommonTextBlock* ItemName;
    UPROPERTY(meta = (BindWidgetOptional)) UCommonTextBlock* ItemDescription;
    UPROPERTY(meta = (BindWidgetOptional)) UCommonNumericTextBlock* ItemCount;
    UPROPERTY(meta = (BindWidgetOptional)) UCommonNumericTextBlock* ItemPrice;
    UPROPERTY(meta = (BindWidgetOptional)) UCommonNumericTextBlock* ShopStock;

    void SetItemData(const FKRItemUIData& InData, int32 OverrideFields = -1);
};
```

#### UKRSlotGridBase (슬롯 그리드)

아이템 슬롯들을 그리드로 배치하고 선택/호버 이벤트를 관리합니다.

```cpp
UCLASS()
class UKRSlotGridBase : public UCommonUserWidget
{
    UPROPERTY(meta = (BindWidget)) UUniformGridPanel* SlotGrid;
    UPROPERTY(EditAnywhere) TSubclassOf<UKRItemSlotBase> SlotClass;
    UPROPERTY(EditAnywhere) int32 Rows = 4;
    UPROPERTY(EditAnywhere) int32 Columns = 4;

    // 이벤트
    UPROPERTY(BlueprintAssignable) FOnSlotIndexEvent OnSelectionChanged;
    UPROPERTY(BlueprintAssignable) FOnSlotIndexEvent OnHoverChanged;

    void InitializeItemGrid(const TArray<FKRItemUIData>& InData);
    void BuildGrid();
};
```

#### UKRItemDescriptionBase (아이템 설명)

선택된 아이템의 상세 정보를 표시합니다.

```cpp
UCLASS()
class UKRItemDescriptionBase : public UCommonUserWidget
{
    UFUNCTION(BlueprintImplementableEvent)
    void UpdateItemInfo(const FName& ItemNameKey, const FName& ItemDescriptionKey,
                        const TSoftObjectPtr<UTexture2D>& ItemIcon, int32 UpgradeLevel = -1);
};
```

### 컴포넌트 재사용 패턴

동일한 컴포넌트들이 다양한 UI에서 재사용됩니다:

```
┌─────────────────────────────────────────────────────────────────┐
│                    COMPONENT REUSE PATTERN                        │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  UKRInventoryMain (인벤토리)                                      │
│  ├── UKRSlotGridBase* InventorySlot                              │
│  │       └── UKRItemSlotBase[] (Icon, Name, Count 표시)          │
│  └── UKRItemDescriptionBase* ItemDescriptionWidget               │
│                                                                   │
│  UKRShopBuy (상점 구매)                                           │
│  ├── UKRSlotGridBase* ShoppingSlot                               │
│  │       └── UKRItemSlotBase[] (Icon, Name, Price, Stock 표시)   │
│  └── UKRItemDescriptionBase* ShopItemDescription                 │
│                                                                   │
│  UKREquipmentMain (장비)                                          │
│  ├── UKRSlotGridBase* EquipmentSlot                              │
│  │       └── UKRItemSlotBase[] (Icon, Name 표시)                 │
│  └── UKRItemDescriptionBase* EquipmentDescription                │
│                                                                   │
│  UKRWeaponUpgradePage (무기 강화)                                 │
│  ├── UKRSlotGridBase* WeaponSlot                                 │
│  └── UKRItemDescriptionBase* UpgradeDescription                  │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

### UI 전용 데이터 (FKRItemUIData)

게임 로직과 UI를 분리하기 위한 UI 전용 데이터 구조체입니다.

```cpp
USTRUCT(BlueprintType)
struct FKRItemUIData
{
    FGameplayTag ItemTag;              // 아이템 식별
    FName ItemNameKey;                 // StringTable 키
    FName ItemDescriptionKey;          // StringTable 키
    TSoftObjectPtr<UTexture2D> ItemIcon;
    int32 Quantity = 0;                // 수량
    int32 Price = -1;                  // 가격 (-1 = 비매품)
    int32 UpgradeLevel = 0;            // 강화 레벨
    int32 ShopStock = 0;               // 상점 재고
};
```

### UI Adapter Library

게임 데이터를 UI 데이터로 변환하는 유틸리티 함수 라이브러리입니다.

```cpp
UCLASS()
class UKRUIAdapterLibrary : public UBlueprintFunctionLibrary
{
    // 인벤토리 → UI 데이터
    static void GetOwnerInventoryUIData(UObject* World, TArray<FKRItemUIData>& Out);
    static void GetInventoryUIDataFiltered(UObject* World, const FGameplayTag& Filter, TArray<FKRItemUIData>& Out);

    // 상점 → UI 데이터
    static void GetShopUIData(UObject* World, TArray<FKRItemUIData>& Out);

    // 장비 → UI 데이터
    static void GetEquippedCategoryUIData(UObject* World, const TArray<FGameplayTag>& SlotTags, TArray<FKRItemUIData>& Out);
    static bool GetEquippedSlotUIData(UObject* World, const FGameplayTag& SlotTag, FKRItemUIData& Out);

    // 퀵슬롯 → UI 데이터
    static bool GetQuickSlotUIData(UObject* World, const FGameplayTag& SlotTag, FKRItemUIData& Out);
};
```

### UI Input Subsystem (Common Input)

UI 전용 입력을 DataTable 기반으로 관리하고, 위젯별 입력 바인딩을 제공합니다.

```cpp
// 입력 액션 카탈로그 (DataTable Row 이름)
struct FKRUIInputRowCatalog
{
    FName Pause = TEXT("Pause");
    FName Back = TEXT("Back");
    FName Select = TEXT("Select");
    FName Navigate = TEXT("Navigate");
    FName Prev = TEXT("Prev");
    FName Next = TEXT("Next");
    FName Increase = TEXT("Increase");
    FName Decrease = TEXT("Decrease");
};

UCLASS()
class UKRUIInputSubsystem : public ULocalPlayerSubsystem
{
    // DataTable 기반 입력 바인딩
    FUIActionBindingHandle BindRow(UCommonActivatableWidget* Widget, FName RowName, FSimpleDelegate Handler);

    // Back 버튼 기본 동작 (라우트 닫기)
    void BindBackDefault(UCommonActivatableWidget* Widget, FName RouteName = NAME_None);

    // UI 모드 전환 (레퍼런스 카운팅)
    void EnterUIMode(bool bShowCursor = true);
    void ReleaseUIMode();
};
```

### UI Router Subsystem

레이어 기반 UI 스택 관리와 라우트 시스템을 제공합니다.

```cpp
// UI 레이어 (우선순위 순)
enum class EKRUILayer : uint8
{
    Game = 0,       // 게임 내 UI (입력 모드 변경 없음)
    GamePopup = 1,  // 게임 팝업 (입력 모드 변경 없음)
    GameMenu = 2,   // 게임 메뉴 (UI 모드로 전환)
    Menu = 3,       // 메뉴 (UI 모드로 전환)
    Modal = 4,      // 모달 (UI 모드로 전환)
    Cinematic = 5   // 시네마틱
};

// 게임 일시정지 정책
enum class EKRUIGameStopPolicy : uint8
{
    None,           // 일시정지 없음
    PauseWhileOpen, // 열려있는 동안 일시정지
    PauseWhileTop   // 최상단일 때만 일시정지
};

UCLASS()
class UKRUIRouterSubsystem : public UGameInstanceSubsystem
{
    // 라우트 등록
    void RegisterRoute(FName Route, const FKRRouteSpec& Spec);

    // 라우트 열기/닫기/토글
    UCommonActivatableWidget* OpenRoute(FName Route);
    bool CloseRoute(FName Route);
    UCommonActivatableWidget* ToggleRoute(FName Route);

    // 이벤트 (Input Subsystem이 구독하여 자동 모드 전환)
    FOnRouteOpened OnRouteOpened;
    FOnRouteClosed OnRouteClosed;
};
```

### 입력 모드 자동 전환 흐름

```
┌─────────────────────────────────────────────────────────────────┐
│                  AUTO INPUT MODE SWITCHING                        │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  Router.OpenRoute("Inventory")                                   │
│      │                                                           │
│      ├─→ 위젯 생성 및 스택에 Push                                 │
│      │                                                           │
│      ├─→ OnRouteOpened 브로드캐스트                               │
│      │       │                                                   │
│      │       └─→ UIInputSubsystem 수신                           │
│      │               │                                           │
│      │               ├─→ Layer가 GameMenu/Menu/Modal?            │
│      │               │       YES → EnterUIMode()                 │
│      │               │               ├─→ 입력 모드: UI Only      │
│      │               │               ├─→ 커서 표시               │
│      │               │               └─→ RefCount++              │
│      │               │                                           │
│      │               └─→ Layer가 Game/GamePopup?                 │
│      │                       YES → 입력 모드 변경 없음            │
│      │                                                           │
│  Router.CloseRoute("Inventory")                                  │
│      │                                                           │
│      ├─→ OnRouteClosed 브로드캐스트                               │
│      │       │                                                   │
│      │       └─→ UIInputSubsystem 수신                           │
│      │               └─→ ReleaseUIMode()                         │
│      │                       ├─→ RefCount--                      │
│      │                       └─→ RefCount == 0?                  │
│      │                               YES → 게임 입력 모드 복원    │
│      │                                                           │
└─────────────────────────────────────────────────────────────────┘
```

### Gameplay Message 기반 통신

```cpp
// 리스너 등록
UGameplayMessageSubsystem& Subsystem = UGameplayMessageSubsystem::Get(World);
Handle = Subsystem.RegisterListener<FKRUIMessage_Boss>(
    FKRUIMessageTags::Boss(), this, &ThisClass::OnBossMessageReceived);

// 메시지 브로드캐스트
FKRUIMessage_Boss Message;
Message.BossASC = GetAbilitySystemComponent();
Subsystem.BroadcastMessage(FKRUIMessageTags::Boss(), Message);
```

### 전체 아키텍처 다이어그램

```
┌─────────────────────────────────────────────────────────────────┐
│                        Game Logic Layer                          │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │
│  │ Inventory   │  │  Combat     │  │   Quest     │              │
│  │ Subsystem   │  │  System     │  │   System    │              │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘              │
└─────────┼────────────────┼────────────────┼─────────────────────┘
          │                │                │
          ▼                ▼                ▼
┌─────────────────────────────────────────────────────────────────┐
│                  Gameplay Message Subsystem                      │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │  Channels: ItemLog, Weapon, QuickSlot, Boss, Quest ...   │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
          │
          ▼
┌─────────────────────────────────────────────────────────────────┐
│                      Adapter Layer                               │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │              UKRUIAdapterLibrary                            │ │
│  │  - MakeUIDataFromItemInstance()                            │ │
│  │  - GetOwnerInventoryUIData()                               │ │
│  └────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
          │
          ▼
┌─────────────────────────────────────────────────────────────────┐
│                         UI Layer                                 │
│  ┌─────────────────┐    ┌─────────────────────────────────────┐ │
│  │ Router Subsystem│◄───│        Input Subsystem              │ │
│  │ - OpenRoute()   │    │ - Auto UI Mode Switch               │ │
│  │ - CloseRoute()  │    │ - Ref Counting                      │ │
│  └────────┬────────┘    └─────────────────────────────────────┘ │
│           │                                                      │
│           ▼                                                      │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │                   Primary Game Layout                       │ │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐           │ │
│  │  │  Game   │ │GameMenu │ │  Menu   │ │  Modal  │  Stacks   │ │
│  │  │  Layer  │ │  Layer  │ │  Layer  │ │  Layer  │           │ │
│  │  └─────────┘ └─────────┘ └─────────┘ └─────────┘           │ │
│  └────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 핵심 설계 원칙 요약

| 원칙 | 구현 방식 | 장점 |
|------|----------|------|
| **모듈화** | `IKRHUDWidgetInterface` | 독립적 개발/테스트, 조합 유연성 |
| **느슨한 결합** | Gameplay Message Subsystem | 발신/수신자 디커플링, 확장 용이 |
| **데이터 분리** | `UKRUIAdapterLibrary` | UI가 게임 로직에 의존하지 않음 |
| **라우트 자동화** | `UKRUIRouterSubsystem` | 레퍼런스 카운팅, 일시정지 정책 자동화 |
| **입력 자동화** | `UKRUIInputSubsystem` | 레이어별 자동 입력 모드 전환 |

---
## Animation System

### LinkedAnimLayer 기반 설계
<img width="1929" height="945" alt="image" src="https://github.com/user-attachments/assets/aa25b398-40a2-41f6-a2ca-506cb0a545ea" />
<img width="1837" height="718" alt="image" src="https://github.com/user-attachments/assets/dc39fedd-f5a1-42fd-a88d-2ca8ea4d60a2" />

전투 모드(무기 종류)에 따라 애니메이션 세트를 동적으로 교체합니다.

```
┌─────────────────────────────────────────────────────────────────┐
│                    LINKED ANIM LAYER                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  Base AnimBP (ABP_Hero)                                          │
│      │                                                           │
│      ├─→ LinkedAnimLayer: ABP_Sword (근접 무기)                   │
│      │       └─→ Attack, Guard, Idle 애니메이션                   │
│      │                                                           │
│      └─→ LinkedAnimLayer: ABP_Gun (원거리 무기)                   │
│              └─→ Aim, Fire, Reload 애니메이션                     │
│                                                                   │
│  EquipmentManager.ActivateCombatMode()                           │
│      └─→ LinkAnimClassLayers(EquipAnimLayer)                     │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

### 모션 매칭 (Motion Matching)

순차적 상태 머신 대신, 캐릭터의 궤적과 현재 상태에 따라 최적의 애니메이션을 자동 선택합니다.
모션 매칭은 최종 보스 AI의 자연스러운 Locomotion을 위해 사용되었습니다. 
<img width="2481" height="948" alt="image" src="https://github.com/user-attachments/assets/47f4aafa-263d-4138-95ba-0d9ac89c9ea4" />


| 구성 요소 | 설명 |
|----------|------|
| **Chooser Table** | 조건에 따른 애니메이션 선택 규칙 |
| **PoseSearch Database** | 모션 매칭용 포즈 데이터베이스 |
| **Trajectory Matching** | 이동 궤적 기반 애니메이션 매칭 |

---

## Camera System

### Frame Update Loop

매 Tick마다 카메라 위치를 계산하는 과정입니다.

```
┌─────────────────────────────────────────────────────────────────┐
│                    CAMERA UPDATE FLOW                             │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  1. Engine → PlayerCameraManager (뷰 계산 요청)                   │
│         │                                                        │
│         ▼                                                        │
│  2. PlayerCameraManager → KRCameraComponent (계산 위임)          │
│         │                                                        │
│         ▼                                                        │
│  3. KRCameraComponent → CameraModeStack (블렌딩 요청)            │
│         │                                                        │
│         ▼                                                        │
│  4. CameraModeStack: 활성 모드들 순회 및 가중치 블렌딩            │
│         │   [Default] ──┐                                        │
│         │   [LockOn]  ──┼─→ Lerp(Weight) → 최종 위치             │
│         │   [Ladder]  ──┘                                        │
│         │                                                        │
│         ▼                                                        │
│  5. 각 Mode: 커브(Curve) + 충돌 체크(Penetration) → 이상적 위치  │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

### Camera Mode Stack & Blending

- **스택 구조**: 여러 카메라 모드가 스택에 쌓임
  - 예: `[Default]` → 사다리 탑승 → `[Default, Ladder]`
- **블렌딩**: `BlendTime`에 따라 서서히 전환
  - `UpdateBlending`에서 `BlendWeight` (0~1) 조절
  - `Lerp`로 부드러운 시점 이동

---

## Troubleshooting

### 1. 애니메이션

| 문제 | 원인 | 해결 |
|-----|------|------|
| 무기 들고 걷기/뛰기 시 양팔 어색함 | 기본 로코모션과 무기 포즈 충돌 | 커스텀 정지 포즈 + BlendMask + LinkedAnimLayer로 자연스럽게 수정 |
| 애니메이션 시퀀스 Bake 후 관절 꺾임 | 미리보기와 실제 가중치 차이 | 가중치를 줄여가며 애니메이션 시퀀스에서 직접 수정 |

### 2. GAS 초기화 Race Condition

| 문제 | 원인 | 해결 |
|-----|------|------|
| BeginPlay에서 ASC가 간헐적으로 nullptr | PlayerState 초기화 속도 차이로 Pawn BeginPlay 시점에 PlayerState 미유효 | **Lazy Initialization** 도입 - 실제 사용 시점에 유효성 검사 후 캐싱 |

### 3. 카메라 회전 보간 문제

| 문제 | 원인 | 해결 |
|-----|------|------|
| 카메라 리셋 시 특정 각도에서 멈추거나 무한 회전 | `FMath::IsNearlyEqual` 사용 시 -179.9° vs 180°를 다르게 판단 | `FMath::FindDeltaAngleDegrees`로 최단 거리 계산 + 리셋 시작 시점 회전값 스냅샷 고정 |

### 4. 레벨 디자인

| 문제 | 원인 | 해결 |
|-----|------|------|
| **나나이트 미적용** | DX11/Vulkan 설정 시 나나이트 파이프라인 미작동 | Default RHI를 **DX12**로 변경 + 머티리얼 블렌드 모드 Opaque + 나나이트 호환성 옵션 체크 |
| **Z-Fighting** (겹친 면 깜빡임) | 두 면의 깊이 값이 부동소수점 오차 내 동일 | 액터 병합 후 겹치는 폴리곤 삭제 → 폴리곤 재연결 작업 |
| **텍스처 늘어남** | UV가 메시 로컬 좌표계 기준, 스케일 변경 시 고정 | `WorldAlignedTexture` 노드로 **Triplanar Mapping** 적용 |
| **Stationary Light 빨간 X 표시** | RGBA 채널에 최대 4개 그림자만 저장, 5개 이상 시 비활성화 | 라이트 목적별 재분류: Static (장식), Stationary (주요), Movable (상호작용) |

---

## Getting Started

### 요구 사항

- **Unreal Engine 5.6** 이상
- **Visual Studio 2022** (Windows)
- **8GB+ RAM** 권장
- **50GB+ 저장 공간**

### 설치 방법

1. **저장소 클론**
   ```bash
   git clone https://github.com/NbcampUnreal/3rd_4th-Team1-CH6-Project.git
   ```

2. **프로젝트 파일 생성**
   ```bash
   # .uproject 파일 우클릭 → Generate Visual Studio project files
   ```

3. **솔루션 열기**
   ```bash
   # KORAProject.sln 파일을 Visual Studio 2022로 열기
   ```

4. **빌드 및 실행**
   ```bash
   # Visual Studio에서 Development Editor | Win64 선택
   # F5 또는 Ctrl+F5로 실행
   ```

> **Note**: 이 프로젝트의 VFX/SFX/레벨 디자인/메타휴먼 관련 파일들은 깃허브에 공유되어 있지 않습니다.

### 에디터 실행 후

1. **메인 레벨 열기**: `Content/Maps/MainMenu` 레벨 로드
2. **Play in Editor**: PIE 모드로 게임 테스트

---

## Project Structure

```
3rd_4th-Team1-CH6-Project/
├── Source/
│   ├── KORAProject/                    # 메인 게임 모듈
│   │   ├── AI/                         # AI 시스템
│   │   │   ├── KRAIEnemyController.h/cpp
│   │   │   ├── KREnemyPawn.h/cpp
│   │   │   └── StateTree/
│   │   │       ├── Evaluator/          # StateTree Evaluator
│   │   │       ├── Task/               # StateTree Task
│   │   │       ├── Condition/          # StateTree Condition
│   │   │       └── PropertyFunctions/
│   │   ├── Animation/                  # 애니메이션 시스템
│   │   ├── Camera/                     # 카메라 시스템
│   │   ├── Characters/                 # 캐릭터 클래스
│   │   │   ├── KRBaseCharacter.h
│   │   │   ├── KRHeroCharacter.h
│   │   │   └── KREnemyCharacter.h
│   │   ├── Components/                 # 컴포넌트
│   │   │   ├── KRStaminaComponent.h
│   │   │   ├── KRCoreDriveComponent.h
│   │   │   └── KRGuardRegainComponent.h
│   │   ├── Data/                       # 데이터 구조체
│   │   ├── Equipment/                  # 장비 시스템
│   │   ├── GAS/                        # Gameplay Ability System
│   │   │   ├── Abilities/
│   │   │   │   ├── HeroAbilities/
│   │   │   │   └── EnemyAbility/
│   │   │   ├── AttributeSets/
│   │   │   ├── ExecCalc/
│   │   │   └── GameplayCues/
│   │   ├── GameplayTag/                # 태그 정의
│   │   ├── Inventory/                  # 인벤토리 시스템
│   │   ├── Item/                       # 아이템/무기
│   │   ├── Player/                     # 플레이어 상태
│   │   ├── Quest/                      # 퀘스트 시스템
│   │   │   ├── Condition/              # Condition Checker
│   │   │   └── Delegates/
│   │   ├── SubSystem/                  # 서브시스템 (13개)
│   │   │   ├── KRInventorySubsystem.h
│   │   │   ├── KRQuestSubsystem.h
│   │   │   ├── KRShopSubsystem.h
│   │   │   ├── KRUIRouterSubsystem.h
│   │   │   └── ...
│   │   └── UI/                         # UI 시스템
│   │       ├── HUD/
│   │       ├── Inventory/
│   │       ├── Equipment/
│   │       ├── PauseMenu/
│   │       └── ...
│   └── KORACustomEditors/              # 에디터 모듈
├── Content/
│   ├── Characters/
│   ├── Data/
│   │   └── DataTables/
│   ├── Maps/
│   └── UI/
├── Config/
├── Docs/                               # 시스템 문서
│   ├── UI_SYSTEM.md
│   └── QUEST_SYSTEM.md
├── Plugins/
└── KORAProject.uproject
```

---

## Team

### 개발팀 정보

<img width="432" height="465" alt="image" src="https://github.com/user-attachments/assets/af792f64-256e-435f-bca5-ea6afc7a4889" />


---

## License

이 프로젝트는 **교육 목적**으로 제작되었습니다.

- **Unreal Engine**: Epic Games의 라이선스를 따릅니다.
- **Lyra Starter Game**: Epic Games의 샘플 프로젝트 라이선스를 따릅니다.

---

## Acknowledgments

- **Epic Games** - Unreal Engine 5 & Lyra Starter Game
- **내일배움캠프** - 교육 프로그램 제공

---

<div align="center">

**Project KORA** - Lyra 기반 소울라이크 액션 RPG

</div>
