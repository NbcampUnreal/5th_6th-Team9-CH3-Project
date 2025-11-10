# ⚔️ Project 9강9조 — Unreal Team Portfolio

> Unreal Engine 5 기반 3D 액션 슈팅 싱글플레이 게임  
> "자유로운 전투 시스템과 구조화된 게임 진행"을 목표로 한 6인 팀 프로젝트

---

## 🎮 프로젝트 소개 (Overview)
<img width="2560" height="1600" alt="image" src="https://github.com/user-attachments/assets/ea4cb7cc-f226-4f2f-8a5e-de6b14707d8b" />


**Project 9강9조**는 언리얼 엔진을 사용해 개발한 3D 액션 게임으로,  
플레이어 캐릭터의 전투 시스템, AI 몬스터, 아이템 구조, 상점, 인벤토리 등  
RPG형 슈팅게임입니다.

📅 **개발 기간:** 2025.10. ~ 2025.11.10.  
👥 **개발 인원:** 6명  
💻 **개발 환경:** Unreal Engine 5.5 / C++ / Blueprint / Visual Studio 2022  

---

## 🌟 주요 기능 (Features)

| 구분 | 기능 | 설명 |
|------|------|------|
| 🧍 캐릭터 시스템 | 캐릭터 조작, 회전 보간, 자유시점, 구르기 | 자유시점 전환 및 매끄러운 이동/회전 구현 |
| ⚔️ 전투 시스템 | 공격, 피격, UI 연동 | 체력 표시 및 피격 연출, 전투 로직 완성 |
| 🤖 AI 몬스터 | 탐지, 추적, 공격 | 플레이어 행동에 반응하는 적 인공지능 |
| 💎 아이템 구조 | 아이템 정보 관리, 드랍/획득 | 무기 및 드랍 아이템 구조화, 데이터 기반 관리 |
| 🏪 상점 & 인벤토리 | 아이템 구매/판매, 인벤토리 컴포넌트 | 아이템 데이터 연동 및 UI 기반 거래 구현 |
| ⚙️ 게임 시스템 | 전체 흐름 관리 | 시작, 전투, 종료 등 게임 진행 제어 및 상태 관리 |

---

## 🧱 프로젝트 구조 (Project Structure)
```
Project9강9조/
┣ Source/
┃ ┣ Project9강9조/
┃ ┃ ┣ Character/ # 캐릭터 및 이동 시스템
┃ ┃ ┣ Combat/ # 전투 관련 로직
┃ ┃ ┣ AI/ # 몬스터 AI 클래스
┃ ┃ ┣ Items/ # 아이템 및 드랍 구조
┃ ┃ ┣ UI/ # 위젯 및 HUD
┃ ┃ ┗ Systems/ # 게임 진행 및 관리
┃ ┗ Project9강9조Editor/
┣ Content/
┃ ┣ Blueprints/
┃ ┣ Animations/
┃ ┣ UI/
┃ ┗ FX/
┣ Config/
┗ README.md
```

---

## 🧠 기술 구현 (Technical Highlights)

### 🤖 AI 몬스터 — *노재욱 (팀장)*
- `AIPerceptionComponent`를 통한 시야 탐지 및 추적  
- Behavior Tree와 Blackboard로 상태 기반 행동 제어  
- 플레이어 거리 기반 공격 패턴 변화  

### ⚙️ 게임 시스템 & 흐름 제어 — *박진연*
- 게임 전반의 상태 관리 (시작, 진행, 종료)  
- 게임 모드, 플레이어 상태, 레벨 전환 로직 통합  
- 각 시스템 간의 인터페이스 연결 및 이벤트 관리  

### ⚔️ 전투 시스템 & UI — *이규진*
- C++ 기반의 공격 및 피격 처리 로직  
- 체력바, 데미지 위젯 UI 연동  
- 콤보 공격 및 크리티컬 연출 구현  

### 🏪 상점 & 인벤토리 — *이종혁*
- 인벤토리 컴포넌트 설계 및 데이터 저장 구조  
- 상점 UI 및 거래 로직 (구매 / 판매 / 갱신)  
- 인벤토리-상점 간 아이템 연동  

### 💎 아이템 구조 & 무기 시스템 — *유용선*
- 아이템 데이터 구조 설계 (내부 정보 및 드랍 분리)  
- 드랍 아이템과 인벤토리 아이템의 일원화된 관리  
- 무기 획득 및 장착 시스템 구현  

### 🎮 캐릭터 시스템 — *원유연*
- 컨트롤러 방향 회전 보간(`RInterpTo`) 및 타이머 기반 보간 구현  
- 자유시점(Free Look) 모드: 카메라 회전만 활성화  
- 구르기 액션: `LaunchCharacter()` + 몽타주 재생 기반의 이동 

---

## 👥 팀 구성 (Team Members)

| 이름 | 역할 | 담당 기능 | GitHub |
|------|------|------------|--------|
| **노재욱 (팀장)** | AI 시스템 | 몬스터 탐지/추적/공격 | [@jaewookno](https://github.com/jaewookno) |
| **박진연** | 게임 시스템 | 게임 진행 흐름 제어 | [@jinyeonpark](https://github.com/jinyeonpark) |
| **이규진** | 전투 / UI | 전투 시스템, 체력바, 위젯 | [@gyujinlee](https://github.com/gyujinlee) |
| **유용선** | 아이템 / 무기 | 아이템 구조, 드랍 및 무기 획득 | [@yongseonyu](https://github.com/yongseonyu) |
| **이종혁** | 상점 / 인벤토리 | 인벤토리 컴포넌트 및 거래 시스템 | [@jonghyuklee](https://github.com/jonghyuklee) |
| **원유연** | 캐릭터 시스템 | 회전 보간, 자유시점, 구르기 | [@yuyeonwon](https://github.com/yuyeonwon) |

---

## 🎬 시연 영상 (Demo Video)
> 🎥 [YouTube 시연 영상 보기](https://youtu.be/example)  
> 🖼️ [시연 GIF 보기](https://github.com/9gang9jo/project/assets/demo.gif)

---

## 🧾 개발 일지 (Dev Log)
| 날짜 | 내용 |
|------|------|
| 2025-10-10 | 프로젝트 초기 세팅 및 팀 역할 분담 |
| 2025-09-01 | 캐릭터 이동 및 카메라 시스템 구축 |
| 2025-09-15 | AI 탐지 / 전투 시스템 구현 |
| 2025-10-05 | 아이템 / 인벤토리 / 상점 연동 |
| 2025-10-25 | UI 및 게임 플로우 통합 |
| 2025-11-05 | 전체 테스트 및 버그 수정 완료 |

---

## ⚙️ 실행 방법 (Getting Started)

# 저장소 클론
git clone https://github.com/9gang9jo/Project9Gang9Jo.git

# 언리얼 에디터에서 .uproject 열기
# (Project9Gang9Jo.uproject 더블 클릭)

# Play 버튼으로 실행
🪪 라이선스 (License)
이 프로젝트는 MIT License 하에 배포됩니다.
자세한 내용은 LICENSE 파일을 참조하세요.

📞 문의 (Contact)
팀 이메일: 9gang9jo.team@gmail.com

GitHub Organization: https://github.com/9gang9jo
