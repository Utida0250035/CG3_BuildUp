#pragma once
#include <string>

#include <wrl/client.h>
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

class ShaderCompiler {

private:

    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    ComPtr<IDxcUtils> dxcUtils_ = nullptr;
    ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;
    ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;

    // vertexShaderのコンパイル結果
    ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;

    // pixelShaderのコンパイル結果
    ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;

    // コンパイルを実行するインターフェース
    ComPtr<IDxcBlob> Compile(const std::wstring& filePath, const wchar_t* profile);

public:
    
    // コンパイルに必要な依存関係を初期化
    void Initialize();

    void CompileShaders();

    /* ゲッター */

    IDxcBlob* GetVertexShaderBlob() const { return vertexShaderBlob_.Get(); }
    IDxcBlob* GetPixelShaderBlob() const { return pixelShaderBlob_.Get(); }

};