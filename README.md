# SimpleDX11Renderer

A basic rendering engine leveraging **DirectX 11** and **Assimp** for rendering 3D models. The project is developed in C++ and includes HLSL shaders.

## Features

- **DirectX 11** integration for rendering.
- **Assimp** library for model importing.
- Basic shader support with HLSL.
- Organized code structure for easy understanding and expansion.

## Prerequisites

- **C++17** or later.
- **Visual Studio 2019** or newer.
- **DirectX 11 SDK**.
- **Assimp library** (integrated within the repository under `External/Assimp`).

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/wjsekrud/SimpleDX11Renderer.git
   cd SimpleDX11Renderer

## Usage
- Open the project in Visual Studio.
- Build the solution.
- Run the executable to load and render 3D models.
- To change displayed model, put model into `Data/`, and adjust `Addmodel()` function in `Source/main.cpp`.

## Directory Structure
- Build/: Contains build files and configuration.
- Data/: Stores 3D model and texture assets.
- External/Assimp: Includes the Assimp library for model loading.
- Source/: Source files for the rendering engine.

## Contributing
- Contributions are welcome. Please fork the repository and create a pull request with detailed explanations of the changes.

## License
- This project is licensed under the MIT License. See the LICENSE file for details.
