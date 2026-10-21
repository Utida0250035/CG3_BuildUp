#pragma once

struct ID3D12Device;
struct ID3D12GraphicsCommandList;
struct ID3D12CommandQueue;
struct ID3D12Resource;

struct IDXGIFactory;
struct IDXGIFactory7;
struct IDXGISwapChain4;
struct IDXGIAdapter;
struct IDXGIAdapter4;

namespace Atrum {
class AtrumEngine;
using Engine = AtrumEngine;
class CommandContext;
class DescriptorAllocator;
class VertexBuffer;
class IndexBuffer;
class Lighting;
class AssetModel;
class Draw;
class DrawSprite;
class FrameDeltaTime;
class RenderDevice;
class Camera;

class ModelStorage;
class TextureStorage;

namespace Input {
class PlayInput;
class DirectInput;
} // namespace Input

namespace Audio {
class AudioManager;
using Manager = AudioManager;
} // namespace Audio

} // namespace Atrum
