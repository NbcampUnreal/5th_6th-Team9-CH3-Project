# ⚔️ 9강9조 — Unreal Team Project 
 
> "Unreal Engine과 C++를 활용한 다수의 적을 쓰러뜨리는 슈터 게임 프로젝트" 만들기<br>
> Unreal Engine 5 기반 3D 액션 슈팅 싱글플레이 게임<br>
> ┗"물건 소환 마법밖에 못 써서 파티에서 쫓겨난 나는, 현대의 '총기'를 소환해 이세계 최강 드래곤 슬레이어가 되었다."

---

## 🎮 프로젝트 소개 (Overview)
<img width="2560" height="1600" alt="image" src="https://github.com/user-attachments/assets/ea4cb7cc-f226-4f2f-8a5e-de6b14707d8b" />

**Project 9강9조**는 언리얼 엔진을 사용해 개발한 3D 액션 게임으로,  
플레이어 캐릭터의 전투 시스템, AI 몬스터, 아이템 구조, 상점, 인벤토리 등  
RPG형 슈팅게임입니다.

🪄**스토리**<br>
게임의 주인공(캐릭터)가 물건 소환 마법 밖에 못써 파티 플레이에서 쫓껴나 무기(총기)를 캐릭터 주변으로 소환하여 드래곤을 잡으러 모험을 떠나는 스토리입니다.
게임 플레이 시간은 최대 12분 정도이며 10분 부터는 캐릭터 체력이 일정 수치만큼 감소하게 됩니다.
게임 플레이 시간 기준 3분, 6분 후 중간 보스 몬스터가 자동 출현하며, 그 이후는 특정 구역에서 최종 보스 몬스터를 쓰러뜨리는 흐름으로 진행됩니다.
레벨업시 캐릭터의 스탯(능력치)를 3가지 선택지 중 1가지를 선택하여 상승 시키며, 상점에서는 무기를 구매하여 장착하거나 이미 구매한 무기를 추가 구매하여 무기 레벨을 높일 수 있습니다.
상점은 랜덤 위치로 스폰되며, 미니맵을 통해 상점 및 몬스터 위치를 파악 할 수 있습니다.

🕹️**조작법**<br>
조작키는 WASD로 전후좌우 이동, 스페이스바로 점프를 하며, F키로 앞구르기를 할 수 있습니다. 
앞구르기는 동작 중 몬스터의 데미지를 받지 않아 회피 기술로 활용 할 수 있습니다.
공격은 자동 공격으로 캐릭터에게 각 무기의 사거리에 따라 접근하면 공격을 하게 됩니다.

🔫**무기**<br>
무기는 총 6가지 입니다.
총 : 권총(기본 장착), 돌격소총, 저격총, 산탄총
투척무기 : 수류탄, 화염병<br>
※ 무기별로 사격 속도, 공격력, 사거리, 몬스터 다중 공격 여부 등이 다르게 구현되어 있어 플레이어가 무기를 적절히 선택하여 공격 패턴을 만들 수 있습니다.

📅 **개발 기간:** 2025.10. ~ 2025.11.10.  
👥 **개발 인원:** 6명  
💻 **개발 환경:** Unreal Engine 5.5 / C++ / Blueprint / Visual Studio 2022  

---

## 🌟 주요 기능 (Features)

| 구분 | 기능 | 설명 |
|------|------|------|
| 🧍 캐릭터 시스템 | 캐릭터 조작, 회전 보간, 구르기, 좌우 이동 후 정면 바라보기, 무기 장착 표시 | 좌우 이동 후 정면 바라보기 및 이동/회전 구현, 무기 장착 후 메시로 시각적 구현 |
| ⚔️ 전투 시스템 | 공격, 피격, UI 연동 | 체력 표시 및 피격 연출, 몬스터 공격 우선 순위 기준으로 특정 몬스터 우선 공격, 무기별 공격 로직 별도 구현 |
| 🤖 AI 몬스터 | 탐지, 추적, 공격 | 플레이어 행동에 반응하는 적 인공지능, 최종/중간 보스 몬스터별 공격 패턴 다각화 |
| 💎 아이템 구조 | 아이템 정보 관리, 드랍/획득 | 무기 및 드랍 아이템 구조화, 데이터 테이블 기반 무기 아이템 관리 |
| 🏪 상점 & 인벤토리 | 아이템 구매/판매, 인벤토리 컴포넌트 | 아이템 데이터 연동 및 상점 UI 기반 거래 구현, 게임 흐름에 따른 상점 아이템 변화 구현 |
| ⚙️ 게임 시스템 | 전체 흐름 관리 | 시작, 전투, 종료 등 게임 진행 제어 및 상태 관리, Wave를 통한 게임 환경 변화(몬스터 스폰) |

