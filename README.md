# tetcipp

[![Language: C++](https://img.shields.io/badge/language-C%2B%2B-blue.svg)](https://isocpp.org/)
[![Renderer: Vulkan](https://img.shields.io/badge/renderer-Vulkan-red.svg)](https://www.khronos.org/vulkan/)
[![Build: CMake](https://img.shields.io/badge/build-CMake-green.svg)](https://cmake.org/)
[![License: (Add License)](https://img.shields.io/badge/license-UNLICENSED-lightgrey.svg)](./LICENSE)

**tetcipp** is a game engine built from scratch in C++ with a custom renderer using the Vulkan API. It's a hobby project designed to explore modern engine architecture, data-oriented design, and low-level graphics programming.

The primary architectural pattern is an **Entity Component System (ECS)**, and the initial goal is to implement a ("a game about falling shapes and complete horizontal lines disappearing") using this engine.

---

## 📸 Showcase

---

## ✨ Core Features

* **Custom Vulkan Renderer:** A rendering pipeline built from the ground up using the modern, high-performance Vulkan graphics API.
* **ECS Architecture:** Leverages an Entity Component System for clean, data-oriented, and performant game logic. Currently implements a custom version of ECS that is based on **[flecs](https://github.com/SanderMertens/flecs)** library.
* **Engine vs. Game Separation:** Designed to maintain a clear boundary between the reusable core engine and the specific game logic.
* **C++ Core:** Written in modern C++.
* **Cross-Platform Build:** Uses **CMake** as the build system, with helper scripts provided for Unix-like environments.

---
## 🛠️ Technologies & Tools

This project is built using a modern stack designed for high performance, modularity, and low-level control over the hardware.

* **Core Language:** **C++** (Modern C++)
    * The entire engine and game logic are written in C++ for maximum performance and direct memory control.
* **Graphics API:** **Vulkan**
    * A custom, low-overhead renderer is implemented using the Vulkan API for explicit control over GPU operations and high-performance rendering.
* **Build System:** **CMake**
    * Used for cross-platform build configuration and dependency management.
* **Shaders:** **GLSL** (with SPIR-V compilation)
    * Shaders for the Vulkan pipeline are written in GLSL and compiled to the SPIR-V intermediate representation.

---

## 🗺️ Roadmap & TODO

This project is actively being developed as a hobby. Here are the next major goals and optimizations on the to-do list:

* [ ] Integrate flecs library instead of using a custom ECS.
* [ ] Optimize Vulkan resource management (e.g., separate family queues, better memory allocation).
* [ ] Refactor and optimize the main loop.
* [ ] Implement a more modular class for time management.
* [ ] Complete the Tetris game logic.

---


