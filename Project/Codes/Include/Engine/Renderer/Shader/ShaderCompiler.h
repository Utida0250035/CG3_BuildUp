#pragma once
#include <string>
#include <vector>

#include <wrl/client.h>
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

namespace Atrum {

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

        // pixelShaderのコンパイル結果(2値抜き(BinaryAlpha)用)
        ComPtr<IDxcBlob> psBinaryAlphaBlob_ = nullptr;

        // コンパイルを実行するインターフェース
        ComPtr<IDxcBlob> Compile(const std::wstring& filePath, const wchar_t* profile, const std::vector<std::wstring>& defines = {});

    public:

        // コンパイルに必要な依存関係を初期化
        void Initialize();

        void CompileShaders();

        /* ゲッター */

        IDxcBlob* GetVertexShaderBlob() const { return vertexShaderBlob_.Get(); }
        IDxcBlob* GetPixelShaderBlob() const { return pixelShaderBlob_.Get(); }
        IDxcBlob* GetPsBinaryAlphaBlob() const { return psBinaryAlphaBlob_.Get(); }

    };

}