---

## 🧱 프로젝트 구조 (Project Structure)
```
Project-9강9조/
┣ Source/
┃ ┣ Promiss9/
┃ ┃ ┣ P9Character/ # 캐릭터 및 이동 시스템
┃ ┃ ┣ P9DragonMonster/ # 드래곤 몬스터
┃ ┃ ┣ P9FinalBossAltar/ # 최종 보스몬스터 스포너
┃ ┃ ┣ P9GameInstance/ # 게임 진행 흐름 관리
┃ ┃ ┣ P9GameMode/ # 게임 모드
┃ ┃ ┣ P9GameState/ # 게임 Wave 관리
┃ ┃ ┣ P9HealingItem/ # 맵 스폰 힐링 아이템
┃ ┃ ┣ P9InventoryComponent/ # 캐릭터 인벤토리 컴포넌트
┃ ┃ ┣ P9ItemActor/ # 아이템 액터
┃ ┃ ┣ P9ItemGameInstanceSubsystem/ # 아이템 액터 정보 관리
┃ ┃ ┣ P9ItemInterface/ # 아이템 정의
┃ ┃ ┣ P9MiddleDragonMonster/ # 첫 번째 중간 보스 몬스터
┃ ┃ ┣ P9Monster/ # 몬스터 클래스
┃ ┃ ┣ P9MonsterSpawner/ # 몬스터 스포너
┃ ┃ ┣ P9MushroomMonster/ # 버섯 몬스터
┃ ┃ ┣ P9PlayerController/ # 플레이어 컨트롤러
┃ ┃ ┣ P9PlayerState/ # 캐릭터 정보(스탯) 관리
┃ ┃ ┣ P9SecondMiddleDragonMonster/ # 두 번째 중간 보스 몬스터
┃ ┃ ┣ P9ShellMonster / # 쉘 몬스터
┃ ┃ ┣ P9Shop/ # 상점
┃ ┃ ┣ P9SlimeMonster/ # 슬라임 몬스터
┃ ┃ ┗ P9WeaponData/ # 무기 아이템 데이터
┃ ┗ Project-9강9조Editor/
┣ Content/
┃ ┣ Asset/
┃ ┃ ┣ AlstraInfinite_TacticalStarterPack/ # 투척 무기 에셋
┃ ┃ ┣ BattleWizardPolyart/ # 캐릭터 에셋
┃ ┃ ┣ FourEvilDragonsHP/ # 드래곤 몬스터 에셋
┃ ┃ ┣ Icon/ # 아이템 및 게임 아이콘 에셋
┃ ┃ ┣ MonsterForSurvivalGame/ 
┃ ┃ ┣ MuzzleFlash/ # 아이템 효과 에셋
┃ ┃ ┣ RPGTinyFantasyForest/ # 레벨(맵) 에셋
┃ ┃ ┣ Sound/ # 음향효과 에셋
┃ ┃ ┣ Vefects/
┃ ┃ ┗ Weapons_Free/ # 사격 무기 에셋
┃ ┣ Blueprints/
┃ ┃ ┣ Character/ # 캐릭터 블루프린트 클래스
┃ ┃ ┣ Monster/ # 몬스터 블루프린트 클래스
┃ ┃ ┣ Shop/ # 상점 블루프린트 클래스
┃ ┃ ┣ System/ # 게임 시스템 블루프린트 클래스
┃ ┃ ┣ UI/ # UI, HUD 블루프린트 클래스
┃ ┃ ┗ Weapon/ # 아이템 액터 블루프린트 클래스
┃ ┣ Inputs/
┃ ┣ Maps/
┃ ┗ FX/
┣ Config/
┗ README.md
```

