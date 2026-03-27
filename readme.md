<div align="center">

```
 ██████╗ ██╗  ██╗███╗   ███╗██╗ ██████╗
██╔═══██╗██║  ██║████╗ ████║██║██╔═══██╗
██║   ██║███████║██╔████╔██║██║██║   ██║
██║   ██║██╔══██║██║╚██╔╝██║██║██║   ██║
╚██████╔╝██║  ██║██║ ╚═╝ ██║██║╚██████╔╝
 ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚═╝ ╚═════╝
```

### *"Your discipline is your legend. Your habits are your powers."*
### *"Tu disciplina es tu leyenda. Tus hábitos son tus poderes."*

![Platform](https://img.shields.io/badge/realm-Linux-blueviolet?style=for-the-badge)
![Language](https://img.shields.io/badge/forged_in-C-orange?style=for-the-badge)
![TUI](https://img.shields.io/badge/interface-ncurses-blue?style=for-the-badge)
![License](https://img.shields.io/badge/license-MIT-green?style=for-the-badge)

</div>

---

> *In the world of Ohmio, there are no shortcuts. Every push-up, every book read, every glass of water — each one is an act of power. Your terminal is your battlefield. Your habits are your weapons. Level up or fade into obscurity.*

> *En el mundo de Ohmio, no existen los atajos. Cada ejercicio, cada libro leído, cada vaso de agua — cada uno es un acto de poder. Tu terminal es tu campo de batalla. Tus hábitos son tus armas. Sube de nivel o desvanécete en el olvido.*

---

## ⚔ What is Ohmio? / ¿Qué es Ohmio?

**EN** — Ohmio is a terminal RPG habit tracker forged in pure C. It transforms your daily routines into a character-building system: complete habits, earn XP, level up, evolve your powers, and redeem real-world rewards with the XP you've earned through discipline.

**ES** — Ohmio es un habit tracker RPG de terminal forjado en C puro. Transforma tus rutinas diarias en un sistema de construcción de personaje: completa hábitos, gana XP, sube de nivel, evoluciona tus poderes y canjea recompensas del mundo real con el XP que has ganado a través de la disciplina.

---

## 🗺 The Panels / Los Paneles

```
┌──────────────────────────────────────────────────────────────────┐
│ ⚔  OHMIO        Kael               LVL 7   XP ████████░░  340/580│
├──────────────┬───────────────────────────────────────────────────┤
│              │                                                   │
│  MENU        │   HABITOS DE HOY                      4/5        │
│              │   ████████████████████████░░░░░░░░░░░░░░░        │
│ ▶ Habitos   │                                                   │
│   Stats      │   [✓] 📚 Leer 30 min        🔥 14  +65xp  Lv3  │
│   Semana     │   [✓]   Ejercicio          🔥 21  +112xp Lv4  │
│   Poderes    │   [ ] 💧 Agua 2L            🔥 6   +50xp  Lv1  │
│   Recomp.    │   [✓] 🧘 Meditacion        🔥 9   +58xp  Lv2  │
│              │   [✓] 💻 Codigo 1hr        🔥 30  +195xp Lv5  │
│              │                                                   │
├──────────────┴───────────────────────────────────────────────────┤
│  [j/k] Navegar   [SPC] Completar   [a] Agregar   [q] Salir      │
└──────────────────────────────────────────────────────────────────┘
```

| Panel | EN | ES |
|-------|----|----|
|  Hábitos | Your daily quests. Complete them or lose your streak. | Tus misiones diarias. Complétalas o pierde tu racha. |
| ★ Stats | Your character sheet. Bars forged from consistency. | Tu hoja de personaje. Barras forjadas desde la consistencia. |
| ◈ Semana | The chronicle of your week. Seven days, seven chances. | La crónica de tu semana. Siete días, siete oportunidades. |
| ✦ Poderes | Your top habits as powers. A pie chart of your soul. | Tus mejores hábitos como poderes. Gráfico de pastel de tu alma. |
| ◉ Recompensas | Spend your XP on real-world rewards you define. | Gasta tu XP en recompensas reales que tú defines. |

---

## The RPG System / El Sistema RPG

### XP & Leveling / XP y Niveles

**EN** — Every habit you complete awards XP. But raw completion is just the beginning. The longer your streak, the more powerful the reward:

**ES** — Cada hábito que completas otorga XP. Pero la simple finalización es solo el comienzo. Cuanto más larga tu racha, más poderosa la recompensa:

```
Base XP  ×  streak_bonus  ×  power_level_bonus  =  XP earned
                 ↑                    ↑
          +10% per 7 days      +15% per power level
               (cap: 3×)
```

### The Five Stats / Los Cinco Stats

*Each habit category forges a different part of your character:*
*Cada categoría de hábito forja una parte diferente de tu personaje:*

| Stat | Category | Symbol |
|------|----------|--------|
| **Strength / Fuerza** | Physical / Físico | 🔴 |
| **Wisdom / Sabiduría** | Mental / Mental | 🟣 |
| **Vitality / Vitalidad** | Health / Salud | 🟢 |
| **Charisma / Carisma** | Social / Social | 🔵 |
| **Discipline / Disciplina** | All / Todos | 🟡 |

> *Discipline is the percentage of perfect days — days where every single habit was completed — out of your total days played.*
>
> *La Disciplina es el porcentaje de días perfectos — días en los que cada hábito fue completado — sobre el total de días jugados.*

### Habit Powers / Poderes de Hábito

**EN** — Every habit starts at Power Level 1. After each 7-day streak, your habit **evolves**. Its XP multiplier grows. It becomes a weapon, not just a task.

**ES** — Cada hábito comienza en Nivel de Poder 1. Después de cada racha de 7 días, tu hábito **evoluciona**. Su multiplicador de XP crece. Se convierte en un arma, no solo una tarea.

```
Streak  1–6  →  Power Lv 1  →  1.00× XP
Streak  7–13 →  Power Lv 2  →  1.15× XP
Streak 14–20 →  Power Lv 3  →  1.30× XP
Streak 21–27 →  Power Lv 4  →  1.45× XP
        ...
        Cap  →               →  3.00× XP
```

### Real-World Rewards / Recompensas del Mundo Real

**EN** — This is the most important mechanic. You define your own rewards. You earn XP through discipline. You spend that XP on things that exist outside the screen. A movie night, a new book, a guilt-free weekend. The currency is real because the effort was real.

**ES** — Esta es la mecánica más importante. Tú defines tus propias recompensas. Ganas XP a través de la disciplina. Gastas ese XP en cosas que existen fuera de la pantalla. Una noche de película, un libro nuevo, un fin de semana sin culpa. La moneda es real porque el esfuerzo fue real.

---

## The Daemon / El Daemon

**EN** — `ohmio-daemon` runs silently in the background. It watches your habits. When you're falling behind, it speaks. When your streak is in danger, it screams.

**ES** — `ohmio-daemon` corre silenciosamente en segundo plano. Vigila tus hábitos. Cuando te estás quedando atrás, habla. Cuando tu racha está en peligro, grita.

| Hour | Message | Urgency |
|------|---------|---------|
| 9:00 | Morning reminder / Recordatorio matutino | Normal |
| 14:00 | Afternoon check / Revisión vespertina | Normal |
| 20:00 | Night warning / Advertencia nocturna | Critical |
| 21:00 | *Streak danger* — only if streak ≥ 3 | Critical 🔥 |

---

## ⚙ Installation / Instalación

### Requirements / Requisitos

```bash
# Arch Linux
sudo pacman -S gcc ncurses sqlite libnotify glib2 base-devel

# A notification daemon / Un daemon de notificaciones
# Pick one / Elige uno:
sudo pacman -S dunst    # or mako, or swaync
```

### Build / Compilar

```bash
git clone https://github.com/yourusername/ohmio.git
cd ohmio
make
```

*Two binaries will emerge / Dos binarios emergerán:*
- `ohmio` — the TUI, your battlefield / la TUI, tu campo de batalla
- `ohmio-daemon` — the silent watcher / el vigilante silencioso

### Deploy the Daemon / Desplegar el Daemon

```bash
mkdir -p ~/.local/bin
cp ohmio-daemon ~/.local/bin/
```

**Hyprland** — add to / agrega a `~/.config/hypr/hyprland.conf`:
```
exec-once = ~/.local/bin/ohmio-daemon
```

---

## 🗡 Keybindings / Controles

| Key / Tecla | Action EN | Acción ES |
|-------------|-----------|-----------|
| `j / ↓` | Move down / next habit | Bajar / siguiente hábito |
| `k / ↑` | Move up / previous habit | Subir / hábito anterior |
| `h / l` `Tab` | Switch panel | Cambiar panel |
| `Space` | Complete habit | Completar hábito |
| `a` | Add new habit | Agregar hábito |
| `e` | Edit selected habit | Editar hábito seleccionado |
| `d` | Delete (with confirmation) | Borrar (con confirmación) |
| `q` | Quit & save | Salir y guardar |

---

## 🏛 Project Structure / Estructura del Proyecto

```
ohmio/
├── include/                  # Headers — the laws of the realm
│   ├── types.h               # Hero, Habit, Reward, GameState
│   ├── hero.h                # XP, leveling, stats
│   ├── habits.h              # Streaks, powers, completion logic
│   ├── db.h                  # The chronicle (SQLite)
│   ├── tui.h                 # The battlefield (ncurses)
│   ├── renderer.h            # The painter of worlds
│   └── panels.h              # The five sacred panels
├── src/
│   ├── main.c                # Where the legend begins
│   ├── core/
│   │   ├── hero.c            # Character engine
│   │   └── habits.c          # Habit engine
│   ├── data/
│   │   └── db.c              # SQLite persistence
│   ├── ui/
│   │   ├── tui.c             # Game loop & input
│   │   ├── renderer.c        # Header, sidebar, footer, bars
│   │   └── panels.c          # All five views + charts
│   └── daemon/
│       └── main_daemon.c     # The silent watcher
└── Makefile
```

---

## Your Chronicle / Tu Crónica

*All data is stored locally. No cloud. No tracking. Just you and your discipline.*
*Todos los datos se guardan localmente. Sin nube. Sin rastreo. Solo tú y tu disciplina.*

```bash
sqlite3 ~/.axiom/data.db

SELECT * FROM hero;
SELECT * FROM habits ORDER BY streak DESC;
SELECT * FROM daily_log ORDER BY date DESC LIMIT 7;
SELECT * FROM completions ORDER BY completed_at DESC LIMIT 20;
```

---

##   Forged With / Forjado Con

| Tool | Purpose EN | Propósito ES |
|------|-----------|--------------|
| [ncurses](https://invisible-island.net/ncurses/) | Terminal UI engine | Motor de interfaz de terminal |
| [SQLite](https://www.sqlite.org/) | Local data persistence | Persistencia local de datos |
| [libnotify](https://gitlab.gnome.org/GNOME/libnotify) | System notifications | Notificaciones del sistema |
| Pure C | No frameworks. No runtime. Just metal. | Sin frameworks. Sin runtime. Solo metal. |

---

<div align="center">

*The path of a thousand levels begins with a single habit.*

*El camino de mil niveles comienza con un solo hábito.*

** Begin your legend. / Comienza tu leyenda. **

</div>
