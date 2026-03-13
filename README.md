# Minesweeper (C++ / SDL3)

A classic **Minesweeper** game implemented in **C++ using SDL3**.

This project recreates the well-known puzzle game while focusing on practicing **event handling, game loop architecture, and graphical rendering in C++**.

The goal of the project was to better understand how interactive applications and simple games are built using low-level libraries instead of full game engines.

---

## Gameplay

The objective of Minesweeper is to **clear a grid without detonating hidden mines**.

The player reveals tiles on the board:

- If a tile contains a mine, the game ends.
- If the tile is empty, the number displayed indicates how many mines exist in adjacent tiles.
- Using these clues, the player must logically determine where mines are located.

The game continues until either:

- all safe tiles are revealed, or  
- a mine is triggered.

---

## Technologies

- **C++**
- **SDL3**
- Custom rendering system
- Event-based input handling
- Game loop implementation

---

## Features

- Fully playable **Minesweeper logic**
- Graphical interface built with **SDL3**
- Mouse interaction for revealing tiles
- Mine generation system
- Adjacent mine detection
- Recursive empty-tile revealing
- Game over and victory conditions

---

## Learning Goals

This project was created to practice core game development concepts:

- implementing a **game loop**
- handling **user input events**
- building a simple **rendering system**
- implementing classic **game logic**
- structuring a small C++ project

It also helped strengthen understanding of **algorithmic thinking and grid-based game mechanics**.