---

## 🧠 기술 구현 (Technical Highlights)

### 🤖 AI 몬스터, 프로젝트 각 클래스간 연동 관리 — *노재욱 (팀장)*
- `AIPerceptionComponent`를 통한 시야 탐지 및 추적  
- Behavior Tree와 Blackboard로 상태 기반 행동 제어  
- 무기에 대한 거리 기반 공격 패턴 변화
- 각 시스템 간의 인터페이스 연결 및 이벤트 관리

### ⚙️ 게임 시스템 & 흐름 제어 — *박진연*
- 게임 전반의 상태 관리 (시작, 진행, 종료)  
- 게임 모드, 플레이어 상태, 레벨 전환 로직 통합
- 게임 테스트로 버그 및 각종 에디터 오류 확인

### ⚔️ 전투 시스템 & UI — *이규진*
- 블루프린트 기반 구현(C++ Delegate 활용)
- 범위 안에 들어온 적 중 가까운 적 타겟팅 로직으로 전투 시스템 구현
- 공격 게임 데이터(체력바, 경험치바, 미니맵 등)와 UI 연동 및 구현

### 🏪 상점 & 인벤토리 — *이종혁*
- 인벤토리 컴포넌트 설계 및 데이터 저장 구조  
- 상점 UI 및 거래 로직 (구매 / 갱신)  
- 인벤토리-상점 간 아이템 연동  

### 💎 아이템 구조 & 무기 시스템 — *유용선*
- 아이템 데이터 구조 설계 (내부 정보 및 드랍 분리)  
- 드랍 아이템과 인벤토리 아이템의 일원화된 관리  
- 아이템 획득 시스템 구현

### 🎮 캐릭터 시스템 — *원유연*
- 컨트롤러 방향 회전 보간(`RInterpTo`) 및 타이머 기반 보간 구현  
- 좌/우이동 후 정면을 바라 보는 로직 구현 
- 구르기 액션: `LaunchCharacter()` + 몽타주 재생 기반의 이동 

## 👥 팀 구성 (Team Members)

| 이름 | 역할 | 담당 기능 | GitHub |
|------|------|------------|--------|
| **노재욱 (팀장)** | AI 시스템 | 몬스터 탐지/추적/공격 | [@NoJaeuk](https://github.com/NoJaeuk) |
| **박진연** | 게임 시스템 | 게임 진행 흐름 제어 | [@JinYeon-Park](https://github.com/JinYeon-Park) |
| **이규진** | 전투 / UI | 전투 시스템, 체력바, 위젯 | [@ggyu-ik](https://github.com/ggyu-ik) |
| **유용선** | 아이템 / 무기 | 아이템 구조, 드랍 및 무기 획득 | [@dtoli0930-prog](https://github.com/dtoli0930-prog) |
| **이종혁** | 상점 / 인벤토리 | 인벤토리 컴포넌트 및 거래 시스템 | [@JJ00ng](https://github.com/JJ00ng) |
| **원유연** | 캐릭터 시스템 | 회전 보간, 자유시점, 구르기 | [@YUYEON91](https://github.com/YUYEON91) |

---

## 🎬 시연 영상 (Demo Video)
> 🎥 [YouTube 시연 영상 보기](https://youtu.be/example)
> 🖼️ [시연 GIF 보기](https://github.com/9gang9jo/project/assets/demo.gif)

---

## 🧾 개발 일지 (Dev Log)
| 날짜 | 내용 |
|------|------|
| 2025-10-20 | 프로젝트 초기 세팅 및 팀 역할 분담 |
| 2025-10-23 | 캐릭터 이동 및 카메라 시스템 구축 |
| 2025-10-24 | AI 탐지 / 전투 시스템 구현 |
| 2025-10-29 | 아이템 / 인벤토리 연결 |
| 2025-10-30 | 상점 연동 / 상인 생성 |
| 2025-10-25 | UI 및 게임 플로우 통합 |
| 2025-11-05 | 맵 세팅 |
| 2025-11-07 | UI 수정 |
| 2025-11-10 | 전체 테스트 및 버그 수정 완료 |

---

## ⚙️ 실행 방법 (Getting Started)

```bash
# 저장소 클론
git clone https://github.com/NbcampUnreal/5th_6th-Team9-CH3-Project.git

# 언리얼 에디터에서 .uproject 열기
# (Project9Gang9Jo.uproject 더블 클릭)

# Play 버튼으로 실행
🪪 라이선스 (License)
이 프로젝트는 MIT License 하에 배포됩니다.

📞 문의 (Contact)
팀장(노재욱) 이메일: shwodnr@gmail.com

GitHub Organization: [https://github.com/9gang9jo](https://github.com/NbcampUnreal/5th_6th-Team9-CH3-Project)
```

## 🎨 에셋 출처 (Asset Credits)

| 구분 | 에셋 이름 | 출처 / 링크 | 제작자 | 라이선스 |
|------|------------|--------------|-----------|------------|
| 캐릭터 | Wizard for Battle Polyart | [Fab.com](https://fab.com/s/b39c9e916ff1) | Dungeon Mason | CC BY 4.0 |
| 맵 | RPG Tiny Fantasy Forest | [Fab.com](https://fab.com/s/77897c6710e7) | Dungeon Mason | CC BY 4.0 |
| 일반 몬스터 | Monster for Survival Game PBR Polyart | [Fab.com](https://fab.com/s/f06519f0e24f) | Dungeon Mason | CC BY 4.0 |
| 보스 몬스터 | Dragon for Boss Monster : Handpainted | [Fab.com](https://fab.com/s/aeaefb514489) | Dungeon Mason | CC BY 4.0 |
| 일반 무기(총) | Weapons FREE - Low Poly 3D Models Pack | [Fab.com](https://fab.com/s/1133d6e887b6) | ithappy | CC BY 4.0 |
| 힐팩, 투척 무기 | Shooter Essentials - PolyPack Starter - Low Poly Assets | [Fab.com](https://www.fab.com/ko/listings/00b6336b-8c9a-4cd7-ab53-767f7cb952ed) | Alstra Infinite | CC BY 4.0 |
| 총알 발사 이펙트 | Zap VFX | [Fab.com](https://www.fab.com/ko/listings/ee38d7fd-b1a9-4a94-bfd2-10772cf9c69e) | Vefects | CC BY 4.0 |
| 총구 화염 이펙트 | Muzzle Flash (Niagara System) | [Fab.com](https://fab.com/s/a348197f2d09) | Kopo study | CC BY 4.0 |
| 힐 파티클 | Particles and Wind Control System | [Fab.com](https://fab.com/s/541f4d0128a8) | Dragon Motion | CC BY 4.0 |
| UI 아이콘 - 권총 | Pistol Icon | [icon-icons.com](https://icon-icons.com/icon/pistol/109357) | Google | Apache 2.0 |
| UI 아이콘 - 샷건 | RPG Icon Set | [itch.io](https://itch.io/) | Kenney | Flaticon License |
| UI 아이콘 - 돌격소총 | 돌격 소총 무료 아이콘 | [flaticon.com](https://www.flaticon.com/kr/free-icon/assault-rifle_3509459) | Nikita Golubev | Flaticon License |
| UI 아이콘 - 저격소총 | 저격 총 무료 아이콘 | [flaticon.com](https://www.flaticon.com/kr/free-icon/sniper-rifle_3509247) | Nikita Golubev | Flaticon License |
| UI 아이콘 - 수류탄 | 수류탄 무료 아이콘 | [flaticon.com](https://www.flaticon.com/kr/free-icon/grenade_5716082?term=%EC%88%98%EB%A5%98%ED%83%84&page=1&position=8&origin=search&related_id=5716082) | Good Ware | Flaticon License |
| UI 아이콘 - 화염병 | 화염병 무료 아이콘 | [flaticon.com](https://www.flaticon.com/kr/free-icon/molotov_4533139?term=%ED%99%94%EC%97%BC%EB%B3%91&page=1&position=23&origin=search&related_id=4533139) | Good Ware | Flaticon License |
| 사운드 | Footstep Sound Pack | [Freesound.org](https://freesound.org/) | InspectorJ | Free (Attribution) |
